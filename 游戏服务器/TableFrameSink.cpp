
#include "StdAfx.h"
#include "TableFrameSink.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_wWaitTime = 0;
	m_wMaxPlayer = GAME_PLAYER;

	//计数变量
	m_lCellScore = 1;
	m_bComparing = false;
	ZeroMemory(m_cbWriteScoreStatus, sizeof(m_cbWriteScoreStatus));

	srand((unsigned int)time(NULL));

	RepositionSink();
	InitRoomRules( );
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//自定规则
	ASSERT(m_pITableFrame->GetCustomRule()!=NULL);

	m_pITableFrame->SetStartMode( START_MODE_ALL_READY );
	m_GameLogic.m_pITableFrame = m_pITableFrame;
	InitRoomRules();
	return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
	ZeroMemory(m_bPlayStatus, sizeof(m_bPlayStatus));
	ZeroMemory(m_bUserCardData, sizeof(m_bUserCardData));
	ZeroMemory(m_bFinishCompare, sizeof(m_bFinishCompare));
	ZeroMemory(m_bSegmentCard, sizeof(m_bSegmentCard));
	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));
	ZeroMemory(m_llBaseScore, sizeof(m_llBaseScore));
	ZeroMemory(m_lFinalScore, sizeof(m_lFinalScore));
	ZeroMemory(m_lFinalScoreP2P, sizeof(m_lFinalScoreP2P));
	ZeroMemory(m_bGun, sizeof(m_bGun));
	
	//计数变量
	m_bComparing = false;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
	m_pITableFrame->SetUserAutoOperate( INVALID_CHAIR, 0 );
	//游戏变量
	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));
	ZeroMemory(m_llBaseScore, sizeof(m_llBaseScore));
	ZeroMemory(m_bSpecialType, sizeof(m_bSpecialType));
	ZeroMemory(m_bUserCardData, sizeof(m_bUserCardData));
	ZeroMemory(m_bSegmentCard, sizeof(m_bSegmentCard));
	ZeroMemory(m_bFinishSegment, sizeof(m_bFinishSegment));
	ZeroMemory(m_bFinishCompare, sizeof(m_bFinishCompare));
	ZeroMemory(m_bPlayStatus, sizeof(m_bPlayStatus));
	ZeroMemory( m_llCardWeight, sizeof( m_llCardWeight ) );
	m_wShowCardCount = 0;
	m_pITableFrame->WriteLog( INVALID_CHAIR, TEXT( "游戏开始" ) );
	//设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);
	BYTE bPlayerCount = 0;

	//设置状态
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem( i ) ;
		if (pServerUserItem == NULL) continue;
		m_bPlayStatus[ i ] = true;
		m_cbWriteScoreStatus[i] = TRUE;
		bPlayerCount++;
	}

	if( bPlayerCount < 2 ) return OnEventGameConclude( INVALID_CHAIR, NULL, GER_DISMISS );
	//中途禁入
	if( HaveRule( GAME_TYPE_CUTIN ) ) m_wMaxPlayer = bPlayerCount;

	if (m_wCutUser != INVALID_CHAIR)
	{
		SendCutUser();
		return true;
	}

	RandCardList();
	
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart)) ;
	//设置庄家
	GameStart.wBanker = m_wBanker;
	CopyMemory(GameStart.bPlayStatus, m_bPlayStatus, sizeof(m_bPlayStatus));

	//发送扑克
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	for(WORD i = 0; i < GAME_PLAYER; i++)
	{			
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ((pIServerUserItem != NULL))
		{
			CopyMemory(GameStart.bCardData, m_bUserCardData[i], sizeof(BYTE)*MAX_COUNT);
			if (HaveRule(GAME_TYPE_SPEACIAL)){
				m_bSpecialType[i] = m_GameLogic.GetCardSpecialType(m_bUserCardData[i], MAX_COUNT, false);
				GameStart.cbSpecialType = m_bSpecialType[i]; //注释掉即为去掉前端弹出是否跳过摆拍的logo
			}
			m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
		}
	}

	for(WORD i = 0; i < GAME_PLAYER; i++)
	{			
		m_GameLogic.SortCardList(m_bUserCardData[i], MAX_COUNT) ;
	}

	m_pITableFrame->SetUserAutoOperate( 0, m_wWaitTime );

	return true;
}


