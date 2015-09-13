//////////////////////////////////////////////////////////////////////
//ICTCLAS简介：计算所汉语词法分析系统ICTCLAS(Institute of Computing Technology, Chinese Lexical Analysis System)，
//             功能有：中文分词；词性标注；未登录词识别。
//             分词正确率高达97.58%(973专家评测结果)，
//             未登录词识别召回率均高于90%，其中中国人名的识别召回率接近98%;
//             处理速度为31.5Kbytes/s。
//著作权：  Copyright?2002-2005中科院计算所 职务著作权人：张华平 刘群
//遵循协议：自然语言处理开放资源许可证1.0
//Email: zhanghp@software.ict.ac.cn
//Homepage:www.nlp.org.cn;mtgroup.ict.ac.cn
/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 *
 * Abstract:
 *      N-Shortest Path Problem for graph in word segement
 *
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *
 ****************************************************************************/
// NShortPath.cpp: implementation of the CNShortPath class.
//
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "NShortPath.h"
#include "Segment.h"
#include <memory.h>
#include <string.h>
//#include <stdlib.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNShortPath::CNShortPath(CDynamicArray *apCost,unsigned int nValueKind)
{
       m_apCost=apCost;//Set the cost
	   m_nValueKind=nValueKind;//Set the value kind
	   m_nVertex=apCost->m_nCol+1;
       if(m_nVertex<apCost->m_nRow+1)
		   m_nVertex=apCost->m_nRow+1;//Get the vertex numbers

	   m_pParent=new CQueue*[m_nVertex-1];//not including the first node
	   m_pWeight=new ELEMENT_TYPE *[m_nVertex-1];
//	   m_pParent=(CQueue **)malloc((m_nVertex-1)*sizeof(CQueue *));//not including the first node
//	   m_pWeight=(ELEMENT_TYPE **)malloc(sizeof(ELEMENT_TYPE *)*(m_nVertex-1));

		for(unsigned int i=0;i<m_nVertex-1;i++)//The queue array for every node
		{
			m_pParent[i]=new CQueue[nValueKind];
			m_pWeight[i]=new ELEMENT_TYPE[nValueKind];
//			m_pParent[i]=(CQueue *)malloc(sizeof(CQueue)*nValueKind);
//			m_pWeight[i]=(ELEMENT_TYPE *)malloc(sizeof(ELEMENT_TYPE)*nValueKind);
		}
}

CNShortPath::~CNShortPath()
{
	 for(unsigned int i=0;i<m_nVertex-1;i++)//The queue array for every node
	 {
		 delete [] m_pWeight[i];
//		 free(m_pWeight[i]);
/*		 for(unsigned int j=0;j<m_nValueKind;j++)
		 {
			 m_pParent[i][j].~CQueue();//;delete 
		 }*/
//		 free(m_pParent[i]);
         delete [] m_pParent[i];
	 }
	 delete [] m_pWeight;
	 delete [] m_pParent;//not including the first node
//	 free(m_pWeight);
//	 free(m_pParent);
}

