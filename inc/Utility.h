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
 * Filename: Utility.h
 * Abstract:
 *           Utility functions for Chinese Language Processing
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-8
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
#if !defined(AFX_CHINESE_UTILITY_H__B6D7EA03_7BCD_46AD_B38C_D8033ACD5813__INCLUDED_)
#define AFX_CHINESE_UTILITY_H__B6D7EA03_7BCD_46AD_B38C_D8033ACD5813__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  CT_SENTENCE_BEGIN 1//Sentence begin 
#define  CT_SENTENCE_END   4//Sentence ending
#define  CT_SINGLE  5//SINGLE byte
#define  CT_DELIMITER CT_SINGLE+1//delimiter
#define  CT_CHINESE   CT_SINGLE+2//Chinese Char
#define  CT_LETTER    CT_SINGLE+3//HanYu Pinyin
#define  CT_NUM       CT_SINGLE+4//HanYu Pinyin
#define  CT_INDEX     CT_SINGLE+5//HanYu Pinyin
#define  CT_OTHER     CT_SINGLE+12//Other
#define POSTFIX_SINGLE "坝邦堡杯城池村单岛道堤店洞渡队法峰府冈港阁宫沟国海号河湖环集江奖礁角街井郡坑口矿里岭楼路门盟庙弄牌派坡铺旗桥区渠泉人山省市水寺塔台滩坛堂厅亭屯湾文屋溪峡县线乡巷型洋窑营屿语园苑院闸寨站镇州庄族陂庵町"
#define POSTFIX_MUTIPLE {"半岛","草原","城市","大堤","大公国","大桥","地区","帝国","渡槽","港口","高速公路","高原","公路","公园","共和国","谷地","广场","国道","海峡","胡同","机场","集镇","教区","街道","口岸","码头","煤矿","牧场","农场","盆地","平原","丘陵","群岛","沙漠","沙洲","山脉","山丘","水库","隧道","特区","铁路","新村","雪峰","盐场","盐湖","渔场","直辖市","自治区","自治县","自治州",""}
                          
#define  TRANS_ENGLISH   "·—阿埃艾爱安昂敖奥澳笆芭巴白拜班邦保堡鲍北贝本比毕彼别波玻博勃伯泊卜布才采仓查差柴彻川茨慈次达大戴代丹旦但当道德得的登迪狄蒂帝丁东杜敦多额俄厄鄂恩尔伐法范菲芬费佛夫福弗甫噶盖干冈哥戈革葛格各根古瓜哈海罕翰汗汉豪合河赫亨侯呼胡华霍基吉及加贾坚简杰金京久居君喀卡凯坎康考柯科可克肯库奎拉喇莱来兰郎朗劳勒雷累楞黎理李里莉丽历利立力连廉良列烈林隆卢虏鲁路伦仑罗洛玛马买麦迈曼茅茂梅门蒙盟米蜜密敏明摩莫墨默姆木穆那娜纳乃奈南内尼年涅宁纽努诺欧帕潘畔庞培佩彭皮平泼普其契恰强乔切钦沁泉让热荣肉儒瑞若萨塞赛桑瑟森莎沙山善绍舍圣施诗石什史士守斯司丝苏素索塔泰坦汤唐陶特提汀图土吐托陀瓦万王旺威韦维魏温文翁沃乌吾武伍西锡希喜夏相香歇谢辛新牙雅亚彦尧叶依伊衣宜义因音英雍尤于约宰泽增詹珍治中仲朱诸卓孜祖佐伽娅尕腓滕济嘉津赖莲琳律略慕妮聂裴浦奇齐琴茹珊卫欣逊札哲智兹芙汶迦珀琪梵斐胥黛"
#define  TRANS_RUSSIAN   "·阿安奥巴比彼波布察茨大德得丁杜尔法夫伏甫盖格哈基加坚捷金卡科可克库拉莱兰勒雷里历利连列卢鲁罗洛马梅蒙米姆娜涅宁诺帕泼普奇齐乔切日萨色山申什斯索塔坦特托娃维文乌西希谢亚耶叶依伊以扎佐柴达登蒂戈果海赫华霍吉季津柯理琳玛曼穆纳尼契钦丘桑沙舍泰图瓦万雅卓兹"
#define  TRANS_JAPANESE  "安奥八白百邦保北倍本比滨博步部彩菜仓昌长朝池赤川船淳次村大代岛稻道德地典渡尔繁饭风福冈高工宫古谷关广桂贵好浩和合河黑横恒宏后户荒绘吉纪佳加见健江介金今进井静敬靖久酒菊俊康可克口梨理里礼栗丽利立凉良林玲铃柳隆鹿麻玛美萌弥敏木纳南男内鸟宁朋片平崎齐千前浅桥琴青清庆秋丘曲泉仁忍日荣若三森纱杉山善上伸神圣石实矢世市室水顺司松泰桃藤天田土万望尾未文武五舞西细夏宪相小孝新星行雄秀雅亚岩杨洋阳遥野也叶一伊衣逸义益樱永由有佑宇羽郁渊元垣原远月悦早造则泽增扎宅章昭沼真政枝知之植智治中忠仲竹助椎子佐阪坂堀荻菅薰浜濑鸠筱"
//Translation type
#define  TT_ENGLISH  0
#define  TT_RUSSIAN  1
#define  TT_JAPANESE  2
//Seperator type
#define  SEPERATOR_C_SENTENCE "。！？：；…"
#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
#define  SEPERATOR_E_SENTENCE "!?:;"
#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
#define  SEPERATOR_LINK "\n\r 　"
//Sentence begin and ending string
#define SENTENCE_BEGIN "始##始"
#define SENTENCE_END "末##末"
//Seperator between two words
#define WORD_SEGMENTER "@" 

