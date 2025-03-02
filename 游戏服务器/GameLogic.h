#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once
#include "afxtempl.h"
//////////////////////////////////////////////////////////////////////////	

//扑克类型
#define CT_INVALID					    	0								//错误类型
#define CT_SINGLE					    	1								//单牌类型
#define CT_DOUBLE			    			2								//只有一对
#define CT_DOUBLE_X2			    		3								//两对牌型
#define CT_THREE_HEAD				     	4								//头道三张
#define CT_THREE_KING				     	5								//头道三张
#define CT_THREE				     		6								//三张牌型
#define CT_FIVE_LINE	    				7								//普通顺子
#define CT_FIVE_COLOR				    	8								//同花
#define CT_THREE_TWO			     		9								//葫芦牌型
#define CT_FOUR_SAME				    	10								//四同
#define CT_FIVE_COLOR_LINE		    		11								//同花顺牌
#define CT_FIVE_SAME				    	12								//五同
#define CT_FOUR_KING				    	13								//四王
#define CT_FIVE_KING				    	14								//五王

//特殊类型
#define CT_EX_INVALID				    	0								//非特殊牌
#define CT_EX_COLOR_X3			    		20								//三同花
#define CT_EX_HALF_XIAO			    		21								//半小 ***
#define CT_EX_HALF_DA			    		22								//半大 ***
#define CT_EX_LINE_X3				    	23								//三顺子
#define CT_EX_LIUDUIBAN				    	24								//六对半
#define CT_EX_LEAST_6				    	25								//6起 ***
#define CT_EX_LEAST_7				    	26								//7起 ***
#define CT_EX_DUI_5_THREE			    	27								//五对三条
#define CT_EX_SITAOSANTIAO			       	28								//四套三条
#define CT_EX_ALL_BLACK			 	    	29								//全黑
#define CT_EX_ALL_RED			 	    	30								//全红
#define CT_EX_ONE_COLOR			 	    	31								//凑一色
#define CT_EX_ALL_XIAO				    	32								//全小
#define CT_EX_ALL_DA                        33                              //全大
#define CT_EX_LEAST_9				    	34								//9起
#define CT_EX_THREE_TDX2				    35								//凤凰三点头 ***
#define CT_EX_ALL_RED_B1                    36                              //全红一点黑 ***
#define CT_EX_ALL_BLACK_R1                  37                              //全红一点黑 ***
#define CT_EX_LEAST_10						38                              //10起 ***
#define CT_EX_SEVEN_SAME					39                              //7同
#define CT_EX_EIGHT_SAME					40                              //8同
#define CT_EX_NINE_SAME 					41                              //9同
#define CT_EX_SANFENGTIANXIA		    	42								//三分天下(三炸弹)
#define CT_EX_SANTONGHUASHUN				43						    	//三同花顺
#define CT_EX_SHIERHUANGZU			    	44						    	//十二皇族
#define CT_EX_YITIAOLONG			    	45								//一条龙
#define CT_EX_YITIAOLONG_EX			    	46								//一条花龙
#define CT_EX_ZHIZUNQINGLONG		     	47								//至尊清龙

//数值掩码
#define	LOGIC_MASK_COLOR		    		0xF0						 	//花色掩码
#define	LOGIC_MASK_VALUE			    	0x0F							//数值掩码

#define	WEIGHT_TYPE			    			44							
						
//分析结构
struct  tagAnalyseCard 
{
	//分析数据	
	BYTE							cbCardCnt;
	BYTE							cbMinLogicValue;
	BYTE							cbMaxLogicValue;
	BYTE							cbMaxCardValue;
	BYTE							cbColorKind;		//非王颜色数量 1-4	
	BYTE							cbColorCount[ 5 ];	//颜色数目 方片-草花-红桃-黑桃-王		
	BYTE 							cbBlockCount[ 8 ];	//同牌数目 单-对-三同-炸弹-五同~8同		
	/**
	 *扑克列表 [8] 单-对-三同-炸弹-五同~8同	
	 *扑克列表 [MAX_COUNT] 牌值
	**/
	BYTE							cbCardData[ 8 ][ MAX_COUNT ];	
	BYTE							cbTempCardData[ MAX_COUNT ];		//扑克列表
	/**
	 *	扑克列表[5]	方片-梅花-红桃-黑桃-总计
	 *	扑克列表[MAX_COUNT]	A-2-3~K
	**/
	BYTE							cbValueCnt[ 5 ][ MAX_COUNT ];		
};