//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	m_pITableFrame->SetUserAutoOperate( INVALID_CHAIR, 0 );
	m_wBanker = INVALID_CHAIR;

	switch (cbReason)
	{
	case GER_DISMISS:			//游戏解散
		{
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));

			GameEnd.bShowBigResult = true;
			GameEnd.cbEndType = GER_DISMISS;

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame( GAME_STATUS_FREE, 2 );
			return true;
		}
	case GER_NORMAL:			//常规结束
		{
			m_wCutUser = INVALID_CHAIR;
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			for(WORD i=0 ; i<GAME_PLAYER ; ++i)
			{
				if( !m_bPlayStatus[ i ] ) continue;
					
				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
				if( !pUserItem ) continue;

				GameEnd.lScoreTimes[i] = m_lFinalScore[i];
				GameEnd.lGameScore[i] = m_lFinalScore[i];
				GameEnd.cbSpecialType[i] = m_bSpecialType[i];
				m_llTotalScore[i] += m_lFinalScore[i];
				ScoreInfoArray[i].lScore = m_lFinalScore[i];

				if( m_lFinalScore[ i ] == 0 ) ScoreInfoArray[ i ].cbType = SCORE_TYPE_DRAW;
				else if( m_lFinalScore[ i ] > 0 ) ScoreInfoArray[ i ].cbType = SCORE_TYPE_WIN;
				else if( m_lFinalScore[ i ] < 0 ) ScoreInfoArray[ i ].cbType = SCORE_TYPE_LOSE;
			}
			m_pITableFrame->CalculateRevenue(ScoreInfoArray, GAME_PLAYER, m_bPlayStatus);
			m_pITableFrame->WriteTableScore(ScoreInfoArray,m_pITableFrame->GetChairCount());

			GameEnd.cbEndType = GER_NORMAL;
			GameEnd.bShowBigResult = m_pITableFrame->GetRoomProgress() == GetRoomGameCount();
			CopyMemory(GameEnd.cbCardData, m_bSegmentCard, sizeof(m_bSegmentCard));
			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			for( int i = 0; i < GAME_PLAYER; i++ )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem( i );
				if (pIServerUserItem && m_cbTimeStartGame) m_pITableFrame->SetUserAutoOperate(i, m_cbTimeStartGame);
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);		

			return true;
		}
	case GER_USER_LEAVE:
		{
			return true;
		}
	}

	ASSERT(FALSE);

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));
			
			//自定义规则
			StatusFree.cbTimeStartGame = m_pITableFrame->GetOperateCountDown(wChairID);
			CopyMemory(StatusFree.bGameStatus, m_bPlayStatus, sizeof(m_bPlayStatus));
			StatusFree.cbWashCount = m_cbWashCount;

			//发送场景
			bool bSuccess =  m_pITableFrame->SendGameScene( pIServerUserItem, &StatusFree, sizeof(StatusFree));
			return bSuccess;
		}
	case GAME_STATUS_PLAY:		//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			StatusPlay.wBanker = m_wBanker;
			StatusPlay.bCompared = m_bComparing;
			StatusPlay.wCurUser = m_wCutUser;
			StatusPlay.wWaitTime = m_pITableFrame->GetOperateCountDown( 0 );
			CopyMemory(StatusPlay.cbHandCardData, m_bUserCardData[wChairID], MAX_COUNT);
			CopyMemory(StatusPlay.bFinishSegment, m_bFinishSegment, sizeof(m_bFinishSegment));
			CopyMemory(StatusPlay.lScoreTimes, m_lScoreTimes, sizeof(m_lScoreTimes));
			if (m_wCutUser == INVALID_CHAIR && m_bPlayStatus[wChairID] && HaveRule(GAME_TYPE_SPEACIAL))
				StatusPlay.cbSpecialType = m_GameLogic.GetCardSpecialType( m_bUserCardData[ wChairID ], MAX_COUNT, false );
			CopyMemory(StatusPlay.bGameStatus,m_bPlayStatus,sizeof(m_bPlayStatus));
			CopyMemory(StatusPlay.cbSegmentCard[wChairID], m_bSegmentCard[wChairID], MAX_COUNT);

			//发送场景
			bool bSuccess =  m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));


			if (m_bComparing/* && !m_bFinishCompare[wChairID]*/)
			{
				m_pITableFrame->SendTableData(wChairID, SUB_S_COMPARE_CARD, &m_cmdCompareCard, sizeof(m_cmdCompareCard));
			}
			
			return bSuccess;
		}
	}

	return false;
}

void CTableFrameSink::InitRoomRules()
{
	m_wMaxPlayer = GAME_PLAYER;
	m_wBanker = INVALID_CHAIR;
	m_wCutUser = INVALID_CHAIR;
	m_byDoubleTimes = 1;
	m_dwPlatfromRatio = 1;

	ZeroMemory( m_llTotalScore, sizeof( m_llTotalScore ) );
	m_cbTimeStartGame = 5;
	m_wWaitTime = 0;
	m_cbWashCount = 0;
	m_cbRandCount = FULL_COUNT;
	CopyMemory( m_cbRandCard, m_GameLogic.m_bCardListData, m_cbRandCount );
}

bool CTableFrameSink::HaveRule(DWORD dwRule)
{
	return ( m_dwCustomRules[ 0 ] & dwRule ) != 0;
}

CString CTableFrameSink::GetRulesStringEx( DWORD dwServerRules, DWORD dwRules[ 5 ] )
{
	CString cstr = TEXT("");
	return cstr;
}

SCORE CTableFrameSink::GetSitScore()
{
	bool bGoldServer = ( m_pGameServiceOption->wServerType & ( GAME_GENRE_GOLD | GAME_GENRE_PERSONAL_S | GAME_GENRE_PERSONAL_G ) ) > 0;
	if( !bGoldServer ) return 0;

	return 0;
}

void CTableFrameSink::OnUserAutoOperate( WORD wChairID )
{
	if( m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE ) return;
	OnTimerCheckTable();
}

