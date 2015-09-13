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
// Result.cpp: implementation of the CResult class.
//
//////////////////////////////////////////////////////////////////////
#include "Result.h"

#define DICT_FILE        "data/coreDict.dct"
#define LEXICAL_FILE     "data/lexical.ctx"
#define BIGRAM_FILE      "data/BigramDict.dct"
#define PERSON_FILE      "data/nr"
#define PLACE_FILE       "data/ns"
#define TRANSPERSON_FILE "data/tr"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static char * concatenate(const char * str1, const char * str2)
{
	static char str0[500];
	//char * p=str0;
	////while (*p++ = *str1++);
	//while ((*p++) = (*str1++));
	//p--;
	////while (*p++ = *str2++);
	//while ((*p++) = (*str2++));
	strcpy(str0,str1);
	strcat(str0,str2);
	return str0;
}

CResult::CResult(const char * strDataPath)
{
	//malloc buffer
	m_pResult=new PWORD_RESULT[MAX_SEGMENT_NUM];
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		m_pResult[i]=new WORD_RESULT[MAX_WORDS];
	}

	m_dictCore.Load(concatenate(strDataPath,DICT_FILE));
	m_POSTagger.LoadContext(concatenate(strDataPath,LEXICAL_FILE));
/*
	m_dictCore.Load("data\\Dict.dct");
	m_POSTagger.LoadContext("data\\trainTest.ctx");
*/
/*
	
	m_dictCore.AddItem("十分",'d'*256,+500);
	m_dictCore.AddItem("十分",'m'*256,-500);
	m_dictCore.AddItem("我国",'n'*256,-2000);
	m_dictCore.AddItem("我国",'r'*256,+2000);
	m_dictCore.AddItem("千年",'t'*256,200);

    m_dictCore.Optimum();
	m_dictCore.Save("data\\coreDictOptimum.dct");
*/


	m_POSTagger.SetTagType();

	m_uPerson.Configure(concatenate(strDataPath,PERSON_FILE),TT_PERSON);
	//Set the person recognition configure
	m_uPlace.Configure(concatenate(strDataPath,PLACE_FILE),TT_PLACE);
	//Set the place recognition configure
	m_uTransPerson.Configure(concatenate(strDataPath,TRANSPERSON_FILE),TT_TRANS_PERSON);
	//Set the transliteration person recognition configure
	
	//0: 只切分不标注; 
	//1: 上位标注;
	//2: 为下位标注（更详细的标注类型）
	m_nOperateType=2;//0:Only Segment;1: First Tag; 2:Second Type
	//制输出结果的表示方法（以及词性标注集的转换），包括北大标准，973标准及XML表示方式。
	m_nOutputFormat=0;//0:PKU criterion;1:973 criterion; 2: XML criterion

	m_dSmoothingPara=0.1;//Smoothing parameter
	m_dictBigram.Load(concatenate(strDataPath,BIGRAM_FILE));

	//fixed @ 2015/09/12
	m_nResultCount = 1;
	for(int i = 0; i < MAX_SEGMENT_NUM; ++i){
		m_dResultPossibility[i] = .0;
	}
}

CResult::~CResult()
{
	//free buffer
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] m_pResult[i];
	}
	delete m_pResult;

}

bool CResult::Output(PWORD_RESULT pItem, char *sResult,bool bFirstWordIgnore)
{
	int i=0;
	char sTempBuffer[WORD_MAXLENGTH],sPOS[3];
	sPOS[2]=0;
	sResult[0]=0;
	if(bFirstWordIgnore)//Ignore first valid
		i=1;
	//依次读切词数据链，对于每个词条，将其词性标记转换为字符串表示方式
	while(pItem[i].sWord[0]!=0&&pItem[i].nHandle!=CT_SENTENCE_END)//Not sentence ending flag
	{
		//Get the POS string
		if(m_nOutputFormat!=0)//Not PKU format
			PKU2973POS(pItem[i].nHandle,sPOS);
		else//PKU format
		{
			sPOS[0]=pItem[i].nHandle/256;
			sPOS[1]=pItem[i].nHandle%256;
		}
		sPOS[m_nOperateType]=0;//Set the sPOS with operate type
		
		if(m_nOutputFormat==0)//PKU format
		{
			sprintf(sTempBuffer,"%s",pItem[i].sWord);
		    strcat(sResult,sTempBuffer);
			if(sPOS[0]!=0)//need POS 
			{
				sprintf(sTempBuffer,"/%s",sPOS);
				strcat(sResult,sTempBuffer);
			}
		    strcat(sResult,"  ");
		}
		else if(m_nOutputFormat==1)//973 format
		{
			sprintf(sTempBuffer,"%s\\",pItem[i].sWord);
		    strcat(sResult,sTempBuffer);
			if(sPOS[0]!=0)//need POS 
			{
				sprintf(sTempBuffer,"[%s]",sPOS);
				strcat(sResult,sTempBuffer);
			}
		}
		else if(m_nOutputFormat==2)//XML format
		{
			if(sPOS[0]!=0)//POS
			{
				sprintf(sTempBuffer,"<any type=\042%s\042>",sPOS);
				strcat(sResult,sTempBuffer);
			}
			sprintf(sTempBuffer,"<src>%s</src>",pItem[i].sWord);
			strcat(sResult,sTempBuffer);
			if(sPOS[0]!=0)
			{
				strcat(sResult,"</any>");
			}
		}
		i++;
	}
	return true;
}

