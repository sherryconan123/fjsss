#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
const BYTE	CGameLogic::m_bCardListData[ FULL_COUNT ] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//黑桃 A - K
	0x4E, 0x4F
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

BYTE CGameLogic::GetCardType( BYTE bCardData[ ], BYTE bCardCount, tagAnalyseCard & AnalyseCard )
{
	//解析扑克
	AnalysebCardData( bCardData, bCardCount, AnalyseCard );
	//开始分析
	if( bCardCount == 3 )
	{
		if( AnalyseCard.cbColorCount[ 4 ] == 3 ) return CT_THREE_KING;
		//三张牌型
		if( IsSameGroup( AnalyseCard, 3, 1 ) )  return CT_THREE_HEAD;
		//对带一张
		if( AnalyseCard.cbBlockCount[ 1 ] + AnalyseCard.cbColorCount[ 4 ] > 0 ) return CT_DOUBLE;
		//单牌类型 
		if( 3 == AnalyseCard.cbBlockCount[ 0 ] ) return CT_SINGLE;
	}
	else if( bCardCount == 5 )//五张牌型
	{
		//五王
		if( AnalyseCard.cbColorCount[ 4 ] == 5 ) return CT_FIVE_KING;
		//四王
		if( AnalyseCard.cbColorCount[ 4 ] == 4 ) return CT_FOUR_KING;
		//五同
		if( IsSameGroup( AnalyseCard, 5, 1 ) )  return CT_FIVE_SAME;
		//同花顺牌
		if( GetLineLen( AnalyseCard, true ) == 5 ) return CT_FIVE_COLOR_LINE;
		//四同
		if( IsSameGroup( AnalyseCard, 4, 1 ) ) return CT_FOUR_SAME;
		//葫芦牌型
		if( IsSameGroup( AnalyseCard, 3, 1, 2, 1 ) ) return CT_THREE_TWO;
		//同花
		if( AnalyseCard.cbColorKind == 1 ) return CT_FIVE_COLOR;
		//顺子
		if( GetLineLen( AnalyseCard ) == 5 ) return CT_FIVE_LINE;
		//三条
		if( IsSameGroup( AnalyseCard, 3, 1) ) return CT_THREE;
		//两对牌型
		if( AnalyseCard.cbBlockCount[ 1 ] + AnalyseCard.cbColorCount[ 4 ] >= 2 )return CT_DOUBLE_X2;
		//只有一对
		if( AnalyseCard.cbBlockCount[ 1 ] + AnalyseCard.cbColorCount[ 4 ] >= 1 )return CT_DOUBLE;
		//单牌类型
		if( AnalyseCard.cbBlockCount[ 0 ] == 5 ) return CT_SINGLE;
	}
	return CT_INVALID;
}

void CGameLogic::AnalysebCardData( BYTE bCardData[], BYTE bCardCount, tagAnalyseCard & AnalyseCard )
{
	ZeroMemory( &AnalyseCard, sizeof( AnalyseCard ) );
	CopyMemory( AnalyseCard.cbTempCardData, bCardData, bCardCount );
	//排列扑克
	SortCardList( AnalyseCard.cbTempCardData, bCardCount, enAscend );//23~K小大
	AnalyseCard.cbCardCnt = bCardCount;
	BYTE bySame, byTempColor, byCardValue;
	for( int i = 0; i < bCardCount; i++ )
	{
		byTempColor = GetCardColor( AnalyseCard.cbTempCardData[ i ] );
		AnalyseCard.cbColorCount[ byTempColor ]++;
		byCardValue = GetCardValue( AnalyseCard.cbTempCardData[ i ] );//
		if( byCardValue < 14 )AnalyseCard.cbValueCnt[ byTempColor ][ byCardValue - 1 ] ++;
		if( byCardValue > AnalyseCard.cbMaxCardValue ) AnalyseCard.cbMaxCardValue = byCardValue;
	}
	//非王颜色种类个数
	for( int i = 0; i < 4; i++ )
	{
		if( AnalyseCard.cbColorCount[ i ] > 0 ) AnalyseCard.cbColorKind++;
	}
	for( int i = 0; i < MAX_COUNT; i++ ) AnalyseCard.cbValueCnt[ 4 ][ i ] = AnalyseCard.cbValueCnt[ 0 ][ i ] + AnalyseCard.cbValueCnt[ 1 ][ i ] + AnalyseCard.cbValueCnt[ 2 ][ i ] + AnalyseCard.cbValueCnt[ 3 ][ i ];
	AnalyseCard.cbMinLogicValue = GetCardLogicValue( AnalyseCard.cbTempCardData[ 0 ] );
	for( int i = 0; i < bCardCount; i++ )
	{
		if( AnalyseCard.cbTempCardData[ i ] > 0x40 ) continue;
		AnalyseCard.cbMaxLogicValue = GetCardLogicValue( AnalyseCard.cbTempCardData[ i ] );
		bySame = 1;
		byCardValue = GetCardValue( AnalyseCard.cbTempCardData[ i ] );//

		for( int j = i + 1; j < bCardCount; j++ )
		{
			if( GetCardValue( AnalyseCard.cbTempCardData[ j ] ) != byCardValue ) break;
			bySame++;
		}
		//设置结果
		BYTE cbIndex = AnalyseCard.cbBlockCount[ bySame - 1 ]++;
		AnalyseCard.cbCardData[ bySame - 1 ][ cbIndex ] = AnalyseCard.cbTempCardData[ i + bySame - 1];//小到大排 i + bySame - 1

		//设置索引
		i += ( bySame - 1 );
	}
}

