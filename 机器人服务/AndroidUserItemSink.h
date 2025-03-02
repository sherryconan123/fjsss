#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////



//游戏对话框
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//信息
protected:
	BYTE							m_cbPlayStatus;							//游戏状态
	BYTE							m_cbSearchCard[MAX_COUNT];				//手牌*MAX_WEAVE

	//控件变量
public:
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口
	
	
	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//玩家摊牌
	bool OnSubShowCard(const void * pBuffer, WORD wDataSize);
	//比较扑克
	bool OnSubComPareCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameConclude(const void * pBuffer, WORD wDataSize);
	//玩家准备
	bool OnSubUserReady(const void * pBuffer, WORD wDataSize);
	//切牌玩家
	bool OnSubCutUser(const void * pBuffer, WORD wDataSize);
	//切牌消息
	bool OnSubCutCard(const void * pBuffer, WORD wDataSize);
	//自动摆牌
	bool OnSubAutoPut(const void * pBuffer, WORD wDataSize);
	//重新摆拍
	bool OnSubReSet(const void * pBuffer, WORD wDataSize);
	//叫倍
	bool OnSubCallPlayer(const void * pBuffer, WORD wDataSize);
	//搜索结果
	bool OnSubSearchResult(const void * pBuffer, WORD wDataSize);
	//洗牌
	bool OnSubWashCard(const void * pBuffer, WORD wDataSize);


};

//////////////////////////////////////////////////////////////////////////

#endif