bool CTableFrameSink::SetRoomRulesEx( DWORD  dwServerRules, DWORD dwRules[ 5 ] )
{
	m_pITableFrame->SetStartMode( START_MODE_ALL_READY );
	m_pITableFrame->WriteLog( INVALID_CHAIR, TEXT( "设置规则：%d %d %s" ), dwServerRules, dwRules[ 0 ], GetRulesStringEx( dwServerRules, dwRules ) );
	InitRoomRules();
	m_dwServerRules = dwServerRules;
	CopyMemory( m_dwCustomRules, dwRules, sizeof( m_dwCustomRules ) );

	m_dwPlatfromRatio = m_pITableFrame->GetPlatformRatio( );
	m_lCellScore = 1;
	if( m_dwCustomRules[ 0 ] & GAME_TYPE_BASE1 ) m_lCellScore = 10;
	if( m_dwCustomRules[ 0 ] & GAME_TYPE_BASE2 ) m_lCellScore = 20;
	if (m_dwCustomRules[0] & GAME_TYPE_BASE3) m_lCellScore = 50;
	if (m_dwCustomRules[0] & GAME_TYPE_BASE4) m_lCellScore = 100;
	if (m_dwCustomRules[0] & GAME_TYPE_BASE5) m_lCellScore = 150;
	if (m_dwCustomRules[0] & GAME_TYPE_BASE6) m_lCellScore = 300;
	if (m_dwCustomRules[0] & GAME_TYPE_BASE7) m_lCellScore = 500;

	if( m_dwCustomRules[ 0 ] & GAME_TYPE_TIME_1 ) m_wWaitTime = 30;
	if( m_dwCustomRules[ 0 ] & GAME_TYPE_TIME_2 ) m_wWaitTime = 45;

	m_wMaxPlayer = 4;
	if( dwServerRules & GAME_TYPE_1_PLAYER ) m_wMaxPlayer = 3;
	if( dwServerRules & GAME_TYPE_2_PLAYER ) m_wMaxPlayer = 2;
	if( dwServerRules & GAME_TYPE_3_PLAYER ) m_wMaxPlayer = 5;
	if( dwServerRules & GAME_TYPE_4_PLAYER ) m_wMaxPlayer = 6;

	int iCnt[ 5 ] = { 1, 1, 1, 1, 0 };//扑克 方 梅 红 黑
	if( m_wMaxPlayer >= 5 ) iCnt[ 3 ]++;//5人加黑桃
	if( m_wMaxPlayer >= 6 ) iCnt[ 0 ]++;//6人加方片
	//计算王数量
	if (dwRules[0] & GAME_TYPE_KING_1)  iCnt[4] = 1;
	if (dwRules[0] & GAME_TYPE_KING_2)  iCnt[4] = 2;
	if (dwRules[0] & GAME_TYPE_KING_3)  iCnt[4] = 3;
	m_cbRandCount = 0;
	for( int i = 0; i < 5; i++ )
	{
		BYTE byLen = i < 4 ? MAX_COUNT : 2;
		while( iCnt[ i ]-- > 0 )
		{
			CopyMemory( m_cbRandCard + m_cbRandCount, m_GameLogic.m_bCardListData + i*MAX_COUNT, byLen );
			m_cbRandCount += byLen;
		}
	}

	return true;
}

WORD CTableFrameSink::GetRoomGameCount()
{
	if( m_dwServerRules & GAME_TYPE_CNT_1 ) return 10;//
	if( m_dwServerRules & GAME_TYPE_CNT_2 ) return 20;//
	if( m_dwServerRules & GAME_TYPE_CNT_3 ) return 30;//
	return 1;
}


WORD CTableFrameSink::GetRoomEndData( VOID * pData )
{
	CMD_S_GameCustomInfo  * pCustomInfo = ( CMD_S_GameCustomInfo  * ) pData;
	WORD wSize = sizeof ( CMD_S_GameCustomInfo );
	ZeroMemory( pCustomInfo, wSize );

	tagScoreInfo ScoreInfo[GAME_PLAYER] = { 0 };
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		ScoreInfo[i].lScore = m_llTotalScore[i];
	}

	m_pITableFrame->CalculateRevenue(ScoreInfo, GAME_PLAYER, m_cbWriteScoreStatus, false);
	CopyMemory(pCustomInfo->llTotalScore, m_llTotalScore, sizeof(m_llTotalScore));
	return wSize;
}


//时间事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return false;
}

void CTableFrameSink::OnTimerCheckTable()
{
	m_pITableFrame->WriteLogEx(INVALID_CHAIR, "所有人开牌！！！");
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bPlayStatus[i]) continue;
		if (!m_bFinishSegment[i]) 
		{
			BYTE cbPutCard[MAX_COUNT] = { 0 };
			m_GameLogic.AutoSortCard(m_bUserCardData[i], cbPutCard);
			CMD_C_ShowCard Cmd = { 0 };
			CopyMemory(Cmd.cbFrontCard, FRONT_DATA(cbPutCard), FRONT_COUNT);
			CopyMemory(Cmd.cbMidCard, MIDDLE_DATA(cbPutCard), MIDDLE_COUNT);
			CopyMemory(Cmd.cbBackCard, BACK_DATA(cbPutCard), BACK_COUNT);
			Cmd.bSpecial = false;
			OnUserShowCard(i, &Cmd, sizeof(Cmd));
		}
	}
}

BYTE CTableFrameSink::GetHeadTimes( WORD wChair )
{
	BYTE bSegType = m_GameLogic.GetTypeByWeight( m_llCardWeight[ wChair ][ 0 ] );
	if( bSegType == CT_THREE_KING ) return 20;
	if( bSegType == CT_THREE_HEAD ) 
	{
		tagAnalyseCard AnalyseCard;
		m_GameLogic.AnalysebCardData( m_bSegmentCard[ wChair ], 3, AnalyseCard );
		for (int i = 2; i >= 0; i--)
		{
			if (AnalyseCard.cbBlockCount[i] == 0) continue;
			if (m_GameLogic.GetCardLogicValue(AnalyseCard.cbCardData[i][0]) == 14) return 10;
		}
		if( !HaveRule( GAME_TYPE_CHONG3 ) ) return 5;
		//冲三倍数
		for( int i = 2; i >= 0; i-- )
		{
			if( AnalyseCard.cbBlockCount[ i ] == 0) continue;
			if (m_GameLogic.GetCardLogicValue(AnalyseCard.cbCardData[i][0]) == 2) return 3;
			else 
			return m_GameLogic.GetCardLogicValue( AnalyseCard.cbCardData[ i ][ 0 ] );
		}
	}
	
	return 1;
}