BYTE CGameLogic::GetCardSpecialType( BYTE bCardData[ ], BYTE bCardCount, BYTE bType )
{
	tagAnalyseCard AnalyseCard;
	AnalysebCardData( bCardData, bCardCount, AnalyseCard );
	if (AnalyseCard.cbColorCount[4] > 0) return CT_EX_INVALID;
	bool bIsLine = ( AnalyseCard.cbColorCount[ 4 ] + AnalyseCard.cbBlockCount[ 0 ] ) == MAX_COUNT;
	BYTE byRedCnt = AnalyseCard.cbColorCount[ 0 ] + AnalyseCard.cbColorCount[ 2 ];
	BYTE byBlackCnt = AnalyseCard.cbColorCount[ 1 ] + AnalyseCard.cbColorCount[ 3 ];

	if( bIsLine && AnalyseCard.cbColorKind == 1 ) return CT_EX_ZHIZUNQINGLONG; //至尊清龙
	if( bIsLine && ( byRedCnt == 0 || byBlackCnt == 0 )) return CT_EX_YITIAOLONG_EX; //一条花龙
	if( bIsLine ) return CT_EX_YITIAOLONG; //一条龙
	//if( IsPelpleX12( AnalyseCard ) ) return CT_EX_SHIERHUANGZU; //十二皇族 A K Q J
	//if( IsLineX3( AnalyseCard, true ) ) return CT_EX_SANTONGHUASHUN; //三同花顺 
	//if( IsSameGroup( AnalyseCard, 4, 3 ) ) return CT_EX_SANFENGTIANXIA; //三分天下(三炸弹)
	if (IsSameGroup(AnalyseCard, 9, 1)) return CT_EX_NINE_SAME;	 //9同	
	if (IsSameGroup(AnalyseCard, 8, 1)) return CT_EX_EIGHT_SAME; //8同	
	if (IsSameGroup(AnalyseCard, 7, 1)) return CT_EX_SEVEN_SAME; //7同	
	if( AnalyseCard.cbColorKind == 1 ) return CT_EX_ONE_COLOR;//凑一色	
	if( byBlackCnt == 0 ) return CT_EX_ALL_RED;//全红	
	if( byRedCnt == 0 ) return CT_EX_ALL_BLACK;//全黑	
	if( byRedCnt == 1 || (byRedCnt == 0 && AnalyseCard.cbColorCount[4] > 0) ) return CT_EX_ALL_BLACK_R1; //全黑一点红
	if( byBlackCnt == 1 || ( byBlackCnt == 0 && AnalyseCard.cbColorCount[ 4 ] > 0 ) ) return CT_EX_ALL_RED_B1; //全红一点黑
	if( bType == 1 ) //部分特殊牌型
	{
		if( AnalyseCard.cbMinLogicValue >= 10 ) return CT_EX_LEAST_10; //10起
		if( IsSameGroup( AnalyseCard, 3, 3, 2, 2 ) ) return CT_EX_THREE_TDX2; //是否凤凰三点头
		if( AnalyseCard.cbMinLogicValue >= 9 ) return CT_EX_LEAST_9; //9起
	}
	if( IsSameGroup( AnalyseCard, 3, 1, 2, 5 ) ) return CT_EX_DUI_5_THREE;	 //五对三条	
	if( AnalyseCard.cbMinLogicValue >= 7 ) return CT_EX_ALL_DA;//全大（8起）
	if( AnalyseCard.cbMaxCardValue < 10 ) return CT_EX_ALL_XIAO;//全小
	//if( IsSameGroup( AnalyseCard, 3, 4 ) ) return CT_EX_SITAOSANTIAO;//四套三条
	//if( AnalyseCard.cbMinLogicValue >= 7 && bType == 1 ) return CT_EX_LEAST_7; //7起 特殊牌型
	//if( AnalyseCard.cbMinLogicValue >= 6 && bType == 1 ) return CT_EX_LEAST_6; //6起 特殊牌型
	if( IsSameGroup( AnalyseCard, 2, 6 ) ) return CT_EX_LIUDUIBAN; //六对半
	if( IsLineX3( AnalyseCard, false ) ) return CT_EX_LINE_X3; //三顺子 
	if (AnalyseCard.cbMinLogicValue >= 6) return CT_EX_HALF_DA; //半大 特殊牌型
	if( AnalyseCard.cbMaxCardValue <= 10) return CT_EX_HALF_XIAO;//半小 特殊牌型
	if( IsColorX3( AnalyseCard ) ) return CT_EX_COLOR_X3;//三同花
	return CT_EX_INVALID;				
}

SCORE CGameLogic::GetCardWeight( BYTE bCardData[], BYTE bCardCount, bool bColor )
{
	tagAnalyseCard AnalyseCard;
	BYTE byType = GetCardType( bCardData, bCardCount, AnalyseCard );
	if( byType == CT_FIVE_LINE || byType == CT_FIVE_COLOR_LINE ) return GetLineWeight( AnalyseCard, byType, bColor );
	else if( byType == CT_FIVE_COLOR ) return GetColorWeight( AnalyseCard, byType, bColor );
	else return GetUnLineWeight( AnalyseCard, byType, bColor );
}

