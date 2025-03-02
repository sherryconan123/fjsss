#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_LESS_1						1									//����ʱ��
#define TIME_LESS_2						2									//����ʱ��

//��Ϸʱ��
#define TIME_START						5									//��ׯʱ��
#define TIME_CALL						2
#define TIME_SHOW_CARD					10									//����ʱ��
#define TIME_COMPLETE_COMPARE			10									//����ʱ��

//��Ϸʱ��
#define IDI_GAME_READY					(101)								//׼����ʱ��
#define IDI_SHOW_CARD					(102)								//���ƶ�ʱ��
#define IDI_COMPLETE_COMPARE			(103)								//���ƶ�ʱ��

#define GAME_STATE_FREE					1									//��Ϸ����
#define GAME_STATE_GAME					2									//��Ϸ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//�ӿڱ���
	m_pIAndroidUserItem=NULL;
	srand((unsigned)time(NULL));   

	m_cbPlayStatus = GAME_STATE_FREE;
	return;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	try
	{
		switch (nTimerID)
		{
			case IDI_GAME_READY:		//��ʼ��ʱ
			{
				//����׼��
				m_pIAndroidUserItem->SendUserReady(NULL, 0);
				return true;
			}
			case IDI_SHOW_CARD:
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_AUTO);
				return true;
			}
			case IDI_COMPLETE_COMPARE:
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_COMPLETE_COMPARE);
			}
		}

	}
	catch (...)
	{
		CString cs;
		cs.Format(TEXT("�쳣ID=%d"), nTimerID);
		CTraceService::TraceString(cs, TraceLevel_Debug);
	}

	return false;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_S_GAME_START:		//��Ϸ��ʼ
			{
				//��Ϣ����
				return OnSubGameStart(pData, wDataSize);
			}
		case SUB_S_SHOW_CARD:		//���̯��
			{
				//��Ϣ����
				return OnSubShowCard(pData, wDataSize);
			}
		case SUB_S_COMPARE_CARD:	//�Ƚ��˿�
			{
				//��Ϣ����
				return OnSubComPareCard(pData, wDataSize);
			}
		case SUB_S_GAME_END:		//��Ϸ����
			{
				//��Ϣ����
				return OnSubGameConclude(pData, wDataSize);
			}
		case SUB_S_CUT_USER:	//
			{
				//��Ϣ����
				return OnSubCutUser(pData, wDataSize);
			}
		case SUB_S_CUT_CARD:
			{
				//��Ϣ����
				return OnSubCutCard(pData, wDataSize);
			}
		case SUB_S_AUTO_PUT:
			{
				 //��Ϣ����
				return OnSubAutoPut(pData, wDataSize);
			}
		case SUB_S_RE_SET:
			{
				//��Ϣ����
				return OnSubReSet(pData, wDataSize);
			}
		case SUB_S_WASH_CARD:
			{
				//��Ϣ����
				return OnSubWashCard(pData, wDataSize);
			}
		//case SUB_S_CALL_TIMES:
		//	{
		//		//��Ϣ����
		//		return OnSubCallPlayer(pData, wDataSize);
		//	}
		//case SUB_S_SEARCH:
		//	{
		//		//��Ϣ����
		//		return OnSubSearchResult(pData, wDataSize);
		//	}
	}

	//�������
	//ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
		case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

			m_cbPlayStatus = GAME_STATE_FREE;
			OnSubUserReady(NULL, 0);
			return true;
		}
		case GAME_STATUS_WAIT:
		{
			m_cbPlayStatus = GAME_STATE_GAME;
			return true;
		}
		case GAME_STATUS_PLAY:
		{
			m_cbPlayStatus = GAME_STATE_GAME;
			return true;
		}
	}

	//ASSERT(FALSE);

	return true;
}

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	CMD_S_GameStart *pCmd = (CMD_S_GameStart *)pBuffer;
	if (wDataSize != sizeof(CMD_S_GameStart)) return false;

	m_cbPlayStatus = GAME_STATE_GAME;

	UINT nElapse = TIME_LESS_2 + (rand() % TIME_SHOW_CARD);
	if (m_cbPlayStatus == GAME_STATE_GAME)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_CARD, nElapse);
	}
	return true;
}
bool CAndroidUserItemSink::OnSubShowCard(const void * pBuffer, WORD wDataSize)
{
	CMD_S_ShowCard *pCmd = (CMD_S_ShowCard *)pBuffer;
	if (wDataSize != sizeof(CMD_S_ShowCard)) return false;

	return true;
}
bool CAndroidUserItemSink::OnSubComPareCard(const void * pBuffer, WORD wDataSize)
{
	CMD_S_CompareCard *pCmd = (CMD_S_CompareCard *)pBuffer;
	if (wDataSize != sizeof(CMD_S_CompareCard)) return false;

	if (m_cbPlayStatus == GAME_STATE_GAME)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_COMPLETE_COMPARE, TIME_COMPLETE_COMPARE);
	}
	return true;
}
bool CAndroidUserItemSink::OnSubGameConclude(const void * pBuffer, WORD wDataSize)
{
	CMD_S_GameEnd *pCmd = (CMD_S_GameEnd *)pBuffer;
	if (wDataSize != sizeof(CMD_S_GameEnd)) return false;

	m_cbPlayStatus = GAME_STATE_FREE;
	OnSubUserReady(NULL, 0);
	return true;
}
bool CAndroidUserItemSink::OnSubUserReady(const void * pBuffer, WORD wDataSize)
{
	UINT nElapse = TIME_LESS_2 + (rand() % TIME_START);

	if (m_cbPlayStatus == GAME_STATE_FREE)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_GAME_READY, nElapse);
	}

	return true;
}
bool CAndroidUserItemSink::OnSubCutUser(const void * pBuffer, WORD wDataSize)
{
	CMD_S_CutUser *pCmd = (CMD_S_CutUser *)pBuffer;
	if (wDataSize != sizeof(CMD_S_CutUser)) return false;

	return true;
}
bool CAndroidUserItemSink::OnSubCutCard(const void * pBuffer, WORD wDataSize)
{
	CMD_S_CutCard *pCmd = (CMD_S_CutCard *)pBuffer;
	if (wDataSize != sizeof(CMD_S_CutCard)) return false;

	return true;
}
bool CAndroidUserItemSink::OnSubAutoPut(const void * pBuffer, WORD wDataSize)
{
	CMD_S_AutoPut *pCmd = (CMD_S_AutoPut *)pBuffer;
	if (wDataSize != sizeof(CMD_S_AutoPut)) return false;

	CMD_C_ShowCard pShowCard = {0};
	CopyMemory(pShowCard.cbFrontCard, FRONT_DATA(pCmd->bSegmentCard), FRONT_COUNT);
	CopyMemory(pShowCard.cbMidCard, MIDDLE_DATA(pCmd->bSegmentCard), MIDDLE_COUNT);
	CopyMemory(pShowCard.cbBackCard, BACK_DATA(pCmd->bSegmentCard), BACK_COUNT);
	pShowCard.bSpecial = false;
		
	//������Ϣ
	m_pIAndroidUserItem->SendSocketData(SUB_C_SHOWCARD, &pShowCard, sizeof(pShowCard));
	return true;
}
bool CAndroidUserItemSink::OnSubReSet(const void * pBuffer, WORD wDataSize)
{
	CMD_S_ReSet *pCmd = (CMD_S_ReSet *)pBuffer;
	if (wDataSize != sizeof(CMD_S_ReSet)) return false;

	return true;
}
bool CAndroidUserItemSink::OnSubCallPlayer(const void * pBuffer, WORD wDataSize)
{
	/*CMD_S_Call *pCmd = (CMD_S_Call *)pBuffer;
	if (wDataSize != sizeof(CMD_S_Call)) return false;*/

	return true;
}
bool CAndroidUserItemSink::OnSubSearchResult(const void * pBuffer, WORD wDataSize)
{
	/*CMD_S_Search *pCmd = (CMD_S_Search *)pBuffer;
	if (wDataSize != sizeof(CMD_S_Search)) return false;
	
	for (BYTE i = 0; i < MAX_COUNT*MAX_WEAVE; i++)
	{
		m_cbSearchCard[i] = pCmd->bSegmentCard[i];
	}

	UINT nElapse = TIME_LESS_2 + (rand() % TIME_SHOW_CARD);
	if (m_cbPlayStatus == GAME_STATE_GAME)
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);
	}*/
	return true;
}
bool CAndroidUserItemSink::OnSubWashCard(const void * pBuffer, WORD wDataSize)
{
	CMD_S_WashCard *pCmd = (CMD_S_WashCard *)pBuffer;
	if (wDataSize != sizeof(CMD_S_WashCard)) return false;

	return true;
}

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