bool CResult::OutputA(PWORD_RESULT pItem, int &curr_pos, int &result_num, 
		result_t *result_arr, bool bFirstWordIgnore)
{
	int i=0;
	//char sTempBuffer[WORD_MAXLENGTH],sPOS[3];
	//char sPOS[3];
	//sPOS[2]=0;
	//sResult[0]=0;
	if(bFirstWordIgnore)//Ignore first valid
		i=1;
	//依次读切词数据链，对于每个词条，将其词性标记转换为字符串表示方式
	while(pItem[i].sWord[0]!=0&&pItem[i].nHandle!=CT_SENTENCE_END)//Not sentence ending flag
	{
		////Get the POS string
		//if(m_nOutputFormat!=0)//Not PKU format
		//	PKU2973POS(pItem[i].nHandle,sPOS);
		//else//PKU format
		//{
		//	sPOS[0]=pItem[i].nHandle/256;
		//	sPOS[1]=pItem[i].nHandle%256;
		//}
		//sPOS[m_nOperateType]=0;//Set the sPOS with operate type
		
		//if(m_nOutputFormat==0)//PKU format
		//{
			//sprintf(sTempBuffer,"%s",pItem[i].sWord);
		    //strcat(sResult,sTempBuffer);
			//if(sPOS[0]!=0)//need POS 
			//{
			//	sprintf(sTempBuffer,"/%s",sPOS);
			//	strcat(sResult,sTempBuffer);
			//}
		    //strcat(sResult,"  ");
			result_arr[result_num].start               = curr_pos;
			curr_pos += (result_arr[result_num].length = strlen(pItem[i].sWord));
			result_arr[result_num].part_of_speech      = pItem[i].nHandle;
			result_arr[result_num++].value             = pItem[i].dValue;
		//}
		//else if(m_nOutputFormat==1)//973 format
		//{
		//	sprintf(sTempBuffer,"%s\\",pItem[i].sWord);
		//    strcat(sResult,sTempBuffer);
		//	if(sPOS[0]!=0)//need POS 
		//	{
		//		sprintf(sTempBuffer,"[%s]",sPOS);
		//		strcat(sResult,sTempBuffer);
		//	}
		//}
		//else if(m_nOutputFormat==2)//XML format
		//{
		//	if(sPOS[0]!=0)//POS
		//	{
		//		sprintf(sTempBuffer,"<any type=\042%s\042>",sPOS);
		//		strcat(sResult,sTempBuffer);
		//	}
		//	sprintf(sTempBuffer,"<src>%s</src>",pItem[i].sWord);
		//	strcat(sResult,sTempBuffer);
		//	if(sPOS[0]!=0)
		//	{
		//		strcat(sResult,"</any>");
		//	}
		//}
		i++;
	}
	return true;
}

//bool CResult::Processing(char *sSentence,unsigned int nCount)
//{
//	int nIndex;
//#if _ICT_DEBUG
//	char *sSegment;
//	sSegment=new char[MAX_SENTENCE_LEN*2];
//#endif
//	//Unigram segment
//	//m_Seg.Segment(sSentence,m_dictCore,nCount);
//	//Bigram segment
//	m_Seg.BiSegment(sSentence, m_dSmoothingPara,m_dictCore,m_dictBigram,nCount);
//	
//	m_nResultCount=m_Seg.m_nSegmentCount;
//	//Record the number of result
//	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
//	{
//#if _ICT_DEBUG
//			m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
//			Output(m_Seg.m_pWordSeg[nIndex],sSegment);
//			printf("POS Tag%d:%s\n",nIndex+1,sSegment);
//#endif
//		m_uPerson.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
//		m_uTransPerson.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
//		m_uPlace.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
//	}		
//	//m_uPerson.Recognition(m_Seg.m_WordSeg[0],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
//	//Person Recognition
//#if _ICT_DEBUG
//	printf("After person recognition.\n");
//#endif
//	//Unigram
//	//m_Seg.OptimumSegmet(nCount);
//	//Bigram
//	m_Seg.BiOptimumSegment(nCount,m_dSmoothingPara,m_dictBigram,m_dictCore);
//
//	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
//	{
//		m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
//#if _ICT_DEBUG
//		Output(m_Seg.m_pWordSeg[nIndex],sSegment);
//		printf("POS Tag%d:%s\n",nIndex+1,sSegment);
//#endif
//	}
//#if _ICT_DEBUG
//	printf("After Sorting.\n");
//#endif
//	Sort();//Sort the ending 
//#if _ICT_DEBUG
//	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
//	{
//		Output(m_pResult[nIndex],sSegment);
//		printf("POS Tag%d(P=Exp(%f)):%s\n",nIndex+1,m_dResultPossibility[nIndex],sSegment);
//	}
//	delete [] sSegment;
//#endif
//	return true;
//}