//顺子牌型解析
SCORE CGameLogic::GetLineWeight( tagAnalyseCard AnalyseCard, BYTE bType, bool bColor )
{
	BYTE byType[ 16 ] = { 0 };
	byType[ 0 ] = bType;
	BYTE byFIndex, bySIndex, byCIndex = 3;
	//最大2张牌值  区分 A23 AKQ
	for (int j = 0; j < 10; j++)
	{
		if (AnalyseCard.cbValueCnt[4][j] == 0 && j < 9) continue;
		byFIndex = j + 4;
		bySIndex = j;
		break;
	}

	if (bySIndex == 0) {//顺子最大为 10JQKA 第二大 2345A
		byFIndex = 0;
		if (AnalyseCard.cbValueCnt[4][12] || AnalyseCard.cbValueCnt[4][11] || AnalyseCard.cbValueCnt[4][10] || AnalyseCard.cbValueCnt[4][9]) bySIndex = 9;
		else bySIndex = 1;
	}
	if (AnalyseCard.cbColorCount[4] > 0 && AnalyseCard.cbValueCnt[4][0] == 0)
	{
		bool bCanLineA = true, bALinebig = false;
		for (int j = 0; j < 10; j++)
		{
			if (AnalyseCard.cbValueCnt[4][j] == 0) continue;
			if (j > 8) bALinebig = true;
			if (j >= 5 && j <= 8) {
				bCanLineA = false;
				break;
			}
		}
		if (bCanLineA) {
			byFIndex = 0;
			bySIndex = (bALinebig ? 9 : 1);//王替A 10JQKA 2345A
		}
	}

	byType[ 1 ] = GetCardLogicValue( byFIndex + 1 ) - 1;
	byType[ 3 ] = GetCardLogicValue( bySIndex + 1 ) - 1;

	for( int i = 0; i < 4; i++ )
	{
		if( bType == CT_FIVE_COLOR_LINE && AnalyseCard.cbColorCount[ i ] == 0 )  continue;//判断同花顺颜色
		if( bType == CT_FIVE_LINE && AnalyseCard.cbValueCnt[ i ][ byFIndex ] == 0 )  continue;//判断顺子最大牌颜色
		byCIndex = i;
		break;
	}
	byType[ 5 ] = byCIndex;
	if (bType == CT_FIVE_COLOR_LINE) byType[6] = 10 - AnalyseCard.cbColorCount[4];

	SCORE lWeight = 0;
	//for( int i = 0; i < 12; i++ ) lWeight = (lWeight << 4 ) + byType[ i ] ;
	CString strOut = L"GetLineWeight  : ";
	for (int i = 0; i < 12; i++) {
		lWeight = (lWeight << 4) + byType[i];
		strOut.Format(L"%s[%d->%d %X]", strOut, i, byType[i], lWeight);
	}
	m_pITableFrame->WriteLog(INVALID_CHAIR, strOut);

	return lWeight;
}
//非顺子牌型分析
SCORE CGameLogic::GetUnLineWeight( tagAnalyseCard AnalyseCard, BYTE bType, bool bColor )
{
	BYTE byIndex = 1;
	BYTE byType[ 16 ] = { 0 };
	byType[ 0 ] = bType;
	if (bType == CT_FIVE_SAME) byType[9] = 10 - AnalyseCard.cbColorCount[4];
	for( int i = 4; i >= 0; i-- )
	{
		for( int j = AnalyseCard.cbBlockCount[ i ]; j > 0; j-- )
		{
			BYTE byColor = GetCardColor( AnalyseCard.cbCardData[ i ][ j - 1 ] );
			BYTE byCardValue = GetCardLogicValue( AnalyseCard.cbCardData[ i ][ j - 1 ] ) - 1;
			if( byIndex == 1 ) //首牌
			{
				if( AnalyseCard.cbColorCount[ 4 ] > 0 ) //王牌替换
				{
					AnalyseCard.cbColorCount[ 4 ]--;
					if( byType[ 0 ] == CT_FIVE_COLOR ) byType[ byIndex++ ] = 13;//同花王替A
					else byColor = 3;//非同花王替黑桃
				}
				//先比花色
				if( bColor ) byType[ byIndex++ ] = byColor;
				else byType[ 8 ] = byColor;
			}

			//从组合数大到小取牌
			byType[ byIndex++ ] = byCardValue;
			if( byIndex >= 7 || j == 0 ) break;
		}
		if( byIndex >= 7 ) break;
	}
	
	SCORE lWeight = 0;
	//for( int i = 0; i < 12; i++ ) lWeight = ( lWeight << 4 ) + byType[ i ];
	CString strOut = L"GetUnLineWeight: ";
	for (int i = 0; i < 12; i++) {
		lWeight = (lWeight << 4) + byType[i];
		strOut.Format(L"%s[%d->%d %X]", strOut, i, byType[i], lWeight);
	}
	m_pITableFrame->WriteLog(INVALID_CHAIR, strOut);
	return lWeight;
}

//同花
SCORE CGameLogic::GetColorWeight( tagAnalyseCard AnalyseCard, BYTE bType, bool bColor )
{
	BYTE byIndex = 1;
	BYTE byType[ 16 ] = { 0 };
	byType[ 0 ] = bType;
	//花色
	for( int i = 0; i < 4; i++ )
	{
		if( AnalyseCard.cbColorCount[ i ] > 0 )
		{
			byType[ bColor ? 2 : 8 ] = i;
			break;
		}
	}

	while( AnalyseCard.cbColorCount[ 4 ] > 0 )
	{
		if( byType[ byIndex ] > 0 ) byIndex++;
		AnalyseCard.cbColorCount[ 4 ]--;
		byType[ byIndex++ ] = 13;//同花王替A
	}
	while( AnalyseCard.cbValueCnt[ 4 ][ 0 ] > 0 )
	{
		if( byType[ byIndex ] > 0 ) byIndex++;
		AnalyseCard.cbValueCnt[ 4 ][ 0 ]--;
		byType[ byIndex++ ] = 13;
	}

	for( int i = MAX_COUNT - 1; i > 0; i-- )
	{
		while( AnalyseCard.cbValueCnt[ 4 ][ i ] > 0 )
		{
			if( byType[ byIndex ] > 0 ) byIndex++;
			if( byIndex >= 7 ) break;
			AnalyseCard.cbValueCnt[ 4 ][ i ]--;
			byType[ byIndex++ ] = i;
		}
		if( byIndex >= 7 ) break;
	}
	
	SCORE lWeight = 0;
	//for( int i = 0; i < 12; i++ ) lWeight = ( lWeight << 4 ) + byType[ i ];
	CString strOut = L"GetColorWeight: ";
	for (int i = 0; i < 12; i++) {
		lWeight = (lWeight << 4) + byType[i];
		strOut.Format(L"%s[%d->%d %X]", strOut, i, byType[i], lWeight);
	}
	m_pITableFrame->WriteLog(INVALID_CHAIR, strOut);
	return lWeight;
}

