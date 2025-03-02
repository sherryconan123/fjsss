#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//��Ϸ����
	DWORD								m_dwPlatfromRatio;						//ƽ̨����
	SCORE                               m_lCellScore;                                      //�׷�
	WORD								m_wMaxPlayer;										//�������
	WORD								m_wBanker;											//ׯ�����

	BYTE								m_cbRandCard[ RAND_COUNT ];
	BYTE								m_cbRandCount;
	BYTE								m_bUserCardData[ GAME_PLAYER ][ MAX_COUNT ];	    	//����˿�
	BYTE								m_bSegmentCard[ GAME_PLAYER ][ MAX_COUNT ];		        //�ֶ��˿�
	SCORE								m_llCardWeight[ GAME_PLAYER ][ 3 ];					//�˿�Ȩ��
	BYTE								m_bSpecialType[ GAME_PLAYER ];						//��������

	SCORE								m_lScoreTimes[ GAME_PLAYER ][ 3 ];						//��Ӯע��
	SCORE								m_llBaseScore[ GAME_PLAYER ][ GAME_PLAYER ][ 3 ];         //��ʱÿ��ע��
	SCORE								m_lFinalScore[ GAME_PLAYER ];							//���յ÷�
	SCORE								m_lFinalScoreP2P[ GAME_PLAYER ][ GAME_PLAYER ];			//���յ÷�

	bool								m_bGun[ GAME_PLAYER ][ GAME_PLAYER ];					//�Ƿ��ǹ
	WORD								m_wRedWeaveUser;									//�첨�����


	WORD								m_wCutUser;
	BYTE								m_cbWashCount;									//ϴ�ƴ���

	WORD								m_wShowCardCount;									//̯�Ƽ���
	bool								m_bComparing;										//����״̬

	BYTE								m_bPlayStatus[ GAME_PLAYER ];							//��Ϸ״̬
	bool								m_bFinishSegment[ GAME_PLAYER ];						//��ɷֶ�
	bool								m_bFinishCompare[ GAME_PLAYER ];						//��ɱ���
	CMD_S_CompareCard					m_cmdCompareCard;

	//�������
public:
	DWORD								m_dwCustomRules[ 5 ];						//���ƹ���
	DWORD								m_dwServerRules;							//���ƹ���
	BYTE								m_byDoubleTimes;
	BYTE								m_cbTimeStartGame;
	WORD								m_wWaitTime;										//�ȴ�ʱ��
	SCORE								m_llTotalScore[ GAME_PLAYER ];
	BYTE								m_cbWriteScoreStatus[GAME_PLAYER];			//д��״̬

	//�������
protected:
	CGameLogic							m_GameLogic;										//��Ϸ�߼�
	ITableFrame							*m_pITableFrame;									//��ܽӿ�
	const tagGameServiceOption			*m_pGameServiceOption;								//���ò���

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface( REFGUID Guid, DWORD dwQueryVer );

	//����ӿ�
public:
	//��λ����
	virtual VOID RepositionSink();
	//��������
	virtual bool Initialization( IUnknownEx * pIUnknownEx );

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota( IServerUserItem * pIServerUserItem ) { return 0; };
	//���ٻ���
	virtual SCORE QueryLessEnterScore( WORD wChairID, IServerUserItem * pIServerUserItem ) { return 0; };
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge( WORD wChairID ){ return true; }

	//��Ϣ����
protected:
	//���̯��
	bool OnUserShowCard( WORD wChairID, const void * pDataBuffer, WORD wDataSize );
	//��ұ������
	bool OnUserCompleteCompare( WORD wChairID );

	bool OnUserCutCard( WORD wChair, double dvalue );
	bool OnUserAutoPut( WORD wChair );
	bool OnUserReSet( WORD wChairID );
	bool OnUserWashCard(WORD wChairID);
	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	void SendCutUser();

	//��Ϸ����
	virtual bool OnEventGameConclude( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason );
	//���ͳ���
	virtual bool OnEventSendGameScene( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret );

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage( DWORD wTimerID, WPARAM wBindParam );
	//�����¼�
	virtual bool OnDataBaseMessage( WORD wRequestID, VOID * pData, WORD wDataSize );
	//�����¼�
	virtual bool OnUserScroeNotify( WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason );

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage( WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );
	//�����Ϣ
	virtual bool OnFrameMessage( WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem );

	//////////////////////////////////////////////////////////////////////////
	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem ) { return true; };
	//�û�����
	virtual bool OnActionUserConnect( WORD wChairID, IServerUserItem * pIServerUserItem ){ return true; };
	//�û�����
	virtual bool OnActionUserSitDown( WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser );
	//�û�����
	virtual bool OnActionUserStandUp( WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser );
	//�û�ͬ��
	virtual bool OnActionUserOnReady( WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize );
	//���û���
	virtual void SetGameBaseScore( LONG lBaseScore ){};

protected:
	//ͳ������
	WORD GetActivePlayerCount();
	//��ȡ����
	void CompareCard();
	//��ȡ����
	void CompareCardGetTimes( WORD i, WORD j );
	//��ʼ����
	void StartCompareCard();
	//����
	void RandCardList();
	WORD FindRedWeaveUser();
	void CalculateFinalScore();
	int CompareSpecial( WORD wFirst, WORD wNext );
	//����Ƿ�������
	BYTE MaPaiTimes( WORD wChairID );
	//����
	BYTE GetMaPai();
	void OnTimerCheckTable();
	BYTE GetHeadTimes( WORD wChair );
	//׼��
	SCORE OnCompare3And5( SCORE lWeight1, SCORE lWeight2 );

public:
	//���ù���
	virtual bool SetRoomRulesEx( DWORD  dwServerRules, DWORD dwRules[ 5 ] );
	//��ȡ����
	virtual	WORD GetRoomGameCount();
	//��ȡ�������
	virtual	BYTE GetPlayerCount(){ return (BYTE)m_wMaxPlayer; };
	//��ȡ�ܽ���
	virtual	WORD GetRoomEndData( VOID * pData );
	//
	bool HaveRule( DWORD dwRule );
	//�������
	void InitRoomRules();
	//��������
	CString GetRulesStringEx( DWORD dwServerRules, DWORD dwRules[ 5 ] );
	//��������
	SCORE GetSitScore();
	//�Զ�����
	void OnUserAutoOperate( WORD wChairID );
	//�����Թ�
	virtual BYTE LookOnMode(){ return 2; };

};

//////////////////////////////////////////////////////////////////////////

#endif
