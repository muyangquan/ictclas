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
 *     Machine Group
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 * Filename: Segment.cpp
 * Abstract:
 *           implementation of the CSegment class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-23
 *
 * Notes:  N-Shortest paths Word segmentation
 *                
 ****************************************************************************/

//#include "stdafx.h"
#include "Segment.h"
#ifndef unix
    #include "..\\Utility\\Dictionary.h"
    #include "..\\Utility\\Utility.h"
#else
    #include "Dictionary.h"
    #include "Utility.h"
#endif
#include "NShortPath.h"
#include <string.h>
#include <math.h>
#include <cstdlib>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSegment::CSegment()
{
	//malloc buffer
	m_pWordSeg=new PWORD_RESULT[MAX_SEGMENT_NUM];
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		m_pWordSeg[i]=new WORD_RESULT[MAX_WORDS];
	}
	m_npWordPosMapTable=0;//Record the start position of possible words
	m_nWordCount=0;//Record the End position of possible words
	m_graphOptimum.SetRowFirst();//Set row first
}

CSegment::~CSegment()
{
	//free buffer
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete m_pWordSeg[i];
	}
	delete m_pWordSeg;
}

bool CSegment::Segment(char *sSentence,CDictionary &dictCore,int nResultCount)
{
	int **nSegRoute;//The segmentation route
	nSegRoute=new int*[MAX_SEGMENT_NUM];
    int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		nSegRoute[i]=new int[MAX_SENTENCE_LEN/2];
		memset(nSegRoute[i],0,MAX_SENTENCE_LEN/2*sizeof(int));
	}
	m_graphSeg.m_segGraph.SetRowFirst(false);
	m_graphOptimum.SetRowFirst(false);
 	m_graphSeg.GenerateWordNet(sSentence,dictCore);
	CNShortPath sp(&m_graphSeg.m_segGraph,nResultCount);
	sp.ShortPath();
	sp.Output((int **)nSegRoute,false,&m_nSegmentCount);
	m_graphOptimum.SetEmpty();//Set graph optimum empty
	i=0;
	while(i<m_nSegmentCount)
	{
		GenerateWord(nSegRoute,i);
		//Gernerate word according the Segmentation route
		i++;
	}
	
	//free the memory
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] nSegRoute[i];//free the pointer memory
	}
	delete [] nSegRoute;//free the pointer array
	
	return true;
}
//Generate Word according the segmentation route
bool CSegment::GenerateWord(int **nSegRoute, int nIndex)
{
	unsigned int i=0,k=0;
	int j,nStartVertex,nEndVertex,nPOS;
	char sAtom[WORD_MAXLENGTH],sNumCandidate[100],sCurWord[100];
	ELEMENT_TYPE fValue;
	while(nSegRoute[nIndex][i]!=-1&&nSegRoute[nIndex][i+1]!=-1&&nSegRoute[nIndex][i]<nSegRoute[nIndex][i+1])
	{
		nStartVertex=nSegRoute[nIndex][i];
		j=nStartVertex;//Set the start vertex
		nEndVertex=nSegRoute[nIndex][i+1];//Set the end vertex
		nPOS=0;
		m_graphSeg.m_segGraph.GetElement(nStartVertex,nEndVertex,&fValue,&nPOS);
		sAtom[0]=0;
		while(j<nEndVertex)
		{//Generate the word according the segmentation route
			strcat(sAtom,m_graphSeg.m_sAtom[j]);
			j++;
		}
		m_pWordSeg[nIndex][k].sWord[0]=0;//Init the result ending
		strcpy(sNumCandidate,sAtom);
		while(sAtom[0]!=0&&(IsAllNum((unsigned char *)sNumCandidate)||IsAllChineseNum(sNumCandidate)))
		{//Merge all seperate continue num into one number
		 //sAtom[0]!=0: add in 2002-5-9
			strcpy(m_pWordSeg[nIndex][k].sWord,sNumCandidate);
			//Save them in the result segmentation
			i++;//Skip to next atom now 
			sAtom[0]=0;
			
			while(j<nSegRoute[nIndex][i+1])
			{//Generate the word according the segmentation route
				strcat(sAtom,m_graphSeg.m_sAtom[j]);
				j++;
			}
			strcat(sNumCandidate,sAtom);
		}
		unsigned int nLen=strlen(m_pWordSeg[nIndex][k].sWord);
		//if(nLen==4&&CC_Find("第上成±—＋∶·．／",m_pWordSeg[nIndex][k].sWord)||nLen==1&&strchr("+-./",m_pWordSeg[nIndex][k].sWord[0]))
		if((nLen==4&&CC_Find("第上成±—＋∶·．／",m_pWordSeg[nIndex][k].sWord)) ||
		   (nLen==1&&strchr("+-./",m_pWordSeg[nIndex][k].sWord[0])))
		{//Only one word
			strcpy(sCurWord,m_pWordSeg[nIndex][k].sWord);//Record current word
			i--;
		}
		else if(m_pWordSeg[nIndex][k].sWord[0]==0)//Have never entering the while loop
		{
			strcpy(m_pWordSeg[nIndex][k].sWord,sAtom);
			//Save them in the result segmentation
			strcpy(sCurWord,sAtom);//Record current word
		}
		else
		{//It is a num
			//if(strcmp("－－",m_pWordSeg[nIndex][k].sWord)==0||strcmp("—",m_pWordSeg[nIndex][k].sWord)==0||m_pWordSeg[nIndex][k].sWord[0]=='-'&&m_pWordSeg[nIndex][k].sWord[1]==0)//The delimiter "－－"
			if(strcmp("－－",m_pWordSeg[nIndex][k].sWord)==0 ||
               strcmp("—",   m_pWordSeg[nIndex][k].sWord)==0 ||
			  (m_pWordSeg[nIndex][k].sWord[0]=='-' && m_pWordSeg[nIndex][k].sWord[1]==0))//The delimiter "－－"
			{
				nPOS=30464;//'w'*256;Set the POS with 'w'
				i--;//Not num, back to previous word
			}
			else
			{//Adding time suffix

				char sInitChar[3];
				unsigned int nCharIndex=0;//Get first char
				sInitChar[nCharIndex]=m_pWordSeg[nIndex][k].sWord[nCharIndex];
				if(sInitChar[nCharIndex]<0)
				{
					nCharIndex+=1;
					sInitChar[nCharIndex]=m_pWordSeg[nIndex][k].sWord[nCharIndex];
				}
				nCharIndex+=1;
				sInitChar[nCharIndex]='\0';
				if(k>0&&(abs(m_pWordSeg[nIndex][k-1].nHandle)==27904||abs(m_pWordSeg[nIndex][k-1].nHandle)==29696)&&(strcmp(sInitChar,"—")==0||sInitChar[0]=='-')&&(strlen(m_pWordSeg[nIndex][k].sWord)>nCharIndex))
				{//3-4月                                 //27904='m'*256
				   //Split the sInitChar from the original word
					strcpy(m_pWordSeg[nIndex][k+1].sWord,m_pWordSeg[nIndex][k].sWord+nCharIndex);
					m_pWordSeg[nIndex][k+1].dValue=m_pWordSeg[nIndex][k].dValue;
					m_pWordSeg[nIndex][k+1].nHandle=27904;
					m_pWordSeg[nIndex][k].sWord[nCharIndex]=0;
					m_pWordSeg[nIndex][k].dValue=0;
					m_pWordSeg[nIndex][k].nHandle=30464;//'w'*256;
					m_graphOptimum.SetElement(nStartVertex,nStartVertex+1,m_pWordSeg[nIndex][k].dValue,m_pWordSeg[nIndex][k].nHandle,m_pWordSeg[nIndex][k].sWord);
					nStartVertex+=1;
					k+=1;
				}
				nLen=strlen(m_pWordSeg[nIndex][k].sWord);
				if((strlen(sAtom)==2&&CC_Find("月日时分秒",sAtom))||strcmp(sAtom,"月份")==0)
				{//2001年
					strcat(m_pWordSeg[nIndex][k].sWord,sAtom);
					strcpy(sCurWord,"未##时");
					nPOS=-29696;//'t'*256;//Set the POS with 'm'
				}
				else if(strcmp(sAtom,"年")==0)
				{
					 if(IsYearTime(m_pWordSeg[nIndex][k].sWord))//strncmp(sAtom,"年",2)==0&&
					 {//1998年，
						strcat(m_pWordSeg[nIndex][k].sWord,sAtom);
						strcpy(sCurWord,"未##时");
						nPOS=-29696;//Set the POS with 't'
					 }
					 else
					 {
						strcpy(sCurWord,"未##数");
						nPOS=-27904;//Set the POS with 'm'
						i--;//Can not be a time word
					 }
				}
     			else
				{
					//早晨/t  五点/t 
					if(strcmp(m_pWordSeg[nIndex][k].sWord+strlen(m_pWordSeg[nIndex][k].sWord)-2,"点")==0)
					{
						strcpy(sCurWord,"未##时");
						nPOS=-29696;//Set the POS with 't'
					}	
					else 
					{
						if(!CC_Find("∶·．／",m_pWordSeg[nIndex][k].sWord+nLen-2)&&m_pWordSeg[nIndex][k].sWord[nLen-1]!='.'&&m_pWordSeg[nIndex][k].sWord[nLen-1]!='/')
						{
							strcpy(sCurWord,"未##数");
							nPOS=-27904;//'m'*256;Set the POS with 'm'
						}
						else if(nLen>strlen(sInitChar))
						{//Get rid of . example 1.
							if(m_pWordSeg[nIndex][k].sWord[nLen-1]=='.'||m_pWordSeg[nIndex][k].sWord[nLen-1]=='/')
								m_pWordSeg[nIndex][k].sWord[nLen-1]=0;
							else
								m_pWordSeg[nIndex][k].sWord[nLen-2]=0;
							strcpy(sCurWord,"未##数");
							nPOS=-27904;//'m'*256;Set the POS with 'm'
							i--;
						}
					}
					i--;//Not num, back to previous word
				}
			}
			fValue=0;
			nEndVertex=nSegRoute[nIndex][i+1];//Ending POS changed to latter
		}
		m_pWordSeg[nIndex][k].nHandle=nPOS;//Get the POS of current word
		m_pWordSeg[nIndex][k].dValue=fValue;//(int)(MAX_FREQUENCE*exp(-fValue));//Return the frequency of current word
		m_graphOptimum.SetElement(nStartVertex,nEndVertex,fValue,nPOS,sCurWord);
		//Generate optimum segmentation graph according the segmentation result
		i++;//Skip to next atom
		k++;//Accept next word
	}
	m_pWordSeg[nIndex][k].sWord[0]=0;
	m_pWordSeg[nIndex][k].nHandle=-1;//Set ending
	return true;
}