bool CResult::Processing(char *sSentence,unsigned int nCount)
{
	int nIndex;
#if _ICT_DEBUG
	char *sSegment;
	sSegment=new char[MAX_SENTENCE_LEN*2];
#endif
	//Unigram segment
	//m_Seg.Segment(sSentence,m_dictCore,nCount);
	//Bigram segment
	//进行二叉分词
	//将传入的句子使用切分器m_Seg进行二元切分
	//生成的结果会存储在m_Seg.m_pWordSeg中 
	//结果个数存在      m_Seg.m_nSegmentCount中
	m_Seg.BiSegment(sSentence, m_dSmoothingPara,m_dictCore,m_dictBigram,nCount);
	
	m_nResultCount=m_Seg.m_nSegmentCount;
	//Record the number of result
	//然后对于每个生成的结果（一般为1，最多10个），分别使用三个未登陆词识别器进行识别，
	//结果存在m_Seg.m_graphOptimum中
	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
	{
#if _ICT_DEBUG
			m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
			Output(m_Seg.m_pWordSeg[nIndex],sSegment);
			printf("POS Tag%d:%s\n",nIndex+1,sSegment);
#endif
		m_uPerson.Recognition(     m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
		m_uTransPerson.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
		m_uPlace.Recognition(      m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
	}		
	//m_uPerson.Recognition(m_Seg.m_WordSeg[0],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
	//Person Recognition
#if _ICT_DEBUG
	printf("After person recognition.\n");
#endif
	//Unigram
	//m_Seg.OptimumSegmet(nCount);
	//Bigram
	//调用m_Seg的二元优化切词
	m_Seg.BiOptimumSegment(nCount,m_dSmoothingPara,m_dictBigram,m_dictCore);

	//对于生成的每个结果（存在m_Seg.m_pWordSeg中），调用词性标注器m_POSTagger进行词性标注。
	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
	{
		//在此处进行词性标注
		m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
#if _ICT_DEBUG
		Output(m_Seg.m_pWordSeg[nIndex],sSegment);
		printf("POS Tag%d:%s\n",nIndex+1,sSegment);
#endif
	}
#if _ICT_DEBUG
	printf("After Sorting.\n");
#endif
	//最后将生成的结果进行排序，最终的结果会按概率高低存储在m_pResult中
	Sort();//Sort the ending 
#if _ICT_DEBUG
	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
	{
		Output(m_pResult[nIndex],sSegment);
		printf("POS Tag%d(P=Exp(%f)):%s\n",nIndex+1,m_dResultPossibility[nIndex],sSegment);
	}
	delete [] sSegment;
#endif
	return true;
}

//Sort the segmentation and POS result according its possibility
bool CResult::Sort()
{
	ELEMENT_TYPE dPossibility[MAX_SEGMENT_NUM],dTemp;
	int nIndex[MAX_SEGMENT_NUM],nTemp;//Index

	memset(dPossibility,0,sizeof(dPossibility));
	//Init the possibility
	
    int i;
	for(i=0;i<m_Seg.m_nSegmentCount;i++)
	{//Computing the possibility
		dPossibility[i]=ComputePossibility(m_Seg.m_pWordSeg[i]);
		nIndex[i]=i;//Record the index
	}
	
	//Sort with Bubble sort algorithm
	for(i=0;i<m_Seg.m_nSegmentCount;i++)
		for(int j=i+1;j<m_Seg.m_nSegmentCount;j++)
		{
			if(dPossibility[i]<dPossibility[j])
			{//Swap the possition and value
				nTemp=nIndex[i];
				dTemp=dPossibility[i];
				nIndex[i]=nIndex[j];
				dPossibility[i]=dPossibility[j];
				nIndex[j]=nTemp;
				dPossibility[j]=dTemp;
			}
		}
	
	for(i=0;i<m_Seg.m_nSegmentCount;i++)
	{//Adjust the segmentation and POS result and store them in the final result array
	 //Store them according their possibility ascendly
		Adjust(m_Seg.m_pWordSeg[nIndex[i]],m_pResult[i]);	
		m_dResultPossibility[i]=dPossibility[i];	
	}
	return true;
}
//Compute the possibility of current segmentation and POS result
ELEMENT_TYPE CResult::ComputePossibility(PWORD_RESULT pItem)
{
	int i=0;
	ELEMENT_TYPE dResultPossibility=0;
	while(pItem[i].sWord[0]!=0)
	{
		dResultPossibility+=pItem[i].dValue;
		//Compute the possibility of logP(Wi|Ti)
		if(pItem[i+1].sWord[0]!=0)//Not the last one
		{//Compute the possibility of logP(Ti|Ti-1)
			dResultPossibility+=log((double)(m_POSTagger.m_context.GetContextPossibility(0,pItem[i].nHandle,pItem[i+1].nHandle)+1));
			dResultPossibility-=log((double)(m_POSTagger.m_context.GetFrequency(0,pItem[i].nHandle)+1));
		}
		i++;
	}
	return dResultPossibility;
}

//Adjust the result with some rules
bool CResult::Adjust(PWORD_RESULT pItem,PWORD_RESULT pItemRet)
{
	int i=0,j=0;
	unsigned int nLen;
	char sSurName[10],sSurName2[10],sGivenName[10];
	bool bProcessed=false;//Have been processed
	while(pItem[i].sWord[0]!=0)
	{
		//fixed @ 2015/09/12
		pItemRet[j].dValue = pItem[i].dValue;

		nLen=strlen(pItem[i].sWord);
		bProcessed=false;
		
		//Rule1: adjust person name
		if(pItem[i].nHandle==28274&&ChineseNameSplit(pItem[i].sWord,sSurName,sSurName2,sGivenName,m_uPerson.m_dict)&&strcmp(pItem[i].sWord,"叶利钦")!=0)//'nr'
		{//Divide name into surname and given name
			
			if(sSurName[0])
			{
				strcpy(pItemRet[j].sWord,sSurName);
				pItemRet[j++].nHandle=28274;
			}
			if(sSurName2[0])
			{
				strcpy(pItemRet[j].sWord,sSurName2);
				pItemRet[j++].nHandle=28274;
			}
			if(sGivenName[0])
			{
				strcpy(pItemRet[j].sWord,sGivenName);
				pItemRet[j++].nHandle=28274;
			}
			bProcessed=true;
		}
		//Rule2 for overlap words ABB 一段段、一片片
		else if(pItem[i].nHandle==27904&&strlen(pItem[i+1].sWord)==2&&strcmp(pItem[i+1].sWord,pItem[i+2].sWord)==0)
		{//(pItem[i+1].nHandle/256=='q'||pItem[i+1].nHandle/256=='a')&&
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
			strcat(pItemRet[j].sWord,pItem[i+2].sWord);
			pItemRet[j].nHandle=27904;
			j+=1;
			i+=2;
			bProcessed=true;
		}
		//Rule3 for overlap words AA
		else if(nLen==2&&strcmp(pItem[i].sWord,pItem[i+1].sWord)==0)
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
		     //24832=='a'*256
			pItemRet[j].nHandle=24832;//a
			if(pItem[i].nHandle/256=='v'||pItem[i+1].nHandle/256=='v')//30208='v'8256
			{
				pItemRet[j].nHandle=30208;
			}
			if(pItem[i].nHandle/256=='n'||pItem[i+1].nHandle/256=='n')//30208='v'8256
			{
				pItemRet[j].nHandle='n'*256;
			}			
			i+=1;
			if(strlen(pItem[i+1].sWord)==2)
			{//AAB:洗/洗/脸、蒙蒙亮
				if((pItemRet[j].nHandle==30208&&pItem[i+1].nHandle/256=='n')||
				   (pItemRet[j].nHandle==24832&&pItem[i+1].nHandle/256=='a')
				   )
				{
					strcat(pItemRet[j].sWord,pItem[i+1].sWord);
					i+=1;
				}
			}
			j+=1;
			bProcessed=true;
		}

		//Rule 4: AAB 洗/洗澡
		else if(nLen==2&&strncmp(pItem[i].sWord,pItem[i+1].sWord,2)==0&&strlen(pItem[i+1].sWord)==4&&(pItem[i].nHandle/256=='v'||pItem[i].nHandle==24832))//v,a
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
		     //24832=='a'*256
			pItemRet[j].nHandle=24832;//'a'
			if(pItem[i].nHandle/256=='v'||pItem[i+1].nHandle/256=='v')//30208='v'8256
			{
				pItemRet[j].nHandle=30208;
			}

			i+=1;
			j+=1;
			bProcessed=true;
		}
		else if(pItem[i].nHandle/256=='u'&&pItem[i].nHandle%256)//uj,ud,uv,uz,ul,ug->u
			pItem[i].nHandle='u'*256;
		else if(nLen==2&&strncmp(pItem[i].sWord,pItem[i+1].sWord,2)==0&&strlen(pItem[i+1].sWord)==4&&strncmp(pItem[i+1].sWord+2,pItem[i+2].sWord,2)==0)
		{//AABB 朴朴素素 枝枝叶叶
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				strcat(pItemRet[j].sWord,pItem[i+2].sWord);
				pItemRet[j].nHandle=pItem[i+1].nHandle;
				i+=2;
				j+=1;
				bProcessed=true;
		}
		else if(pItem[i].nHandle==28275)//PostFix
		{
			if(m_uPlace.m_dict.IsExist(pItem[i+1].sWord,4))
			{
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				pItemRet[j].nHandle=28275;
				i+=1;
				j+=1;
				bProcessed=true;
			}
			else if(strlen(pItem[i+1].sWord)==2&&CC_Find("队",pItem[i+1].sWord))
			{
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				pItemRet[j].nHandle=28276;
				i+=1;
				j+=1;
				bProcessed=true;
			}
			else if(strlen(pItem[i+1].sWord)==2&&CC_Find("语文字杯",pItem[i+1].sWord))
			{
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				pItemRet[j].nHandle=28282;
				i+=1;
				j+=1;
				bProcessed=true;
			}
			else if(strlen(pItem[i+1].sWord)==2&&CC_Find("裔",pItem[i+1].sWord))
			{
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				pItemRet[j].nHandle=28160;
				i+=1;
				j+=1;
				bProcessed=true;
			}
		}
		else if(pItem[i].nHandle==30208||pItem[i].nHandle==28160)//v
		{
			if(strlen(pItem[i+1].sWord)==2&&CC_Find("员",pItem[i+1].sWord))
			{
				strcpy(pItemRet[j].sWord,pItem[i].sWord);
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				pItemRet[j].nHandle=28160;
				i+=1;
				j+=1;
				bProcessed=true;
			}
		}
		else if(pItem[i].nHandle==28280)
		{//www/nx ./w sina/nx; ＥＩＭ/nx  -６０１/m 
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			pItemRet[j].nHandle=28280;
			while(pItem[i+1].nHandle==28280||strstr(".．",pItem[i+1].sWord)||(pItem[i+1].nHandle==27904&&IsAllNum((unsigned char *)pItem[i+1].sWord)))
			{
				strcat(pItemRet[j].sWord,pItem[i+1].sWord);
				i+=1;
			}
			j+=1;
			bProcessed=true;
		}

		if(!bProcessed)
		{//If not processed,that's mean: not need to adjust;
		 //just copy to the final result
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			pItemRet[j++].nHandle=pItem[i].nHandle;
		}
		i++;
	}
	pItemRet[j].sWord[0]=0;//Set ending
	return true;
}

