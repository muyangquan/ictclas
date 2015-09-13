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
 *           Queue 
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUE_H__382C7319_66D8_4041_ABA2_EE25B9D15D9C__INCLUDED_)
#define AFX_QUEUE_H__382C7319_66D8_4041_ABA2_EE25B9D15D9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DynamicArray.h"

struct tagQueueElem{
    unsigned int nParent;//the weight
	unsigned int nIndex;//number of index in the parent node
	ELEMENT_TYPE eWeight;//the weight of last path
    struct tagQueueElem *next;
};
typedef struct tagQueueElem QUEUE_ELEMENT,*PQUEUE_ELEMENT;

class CQueue  
{
public:
	bool IsSingle();
	bool IsEmpty(bool bBrowsed=false);
	int Push(unsigned int nValue=0,//The value for parent node
			 unsigned int nIndex=0,//number of index in the parent node
             ELEMENT_TYPE eWeight=0//the weight of last path 
			 );
	int Pop( unsigned int *npValue,//The value for node
			 unsigned int *npIndex,//number of index in the parent node
		     ELEMENT_TYPE *epWeight=0,//the weight of last path 
			 bool  bModify=true,//false: not modify the data
			 bool bFirstGet=true//first get data,just for browse 
		   );
	
	CQueue();
	virtual ~CQueue();
private:
	PQUEUE_ELEMENT m_pHead;//The chain sort according the weight of shortest path
	PQUEUE_ELEMENT m_pLastAccess;//The node last accessed
};

#endif // !defined(AFX_QUEUE_H__382C7319_66D8_4041_ABA2_EE25B9D15D9C__INCLUDED_)