SCORE CTableFrameSink::OnCompare3And5( SCORE lWeight1, SCORE lWeight2 )
{
	SCORE lType3 = m_GameLogic.GetTypeByWeight( lWeight1 );
	SCORE lTempWeight = 0;
	if( lType3 >= CT_THREE_HEAD && lType3 <= CT_THREE_KING ) lTempWeight = CT_THREE - lType3;
	lTempWeight = SCORE( lTempWeight << WEIGHT_TYPE );
	return lWeight2 - lWeight1 - lTempWeight;
}
void CTableFrameSink::RandCardList()
{
	BYTE cbCardTemp[FULL_COUNT *2] = { 0 };
	m_GameLogic.RandCardList(m_cbRandCard, cbCardTemp, m_cbRandCount);
	BYTE cbIdx = 0;
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_bPlayStatus[ i ] ) continue;
		CopyMemory( m_bUserCardData[ i ], cbCardTemp + ( cbIdx * MAX_COUNT ), MAX_COUNT );
		cbIdx++;
		if( cbIdx * MAX_COUNT >= m_cbRandCount ) break;
	}
// #ifdef DEBUG
// 	BYTE byTempCard[MAX_COUNT] = { 0x02, 0x12, 0x22, 0x23, 0x13, 0x36, 0x26, 0x16, 0x3b, 0x1b, 0x0b, 0x2b, 0x3c,  };
// 	BYTE byTempCard2[MAX_COUNT] = { 0x4f, 0x4e, 0x4f, 0x19, 0x29, 0x09, 0x39, 0x09, 0x37, 0x1b, 0x24, 0x03, 0x02, };
// 	CopyMemory(m_bUserCardData[0], byTempCard, MAX_COUNT);
// 	CopyMemory(m_bUserCardData[1], byTempCard2, MAX_COUNT);
// #endif
	

	m_pITableFrame->WriteLog(INVALID_CHAIR, TEXT("玩家手牌:"));
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bPlayStatus[i]) continue;
		CString strLog;
		for (WORD j = 0; j < MAX_COUNT; j++)
		{
			strLog.AppendFormat(TEXT("0x%02x, "), m_bUserCardData[i][j]);
		}
		m_pITableFrame->WriteLog(i, strLog);
		m_pITableFrame->WriteLog( i, TEXT( "玩家手牌: %s " ), m_GameLogic.GetCardListString( m_bUserCardData[ i ], MAX_COUNT ) );
	}
	
}

//数据事件
bool CTableFrameSink::OnDataBaseMessage(WORD wRequestID, void * pData, WORD wDataSize)
{
	return false;
}
//积分事件
bool CTableFrameSink::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	return true;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_SHOWCARD:				//玩家摊牌
		{
			if( m_pITableFrame->GetGameStatus( ) != GAME_STATUS_PLAY ) return true;
			return OnUserShowCard(pIServerUserItem->GetChairID() , pData, wDataSize) ;
		}
	case SUB_C_COMPLETE_COMPARE:	//完成比较
		{
			if( m_pITableFrame->GetGameStatus( ) != GAME_STATUS_PLAY ) return true;
			return OnUserCompleteCompare(pIServerUserItem->GetChairID());
		}
	case SUB_C_CUT_USER:
		{
			if( m_pITableFrame->GetGameStatus( ) != GAME_STATUS_FREE ) return true;	
			if (m_wCutUser == INVALID_CHAIR) m_wCutUser = pIServerUserItem->GetChairID();
			return true;
		}
	case SUB_C_CUT_CARD:
		{
			if( m_pITableFrame->GetGameStatus( ) != GAME_STATUS_PLAY )return true;
			CMD_C_CutCard *pCmd = (CMD_C_CutCard *)pData;
			if (wDataSize != sizeof(CMD_C_CutCard)) return false;

			return OnUserCutCard(pIServerUserItem->GetChairID(), pCmd->dValue);
		}
	case SUB_C_AUTO:
		{
			if( m_pITableFrame->GetGameStatus( ) != GAME_STATUS_PLAY ) return true;
			return OnUserAutoPut(pIServerUserItem->GetChairID());
		}
	case SUB_C_RESET:
		{
			if( m_pITableFrame->GetGameStatus() != GAME_STATUS_PLAY ) return true;
			return OnUserReSet( pIServerUserItem->GetChairID() );
		}	
	case SUB_C_WASH_CARD:
		{
			if (m_pITableFrame->GetGameStatus() != GAME_STATUS_FREE) return true;
			return OnUserWashCard(pIServerUserItem->GetChairID());
		}
	}
	return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//玩家摊牌