////Paragraph Segment and POS Tagging
//bool CResult::ParagraphProcessing(char *sParagraph,char *sResult)
//{
//	char *sSentence,sChar[3];
//	char *sSentenceResult;
//	unsigned int nLen=strlen(sParagraph)+13;
//	sSentence=new char[nLen];//malloc buffer
//	sSentenceResult=new char[nLen*3];//malloc buffer
//	sSentence[0]=0;
//	//unsigned int nPosIndex=0,nParagraphLen=strlen(sParagraph),nSentenceIndex=0;
//	unsigned int nPosIndex=0,nParagraphLen=strlen(sParagraph);
//	sChar[2]=0;
//	sResult[0]=0;//Init the result
//	bool bFirstIgnore=true;
//	strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
//	while(nPosIndex<nParagraphLen)
//	{//Find a whole sentence which separated by ! . \n \r
//		sChar[0]=sParagraph[nPosIndex];//Get a char
//		sChar[1]=0;
//		if(sParagraph[nPosIndex]<0)
//		{//double byte char
//			nPosIndex+=1;
//			sChar[1]=sParagraph[nPosIndex];
//		}
//		nPosIndex+=1;
///*
//#define  SEPERATOR_C_SENTENCE "。！？：；…"
//#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
//#define  SEPERATOR_E_SENTENCE "!?:;"
//#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
//#define  SEPERATOR_LINK "\n\r 　"
//*/
//		if(CC_Find(SEPERATOR_C_SENTENCE,sChar)||CC_Find(SEPERATOR_C_SUB_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar)||strstr(SEPERATOR_E_SUB_SENTENCE,sChar)||strstr(SEPERATOR_LINK,sChar))
//		{//Reach end of a sentence.Get a whole sentence
//			if(!strstr(SEPERATOR_LINK,sChar))//Not link seperator
//			{
//				strcat(sSentence,sChar);
//			}
//			if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
//			{
//				if(!strstr(SEPERATOR_C_SUB_SENTENCE,sChar)&&!strstr(SEPERATOR_E_SUB_SENTENCE,sChar))
//					strcat(sSentence,SENTENCE_END);//Add sentence ending flag
//
//				Processing(sSentence,1);//Processing and output the result of current sentence.
//				Output(m_pResult[0],sSentenceResult,bFirstIgnore);//Output to the imediate result
//				//bFirstIgnore=true;
//				strcat(sResult,sSentenceResult);//Store in the result buffer
//			}
//			if(strstr(SEPERATOR_LINK,sChar))//Link the result with the SEPERATOR_LINK
//			{
//				strcat(sResult,sChar);
//				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
//
//				//sSentence[0]=0;//New sentence, and begin new segmentation
//				//bFirstIgnore=false;
//			}
//			else if(strstr(SEPERATOR_C_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar))
//			{
//				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
//				//sSentence[0]=0;//New sentence, and begin new segmentation
//				//bFirstIgnore=false;
//			}
//			else
//			{
//				strcpy(sSentence,sChar);//reset current sentence, and add the previous end at begin position
//			}
//		}
//		else //Other chars and store in the sentence buffer
//			strcat(sSentence,sChar);
//	}
//	if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
//	{
//		strcat(sSentence,SENTENCE_END);//Add sentence ending flag
//		Processing(sSentence,1);//Processing and output the result of current sentence.
//		Output(m_pResult[0],sSentenceResult,bFirstIgnore);//Output to the imediate result
//		strcat(sResult,sSentenceResult);//Store in the result buffer
//	}
//	delete []  sSentence;//FREE sentence buffer 	
//	delete []  sSentenceResult;//free buffer
//	return true;
//}