bool CGameLogic::IsPelpleX12( tagAnalyseCard AnalyseCard )
{
	BYTE byCnt = AnalyseCard.cbColorCount[ 4 ] + AnalyseCard.cbValueCnt[ 4 ][ 0 ];//王+A
	for( int j = 10; j < MAX_COUNT; j++ ) byCnt += AnalyseCard.cbValueCnt[ 4 ][ j ];
	if( byCnt >= 12 ) return true;
	return false;
}

bool CGameLogic::IsLineX3( tagAnalyseCard AnalyseCard, bool bSameColor )
{
	tagAnalyseCard TempAnalyseCard, TempAnalyseCard2, TempAnalyseCard3;
	BYTE byTempCard[ 5 ] = { 0 };

	for( int i = 0; i <= MAX_COUNT - 4;i++ )
	{
		//第一组顺子
		TempAnalyseCard = AnalyseCard;
		if( !GetLine( AnalyseCard, bSameColor, i, byTempCard ) ) continue;
		RemoveCard( byTempCard, 5, TempAnalyseCard.cbTempCardData, TempAnalyseCard.cbCardCnt );
		TempAnalyseCard.cbCardCnt -= 5;

		for( int i = 0; i <= MAX_COUNT - 4; i++ )
		{
			//第二组顺子
			AnalysebCardData( TempAnalyseCard.cbTempCardData, TempAnalyseCard.cbCardCnt, TempAnalyseCard2 );
			if( !GetLine( TempAnalyseCard2, bSameColor, i, byTempCard ) ) continue;
			RemoveCard( byTempCard, 5, TempAnalyseCard2.cbTempCardData, TempAnalyseCard2.cbCardCnt );
			TempAnalyseCard2.cbCardCnt -= 5;

			//头道
			AnalysebCardData( TempAnalyseCard2.cbTempCardData, TempAnalyseCard2.cbCardCnt, TempAnalyseCard3 );
			if( GetLineLen( TempAnalyseCard3, bSameColor ) == 3 ) return true;
		}
	}
	return false;
}

bool CGameLogic::IsColorX3( tagAnalyseCard AnalyseCard )
{
	if( AnalyseCard.cbColorKind == 4 ) return false; 
	BYTE byC5 = 0, byKing = AnalyseCard.cbColorCount[ 4 ];
	//筛选5同花
	for( BYTE NeedKing = 0; NeedKing <= byKing; NeedKing++ )
	{
		BYTE byCnt = 5 - NeedKing;
		for( BYTE j = 0; j < 4; j++ )
		{
			while( byKing >= NeedKing && AnalyseCard.cbColorCount[ j ] >= byCnt ) byKing -= NeedKing, ++byC5, AnalyseCard.cbColorCount[ j ] -= byCnt;
		}
	}
	if( byC5 < 2 ) return false;

	//筛选3同花
	for( int i = 0; i < 4; i++ )
	{
		if( AnalyseCard.cbColorCount[ i ] + byKing == 3 ) return true;
	}
	return false;
}

bool CGameLogic::IsSameGroup( tagAnalyseCard AnalyseCard, BYTE bySame1, BYTE byCnt1, BYTE bySame2/*=0*/, BYTE byCnt2/*=0 */ )
{
	if( !GetSameCnt( AnalyseCard, bySame1, byCnt1 ) ) return false;
	if( byCnt2 > 0 && !GetSameCnt( AnalyseCard, bySame2, byCnt2 ) ) return false;
	return true;
}

bool CGameLogic::GetSameCnt(tagAnalyseCard &AnalyseCard, BYTE bySame, BYTE byCnt, BYTE byCard[] /*= NULL*/)
{
	if (bySame <= 1) return false;
	BYTE byTempCard, Index = 0;
	for (int i = 0; i < byCnt; i++)
	{
		byTempCard = GetSameCard(AnalyseCard, bySame);
		if (byTempCard == 0) return false;
	}
	if (byCard != NULL && byCnt == 1)
	{
		if (byTempCard >= 0x40)
		{
			for (int i = 0; i < bySame; i++) byCard[i] = 0x40;
		}
		else
		{
			for (int i = 0; i < bySame; i++)
			{
				if (AnalyseCard.cbValueCnt[4][byTempCard - 1] == 0)
				{
					byCard[i] = 0x40;
				}
				else
				{
					byCard[i] = byTempCard;
					AnalyseCard.cbValueCnt[4][byTempCard - 1]--;
				}
			}
		}
		for (int k = 0; k < bySame; k++)
		{
			for (int a = 0; a < MAX_COUNT; a++)
			{
				if (AnalyseCard.cbTempCardData[a] == 0) continue;
				if (byCard[k] >= 0x40 && AnalyseCard.cbTempCardData[a] < 0x40) continue; //查找王牌
				if (byCard[k] < 0x40 && byCard[k] != GetCardValue(AnalyseCard.cbTempCardData[a]))  continue;//查找散排
				byCard[k] = AnalyseCard.cbTempCardData[a];
				if (a + 1 < MAX_COUNT) MoveMemory(AnalyseCard.cbTempCardData + a, AnalyseCard.cbTempCardData + a + 1, MAX_COUNT - a - 1);
				AnalyseCard.cbTempCardData[MAX_COUNT - 1] = 0;
				break;
			}
		}
	}

	return true;
}

