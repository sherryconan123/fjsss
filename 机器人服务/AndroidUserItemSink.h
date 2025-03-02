#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////



//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//��Ϣ
protected:
	BYTE							m_cbPlayStatus;							//��Ϸ״̬
	BYTE							m_cbSearchCard[MAX_COUNT];				//����*MAX_WEAVE

	//�ؼ�����
public:
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�
	
	
	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//���̯��
	bool OnSubShowCard(const void * pBuffer, WORD wDataSize);
	//�Ƚ��˿�
	bool OnSubComPareCard(const void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameConclude(const void * pBuffer, WORD wDataSize);
	//���׼��
	bool OnSubUserReady(const void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubCutUser(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubCutCard(const void * pBuffer, WORD wDataSize);
	//�Զ�����
	bool OnSubAutoPut(const void * pBuffer, WORD wDataSize);
	//���°���
	bool OnSubReSet(const void * pBuffer, WORD wDataSize);
	//�б�
	bool OnSubCallPlayer(const void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubSearchResult(const void * pBuffer, WORD wDataSize);
	//ϴ��
	bool OnSubWashCard(const void * pBuffer, WORD wDataSize);


};

//////////////////////////////////////////////////////////////////////////

#endif