//Paragraph Segment and POS Tagging
//处理一段文字
//它需要将一段文字根据标点符号切成若干子句
//子句首尾会根据需要添加上句子开始结束标记后再送入Processing进行处理
//使用了一些技巧来减少重复操作，加快速度。
bool CResult::ParagraphProcessing(const char *sParagraph,char *sResult)
{
	char *sSentence,sChar[3];
	char *sSentenceResult;
	unsigned int nLen=strlen(sParagraph)+13;
	sSentence=new char[nLen];//malloc buffer
	sSentenceResult=new char[nLen*3];//malloc buffer
	sSentence[0]=0;
	unsigned int nPosIndex=0,nParagraphLen=strlen(sParagraph);
	sChar[2]=0;
	sResult[0]=0;//Init the result
	bool bFirstIgnore=true;
	strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
	//依次读入字符（可能是全角可能是半角，这个部分处理得不好，应该写一个函数来操作，否则代码变得很长）。
	while(nPosIndex<nParagraphLen)
	{//Find a whole sentence which separated by ! . \n \r
		sChar[0]=sParagraph[nPosIndex];//Get a char
		sChar[1]=0;
		if(sParagraph[nPosIndex]<0)
		{//double byte char
			nPosIndex+=1;
			sChar[1]=sParagraph[nPosIndex];
		}
		nPosIndex+=1;

/*
//全角的句子分割符
#define  SEPERATOR_C_SENTENCE "。！？：；…"
//全角的子句分割符
#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
//半角的句子分割符
#define  SEPERATOR_E_SENTENCE "!?:;"
//半角的子句分割符
#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
//连接符
#define  SEPERATOR_LINK "\n\r 　"
*/
		//但在进行原子切分之前，首先要进行断句处理。
		//所谓断句，就是根据分隔符、回车换行符等语句的分隔标志，把源字符串分隔成多个稍微简单一点的短句，
		//再进行分词处理，最后再把各个分词结果合起来，形成最终的分词结果。
		if(CC_Find(SEPERATOR_C_SENTENCE,sChar)||CC_Find(SEPERATOR_C_SUB_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar)||strstr(SEPERATOR_E_SUB_SENTENCE,sChar)||strstr(SEPERATOR_LINK,sChar))
		{//Reach end of a sentence.Get a whole sentence
			//分成短句之后，即可进行原子分词，所谓原子,是指该短句中不可分割的最小语素单位。
			//一个汉字、短句前后的开始结束标识字段、全角标点符号、连在一起的数字字母单字节字符等。
			if(!strstr(SEPERATOR_LINK,sChar))//Not link seperator
			{
				strcat(sSentence,sChar);
			}
			//对于已经生成的待处理句子串，如果非空也不是只有开始标记: 送入Processing进行处理
			if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
			{
				if(!strstr(SEPERATOR_C_SUB_SENTENCE,sChar)&&!strstr(SEPERATOR_E_SUB_SENTENCE,sChar))
					strcat(sSentence,SENTENCE_END);//Add sentence ending flag

				//只生成一个结果，并将生成的切分结果输出并追加到结果串中
				//注意如果待处理句子串的最后一个字符不是子句结束符则要追加一个句子结束符
				Processing(sSentence,1);//Processing and output the result of current sentence.
				//将PWORD_RESULT类型数据中的切词结构转成字符串表示形式
				Output(m_pResult[0],sSentenceResult,bFirstIgnore);//Output to the imediate result
				//bFirstIgnore=true;
				strcat(sResult,sSentenceResult);//Store in the result buffer
			}
			if(strstr(SEPERATOR_LINK,sChar))//Link the result with the SEPERATOR_LINK
			{
#if _ICT_DEBUG
				printf("SEPERATOR_LINK: ->%s<-\n", sChar);
#endif
				strcat(sResult,sChar);
				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag

				//sSentence[0]=0;//New sentence, and begin new segmentation
				//bFirstIgnore=false;
			}
			else if(strstr(SEPERATOR_C_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar))
			{
				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
				//sSentence[0]=0;//New sentence, and begin new segmentation
				//bFirstIgnore=false;
			}
			else
			{
				//处理结束之后，要重置待处理的句子
				strcpy(sSentence,sChar);//reset current sentence, and add the previous end at begin position
			}
		}
		//如果不是全角的句子（子句）结束符、不是半角的句子（子句）结束符也不是是段落结束符，
		//则追加到待处理的句子串中再取下一个字符，否则进一步处理。
		else //Other chars and store in the sentence buffer
			strcat(sSentence,sChar);
	}//while 
	if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
	{
		//如果最后一个字符是换行符或者是句子结束符则重置新句子为开始标记
		//否则简单的把原来的最后一个字符拷入新句子
		strcat(sSentence,SENTENCE_END);//Add sentence ending flag
		//待用来读字符的索引超出段落长度之后，不要忘记把未处理完的句子送入Processing进行切分
		Processing(sSentence,1);//Processing and output the result of current sentence.
		Output(m_pResult[0],sSentenceResult,bFirstIgnore);//Output to the imediate result
		strcat(sResult,sSentenceResult);//Store in the result buffer
	}
	delete []  sSentence;//FREE sentence buffer 	
	delete []  sSentenceResult;//free buffer
	return true;
}