bool CTableFrameSink::OnUserShowCard(WORD wChairID , const void * pDataBuffer, WORD wDataSize)
{
	//数据验证 
	ASSERT(INVALID_CHAIR!=wChairID) ;
	if(INVALID_CHAIR==wChairID) return false ;

	ASSERT(sizeof(CMD_C_ShowCard)==wDataSize) ;
	if(sizeof(CMD_C_ShowCard)!=wDataSize) return false ;

	if (m_bFinishSegment[wChairID]) return true;

	CMD_C_ShowCard *pSetSeg = (CMD_C_ShowCard*) pDataBuffer ;
	
	//校验扑克
	BYTE bCardDataTemp[ MAX_COUNT ], bCardDataTemp2[ MAX_COUNT ];
	CopyMemory(FRONT_DATA(bCardDataTemp), pSetSeg->cbFrontCard, FRONT_COUNT);
	CopyMemory(MIDDLE_DATA(bCardDataTemp), pSetSeg->cbMidCard, MIDDLE_COUNT);
	CopyMemory(BACK_DATA(bCardDataTemp), pSetSeg->cbBackCard, BACK_COUNT);
	CopyMemory( bCardDataTemp2, m_bUserCardData[ wChairID ], MAX_COUNT );

	m_pITableFrame->WriteLog( wChairID, L"摊牌 头: %s", m_GameLogic.GetCardListString( pSetSeg->cbFrontCard, FRONT_COUNT ) );
	m_pITableFrame->WriteLog( wChairID, L"摊牌 中: %s", m_GameLogic.GetCardListString( pSetSeg->cbMidCard, MIDDLE_COUNT ) );
	m_pITableFrame->WriteLog( wChairID, L"摊牌 尾: %s", m_GameLogic.GetCardListString( pSetSeg->cbBackCard, BACK_COUNT ) );

	if( !m_GameLogic.RemoveCard( bCardDataTemp, MAX_COUNT, bCardDataTemp2, MAX_COUNT ) ) return false;
	if( !pSetSeg->bSpecial )
	{
		m_bSpecialType[ wChairID ] = CT_EX_INVALID;
		m_llCardWeight[ wChairID ][ 0 ] = m_GameLogic.GetCardWeight( pSetSeg->cbFrontCard, FRONT_COUNT, HaveRule( GAME_TYPE_COMPARECOLORFIRST ) );
		m_llCardWeight[ wChairID ][ 1 ] = m_GameLogic.GetCardWeight( pSetSeg->cbMidCard, MIDDLE_COUNT, HaveRule( GAME_TYPE_COMPARECOLORFIRST ) );
		m_llCardWeight[ wChairID ][ 2 ] = m_GameLogic.GetCardWeight( pSetSeg->cbBackCard, BACK_COUNT, HaveRule( GAME_TYPE_COMPARECOLORFIRST ) );

		if( m_llCardWeight[ wChairID ][ 1 ] > m_llCardWeight[ wChairID ][ 2 ] && OnCompare3And5( m_llCardWeight[ wChairID ][ 0 ], m_llCardWeight[ wChairID ][ 2 ] ) >= 0)
		{
			SCORE lTempWeight = m_llCardWeight[ wChairID ][ 1 ];
			m_llCardWeight[ wChairID ][ 1 ] = m_llCardWeight[ wChairID ][ 2 ];
			m_llCardWeight[ wChairID ][ 2 ] = lTempWeight;
			CopyMemory( BACK_DATA( bCardDataTemp ), pSetSeg->cbMidCard, MIDDLE_COUNT );
			CopyMemory( MIDDLE_DATA( bCardDataTemp ), pSetSeg->cbBackCard, BACK_COUNT );
			CopyMemory( pSetSeg->cbMidCard, MIDDLE_DATA( bCardDataTemp ), MIDDLE_COUNT );
			CopyMemory( pSetSeg->cbBackCard, BACK_DATA( bCardDataTemp ), BACK_COUNT );
		}

		if( OnCompare3And5( m_llCardWeight[ wChairID ][ 0 ], m_llCardWeight[ wChairID ][ 1 ] ) < 0 || m_llCardWeight[ wChairID ][ 1 ] > m_llCardWeight[ wChairID ][ 2 ] )//|| m_llCardWeight[ wChairID ][ 1 ] > m_llCardWeight[ wChairID ][ 2 ] 
		{
			CMD_S_ReSet ReSet = { 0 };
			m_pITableFrame->SendLookonData( wChairID, SUB_S_RE_SET, &ReSet, sizeof( ReSet ) );
			CopyMemory( ReSet.bCardData, m_bUserCardData[ wChairID ], sizeof( BYTE ) *MAX_COUNT );
			m_pITableFrame->SendTableData( wChairID, SUB_S_RE_SET, &ReSet, sizeof( ReSet ) );
			return true;
		}
	}

	//设置数据
	CopyMemory(m_bSegmentCard[wChairID], bCardDataTemp,  MAX_COUNT);
	m_bFinishSegment[wChairID] = true;
	m_wShowCardCount++;
	
	//通知开牌
	CMD_S_ShowCard  ShowCard ;
	ZeroMemory(&ShowCard, sizeof(ShowCard));
	ShowCard.wCurrentUser = wChairID;
	m_pITableFrame->SendLookonData( INVALID_CHAIR, SUB_S_SHOW_CARD, &ShowCard, sizeof( ShowCard ) );
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if ((pIServerUserItem == NULL) )continue;


		if( i == wChairID && m_bPlayStatus[ i ] )
		{
			CopyMemory(ShowCard.bFrontCard, pSetSeg->cbFrontCard, FRONT_COUNT);
			CopyMemory(ShowCard.bMidCard, pSetSeg->cbMidCard, MIDDLE_COUNT);
			CopyMemory(ShowCard.bBackCard, pSetSeg->cbBackCard, BACK_COUNT);
		}
		else
		{
			ZeroMemory(ShowCard.bFrontCard, FRONT_COUNT);
			ZeroMemory(ShowCard.bMidCard, MIDDLE_COUNT);
			ZeroMemory(ShowCard.bBackCard, BACK_COUNT);
		}

		m_pITableFrame->SendTableData(i,SUB_S_SHOW_CARD,&ShowCard,sizeof(ShowCard));
	}

	//开始比牌
	if( GetActivePlayerCount( ) == m_wShowCardCount ) StartCompareCard( );
		
	return true;
}

