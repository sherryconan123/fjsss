#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
	DWORD								m_dwPlatfromRatio;						//平台倍率
	SCORE                               m_lCellScore;                                      //底分
	WORD								m_wMaxPlayer;										//最大人数
	WORD								m_wBanker;											//庄家玩家

	BYTE								m_cbRandCard[ RAND_COUNT ];
	BYTE								m_cbRandCount;
	BYTE								m_bUserCardData[ GAME_PLAYER ][ MAX_COUNT ];	    	//玩家扑克
	BYTE								m_bSegmentCard[ GAME_PLAYER ][ MAX_COUNT ];		        //分段扑克
	SCORE								m_llCardWeight[ GAME_PLAYER ][ 3 ];					//扑克权重
	BYTE								m_bSpecialType[ GAME_PLAYER ];						//特殊牌型

	SCORE								m_lScoreTimes[ GAME_PLAYER ][ 3 ];						//输赢注数
	SCORE								m_llBaseScore[ GAME_PLAYER ][ GAME_PLAYER ][ 3 ];         //临时每道注数
	SCORE								m_lFinalScore[ GAME_PLAYER ];							//最终得分
	SCORE								m_lFinalScoreP2P[ GAME_PLAYER ][ GAME_PLAYER ];			//最终得分

	bool								m_bGun[ GAME_PLAYER ][ GAME_PLAYER ];					//是否打枪
	WORD								m_wRedWeaveUser;									//红波浪玩家


	WORD								m_wCutUser;
	BYTE								m_cbWashCount;									//洗牌次数

	WORD								m_wShowCardCount;									//摊牌计数
	bool								m_bComparing;										//比牌状态

	BYTE								m_bPlayStatus[ GAME_PLAYER ];							//游戏状态
	bool								m_bFinishSegment[ GAME_PLAYER ];						//完成分段
	bool								m_bFinishCompare[ GAME_PLAYER ];						//完成比牌
	CMD_S_CompareCard					m_cmdCompareCard;

	//房卡相关
public:
	DWORD								m_dwCustomRules[ 5 ];						//定制规则
	DWORD								m_dwServerRules;							//定制规则
	BYTE								m_byDoubleTimes;
	BYTE								m_cbTimeStartGame;
	WORD								m_wWaitTime;										//等待时间
	SCORE								m_llTotalScore[ GAME_PLAYER ];
	BYTE								m_cbWriteScoreStatus[GAME_PLAYER];			//写分状态

	//组件变量
protected:
	CGameLogic							m_GameLogic;										//游戏逻辑
	ITableFrame							*m_pITableFrame;									//框架接口
	const tagGameServiceOption			*m_pGameServiceOption;								//配置参数

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface( REFGUID Guid, DWORD dwQueryVer );

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization( IUnknownEx * pIUnknownEx );

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota( IServerUserItem * pIServerUserItem ) { return 0; };
	//最少积分
	virtual SCORE QueryLessEnterScore( WORD wChairID, IServerUserItem * pIServerUserItem ) { return 0; };
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge( WORD wChairID ){ return true; }

	//消息处理
protected:
	//玩家摊牌
	bool OnUserShowCard( WORD wChairID, const void * pDataBuffer, WORD wDataSize );
	//玩家比牌完成
	bool OnUserCompleteCompare( WORD wChairID );

	bool OnUserCutCard( WORD wChair, double dvalue );
	bool OnUserAutoPut( WORD wChair );
	bool OnUserReSet( WORD wChairID );
	bool OnUserWashCard(WORD wChairID);
	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	void SendCutUser();

	//游戏结束
	virtual bool OnEventGameConclude( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason );
	//发送场景
	virtual bool OnEventSendGameScene( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret );

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage( DWORD wTimerID, WPARAM wBindParam );
	//数据事件
	virtual bool OnDataBaseMessage( WORD wRequestID, VOID * pData, WORD wDataSize );
	//积分事件
	virtual bool OnUserScroeNotify( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason );

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage( WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	//框架消息
	virtual bool OnFrameMessage( WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );

	//////////////////////////////////////////////////////////////////////////
	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem ) { return true; };
	//用户重入
	virtual bool OnActionUserConnect( WORD wChairID, IServerUserItem * pIServerUserItem ){ return true; };
	//用户坐下
	virtual bool OnActionUserSitDown( WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser );
	//用户起立
	virtual bool OnActionUserStandUp( WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser );
	//用户同意
	virtual bool OnActionUserOnReady( WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize );
	//设置基数
	virtual void SetGameBaseScore( LONG lBaseScore ){};

protected:
	//统计人数
	WORD GetActivePlayerCount();
	//获取倍数
	void CompareCard();
	//获取倍数
	void CompareCardGetTimes( WORD i, WORD j );
	//开始比牌
	void StartCompareCard();
	//发牌
	void RandCardList();
	WORD FindRedWeaveUser();
	void CalculateFinalScore();
	int CompareSpecial( WORD wFirst, WORD wNext );
	//玩家是否有马牌
	BYTE MaPaiTimes( WORD wChairID );
	//马牌
	BYTE GetMaPai();
	void OnTimerCheckTable();
	BYTE GetHeadTimes( WORD wChair );
	//准备
	SCORE OnCompare3And5( SCORE lWeight1, SCORE lWeight2 );

public:
	//设置规则
	virtual bool SetRoomRulesEx( DWORD  dwServerRules, DWORD dwRules[ 5 ] );
	//获取规则
	virtual	WORD GetRoomGameCount();
	//获取玩家数量
	virtual	BYTE GetPlayerCount(){ return (BYTE)m_wMaxPlayer; };
	//获取总结算
	virtual	WORD GetRoomEndData( VOID * pData );
	//
	bool HaveRule( DWORD dwRule );
	//房间规则
	void InitRoomRules();
	//规则文字
	CString GetRulesStringEx( DWORD dwServerRules, DWORD dwRules[ 5 ] );
	//坐下限制
	SCORE GetSitScore();
	//自动操作
	void OnUserAutoOperate( WORD wChairID );
	//允许旁观
	virtual BYTE LookOnMode(){ return 2; };

};

//////////////////////////////////////////////////////////////////////////

#endif