bool CResult::ParagraphProcessingA(const char *sParagraph, result_t *result_arr, int &result_num)
{
	char *sSentence,sChar[3];
	//char *sSentenceResult;
	unsigned int nLen=strlen(sParagraph)+13;
	sSentence=new char[nLen];//malloc buffer
	//sSentenceResult=new char[nLen*3];//malloc buffer
	sSentence[0]=0;
	unsigned int nPosIndex=0,nParagraphLen=strlen(sParagraph);
	sChar[2]=0;
	//sResult[0]=0;//Init the result
	bool bFirstIgnore=true;
	strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag

	int curr_pos = 0; //当前的位置
	result_num   = 0; //结果个数

	while(nPosIndex<nParagraphLen)
	{//Find a whole sentence which separated by ! . \n \r
		sChar[0]=sParagraph[nPosIndex];//Get a char
		sChar[1]=0;
		if(sParagraph[nPosIndex]<0)
		{//double byte char
			nPosIndex+=1;
			sChar[1]=sParagraph[nPosIndex];
		}
		nPosIndex+=1;

		if(CC_Find(SEPERATOR_C_SENTENCE,sChar)||CC_Find(SEPERATOR_C_SUB_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar)||strstr(SEPERATOR_E_SUB_SENTENCE,sChar)||strstr(SEPERATOR_LINK,sChar))
		{//Reach end of a sentence.Get a whole sentence
			if(!strstr(SEPERATOR_LINK,sChar))//Not link seperator
			{
				strcat(sSentence,sChar);
			}
			//对于已经生成的待处理句子串，如果非空也不是只有开始标记: 送入Processing进行处理
			if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
			{
				if(!strstr(SEPERATOR_C_SUB_SENTENCE,sChar)&&!strstr(SEPERATOR_E_SUB_SENTENCE,sChar))
					strcat(sSentence,SENTENCE_END);//Add sentence ending flag

				Processing(sSentence,1);//Processing and output the result of current sentence.
				OutputA(m_pResult[0], curr_pos, result_num, result_arr, bFirstIgnore);
			}
			if(strstr(SEPERATOR_LINK,sChar))//Link the result with the SEPERATOR_LINK
			{
				curr_pos += strlen(sChar);
				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
			}
			else if(strstr(SEPERATOR_C_SENTENCE,sChar)||strstr(SEPERATOR_E_SENTENCE,sChar))
			{
				strcpy(sSentence,SENTENCE_BEGIN);//Add a sentence begin flag
			}
			else
			{
				//处理结束之后，要重置待处理的句子
				strcpy(sSentence,sChar);//reset current sentence, and add the previous end at begin position
			}
		}
		//如果不是全角的句子（子句）结束符、不是半角的句子（子句）结束符也不是是段落结束符，
		//则追加到待处理的句子串中再取下一个字符，否则进一步处理。
		else //Other chars and store in the sentence buffer
			strcat(sSentence,sChar);
	}//while 
	if(sSentence[0]!=0&&strcmp(sSentence,SENTENCE_BEGIN)!=0)
	{
		//如果最后一个字符是换行符或者是句子结束符则重置新句子为开始标记
		//否则简单的把原来的最后一个字符拷入新句子
		strcat(sSentence,SENTENCE_END);//Add sentence ending flag
		//待用来读字符的索引超出段落长度之后，不要忘记把未处理完的句子送入Processing进行切分
		Processing(sSentence,1);//Processing and output the result of current sentence.
		OutputA(m_pResult[0], curr_pos, result_num, result_arr, bFirstIgnore);
	}
	delete []  sSentence;//FREE sentence buffer 	
	//delete []  sSentenceResult;//free buffer
	return true;
}

