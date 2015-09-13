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
 *           N-Shortest Path Problem for graph in word segement
 *           
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *
 ****************************************************************************/
// NShortPath.h: interface for the CNShortPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NSHORTPATH_H__817D57F2_F3D8_40C8_A57E_20570862BCB3__INCLUDED_)
#define AFX_NSHORTPATH_H__817D57F2_F3D8_40C8_A57E_20570862BCB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DynamicArray.h"
#include "Queue.h"
#include <stdio.h>
class CNShortPath  
{
public:
	int m_nResultCount;
	int Output(int **nResult,bool bBest,int *npCount);
	int ShortPath();
	CNShortPath(CDynamicArray *aCost,unsigned int nValueKind=1);
	virtual ~CNShortPath();
private:
	void GetPaths(unsigned int nNode,unsigned int nIndex,int **nResult=0,bool bBest=false);
	CDynamicArray *m_apCost;
    unsigned int m_nValueKind;//The number of value kinds
    unsigned int m_nVertex;//The number of vertex in the graph
	CQueue   **m_pParent;//The 2-dimension array for the nodes
	ELEMENT_TYPE **m_pWeight;//The weight of node
};

#endif // !defined(AFX_NSHORTPATH_H__817D57F2_F3D8_40C8_A57E_20570862BCB3__INCLUDED_)