BYTE CGameLogic::GetSameCard(tagAnalyseCard &AnalyseCard, BYTE bySame)
{
	//同牌数量上限(1副4张 2副8张)  当前牌总数  取出后剩余数量
	BYTE byMaxCnt = 8, byTempCalc = 0;
	//优先倍数关系
	for (BYTE byTempSame = bySame; byTempSame <= byMaxCnt; byTempSame += bySame)
	{
		if (AnalyseCard.cbBlockCount[byTempSame - 1] == 0) continue;
		AnalyseCard.cbBlockCount[byTempSame - 1]--;
		byTempCalc = byTempSame - bySame;//取出后剩余数量
		if (byTempCalc > 0)
		{
			AnalyseCard.cbCardData[byTempCalc - 1][AnalyseCard.cbBlockCount[byTempCalc - 1]] = AnalyseCard.cbCardData[byTempSame - 1][AnalyseCard.cbBlockCount[byTempSame - 1]];
			AnalyseCard.cbBlockCount[byTempCalc - 1]++;
		}
		return GetCardValue(AnalyseCard.cbCardData[byTempSame - 1][AnalyseCard.cbBlockCount[byTempSame - 1]]);
	}
	//在多于目标的牌中扣除
	for (int i = byMaxCnt - 1; i >= bySame; i--)
	{
		if (AnalyseCard.cbBlockCount[i] == 0) continue;
		byTempCalc = i + 1 - bySame;//取出后剩余数量
		AnalyseCard.cbBlockCount[i]--;
		if (byTempCalc > 0)
		{
			AnalyseCard.cbCardData[byTempCalc - 1][AnalyseCard.cbBlockCount[byTempCalc - 1]] = AnalyseCard.cbCardData[i][AnalyseCard.cbBlockCount[i]];
			AnalyseCard.cbBlockCount[byTempCalc - 1]++;
		}
		return GetCardValue(AnalyseCard.cbCardData[i][AnalyseCard.cbBlockCount[i]]);
	}
	//带混拼凑
	for (int i = bySame - 2; i >= 0; i--)
	{
		if (AnalyseCard.cbBlockCount[i] == 0) continue;
		if (i + 1 + AnalyseCard.cbColorCount[4] < bySame) continue;
		AnalyseCard.cbBlockCount[i]--;
		AnalyseCard.cbColorCount[4] -= (bySame - (i + 1));//消耗混
		return GetCardValue(AnalyseCard.cbCardData[i][AnalyseCard.cbBlockCount[i]]);
	}
	//纯混拼凑
	if (AnalyseCard.cbColorCount[4] >= bySame)
	{
		AnalyseCard.cbColorCount[4] -= bySame;
		return 0x40;
	};
	return 0;
}

bool CGameLogic::GetLine(tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byStart, BYTE byCard[] /*= NULL */)
{
	if (byStart > 9) return false;
	BYTE NeedKing = 0, byTempIndex = 0, byMinKing = 5, byColorIndex = 0;
	//计算花色 
	for (BYTE byColor = 0; byColor < 4; byColor++)
	{
		NeedKing = 0;
		if (!bSameColor) byColor = 4;
		for (int i = byStart; i < byStart + 5; i++)
		{
			byTempIndex = i == 13 ? 0 : i;//A
			if (AnalyseCard.cbValueCnt[byColor][byTempIndex] == 0) NeedKing++;
		}

		if (NeedKing <= AnalyseCard.cbColorCount[4] && NeedKing < byMinKing)
		{
			byMinKing = NeedKing;
			byColorIndex = byColor;
		}
	}
	//不成顺
	if (byMinKing == 5) return false;

	if (byCard)
	{
		for (int i = 0; i < 5; i++)
		{
			byCard[i] = 0;
			byTempIndex = byStart + i;
			if (byTempIndex == 13) byTempIndex = 0;

			for (int j = 0; j < MAX_COUNT; j++)
			{
				if (AnalyseCard.cbTempCardData[j] == 0) continue;
				if (GetCardValue(AnalyseCard.cbTempCardData[j]) - 1 != byTempIndex) continue;
				if (byColorIndex != 4 && GetCardColor(AnalyseCard.cbTempCardData[j]) != byColorIndex) continue;
				byCard[i] = AnalyseCard.cbTempCardData[j];
				AnalyseCard.cbTempCardData[j] = 0;
				break;
			}
			if (byCard[i] == 0)
			{
				for (int j = 0; j < MAX_COUNT; j++)
				{
					if (AnalyseCard.cbTempCardData[j] < 0x40) continue;
					byCard[i] = AnalyseCard.cbTempCardData[j];
					AnalyseCard.cbTempCardData[j] = 0;
					break;
				}
			}
			if (byCard[i] == 0) return false;
		}
	}

	return true;
}

BYTE CGameLogic::GetLineLen(tagAnalyseCard AnalyseCard, bool bSameColor /*= false*/)
{
	if( bSameColor && AnalyseCard.cbColorKind > 1 ) return 1;
	BYTE byLen = 1, byTempKing = AnalyseCard.cbColorCount[ 4 ], byTempIndex = 0;
	BYTE bySum = AnalyseCard.cbValueCnt[ 4 ][ 1 ] + AnalyseCard.cbValueCnt[ 4 ][ 2 ] + AnalyseCard.cbValueCnt[ 4 ][ 3 ] + AnalyseCard.cbValueCnt[ 4 ][ 4 ];
	for (BYTE i = 0; i <= MAX_COUNT - 1 + AnalyseCard.cbColorCount[4]; i++)
	{
		if (AnalyseCard.cbValueCnt[4][i] == 0) continue;
		if( i == 0 && bySum == 0 ) continue;

		for( int j = i + 1; j < i + 5; j++ )
		{
			BYTE byTempIndex = j;
			if (byTempIndex == 13) byTempIndex = 0;//10 J Q K A
			if (byTempIndex > 13 || AnalyseCard.cbValueCnt[4][byTempIndex] == 0)// J Q K A + 王
			{
				if (byTempKing > 0) byTempKing--;
				else break;
			}
			byLen++;
		}
		break;
	}
	return byLen;
}

