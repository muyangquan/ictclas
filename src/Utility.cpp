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
 * Filename: Utility.c
 * Abstract:
 *           Utility functions for Chinese Language Processing
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-8
 *
 * Notes:
 *                
 ****************************************************************************/
//#include "stdafx.h"
#include "Utility.h"
#include <stdio.h>
#include <string.h>
/*********************************************************************
 *
 *  Func Name  : GB2312_Generate
 *
 *  Description:  Generate the GB2312 List file
 *              
 *
 *  Parameters : sFilename: the file name for the output GB2312 List
 *    
 *  Returns    : bool
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-8
 *********************************************************************/
bool GB2312_Generate(char *sFileName)
{
   FILE *fp;
   unsigned int i,j;

   if((fp=fopen(sFileName,"wt"))==NULL)
	   return false;//fail while opening the file
   for(i=161;i<255;i++)
	   for(j=161;j<255;j++)
		   fprintf(fp,"%c%c,%d,%d\n",i,j,i,j);
   fclose(fp);
   return true;
}
/*********************************************************************
 *
 *  Func Name  : CC_Generate
 *
 *  Description:  Generate the Chinese Char List file
 *              
 *
 *  Parameters : sFilename: the file name for the output CC List
 *    
 *  Returns    : bool
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-8
 *********************************************************************/
bool CC_Generate(char *sFileName)
{
   FILE *fp;
   unsigned int i,j;
   if((fp=fopen(sFileName,"wt"))==NULL)
	   return false;//fail while opening the file
   for(i=176;i<255;i++)
	   for(j=161;j<255;j++)
		   fprintf(fp,"%c%c,%d,%d\n",i,j,i,j);
   fclose(fp);
   return true;
}
/*********************************************************************
 *
 *  Func Name  : CC_Find
 *
 *  Description: Find a Chinese sub-string in the Chinese String 
 *              
 *
 *  Parameters :  string:Null-terminated string to search
 *
 * 			      strCharSet:Null-terminated string to search for
 *
 *  Returns    : char *
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-8
 *********************************************************************/
char *CC_Find(const char *string, const char *strCharSet)
{
   //char *cp=strstr(string,strCharSet);
   char *cp=const_cast<char*>(strstr(string,strCharSet));
   if(cp!=NULL&&(cp-string)%2==1)
   {
	  return NULL;
   }
   return cp;
}
/*********************************************************************
 *
 *  Func Name  : charType
 *
 *  Description: Judge the type of sChar or (sChar,sChar+1)
 *              
 *
 *  Parameters : sFilename: the file name for the output CC List
 *    
 *  Returns    : int : the type of char
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-8
 *********************************************************************/
int charType(unsigned char *sChar)
{
  if(*sChar<128)
  {
	 //if(strchr("\042!,.?()[]{}+=",(int)*sChar))
	 //fixed @ 2015/09/19 增加了单引号
	 if(strchr("\042\x27!,.?()[]{}+=",(int)*sChar))
		 return CT_DELIMITER;
	 return CT_SINGLE;
  }
  else if(*sChar==162)
	  return CT_INDEX;
  else if(*sChar==163&&*(sChar+1)>175&&*(sChar+1)<186)
	  return CT_NUM;
  //else if(*sChar==163&&(*(sChar+1)>=193&&*(sChar+1)<=218||*(sChar+1)>=225&&*(sChar+1)<=250))
  else if(*sChar==163 && ((*(sChar+1)>=193&&*(sChar+1)<=218) || (*(sChar+1)>=225&&*(sChar+1)<=250)))
	  return CT_LETTER;
  else if(*sChar==161||*sChar==163)
	  return CT_DELIMITER;
  else if(*sChar>=176&&*sChar<=247)
      return CT_CHINESE;
  else
      return CT_OTHER;
}
/*********************************************************************
 *
 *  Func Name  : GetCCPrefix
 *
 *  Description: Get the max Prefix string made up of Chinese Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-8
 *********************************************************************/