//DEL bool CSegment::GetSegmentResult(int nIndex,char *sResult)
//DEL {
//DEL 	int i=0;
//DEL 	char sTempBuffer[WORD_MAXLENGTH];
//DEL 	sResult[0]=0;
//DEL 	if(nIndex<0||nIndex>=m_nSegmentCount)
//DEL 		return false;
//DEL 	while(m_WordSeg[nIndex][i].sWord[0]!=0)
//DEL 	{
//DEL 		sprintf(sTempBuffer,"%s/%c%c",m_WordSeg[nIndex][i].sWord,m_WordSeg[nIndex][i].nHandle/256,m_WordSeg[nIndex][i].nHandle%256);
//DEL 		strcat(sResult,sTempBuffer);
//DEL 		strcat(sResult,"  ");
//DEL 		i++;
//DEL 	}
//DEL 	return true;
//DEL }
//Word Segmentation based on optimum segmentation graph

//After unknown word recognition
bool CSegment::OptimumSegmet(int nResultCount)
{
	int **nSegRoute;//The segmentation route
	nSegRoute=new int*[MAX_SEGMENT_NUM];
    int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		nSegRoute[i]=new int[MAX_SENTENCE_LEN/2];
	}
	CNShortPath sp(&m_graphOptimum,nResultCount);
	sp.ShortPath();
	sp.Output((int **)nSegRoute,false,&m_nSegmentCount);
	i=0;
	m_graphSeg.m_segGraph=m_graphOptimum;
	m_graphOptimum.SetEmpty();//Set graph optimum empty
	while(i<m_nSegmentCount)
	{
		GenerateWord(nSegRoute,i);
		//Gernerate word according the Segmentation route
		i++;
	}
	
	//free the memory
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] nSegRoute[i];//free the pointer memory
	}
	delete [] nSegRoute;//free the pointer array
	return true;
}