int CNShortPath::ShortPath()
{
	unsigned int nCurNode=1,nPreNode,i,nIndex;
	ELEMENT_TYPE eWeight;
	PARRAY_CHAIN pEdgeList;

    for(;nCurNode<m_nVertex;nCurNode++)
	{
	   CQueue queWork;
	   eWeight=m_apCost->GetElement(-1,nCurNode,0,&pEdgeList);//Get all the edges
       while(pEdgeList!=0 && pEdgeList->col==nCurNode)
	   {
		   nPreNode=pEdgeList->row;
		   eWeight=pEdgeList->value;//Get the value of edges
           for(i=0;i<m_nValueKind;i++)
		   {
			   if(nPreNode>0)//Push the weight and the pre node infomation
			   {
				   if(m_pWeight[nPreNode-1][i]==INFINITE_VALUE)
					   break;
		           queWork.Push(nPreNode,i,eWeight+m_pWeight[nPreNode-1][i]);
			   }
			   else
			   {
				   queWork.Push(nPreNode,i,eWeight);
				   break;
			   }
		   }//end for
           pEdgeList=pEdgeList->next;
		   
	   }//end while
       
	   //Now get the result queue which sort as weight.
	   //Set the current node information
	   for(i=0;i<m_nValueKind;i++)
	   {
			m_pWeight[nCurNode-1][i]=INFINITE_VALUE;
	   }
	   //memset((void *),(int),sizeof(ELEMENT_TYPE)*);
       //init the weight
	   i=0;	   
       while(i<m_nValueKind&&queWork.Pop(&nPreNode,&nIndex,&eWeight)!=-1)
	   {//Set the current node weight and parent
		   if(m_pWeight[nCurNode-1][i]==INFINITE_VALUE)
			   m_pWeight[nCurNode-1][i]=eWeight;
		   else if(m_pWeight[nCurNode-1][i]<eWeight)//Next queue
		   {
			   i++;//Go next queue and record next weight
			   if(i==m_nValueKind)//Get the last position
				   break;
			   m_pWeight[nCurNode-1][i]=eWeight;
		   }
           m_pParent[nCurNode-1][i].Push(nPreNode,nIndex);
	   }
	}//end for

	return 1;
}
//bBest=true: only get one best result and ignore others
//Added in 2002-1-24
void CNShortPath::GetPaths(unsigned int nNode,unsigned int nIndex,int **nResult,bool bBest)
{
    CQueue queResult;
	unsigned int nCurNode,nCurIndex,nParentNode,nParentIndex,nResultIndex=0;
    
	if(m_nResultCount>=MAX_SEGMENT_NUM)//Only need 10 result
		return ;
	nResult[m_nResultCount][nResultIndex]=-1;//Init the result 
	queResult.Push(nNode,nIndex);
    nCurNode=nNode;
	nCurIndex=nIndex;
    bool bFirstGet;
    while(!queResult.IsEmpty())
	{
		while(nCurNode>0)//
		{//Get its parent and store them in nParentNode,nParentIndex
			if(m_pParent[nCurNode-1][nCurIndex].Pop(&nParentNode,&nParentIndex,0,false,true)!=-1)
			{
			   nCurNode=nParentNode;
			   nCurIndex=nParentIndex;
			}
			if(nCurNode>0)
                queResult.Push(nCurNode,nCurIndex);
		}
		if(nCurNode==0)
		{ //Get a path and output
  		   nResult[m_nResultCount][nResultIndex++]=nCurNode;//Get the first node
		   bFirstGet=true;
		   nParentNode=nCurNode;
		   while(queResult.Pop(&nCurNode,&nCurIndex,0,false,bFirstGet)!=-1)
		   {
			   nResult[m_nResultCount][nResultIndex++]=nCurNode;
    	       bFirstGet=false;
			   nParentNode=nCurNode;
		   }
		   nResult[m_nResultCount][nResultIndex]=-1;//Set the end
		   m_nResultCount+=1;//The number of result add by 1
		   if(m_nResultCount>=MAX_SEGMENT_NUM)//Only need 10 result
				return ;
		   nResultIndex=0;
		   nResult[m_nResultCount][nResultIndex]=-1;//Init the result 

		   if(bBest)//Return the best result, ignore others
			   return ;
		}
		queResult.Pop(&nCurNode,&nCurIndex,0,false,true);//Read the top node
        while(queResult.IsEmpty()==false&&(m_pParent[nCurNode-1][nCurIndex].IsSingle()||m_pParent[nCurNode-1][nCurIndex].IsEmpty(true)))
		{
	       queResult.Pop(&nCurNode,&nCurIndex,0);//Get rid of it
		   queResult.Pop(&nCurNode,&nCurIndex,0,false,true);//Read the top node
		}
        if(queResult.IsEmpty()==false&&m_pParent[nCurNode-1][nCurIndex].IsEmpty(true)==false)
		{
			   m_pParent[nCurNode-1][nCurIndex].Pop(&nParentNode,&nParentIndex,0,false,false);
			   nCurNode=nParentNode;
			   nCurIndex=nParentIndex;
			   if(nCurNode>0)
			       queResult.Push(nCurNode,nCurIndex);
		}
	}
}
int CNShortPath::Output(int **nResult,bool bBest,int *npCount)
{//sResult is a string array
  unsigned int i;
  
  m_nResultCount=0;//The 
  if(m_nVertex<2)
  {
	  nResult[0][0]=0;
	  nResult[0][1]=1;
	  *npCount=1;
	  return 1;
  }
  for(i=0;i<m_nValueKind&&m_pWeight[m_nVertex-2][i]<INFINITE_VALUE;i++)
  {
	  GetPaths(m_nVertex-1,i,nResult,bBest);
	  *npCount=m_nResultCount;
	  if(nResult[i][0]!=-1&&bBest)//Get the best answer
		  return 1;
      if(m_nResultCount>=MAX_SEGMENT_NUM)//Only need 10 result
	 	  return 1;
  }
  return 1;
}