bool CGameLogic::GetMaxLine( tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byCard[] /*= NULL */ )
{
	for( int i = 9; i >= 0;i-- )
	{
		if( GetLine( AnalyseCard, bSameColor, i, byCard ) ) return true;
	}
	return false;
}

bool CGameLogic::GetBomb(tagAnalyseCard AnalyseCard, BYTE cbCard[] /*= NULL*/)
{
	if (GetSameCnt(AnalyseCard, 4, 1, cbCard))
	{
		for (int i = 0; i < 8; i++) //不参与组合最小散牌
		{
			for (int j = 0; j < AnalyseCard.cbBlockCount[i]; j++)
			{
				if (AnalyseCard.cbColorCount[GetCardColor(AnalyseCard.cbCardData[i][j])] == 5) continue;//跳过同花
				cbCard[4] = AnalyseCard.cbCardData[i][j];
				AnalyseCard.cbBlockCount[i]--;
				if (j + 1 < MAX_COUNT) MoveMemory(AnalyseCard.cbCardData[i] + j, AnalyseCard.cbCardData[i] + j + 1, MAX_COUNT - j - 1);
				return true;
			}
		}
		for (int i = 0; i < 8; i++) //不参与组合最小散牌
		{
			if (AnalyseCard.cbBlockCount[i] == 0) continue;
			cbCard[4] = AnalyseCard.cbCardData[i][0];
			AnalyseCard.cbBlockCount[i]--;
			MoveMemory(AnalyseCard.cbCardData[i], AnalyseCard.cbCardData[i] + 1, 7);
			return true;
		}
		return true;
	}
	return false;
}

bool CGameLogic::GetHuLu(tagAnalyseCard AnalyseCard, BYTE cbCard[] /*= NULL*/)
{
	if (GetSameCnt(AnalyseCard, 3, 1, cbCard))
	{
		BYTE byTempCard[5] = { 0 };
		if (GetSameCnt(AnalyseCard, 2, 1, byTempCard))
		{
			CopyMemory(cbCard + 3, byTempCard, 2);
			return true;
		}
	}
	return false;
}

bool CGameLogic::GetSameColor(tagAnalyseCard AnalyseCard, BYTE cbCard[] /*= NULL*/)
{
	BYTE cbIndex = 0;
	for (BYTE NeedKing = 0; NeedKing <= AnalyseCard.cbColorCount[4]; NeedKing++)
	{
		for (INT j = 3; j >= 0; j--)
		{
			if (AnalyseCard.cbColorCount[j] >= 5 - NeedKing)
			{
				for (int l = 0; l < MAX_COUNT; l++) // 查找单张同花 （留下三张或对子给之后的牌）
				{
					if (AnalyseCard.cbValueCnt[j][l] != 1) continue;
					cbCard[cbIndex++] = (j << 4) + l + 1;
					if (cbIndex >= 5) return true;
				}
				for (int l = 0; l < MAX_COUNT; l++) // 查找多张同花
				{
					if (AnalyseCard.cbValueCnt[j][l] <= 1) continue;
					for (int k = 0; k < AnalyseCard.cbValueCnt[j][l]; k++)
					{
						cbCard[cbIndex++] = (j << 4) + l + 1;
						if (cbIndex >= 5) return true;
					}
				}
				for (int l = 0; l < MAX_COUNT; l++)
				{
					if (AnalyseCard.cbTempCardData[l] < 0x40) continue;
					cbCard[cbIndex++] = AnalyseCard.cbTempCardData[l];
					if (cbIndex >= 5) return true;
				}
			}
		}
	}
	return false;
}

bool CGameLogic::GetDoubleX2(tagAnalyseCard AnalyseCard, BYTE cbCard[] /*= NULL*/)
{
	if (GetSameCnt(AnalyseCard, 2, 1, cbCard))
	{
		BYTE byTempCard[5] = { 0 };
		if (GetSameCnt(AnalyseCard, 2, 1, byTempCard))
		{
			CopyMemory(cbCard + 2, byTempCard, 2);
			GetSingleCard(AnalyseCard, 1, cbCard);
			return true;
		}
	}
	return false;
}

bool CGameLogic::GetSingleCard(tagAnalyseCard &AnalyseCard, BYTE cbCnt, BYTE cbCard[] /*= NULL*/)
{
	int iCount = 5 - cbCnt;
	for (BYTE i = iCount; i < 5; ++i) cbCard[i] = 0;
	//三条取两张 两对取一张 一对取三张 杂牌取五张
	//中道要大于头道 拆条 拆对
	//去除重复牌
	bool bHave = false;
	for (int i = 0; i < cbCnt; i++)
	{
		bHave = false;
		for (int j = 7; j >= 0; j--)
		{
			for (int k = AnalyseCard.cbBlockCount[j] - 1; k >= 0; k--)
			{
				if (cbCnt == 2 && GetCardValue(AnalyseCard.cbCardData[j][k]) == GetCardValue(cbCard[0])) continue;
				if (cbCnt == 1 && (GetCardValue(AnalyseCard.cbCardData[j][k]) == GetCardValue(cbCard[0]) || GetCardValue(AnalyseCard.cbCardData[j][k]) == GetCardValue(cbCard[2]))) continue;
				if (cbCnt == 3 && GetCardValue(AnalyseCard.cbCardData[j][k]) == GetCardValue(cbCard[0])) continue;
				if (AnalyseCard.cbCardData[j][k] == 0) continue;
				cbCard[MIDDLE_COUNT - 1 - i] = AnalyseCard.cbCardData[j][k];
				AnalyseCard.cbCardData[j][k] = 0;
				bHave = true;
				break;
			}
			if (bHave) break;
		}
	}

	if (cbCard != NULL) {
		for (int i = 0; i < CountArray(cbCard); i++){
			if (!cbCard[i]) return false;
		}
		return true;
	}
	return false;
}