bool CResult::FileProcessing(char *sSourceFile,char *sResultFile)
{
	FILE *fpSource,*fpResult;//The file pointer of read and write
	char *sParagraph,*sParagraphResult;
	//int nLineIndex=1;
	sParagraph=new char[4*1024];
	sParagraphResult=new char[8*1024];
    if((fpSource=fopen(sSourceFile,"rt"))==NULL)
		return false;//Cannot open the source file to read
    if((fpResult=fopen(sResultFile,"wt"))==NULL) 
		return false;//Cannot open the result  file to write
	if(m_nOutputFormat==2)//XML format
		fprintf(fpResult,"<?xml version=\042 1.0\042 encoding=\042gb2312\042?><result>");
	while(!feof(fpSource))
	{
		if(fgets(sParagraph,4*1024,fpSource)==0)//Get a paragrah
			continue;
#ifndef unix
		TRACE("%d\n",nLineIndex++);
#endif
		ParagraphProcessing(sParagraph,sParagraphResult);
		fprintf(fpResult,"%s",sParagraphResult);
	}
	delete [] sParagraph;
	delete [] sParagraphResult;
	fclose(fpSource);	
	if(m_nOutputFormat==2)//XML format
		fprintf(fpResult,"</result>");
	fclose(fpResult);
	return true;
}