bool CTableFrameSink::OnUserCompleteCompare(WORD wChairID)
{
	if( !m_bComparing || !m_bPlayStatus[ wChairID ] || m_bFinishCompare[ wChairID ] ) return true;
		
	m_bFinishCompare[wChairID] = true;

	//if (!m_pITableFrame->GetTableUserItem(wChairID)->IsAndroidUser())
	{
		m_bComparing = false;
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}

	return true;
}

void CTableFrameSink::SendCutUser()
{
	if (m_wCutUser == INVALID_CHAIR) return;
	CMD_S_CutUser Cmd = { 0 };
	Cmd.wChairID = m_wCutUser;
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CUT_USER, &Cmd, sizeof(Cmd));
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CUT_USER, &Cmd, sizeof(Cmd));
}


bool CTableFrameSink::OnUserCutCard(WORD wChair, double dvalue)
{
	ASSERT(wChair == m_wCutUser);
	CMD_S_CutCard Cmd = { 0 };
	Cmd.wChairID = wChair;
	Cmd.dValue = dvalue;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CUT_CARD, &Cmd, sizeof(Cmd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CUT_CARD, &Cmd, sizeof(Cmd));
	m_wCutUser = INVALID_CHAIR;
	return OnEventGameStart();
}

bool CTableFrameSink::OnUserAutoPut(WORD wChair)
{
	BYTE cbPutCard[MAX_COUNT] = { 0 };
	m_GameLogic.AutoSortCard(m_bUserCardData[wChair], cbPutCard);
	CMD_S_AutoPut Cmd = { 0 };
	CopyMemory(Cmd.bSegmentCard, cbPutCard, MAX_COUNT);
	m_pITableFrame->SendTableData(wChair, SUB_S_AUTO_PUT, &Cmd, sizeof(Cmd));
	m_pITableFrame->SendLookonData(wChair, SUB_S_AUTO_PUT, &Cmd, sizeof(Cmd));
	return true;
}


bool CTableFrameSink::OnUserReSet( WORD wChairID )
{
	//数据验证 
	if( INVALID_CHAIR == wChairID ) return true;
	if( !m_bFinishSegment[ wChairID ] ) return true;
	if( m_bComparing ) return true;
	m_pITableFrame->WriteLogEx(wChairID, "重摆！！！");
	m_bFinishSegment[ wChairID ] = false;
	m_wShowCardCount--;
	m_bSpecialType[ wChairID ] = m_GameLogic.GetCardSpecialType( m_bUserCardData[ wChairID ], MAX_COUNT, false );;
	CMD_S_ReSet ReSet = {0};
	ReSet.cbSpecialType = m_bSpecialType[ wChairID ];
	m_pITableFrame->SendLookonData( wChairID, SUB_S_RE_SET, &ReSet, sizeof( ReSet ) );
	CopyMemory( ReSet.bCardData, m_bUserCardData[ wChairID ], sizeof( BYTE ) *MAX_COUNT );
	m_pITableFrame->SendTableData( wChairID, SUB_S_RE_SET, &ReSet, sizeof( ReSet ) );
	
	return true;
}

bool CTableFrameSink::OnUserWashCard(WORD wChairID)
{
	//数据验证 
	if (INVALID_CHAIR == wChairID) return true;
	m_pITableFrame->SetUserAutoOperate(INVALID_CHAIR, 0);
	CMD_S_WashCard Cmd = { 0 };
	m_cbWashCount++;
	Cmd.cbWash = 1;
	Cmd.wWashUser = wChairID;
	Cmd.cbWashCount = m_cbWashCount;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_WASH_CARD, &Cmd, sizeof(Cmd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_WASH_CARD, &Cmd, sizeof(Cmd));
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem && m_cbTimeStartGame) m_pITableFrame->SetUserAutoOperate(i, m_cbTimeStartGame-2);
	}

	return true;
}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	ASSERT(wChairID!=INVALID_CHAIR);
	if( bLookonUser ) return true;
	if( wChairID == m_wBanker ) m_wBanker = INVALID_CHAIR;
	return true;
}

bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	return true;
}

//游戏人数
WORD CTableFrameSink::GetActivePlayerCount()
{
	//计算人数
	WORD wPlayerCount = 0;
	for(WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_bPlayStatus[i])
			wPlayerCount++;
	}

	return wPlayerCount;
}

//开始比牌
void CTableFrameSink::StartCompareCard()
{
	CompareCard();

	CMD_S_CompareCard Cmd;
	ZeroMemory (&Cmd, sizeof(Cmd));

	Cmd.wBankUser = m_wBanker;
	CopyMemory(Cmd.bSegmentCard, m_bSegmentCard, sizeof(m_bSegmentCard));
	CopyMemory(Cmd.llScoreTimes, m_lScoreTimes, sizeof(m_lScoreTimes));
	CopyMemory(Cmd.bSpecialType, m_bSpecialType, sizeof(m_bSpecialType));
	CopyMemory(Cmd.llFinalScore, m_lFinalScore, sizeof(m_lFinalScore));
	CopyMemory(Cmd.bGun, m_bGun, sizeof(m_bGun));
	m_bComparing = true;

	memcpy(&m_cmdCompareCard, &Cmd, sizeof(Cmd));

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_COMPARE_CARD , &Cmd , sizeof(Cmd));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_COMPARE_CARD , &Cmd , sizeof(Cmd)); 

	if( !m_bComparing ) OnEventGameConclude( INVALID_CHAIR, NULL, GER_NORMAL );
}