void CGameLogic::AutoSortCard( BYTE bCardData[], BYTE bPutCardData[] )
{
	tagAnalyseCard AnalyseCard;
	BYTE byTempCard[ MAX_COUNT ] = { 0 }, bySortCard[ 5 ] = { 0 };
	CopyMemory( byTempCard, bCardData, MAX_COUNT );
	//分析尾道牌
	AnalysebCardData( byTempCard, MAX_COUNT, AnalyseCard );
	GetMaxCard( AnalyseCard, bySortCard );
	RemoveCard( bySortCard, BACK_COUNT, byTempCard, MAX_COUNT );
	CopyMemory( bPutCardData + BACK_OFFSET, bySortCard, BACK_COUNT );
	//分析中道牌
	AnalysebCardData( byTempCard, MAX_COUNT - BACK_COUNT, AnalyseCard );
	GetMaxCard( AnalyseCard, bySortCard );
	RemoveCard( bySortCard, MIDDLE_COUNT, byTempCard, MAX_COUNT - BACK_COUNT );
	CopyMemory( bPutCardData + MIDDLE_OFFSET, bySortCard, MIDDLE_COUNT );

	CopyMemory( bPutCardData, byTempCard, FRONT_COUNT );
}

void CGameLogic::GetMaxCard( tagAnalyseCard AnalyseCard, BYTE cbPutCardData[] )
{
	//五同
	if (GetSameCnt( AnalyseCard, 5, 1, cbPutCardData)) return;
	//同花顺
	if (GetMaxLine( AnalyseCard, true, cbPutCardData)) return;
	//炸弹
	if (GetBomb(AnalyseCard, cbPutCardData)) return;
	//葫芦
	if (GetHuLu(AnalyseCard, cbPutCardData)) return;
	//同花
	if (GetSameColor(AnalyseCard, cbPutCardData)) return;
	//顺子
	if (GetMaxLine( AnalyseCard, false, cbPutCardData)) return;
	//三条
	if (GetSameCnt( AnalyseCard, 3, 1, cbPutCardData )){ GetSingleCard(AnalyseCard, 2, cbPutCardData); return; }
	//2对
	if (GetDoubleX2(AnalyseCard, cbPutCardData)) return;
	//1对
	if( GetSameCnt( AnalyseCard, 2, 1, cbPutCardData ) ){ GetSingleCard(AnalyseCard, 3, cbPutCardData); return; }
	//散牌
	GetSingleCard(AnalyseCard, 5, cbPutCardData);
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardValue = GetCardValue(bCardData);
	if (bCardValue >= 14) return bCardValue + 1;

	//转换数值
	return (bCardValue == 1) ? (bCardValue + 13) : bCardValue;
}

BYTE CGameLogic::SwitchCardToIndex( BYTE bCardData )
{
	if( bCardData == 0x4e ) return 52;
	if( bCardData == 0x4f ) return 53;
	BYTE bCardColor = GetCardColor( bCardData );
	BYTE bCardValue = GetCardValue( bCardData );
	return bCardColor * 13 + bCardValue - 1;
}