int CSegment::GetResultCount(PWORD_RESULT pItem)
{
	int nCount=0;
	while(pItem[nCount].sWord[0]!=0)
	{
		nCount+=1;
	}
	return nCount;
}

bool CSegment::GetLastWord(PWORD_RESULT pItem, char *sWordRet)
{
	int nCount=0;
	sWordRet[0]=0;
	while(pItem[nCount].sWord[0]!=0)
	{
		strcpy(sWordRet,pItem[nCount].sWord);
		nCount+=1;
	}
	return !sWordRet[0];
}

bool CSegment::IsYearTime(char *sNum)
{//Judge whether the sNum is a num genearating year
	unsigned int nLen=strlen(sNum);
	char sTemp[3];
	strncpy(sTemp,sNum,2);
	sTemp[2]=0;
	//if(IsAllSingleByte((unsigned char *)sNum)&&(nLen==4||nLen==2&&sNum[0]>'4'))//1992年, 90年
	if(IsAllSingleByte((unsigned char *)sNum) && (nLen==4 || (nLen==2&&sNum[0]>'4')))//1992年, 90年
		return true;
	//if(IsAllNum((unsigned char *)sNum)&&(nLen>=6||nLen==4&&CC_Find("５６７８９",sTemp)))
	if(IsAllNum((unsigned char *)sNum) && (nLen>=6 || (nLen==4&&CC_Find("５６７８９",sTemp))))
		return true;
	if(GetCharCount("零○一二三四五六七八九壹贰叁肆伍陆柒捌玖",sNum)==(int)nLen/2&&nLen>=3)
		return true;
	if(nLen==8&&GetCharCount("千仟零○",sNum)==2)//二仟零二年
		return true;
	if(nLen==2&&GetCharCount("千仟",sNum)==1)
		return true;
	if(nLen==4&&GetCharCount("甲乙丙丁戊己庚辛壬癸",sNum)==1&&GetCharCount("子丑寅卯辰巳午未申酉戌亥",sNum+2)==1)
		return true;
	return false;
}

