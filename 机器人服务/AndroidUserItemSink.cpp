#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS_1						1									//最少时间
#define TIME_LESS_2						2									//最少时间

//游戏时间
#define TIME_START						5									//叫庄时间
#define TIME_CALL						2
#define TIME_SHOW_CARD					10									//摆牌时间
#define TIME_COMPLETE_COMPARE			10									//比牌时间

//游戏时间
#define IDI_GAME_READY					(101)								//准备定时器
#define IDI_SHOW_CARD					(102)								//摆牌定时器
#define IDI_COMPLETE_COMPARE			(103)								//比牌定时器

#define GAME_STATE_FREE					1									//游戏空闲
#define GAME_STATE_GAME					2									//游戏中

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//接口变量
	m_pIAndroidUserItem=NULL;
	srand((unsigned)time(NULL));   

	m_cbPlayStatus = GAME_STATE_FREE;
	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	try
	{
		switch (nTimerID)
		{
			case IDI_GAME_READY:		//开始定时
			{
				//发送准备
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
		cs.Format(TEXT("异常ID=%d"), nTimerID);
		CTraceService::TraceString(cs, TraceLevel_Debug);
	}

	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
		case SUB_S_GAME_START:		//游戏开始
			{
				//消息处理
				return OnSubGameStart(pData, wDataSize);
			}
		case SUB_S_SHOW_CARD:		//玩家摊牌
			{
				//消息处理
				return OnSubShowCard(pData, wDataSize);
			}
		case SUB_S_COMPARE_CARD:	//比较扑克
			{
				//消息处理
				return OnSubComPareCard(pData, wDataSize);
			}
		case SUB_S_GAME_END:		//游戏结束
			{
				//消息处理
				return OnSubGameConclude(pData, wDataSize);
			}
		case SUB_S_CUT_USER:	//
			{
				//消息处理
				return OnSubCutUser(pData, wDataSize);
			}
		case SUB_S_CUT_CARD:
			{
				//消息处理
				return OnSubCutCard(pData, wDataSize);
			}
		case SUB_S_AUTO_PUT:
			{
				 //消息处理
				return OnSubAutoPut(pData, wDataSize);
			}
		case SUB_S_RE_SET:
			{
				//消息处理
				return OnSubReSet(pData, wDataSize);
			}
		case SUB_S_WASH_CARD:
			{
				//消息处理
				return OnSubWashCard(pData, wDataSize);
			}
		//case SUB_S_CALL_TIMES:
		//	{
		//		//消息处理
		//		return OnSubCallPlayer(pData, wDataSize);
		//	}
		//case SUB_S_SEARCH:
		//	{
		//		//消息处理
		//		return OnSubSearchResult(pData, wDataSize);
		//	}
	}

	//错误断言
	//ASSERT(FALSE);

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
		case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			//消息处理
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

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
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
		
	//发送信息
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

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