int CGameLogic::GetSpecialWeights(BYTE bType)
{
	switch (bType)
	{
	case CT_EX_COLOR_X3: return 4;
	case CT_EX_HALF_XIAO:
	case CT_EX_HALF_DA: return 8;
	case CT_EX_LINE_X3: 
	case CT_EX_LIUDUIBAN: return 4;	
	case CT_EX_DUI_5_THREE: return 6;			
	case CT_EX_SITAOSANTIAO: 	
	case CT_EX_ALL_BLACK:
	case CT_EX_ALL_RED: 
	case CT_EX_ALL_XIAO:	
	case CT_EX_ALL_DA: return 10;
	case CT_EX_ALL_RED_B1: 
	case CT_EX_ALL_BLACK_R1:
	case CT_EX_SEVEN_SAME: return 20;
	case CT_EX_EIGHT_SAME: return 40;
	case CT_EX_NINE_SAME: return 60;
	case CT_EX_SANTONGHUASHUN: 
	case CT_EX_SANFENGTIANXIA:
	case CT_EX_SHIERHUANGZU: 
	case CT_EX_YITIAOLONG: 
	case CT_EX_YITIAOLONG_EX: return 54;
	case CT_EX_ZHIZUNQINGLONG: return 108;
		
	default:
		return 0;
	}
}

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount, enSortCardType SortCardType)
{
	ASSERT(bCardCount >= 1 && bCardCount <= 13);
	if (bCardCount<1 || bCardCount>13) return;

	//转换数值
	BYTE bLogicVolue[13];
	for (BYTE i = 0; i < bCardCount; i++)	bLogicVolue[i] = GetCardLogicValue(bCardData[i]);

	if (enDescend == SortCardType)
	{
		//排序操作
		bool bSorted = true;
		BYTE bTempData, bLast = bCardCount - 1;
		BYTE m_bCardCount = 1;
		do
		{
			bSorted = true;
			for (BYTE i = 0; i < bLast; i++)
			{
				if ((bLogicVolue[i] < bLogicVolue[i + 1]) ||
					((bLogicVolue[i] == bLogicVolue[i + 1]) && (bCardData[i] < bCardData[i + 1])))
				{
					//交换位置
					bTempData = bCardData[i];
					bCardData[i] = bCardData[i + 1];
					bCardData[i + 1] = bTempData;
					bTempData = bLogicVolue[i];
					bLogicVolue[i] = bLogicVolue[i + 1];
					bLogicVolue[i + 1] = bTempData;
					bSorted = false;
				}
			}
			bLast--;
		} while (bSorted == false);
	}
	else if (enAscend == SortCardType)
	{
		//排序操作
		bool bSorted = true;
		BYTE bTempData, bLast = bCardCount - 1;
		BYTE m_bCardCount = 1;
		do
		{
			bSorted = true;
			for (BYTE i = 0; i<bLast; i++)
			{
				if ((bLogicVolue[i]>bLogicVolue[i + 1]) ||
					((bLogicVolue[i] == bLogicVolue[i + 1]) && (bCardData[i] > bCardData[i + 1])))
				{
					//交换位置
					bTempData = bCardData[i];
					bCardData[i] = bCardData[i + 1];
					bCardData[i + 1] = bTempData;
					bTempData = bLogicVolue[i];
					bLogicVolue[i] = bLogicVolue[i + 1];
					bLogicVolue[i + 1] = bTempData;
					bSorted = false;
				}
			}
			bLast--;
		} while (bSorted == false);
	}
	else if (enColor == SortCardType)
	{
		//排序操作
		bool bSorted = true;
		BYTE bTempData, bLast = bCardCount - 1;
		BYTE m_bCardCount = 1;
		BYTE bColor[13];
		for (BYTE i = 0; i < bCardCount; i++)	bColor[i] = GetCardColor(bCardData[i]);
		do
		{
			bSorted = true;
			for (BYTE i = 0; i < bLast; i++)
			{
				if ((bColor[i] < bColor[i + 1]) ||
					((bColor[i] == bColor[i + 1]) && (GetCardLogicValue(bCardData[i]) < GetCardLogicValue(bCardData[i + 1]))))
				{
					//交换位置
					bTempData = bCardData[i];
					bCardData[i] = bCardData[i + 1];
					bCardData[i + 1] = bTempData;
					bTempData = bColor[i];
					bColor[i] = bColor[i + 1];
					bColor[i + 1] = bTempData;
					bSorted = false;
				}
			}
			bLast--;
		} while (bSorted == false);
	}

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	//混乱准备
	BYTE bTotalCount = sizeof(m_bCardListData);
	BYTE bCardData[sizeof(m_bCardListData)];
	CopyMemory(bCardData, m_bCardListData, sizeof(m_bCardListData));

	srand((unsigned)time(NULL));

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (bTotalCount - bRandCount);
		bCardBuffer[bRandCount++] = bCardData[bPosition];
		bCardData[bPosition] = bCardData[bTotalCount - bRandCount];
	} while (bRandCount < bBufferCount);

	return;
}

void CGameLogic::RandCardList(BYTE cbRandBuffer[], BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbRandCount = cbBufferCount;
	BYTE cbCardData[FULL_COUNT * 2];
	CopyMemory(cbCardData, cbRandBuffer, cbBufferCount);

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (cbRandCount - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[cbRandCount - bRandCount];
	} while (bRandCount < cbBufferCount &&  bRandCount < cbRandCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount)
{
	//检验数据
	ASSERT(bRemoveCount <= bCardCount);

	//定义变量
	BYTE bDeleteCount = 0, bTempCardData[MAX_COUNT];
	if (bCardCount > CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData, bCardData, bCardCount*sizeof(bCardData[0]));

	//置零扑克
	for (BYTE i = 0; i < bRemoveCount; i++)
	{
		for (BYTE j = 0; j < bCardCount; j++)
		{
			if (bTempCardData[j] == 0) continue;
			if (bRemoveCard[i] == bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j] = 0;
				break;
			}
		}
	}
	if (bDeleteCount != bRemoveCount) return false;

	//清理扑克
	ZeroMemory(bCardData, bCardCount);
	BYTE bCardPos = 0;
	for (BYTE i = 0; i < bCardCount; i++)
	{
		if (bTempCardData[i] != 0) bCardData[bCardPos++] = bTempCardData[i];
	}

	return true;
}

CString CGameLogic::GetCardString( BYTE cbCardData )
{
	if( cbCardData == 0x4e ) return TEXT( "[小王]" );
	if( cbCardData == 0x4f ) return TEXT( "[大王]" );
	CString strCard = TEXT( "[" );
	if( GetCardColor( cbCardData ) == 0 )	  strCard += TEXT( "方片" );
	if( GetCardColor( cbCardData ) == 1 ) strCard += TEXT( "梅花" );
	if( GetCardColor( cbCardData ) == 2 ) strCard += TEXT( "红桃" );
	if( GetCardColor( cbCardData ) == 3 ) strCard += TEXT( "黑桃" );
	BYTE byValue = GetCardValue( cbCardData );
	if( byValue == 1 ) strCard += TEXT( "A]" );
	else if( byValue == 0xB ) strCard += TEXT( "J]" );
	else if( byValue == 0xC ) strCard += TEXT( "Q]" );
	else if( byValue == 0xD ) strCard += TEXT( "K]" );
	else strCard.Format( TEXT( "%s%d]" ), strCard, byValue );
	return strCard;
}

CString CGameLogic::GetCardListString( BYTE cbCardData[], BYTE cbCardCount )
{
	CString strCards = TEXT( "" );
	for( int i = 0; i < cbCardCount; i++ )
	{
		strCards += GetCardString( cbCardData[ i ] );
	}
	return strCards;
}

/////////////////////////////////////////////////////////////////////////