unsigned int  GetCCPrefix(unsigned char *sSentence)
{
   unsigned int nLen=strlen((const char *)sSentence),nCurPos=0;
   while(nCurPos<nLen&&sSentence[nCurPos]>175&&sSentence[nCurPos]<248)
   {
      nCurPos+=2;//Get next Chinese Char
   }
   return nCurPos;
}
/*********************************************************************
 *
 *  Func Name  : IsAllSingleByte
 *
 *  Description: Judge the string is all made up of Single Byte Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllChinese(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&sString[i]<248&&sString[i]>175)
	{
		i+=2;
	}
	if(i<nLen)
		return false;
	return true;
}
/*********************************************************************
 *
 *  Func Name  : IsAllNonChinese
 *
 *  Description: Judge the string is all made up of Single Byte Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllNonChinese(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen)
	{
		if(sString[i]<248&&sString[i]>175)
			return false;
		if(sString[i]>128)
			i+=2;
		else
			i+=1;
	}
    return true;
}
/*********************************************************************
 *
 *  Func Name  : IsAllSingleByte
 *
 *  Description: Judge the string is all made up of Single Byte Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllSingleByte(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen&&sString[i]<128)
	{
		i++;
	}
	if(i<nLen)
		return false;
	return true;
}
/*********************************************************************
 *
 *  Func Name  : IsAllNum
 *
 *  Description: Judge the string is all made up of Num Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllNum(unsigned char *sString)
{

	unsigned int nLen=strlen((const char *)sString),i=0;
	char sChar[3];
	sChar[2]=0;
	if(i<nLen)//Get prefix such as + -
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(!strstr("±+—-＋",sChar))
		{
			i=0;
		}
	}
	while(i<nLen-1&&sString[i]==163&&sString[i+1]>175&&sString[i+1]<186)
	{
		i+=2;
	}
	if(i<nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(CC_Find("∶·．／",sChar)||sChar[0]=='.'||sChar[0]=='/')
		{//98．1％
			while(i<nLen-1&&sString[i]==163&&sString[i+1]>175&&sString[i+1]<186)
			{
				i+=2;
			}
		}	
		else
		{
			i-=strlen(sChar);
		}
	}

	if(i>=nLen)
		return true;
	while(i<nLen&&sString[i]>'0'-1&&sString[i]<'9'+1)
	{//single byte number char
		i+=1;
	}
	if(i<nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(CC_Find("∶·．／",sChar)||sChar[0]=='.'||sChar[0]=='/')
		{//98．1％
			while(i<nLen&&sString[i]>'0'-1&&sString[i]<'9'+1)
			{
				i+=1;
			}
		}	
		else
		{
			i-=strlen(sChar);
		}
	}
	if(i<nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(!CC_Find("百千万亿佰仟％‰",sChar)&&sChar[0]!='%')
			i-=strlen(sChar);
	}
	if(i>=nLen)
		return true;
	return false;
}
/*********************************************************************
 *
 *  Func Name  : IsAllIndex
 *
 *  Description: Judge the string is all made up of Index Num Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllIndex(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&sString[i]==162)
	{
		i+=2;
	}
	if(i>=nLen)
		return true;
	 //while(i<nLen&&(sString[i]>'A'-1&&sString[i]<'Z'+1)||(sString[i]>'a'-1&&sString[i]<'z'+1))
	 while(i<nLen && ((sString[i]>'A'-1&&sString[i]<'Z'+1)||(sString[i]>'a'-1&&sString[i]<'z'+1)))
	 {//single byte number char
		i+=1;
	 }

	if(i<nLen)
		return false;
	return true;

}
/*********************************************************************
 *
 *  Func Name  : IsAllLetter
 *
 *  Description: Judge the string is all made up of Letter Char
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllLetter(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&sString[i]==163&&((sString[i+1]>=193&&sString[i+1]<=218)||(sString[i+1]>=225&&sString[i+1]<=250)))
	{
		i+=2;
	}
	if(i<nLen)
		return false;

	return true;
}
/*********************************************************************
 *
 *  Func Name  : IsAllDelimiter
 *
 *  Description: Judge the string is all made up of Delimiter
 *              
 *
 *  Parameters : sSentence: the original sentence which includes Chinese or Non-Chinese char
 *    
 *  Returns    : the end of the sub-sentence
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-24
 *********************************************************************/
bool IsAllDelimiter(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&(sString[i]==161||sString[i]==163))
	{
		i+=2;
	}
	if(i<nLen)
		return false;
	return true;
}
/*********************************************************************
 *
 *  Func Name  : BinarySearch
 *
 *  Description: Lookup the index of nVal in the table nTable which length is nTableLen
 *
 *  Parameters : nPOS: the POS value
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-25
 *********************************************************************/
int BinarySearch(int nVal, int *nTable,int nTableLen)
{
	int nStart=0,nEnd=nTableLen-1,nMid=(nStart+nEnd)/2;
	while(nStart<=nEnd)//Binary search
	{
       if(nTable[nMid]==nVal)
	   {
			return nMid;//find it
	   }
	   else if(nTable[nMid]<nVal)
	   {
		   nStart=nMid+1;
	   }
	   else
       {
		   nEnd=nMid-1;
	   }
	   nMid=(nStart+nEnd)/2;
	}
	return -1;//Can not find it;
}
/*********************************************************************
 *
 *  Func Name  : IsForeign
 *
 *  Description: Decide whether the word is not a Non-fereign word
 *
 *  Parameters : sWord: the word
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-26
 *********************************************************************/
bool IsForeign(char *sWord)
{
  int nForeignCount=GetForeignCharCount(sWord),nCharCount=strlen(sWord);
  if(nCharCount>2||nForeignCount>=1*nCharCount/2)
	  return true;
  return false;
}
/*********************************************************************
 *
 *  Func Name  : IsAllForeign
 *
 *  Description: Decide whether the word is not a Non-fereign word
 *
 *  Parameters : sWord: the word
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-3-25
 *********************************************************************/