bool CResult::PKU2973POS(int nHandle, char *sPOS973)
{
	int nHandleSet[46]={24832,24932,24935,24942,25088,25344,25600,25703,25856,26112,26368,26624,26880,27136,27392,27648,27904,28160,28263,28274,28275,28276,28280,28282,28416,28672,28928,29184,29440,29696,29799,29952,30052,30055,30058,30060,30070,30074,30208,30308,30311,30318,30464,30720,30976,31232};
						//   "a", "ad","ag","an","b", "c", "d", "dg","e", "f","g", "h", "i", "j", "k", "l", "m", "n", "ng","nr","ns","nt","nx","nz","o", "p", "q", "r", "s", "t", "tg","u", "ud","ug","uj","ul","uv","uz","v", "vd","vg","vn","w", "x", "y", "z"
	char sPOSRelated[46][3]={"a", "ad","ga","an","f", "c", "d", "d", "e","nd","g", "h", "i", "j", "k", "l", "m", "n", "gn","nh","ns","ni","ws", "nz","o", "p", "q", "r", "nl","nt","gt","u", "ud","ug","uj","ul","uv","uz","v", "vd","gv","vn","w", "x", "u", "a"};
/* 
 "Bg","gf",
 "Rg","gr",
 "Mg","gm",
 "Yg","u",
 "Ug","u",
 "Qg","q",
*/

	int nIndex=BinarySearch(nHandle,nHandleSet,46);
	if(nIndex==-1)
		strcpy(sPOS973,"@");
	else
		strcpy(sPOS973,sPOSRelated[nIndex]);
	return true;
}

bool CResult::ChineseNameSplit(char *sPersonName, char *sSurname, char *sSurname2, char *sGivenName, CDictionary &personDict)
{
	int nSurNameLen=4,nLen=strlen(sPersonName),nFreq,i=0,nCharType,nFreqGiven;
	char sTemp[3];
	if(nLen<3||nLen>8)//Not a traditional Chinese person name
		return false;
	while(i<nLen)//No Including non-CHinese char
	{
		nCharType=charType((unsigned char*)sPersonName+i);
		if(nCharType!=CT_CHINESE&&nCharType!=CT_OTHER)
			return false;
		i+=2;
	}
	sSurname2[0]=0;//init 
	strncpy(sSurname,sPersonName,nSurNameLen);	
	sSurname[nSurNameLen]=0;
	if(!personDict.IsExist(sSurname,1))
	{
		nSurNameLen=2;
		sSurname[nSurNameLen]=0;
		if(!personDict.IsExist(sSurname,1))
		{
			nSurNameLen=0;
			sSurname[nSurNameLen]=0;
		}
	}
	strcpy(sGivenName,sPersonName+nSurNameLen);
	if(nLen>6)
	{
		strncpy(sTemp,sPersonName+nSurNameLen,2);
		sTemp[2]=0;//Get the second possible surname
		if(personDict.IsExist(sTemp,1))
		{//Hongkong women's name: Surname+surname+given name
			strcpy(sSurname2,sTemp);
			strcpy(sGivenName,sPersonName+nSurNameLen+2);
		}
	}
	nFreq=personDict.GetFrequency(sSurname,1);
	strncpy(sTemp,sGivenName,2);
	sTemp[2]=0;
	nFreqGiven=personDict.GetFrequency(sTemp,2);
	char sz_zhang[3] = "张";
	char sz_hua[3]   = "华";
	//if(nSurNameLen!=4&&((nSurNameLen==0&&nLen>4)||strlen(sGivenName)>4||(GetForeignCharCount(sPersonName)>=3&&nFreq<personDict.GetFrequency("张",1)/40&&nFreqGiven<personDict.GetFrequency("华",2)/20)||(nFreq<10&&GetForeignCharCount(sGivenName)==(nLen-nSurNameLen)/2)))
	if(nSurNameLen!=4&&((nSurNameLen==0&&nLen>4)||strlen(sGivenName)>4||(GetForeignCharCount(sPersonName)>=3&&nFreq<personDict.GetFrequency(sz_zhang,1)/40&&nFreqGiven<personDict.GetFrequency(sz_hua,2)/20)||(nFreq<10&&GetForeignCharCount(sGivenName)==(nLen-nSurNameLen)/2)))
		return false;
	if(nLen==4&&m_uPerson.IsGivenName(sPersonName))
	{//Single Surname+given name
		return false;
	}
	return true;
}

