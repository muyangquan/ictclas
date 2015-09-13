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
//Result.h: interface for the CResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_)
#define AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Span.h"
#include "Utility.h"
#include "Segment.h"
#include "Dictionary.h"
#include "UnknowWord.h"
#include "Dictionary.h"

#define _ICT_DEBUG 0

class CResult  
{
public:
	struct result_t{
		int    start;          //start position,词语在输入句子中的开始位置
		int    length;         //length,词语的长度
		int    part_of_speech; //词性(参见PKU2973POS()函数中的nHandleSet数组及doc/part_of_speech.txt文件)
		double value;          //The -log(frequency/MAX)
	};
	bool ParagraphProcessingA(const char *sParagraph, result_t *result_arr, int &result_num);
	bool OutputA(PWORD_RESULT pItem, int &curr_pos, int &result_num, result_t *result_arr, 
			bool bFirstWordIgnore=false);

public:
	double m_dSmoothingPara;
	bool Processing(char *sSentence,unsigned int nCount);
	bool ParagraphProcessing(const char *sParagraph,char *sResult);
	bool FileProcessing(char *sFilename,char *sResultFile);
	PWORD_RESULT *m_pResult;
	//The buffer which store the segment and POS result
	//and They stored order by its possibility
	ELEMENT_TYPE m_dResultPossibility[MAX_SEGMENT_NUM];
	int m_nResultCount;
	bool Output(PWORD_RESULT pItem,char *sResult,bool bFirstWordIgnore=false);
	CResult(const char * strDataPath="./");  // strDataPath is the directory where "Data" is located. Default is the current directory.
	virtual ~CResult();
	int m_nOperateType;//0:Only Segment;1: First Tag; 2:Second Type
	int m_nOutputFormat;//0:PKU criterion;1:973 criterion; 2: XML criterion
private:
	CSegment m_Seg;//Seg class
	CDictionary m_dictCore,m_dictBigram;//Core dictionary,bigram dictionary
	CSpan m_POSTagger;//POS tagger
	CUnknowWord m_uPerson,m_uTransPerson,m_uPlace;//Person recognition
protected:
	bool ChineseNameSplit(char *sPersonName,char *sSurname, char *sSurname2,char *sGivenName,CDictionary &personDict);
	bool PKU2973POS(int nHandle,char *sPOS973);
	bool Adjust(PWORD_RESULT pItem,PWORD_RESULT pItemRet);
	ELEMENT_TYPE ComputePossibility(PWORD_RESULT pItem);
	bool Sort();
};

#endif // !defined(AFX_RESULT_H__DEB31BCA_0410_4D5E_97EA_78F9B16B8938__INCLUDED_)