bool IsAllForeign(char *sWord)
{
  unsigned int nForeignCount=(unsigned int)GetForeignCharCount(sWord);
  if(2*nForeignCount==strlen(sWord))
	  return true;
  return false;
}
/*********************************************************************
 *
 *  Func Name  : IsForeign
 *
 *  Description: Decide whether the word is Chinese Num word
 *
 *  Parameters : sWord: the word
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-26
 *********************************************************************/
bool IsAllChineseNum(char *sWord)
{//百分之五点六的人早上八点十八分起床
  unsigned int  k; 
  char tchar[3];
  char ChineseNum[]="零○一二两三四五六七八九十廿百千万亿壹贰叁肆伍陆柒捌玖拾佰仟∶·．／点";//
  char sPrefix[]="几数第上成";
  for(k = 0; k < strlen(sWord); k+=2)
  {
     strncpy(tchar,sWord+k,2) ;
     tchar[2]='\0';
	 if(strncmp(sWord+k,"分之",4)==0)//百分之五
	 {
		k+=2;
		continue;
	 }

	 if(!CC_Find(ChineseNum, tchar)&&!(k==0&&CC_Find(sPrefix, tchar)))
		 return false;
  }
  return true;
}
/*********************************************************************
 *
 *  Func Name  : GetForeignCharCount
 *
 *  Description: 
 *
 *  Parameters : sWord: the word
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-4-4
 *              2.Modify  2002-5-21
 *********************************************************************/
int GetForeignCharCount(char *sWord)
{
  unsigned int nForeignCount,nCount;
  nForeignCount=GetCharCount(TRANS_ENGLISH,sWord);//English char counnts
  nCount=GetCharCount(TRANS_JAPANESE,sWord);//Japan char counnts
  if(nForeignCount<=nCount)
	nForeignCount=nCount;
  nCount=GetCharCount(TRANS_RUSSIAN,sWord);//Russian char counnts
  if(nForeignCount<=nCount)
	nForeignCount=nCount;
  return nForeignCount;
}
/*********************************************************************
 *
 *  Func Name  : GetCharCount
 *
 *  Description: Get the count of char which is in sWord and in sCharSet
 *
 *  Parameters : sWord: the word
 * 
 *  Returns    : COUNT
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-5-21
 *********************************************************************/
int GetCharCount(const char *sCharSet,char *sWord)
{
  unsigned int  k=0; 
  char tchar[3];
  int nCount=0;
  tchar[2]=0;
  while(k < strlen(sWord))
  {
     tchar[0]=sWord[k];
 	 tchar[1]=0;
	 if(sWord[k]<0)
	 {
		 tchar[1]=sWord[k+1];
		 k+=1;
	 }
	 k+=1;
	 if((tchar[0]<0&&CC_Find(sCharSet, tchar))||strchr(sCharSet,tchar[0]))
          nCount++;
  }
  return nCount;
}
/*********************************************************************
 *
 *  Func Name  : GetForeignCharCount
 *
 *  Description: Return the foreign type 
 *
 *  Parameters : sWord: the word
 *
 *  Returns    : the index value
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-4-4
 *              2.Modify  2002-5-21
 *********************************************************************/
int GetForeignType(char *sWord)
{
  unsigned int nForeignCount,nCount,nType=TT_ENGLISH;
  nForeignCount=GetCharCount(TRANS_ENGLISH,sWord);//English char counnts
  nCount=GetCharCount(TRANS_RUSSIAN,sWord);//Russian char counnts
  if(nForeignCount<nCount)
  {
	  nForeignCount=nCount;
	  nType=TT_RUSSIAN;
  }
  nCount=GetCharCount(TRANS_JAPANESE,sWord);//Japan char counnts
  if(nForeignCount<nCount)
  {
	  nForeignCount=nCount;
	  nType=TT_JAPANESE;
  }
  return nType;
}
bool PostfixSplit(char *sWord, char *sWordRet, char *sPostfix)
{
	char sSinglePostfix[]=POSTFIX_SINGLE;
	char sMultiPostfix[][9]=POSTFIX_MUTIPLE;
	unsigned int nPostfixLen=0,nWordLen=strlen(sWord);
	int i=0;

	while(sMultiPostfix[i][0]!=0&&strncmp(sWord+nWordLen-strlen(sMultiPostfix[i]),sMultiPostfix[i],strlen(sMultiPostfix[i]))!=0)
	{//Try to get the postfix of an address
		i++;
	}
	strcpy(sPostfix,sMultiPostfix[i]);
	nPostfixLen=strlen(sMultiPostfix[i]);//Get the length of place postfix

	if(nPostfixLen==0)
	{
		sPostfix[2]=0;
		strncpy(sPostfix,sWord+nWordLen-2,2);
		if(CC_Find(sSinglePostfix,sPostfix))
			nPostfixLen=2;
	}
	
	strncpy(sWordRet,sWord,nWordLen-nPostfixLen);
	sWordRet[nWordLen-nPostfixLen]=0;//Get the place name which have erasing the postfix
	sPostfix[nPostfixLen]=0;
    return true;
}
