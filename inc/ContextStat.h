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
 * Filename: ContextStat.h
 * Abstract:
 *           
 *			 interface for the CContextStat class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-24
 *
 * Notes:
 *                
 * 
 ****************************************************************************/

#if !defined(AFX_CONTEXTSTAT_H__DA515FDC_F8F9_48F6_B25D_D2B91011528B__INCLUDED_)
#define AFX_CONTEXTSTAT_H__DA515FDC_F8F9_48F6_B25D_D2B91011528B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct tagContext{
	int nKey;//The key word
	int **aContextArray;//The context array
	int *aTagFreq;//The total number a tag appears
	int nTotalFreq;//The total number of all the tags
	struct tagContext *next;//The chain pointer to next Context
};
typedef struct tagContext MYCONTEXT,*PMYCONTEXT;
class CContextStat  
{
public:
	bool SetTableLen(int nTableLe);
	int GetFrequency(int nKey,int nSymbol);
	double GetContextPossibility(int nKey,int nPrev,int nCur);
	bool Load(char *sFilename);
	bool Save(char *sFilename);
	bool Add(int nKey,int nPrevSymbol,int nCurrentSymbol,int nFrequency);
	bool SetSymbol(int *nSymbol);
	CContextStat();
	virtual ~CContextStat();
private:
	int m_nTableLen;
	int *m_pSymbolTable;
	PMYCONTEXT m_pContext;
	int m_nCategory;
protected:
	bool GetItem(int nKey,PMYCONTEXT *pItemRet);
};

#endif // !defined(AFX_CONTEXTSTAT_H__DA515FDC_F8F9_48F6_B25D_D2B91011528B__INCLUDED_)