bool GB2312_Generate(char *sFileName);
//Generate the GB2312 List file
bool CC_Generate(char *sFileName);
//Generate the Chinese Char List file
char *CC_Find(const char *string, const char *strCharSet);
//Find a Chinese sub-string in the Chinese String 
int charType(unsigned char *sChar);
//Judge the type of sChar or (sChar,sChar+1)
unsigned int GetCCPrefix(unsigned char *sSentence);
//Get the max Prefix string made up of Chinese Char
bool IsAllChinese(unsigned char *sString);
//Judge the string is all made up of Chinese Char
bool IsAllNonChinese(unsigned char *sString);
//Judge the string is all made up of non-Chinese Char
bool IsAllSingleByte(unsigned char *sString);
//Judge the string is all made up of Single Byte Char
bool IsAllNum(unsigned char *sString);
//Judge the string is all made up of Num Char
bool IsAllIndex(unsigned char *sString);
//Judge the string is all made up of Index Num Char
bool IsAllLetter(unsigned char *sString);
//Judge the string is all made up of Letter Char
bool IsAllDelimiter(unsigned char *sString);
//Judge the string is all made up of Delimiter
int BinarySearch(int nVal, int *nTable,int nTableLen);
//Binary search a value in a table which len is nTableLen
bool IsForeign(char *sWord);
//sWord maybe is a foreign translation
bool IsAllChineseNum(char *sWord);
//Decide whether the word is Chinese Num word
bool IsAllForeign(char *sWord);
//Decide whether the word is all  foreign translation
int GetForeignCharCount(char *sWord);
//Decide whether the word is all  non-foreign translation
int GetCharCount(const char *sCharSet,char *sWord);
//Get the count of char which is in sWord and in sCharSet
int GetForeignType(char *sWord);
//Return the foreign type 
bool PostfixSplit(char *sWord, char *sWordRet, char *sPostfix);
//Get the postfix
//bool IsSingleByteDelimiter(char cByteChar);
//Judge whether it's a num

#endif // !defined(AFX_CHINESE_UTILITY_H__B6D7EA03_7BCD_46AD_B38C_D8033ACD5813__INCLUDED_)