//获取倍数
void CTableFrameSink::CompareCard()
{
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		if( !m_bPlayStatus[ i ] ) continue;
		for( WORD j = i + 1; j < GAME_PLAYER; j++ )
		{
			if( !m_bPlayStatus[ j ] ) continue;
			CompareCardGetTimes( i, j );
		}
	}

	//为了方便下列函数对马牌加倍显示在每道上
	ZeroMemory(m_lScoreTimes, sizeof(m_lScoreTimes));

	m_wRedWeaveUser = FindRedWeaveUser();
	CalculateFinalScore();
}

void CTableFrameSink::CompareCardGetTimes(WORD i, WORD j)
{
	if (m_bSpecialType[i] != CT_EX_INVALID || m_bSpecialType[j] != CT_EX_INVALID)
	{
		int nRes = CompareSpecial(i, j);
		SCORE llCur = m_lCellScore * m_byDoubleTimes * m_dwPlatfromRatio;

		if( nRes > 0 ) llCur *= m_GameLogic.GetSpecialWeights( m_bSpecialType[ i ] );
		else if( nRes < 0 ) llCur *= -m_GameLogic.GetSpecialWeights( m_bSpecialType[ j ] );
		else llCur = 0;

		m_lFinalScoreP2P[i][j] += llCur;
		m_lFinalScoreP2P[j][i] -= llCur;
		return;
	}

	//检查胜负	
	SCORE nWindInfo[ 3 ] = { m_llCardWeight[ i ][ 0 ] - m_llCardWeight[ j ][ 0 ], m_llCardWeight[ i ][ 1 ] - m_llCardWeight[ j ][ 1 ], m_llCardWeight[ i ][ 2 ] - m_llCardWeight[ j ][ 2 ] };
	//打枪判断
	m_bGun[ i ][ j ] = ( nWindInfo[ 0 ] > 0 ) && ( nWindInfo[ 1 ] > 0 ) && ( nWindInfo[ 2 ] > 0 );
	m_bGun[ j ][ i ] = ( nWindInfo[ 0 ] < 0 ) && ( nWindInfo[ 1 ] < 0 ) && ( nWindInfo[ 2 ] < 0 );

	//判断输赢
	for (BYTE bIdx = 0; bIdx < 3; bIdx++)
	{
		WORD wWinUser = nWindInfo[bIdx] > 0 ? i : j;
		SCORE nMultiple = 0;
		if( nWindInfo[ bIdx ] > 0 ) nMultiple = m_lCellScore * m_dwPlatfromRatio;
		else if( nWindInfo[ bIdx ] < 0 ) nMultiple = -m_lCellScore * m_dwPlatfromRatio;

		BYTE bSegType = m_GameLogic.GetTypeByWeight( m_llCardWeight[ wWinUser ][ bIdx ] );

		//第一注
		if (bIdx == 0)
		{
			nMultiple *= GetHeadTimes( wWinUser );
		}
		//第二注 中道
		if (bIdx == 1)
		{
			if (bSegType == CT_FIVE_COLOR)  nMultiple *= 2;
			if (bSegType == CT_THREE_TWO)  nMultiple *= 3;
			if( bSegType == CT_FOUR_SAME )  nMultiple *= 8;
			if( bSegType == CT_FIVE_COLOR_LINE ) nMultiple *= 10;
			if( bSegType == CT_FIVE_SAME ) nMultiple *= 20;
			if( bSegType == CT_FOUR_KING ) nMultiple *= 20;
			if( bSegType == CT_FIVE_KING) nMultiple *= 30;
		}
		//第三注 尾道
		if (bIdx == 2)
		{
			if( bSegType == CT_FOUR_SAME ) nMultiple *= 4;
			if( bSegType == CT_FIVE_COLOR_LINE ) nMultiple *= 5;
			if( bSegType == CT_FIVE_SAME ) nMultiple *= 10;
			if( bSegType == CT_FOUR_KING ) nMultiple *= 20;
			if( bSegType == CT_FIVE_KING ) nMultiple *= 30;
		}

		m_lScoreTimes[i][bIdx] += nMultiple;
		m_lScoreTimes[j][bIdx] -= nMultiple;

		m_llBaseScore[i][j][bIdx] += nMultiple;
		m_llBaseScore[j][i][bIdx] -= nMultiple;

		//玩家对玩家总分
		m_lFinalScoreP2P[i][j] += nMultiple;
		m_lFinalScoreP2P[j][i] -= nMultiple;
	}
}

WORD CTableFrameSink::FindRedWeaveUser()
{
	//if( !HaveRule( GAME_TYPE_ENABLEREDWEAVE ) ) return INVALID_CHAIR;
	WORD wUserCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if( m_bPlayStatus[ i ] ) wUserCount++;	
	}

	if( wUserCount < 4 )return INVALID_CHAIR;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if( !m_bPlayStatus[ i ] ) continue;
		WORD wGun = 0;
		for (WORD j = 0; j < GAME_PLAYER; j++)
		{
			if( m_bGun[ i ][ j ] )wGun++;
		}
		if( wGun + 1 == wUserCount ) return i;
	}

	return INVALID_CHAIR;
}

//玩家是否有马牌
BYTE CTableFrameSink::MaPaiTimes( WORD wChairID )
{
	BYTE bMaPai = GetMaPai();
	BYTE byTimes = 1;
	for (BYTE i = 0; i < MAX_COUNT; i++)
	{
		if( m_bSegmentCard[ wChairID ][ i ] == bMaPai ) byTimes *= 2;
	}
	return byTimes;
}