//排列类型
enum enSortCardType
{
	enDescend,								//降序类型 
	enAscend,								//升序类型
	enColor									//花色类型
};

//////////////////////////////////////////////////////////////////////////	

//游戏逻辑类
class CGameLogic
{
	//变量定义
public:
	static const BYTE				m_bCardListData[ FULL_COUNT ];				//扑克数据
	ITableFrame						*m_pITableFrame;									//框架接口
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();
	//混乱扑克
	void RandCardList( BYTE cbRandBuffer[ ], BYTE cbCardBuffer[ ], BYTE cbBufferCount );

	//类型函数
public:
	//获取类型
	void AnalysebCardData( BYTE bCardData[ ], BYTE bCardCount, tagAnalyseCard & AnalyseCard );
	//获取类型
	BYTE GetCardSpecialType( BYTE bCardData[ ], BYTE bCardCount, BYTE bType );
	//获取类型
	BYTE GetCardType( BYTE bCardData[ ], BYTE bCardCount, tagAnalyseCard & AnalyseCard );
	//获取权重
	SCORE GetCardWeight(BYTE bCardData[], BYTE bCardCount, bool bColor);
	//获取权重
	SCORE GetLineWeight( tagAnalyseCard AnalyseCard, BYTE bType, bool bColor );
	//获取权重
	SCORE GetUnLineWeight(tagAnalyseCard AnalyseCard, BYTE bType, bool bColor);
	//获取权重
	SCORE GetColorWeight(tagAnalyseCard AnalyseCard, BYTE bType, bool bColor);
	//获取类型
	BYTE GetTypeByWeight( SCORE lWeight ){ return ( BYTE ) ( lWeight >> WEIGHT_TYPE ); };
	
	//特殊牌型
public:
	//是否十二皇族
	bool IsPelpleX12( tagAnalyseCard AnalyseCard );
	//是否三顺子
	bool IsLineX3( tagAnalyseCard AnalyseCard, bool bSameColor );
	//是否3同花
	bool IsColorX3( tagAnalyseCard AnalyseCard );
	//组合牌
	bool IsSameGroup( tagAnalyseCard AnalyseCard, BYTE bySame1, BYTE byCnt1, BYTE bySame2=0, BYTE byCnt2=0 );
	//抽取牌
	bool GetSameCnt(tagAnalyseCard &AnalyseCard, BYTE bySame, BYTE byCnt, BYTE byCard[] = NULL);
	//抽取牌
	BYTE GetSameCard( tagAnalyseCard &AnalyseCard, BYTE bySame );
	//顺子长度
	BYTE GetLineLen(tagAnalyseCard AnalyseCard, bool bSameColor = false);
	//抽取牌
	bool GetLine( tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byStart = INVALID_BYTE, BYTE byCard[ ] = NULL );
	//抽取牌
	bool GetMaxLine( tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byCard[] = NULL );
	//抽取炸弹
	bool GetBomb(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//抽取葫芦
	bool GetHuLu(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//抽取同花
	bool GetSameColor(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//抽取两对
	bool GetDoubleX2(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//抽取单张
	bool GetSingleCard(tagAnalyseCard &AnalyseCard, BYTE cbCnt, BYTE cbCard[] = NULL);

	//逻辑函数
public:
	//自动摆牌
	void AutoSortCard( BYTE bCardData[], BYTE bPutCardData[] );
	//手中最大组合
	void GetMaxCard( tagAnalyseCard AnalyseCard, BYTE cbPutCardData[ ] );

	//逻辑函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData);
	BYTE SwitchCardToIndex( BYTE bCardData );
	int GetSpecialWeights(BYTE bType);

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE bCardData) { return bCardData&LOGIC_MASK_VALUE; } //十六进制前面四位表示牌的数值
	//获取花色
	BYTE GetCardColor(BYTE bCardData) { return (bCardData&LOGIC_MASK_COLOR) >> 4; } //十六进制后面四位表示牌的花色 

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE bCardData[], BYTE bCardCount, enSortCardType SortCardType = enDescend);
	//混乱扑克
	void RandCardList(BYTE bCardBuffer[], BYTE bBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount);

public:
	CString GetCardString( BYTE cbCardData );
	CString GetCardListString( BYTE cbCardData[], BYTE cbCardCount );
};

//////////////////////////////////////////////////////////////////////////

#endif
