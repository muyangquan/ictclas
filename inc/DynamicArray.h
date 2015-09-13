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
 *           Dynamic array, and the array is generally great and sparse.
 *           Dynamic Array Definition 
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// DynamicArray.h: interface for the CDynamicArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNAMICARRAY_H__C47E8C64_17A2_467F_8094_1DFDCC39A943__INCLUDED_)
#define AFX_DYNAMICARRAY_H__C47E8C64_17A2_467F_8094_1DFDCC39A943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MIN_PROBLEM 1
#if MIN_PROBLEM==1//The shortest path
	#define INFINITE_VALUE 10000.00//infinite value
#else//The longest path
	#define INFINITE_VALUE 0.00//infinite value
#endif
#define ELEMENT_TYPE double//the type of element

//#define ROW_FIRST 0//Row first in the array store

struct tagArrayChain{
	unsigned int col,row;//row and column
    ELEMENT_TYPE value;//The value of the array
	int nPOS;
	int nWordLen;
	char *sWord;
	//The possible POS of the word related to the segmentation graph
    struct tagArrayChain *next;
};
typedef struct tagArrayChain ARRAY_CHAIN,*PARRAY_CHAIN;

class CDynamicArray  
{
public:
	bool SetRowFirst(bool RowFirst=true);
	unsigned int GetTail(PARRAY_CHAIN *pTailRet);
	//Get the tail Element buffer and return the count of elements
	PARRAY_CHAIN GetHead();
	//Get the head Element
	bool GetElement(int nRow, int nCol,ELEMENT_TYPE  *pRetValue,int *pRetPOS=0,char *sRetWord=0);
	void SetEmpty();
	CDynamicArray(bool bRowFirst=false);
	virtual ~CDynamicArray();
	const CDynamicArray & operator =(const CDynamicArray &);
	bool operator ==(const CDynamicArray &array);

	ELEMENT_TYPE GetElement( int nRow, int nCol,PARRAY_CHAIN pStart=0,PARRAY_CHAIN *pRet=0);
	int SetElement(unsigned int nRow, unsigned int nCol, ELEMENT_TYPE fValue,int nPOS,char *sWord=0);
	unsigned int m_nCol,m_nRow;//The row and col of the array
	bool m_bRowFirst;
private:
    PARRAY_CHAIN m_pHead;//The head pointer of array chain
};

#endif // !defined(AFX_DYNAMICARRAY_H__C47E8C64_17A2_467F_8094_1DFDCC39A943__INCLUDED_)