//马牌
BYTE CTableFrameSink::GetMaPai()
{
	if( HaveRule( GAME_TYPE_MAPAI_1 ) ) return 0x21;
	if( HaveRule( GAME_TYPE_MAPAI_2 ) ) return 0x25;
	if( HaveRule( GAME_TYPE_MAPAI_3 ) ) return 0x2A;
	return 0;
}

void CTableFrameSink::CalculateFinalScore()
{
	//打枪和红波浪翻倍
	BYTE byTimes = 2, byTimes2 = 1;
	if (m_wRedWeaveUser != INVALID_CHAIR) byTimes2 *= 2;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bPlayStatus[i]) continue;
		for (WORD j = 0; j < GAME_PLAYER; j++)
		{
			if (j == i || !m_bPlayStatus[j]) continue;

			if (m_bGun[i][j])
			{
				m_lFinalScoreP2P[i][j] *= byTimes;
				m_lFinalScoreP2P[j][i] *= byTimes;
			}
			if (i == m_wRedWeaveUser)
			{
				m_lFinalScoreP2P[i][j] *= byTimes2;
				m_lFinalScoreP2P[j][i] *= byTimes2;
			}
		}
	}
	//马牌翻倍
	if (GetMaPai() != 0)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if( !m_bPlayStatus[ i ] )continue;
			BYTE byTimes = MaPaiTimes( i );
				
			for (WORD j = 0; j < GAME_PLAYER; j++)
			{
				if( j == i || !m_bPlayStatus[ j ] )continue;
				m_lFinalScoreP2P[ i ][ j ] *= byTimes;
				m_lFinalScoreP2P[ j ][ i ] *= byTimes;
				////////////////////////////////////////////////////////////////////////////
				//新需求为 如果有马牌：每道的分数显示马牌翻倍后的分数
				for (BYTE bIdx = 0; bIdx < 3; bIdx++)
				{
					m_llBaseScore[ i ][ j ][ bIdx ] *= byTimes;
					m_llBaseScore[ j ][ i ][ bIdx ] *= byTimes;
				}
				////////////////////////////////////////////////////////////////////////////
			}
		}
	}

	//计算总分
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bPlayStatus[i])continue;

		for (WORD j = 0; j < GAME_PLAYER; j++)
		{
			if (j == i || !m_bPlayStatus[j])continue;
			m_lFinalScore[i] += m_lFinalScoreP2P[i][j];

			////////////////////////////////////////////////////////////////////////////
			//马牌翻倍后每道显示的分数
			for (BYTE bIdx = 0; bIdx < 3; bIdx++)
			{
				m_lScoreTimes[i][bIdx] += m_llBaseScore[i][j][bIdx];
			}
		}
	}
	//大联盟不可以负分
	if ((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD | GAME_GENRE_PERSONAL_S | GAME_GENRE_PERSONAL_G)) > 0)
	{
		if (m_pITableFrame->GetMinusMark() == false)//true是可以负分
		{
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (!m_bPlayStatus[i])continue;
				if (m_pITableFrame->GetTableUserItem(i))
				{
					if (m_pITableFrame->GetTableUserItem(i)->GetUserScore() + m_lFinalScore[i]<0)//分不够了
					{
						//加上赢的分
						SCORE UserScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();
						for (WORD j = 0; j < GAME_PLAYER; j++)
						{
							if (j == i || !m_bPlayStatus[j])continue;
							if (m_lFinalScoreP2P[i][j] <= 0)continue;
							UserScore += m_lFinalScoreP2P[i][j];
						}
						//排序
						WORD Index[GAME_PLAYER] = { 0, 1, 2, 3 };//存储ID
						//j和z是下标
						for (int j = 0; j < GAME_PLAYER; j++)
						{
							for (int z = j + 1; z < GAME_PLAYER; z++)
							{
								if (m_lFinalScoreP2P[Index[z]][i] > m_lFinalScoreP2P[Index[j]][i])
								{
									int temp = Index[j];
									Index[j] = Index[z];
									Index[z] = temp;
								}
							}
						}
						//开始扣分
						for (int j = 0; j < GAME_PLAYER; j++)
						{
							if (!m_bPlayStatus[Index[j]] || Index[j] == i)continue;
							if (m_lFinalScoreP2P[Index[j]][i] <= 0)continue;
							if (UserScore - m_lFinalScoreP2P[Index[j]][i] >= 0)
							{
								UserScore -= m_lFinalScoreP2P[Index[j]][i];
							}
							else
							{
								m_lFinalScore[Index[j]] -= m_lFinalScoreP2P[Index[j]][i] - UserScore;//拿出多余的分
								//剩下的人全部拿出
								for (int z = j + 1; z < GAME_PLAYER; z++)
								{
									if (!m_bPlayStatus[Index[z]] || Index[z] == i)continue;
									if (m_lFinalScoreP2P[Index[z]][i] <= 0)continue;
									m_lFinalScore[Index[z]] -= m_lFinalScoreP2P[Index[z]][i];
								}

								break;
							}
						}
						m_lFinalScore[i] = -m_pITableFrame->GetTableUserItem(i)->GetUserScore();
					}
				}
			}
		}
	}
}

int CTableFrameSink::CompareSpecial(WORD wFirst, WORD wNext)
{
	return m_byDoubleTimes*( m_GameLogic.GetSpecialWeights( m_bSpecialType[ wFirst ] ) - m_GameLogic.GetSpecialWeights( m_bSpecialType[ wNext ] ) );
}

//////////////////////////////////////////////////////////////////////////