//CDynamicArray &aWord: the words array
//CDynamicArray &aWordBinaryNet:the net between words
//double dSmoothingPara: the parameter of data smoothing
//CDictionary &DictBinary: the binary dictionary
//CDictionary &DictCore: the Core dictionary
bool CSegment::BiGraphGenerate(CDynamicArray &aWord, CDynamicArray &aBinaryWordNet,double dSmoothingPara,CDictionary &DictBinary,CDictionary &DictCore)
{
	PARRAY_CHAIN pTail,pCur,pNextWords;//Temp buffer
	unsigned int nWordIndex=0,nTwoWordsFreq=0,nCurWordIndex,nNextWordIndex;
	//nWordIndex: the index number of current word
	double dCurFreqency,dValue,dTemp;
	char sTwoWords[WORD_MAXLENGTH];
	m_nWordCount=aWord.GetTail(&pTail);//Get tail element and return the words count
	if(m_npWordPosMapTable)
	{//free buffer
		delete [] m_npWordPosMapTable;
		m_npWordPosMapTable=0;
	}
	if(m_nWordCount>0)//Word count is greater than 0
		m_npWordPosMapTable=new int[m_nWordCount];//Record the  position of possible words
	pCur=aWord.GetHead();
	while(pCur!=NULL)//Set the position map of words
	{
		m_npWordPosMapTable[nWordIndex++]=pCur->row*MAX_SENTENCE_LEN+pCur->col;
		pCur=pCur->next;
	}

	pCur=aWord.GetHead();
	while(pCur!=NULL)//
	{
		if(pCur->nPOS>=0)//It's not an unknown words
			dCurFreqency=pCur->value;
		else//Unknown words
			dCurFreqency=DictCore.GetFrequency(pCur->sWord,2);
		aWord.GetElement(pCur->col,-1,pCur,&pNextWords);//Get next words which begin with pCur->col
		while(pNextWords&&pNextWords->row==pCur->col)//Next words
		{	
			//Current words frequency
			strcpy(sTwoWords,pCur->sWord);
			strcat(sTwoWords,WORD_SEGMENTER);
			strcat(sTwoWords,pNextWords->sWord);
			nTwoWordsFreq=DictBinary.GetFrequency(sTwoWords,3);
			//Two linked Words frequency
			dTemp=(double)1/MAX_FREQUENCE;
			//Smoothing
			dValue=-log(dSmoothingPara*(1+dCurFreqency)/(MAX_FREQUENCE+80000)+(1-dSmoothingPara)*((1-dTemp)*nTwoWordsFreq/(1+dCurFreqency)+dTemp));
			//-log{a*P(Ci-1)+(1-a)P(Ci|Ci-1)} Note 0<a<1
			if(pCur->nPOS<0)//Unknown words: P(Wi|Ci);while known words:1
			    dValue+=pCur->value;

			//Get the position index of current word in the position map table
			nCurWordIndex=BinarySearch(pCur->row*MAX_SENTENCE_LEN+pCur->col,m_npWordPosMapTable,m_nWordCount);
			nNextWordIndex=BinarySearch(pNextWords->row*MAX_SENTENCE_LEN+pNextWords->col,m_npWordPosMapTable,m_nWordCount);
			aBinaryWordNet.SetElement(nCurWordIndex,nNextWordIndex,dValue,pCur->nPOS);
			pNextWords=pNextWords->next;//Get next word
		}
		pCur=pCur->next;
	}
	return true;
}

bool CSegment::BiSegment(char *sSentence, double dSmoothingPara, CDictionary &dictCore, CDictionary &dictBinary, unsigned int nResultCount)
{
	int **nSegRoute;//The segmentation route
	nSegRoute=new int*[MAX_SEGMENT_NUM];
	unsigned int nLen=strlen(sSentence)+10;
    int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		nSegRoute[i]=new int[nLen/2];
		memset(nSegRoute[i],-1,nLen/2*sizeof(int));
	}
 	m_graphSeg.GenerateWordNet(sSentence,dictCore,true);//Generate words array
    CDynamicArray aBiwordsNet;
	BiGraphGenerate(m_graphSeg.m_segGraph,aBiwordsNet,dSmoothingPara,dictBinary,dictCore);
    //Generate the biword link net
    
	CNShortPath sp(&aBiwordsNet,nResultCount);
	sp.ShortPath();
	sp.Output(nSegRoute,false,&m_nSegmentCount);
	

	m_graphOptimum.SetEmpty();//Set graph optimum empty
	i=0;
	while(i<m_nSegmentCount)
	{
		BiPath2UniPath(nSegRoute[i]);
		//Path convert to unipath
		GenerateWord(nSegRoute,i);
		//Gernerate word according the Segmentation route
		i++;
	}

	//free the memory
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] nSegRoute[i];//free the pointer memory
	}
	delete [] nSegRoute;//free the pointer array
	
	return true;
}

bool CSegment::BiPath2UniPath(int *npPath)
{//BiPath convert to unipath
	int i=0,nTemp=-1;
	if(!m_npWordPosMapTable)
		return false;
	while(npPath[i]!=-1&&npPath[i]<m_nWordCount)
	{
		nTemp=m_npWordPosMapTable[npPath[i]];
		npPath[i]=nTemp/MAX_SENTENCE_LEN;
		i++;
	}
	if(nTemp>0)
		npPath[i++]=nTemp%MAX_SENTENCE_LEN;
	npPath[i]=-1;
	return true;	
}

bool CSegment::BiOptimumSegment(unsigned int nResultCount,double dSmoothingPara, CDictionary &dictBinary, CDictionary &dictCore)
{
	int **nSegRoute;//The segmentation route
	nSegRoute=new int*[MAX_SEGMENT_NUM];
    int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		nSegRoute[i]=new int[MAX_SENTENCE_LEN/2];
		memset(nSegRoute[i],-1,MAX_SENTENCE_LEN/2*sizeof(int));
	}
    CDynamicArray aBiwordsNet;
	BiGraphGenerate(m_graphOptimum,aBiwordsNet,dSmoothingPara,dictBinary,dictCore);
    //Generate the biword link net
    
	CNShortPath sp(&aBiwordsNet,nResultCount);
	sp.ShortPath();
	sp.Output((int **)nSegRoute,false,&m_nSegmentCount);
	i=0;
	m_graphSeg.m_segGraph=m_graphOptimum;
	m_graphOptimum.SetEmpty();//Set graph optimum empty

	while(i<m_nSegmentCount)
	{
		BiPath2UniPath(nSegRoute[i]);
		//Path convert to unipath
		GenerateWord(nSegRoute,i);
		//Gernerate word according the Segmentation route
		i++;
	}
	
	//free the memory
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] nSegRoute[i];//free the pointer memory
	}
	delete [] nSegRoute;//free the pointer array
	return true;
}
