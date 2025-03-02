#ifndef CMD_THIRTEEN_HEAD_FILE
#define CMD_THIRTEEN_HEAD_FILE

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID							63000								//游戏 I D
#define GAME_PLAYER						6									//游戏人数
#define GAME_NAME						TEXT("福建十三张")				    //游戏名字
#define DLL_NAME						TEXT("ThirteenFJ.dll")				

#define VERSION_SERVER					PROCESS_VERSION(1,0,1)				//程序版本
#define VERSION_CLIENT					PROCESS_VERSION(1,0,1)				//程序版本

//常量数据
#define MAX_COUNT						13									//扑克数目
#define	FULL_COUNT						54
#define	RAND_COUNT						200

#define FRONT_COUNT						3
#define MIDDLE_COUNT					5
#define BACK_COUNT						5

#define FRONT_OFFSET					0
#define MIDDLE_OFFSET					(FRONT_COUNT)
#define BACK_OFFSET						(FRONT_COUNT + MIDDLE_COUNT)

#define FRONT_DATA(p)					((p) + FRONT_OFFSET)
#define MIDDLE_DATA(p)					((p) + MIDDLE_OFFSET)
#define BACK_DATA(p)					((p) + BACK_OFFSET)
#define INVALID_TIMES					0xFF

////////////////////////////////////////////////////////////////////////// ServerRules 16-31
//人数 （4）
#define GAME_TYPE_1_PLAYER			0x00010000		   //3人             
#define GAME_TYPE_2_PLAYER			0x00020000		   //2人             
#define GAME_TYPE_3_PLAYER			0x00040000		   //6人      
#define GAME_TYPE_4_PLAYER			0x00080000		   //5人      

#define GAME_TYPE_CNT_1				0x00100000							//10
#define GAME_TYPE_CNT_2				0x00200000							//20
#define GAME_TYPE_CNT_3				0x00400000							//30

////////////////////////////////////////////////////////////////////////// RULES 0
#define GAME_TYPE_BASE1				0x00000001							//10
#define GAME_TYPE_BASE2				0x00000002							//20
#define GAME_TYPE_BASE3				0x00000004							//50
#define GAME_TYPE_BASE4				0x00000008							//100
#define GAME_TYPE_BASE5				0x00000010							//150
#define GAME_TYPE_BASE6				0x00000020							//300
#define GAME_TYPE_BASE7				0x00000040							//500
//比牌方式
#define GAME_TYPE_COMPARECOLORFIRST	0x00000080		   //优先比花色       7 
//王牌
#define GAME_TYPE_KING_1			0x00000100					//王牌1  双王
#define GAME_TYPE_KING_2			0x00000200					//王牌2  四王
#define GAME_TYPE_KING_3			0x00000400					//王牌3  六王
//马牌
#define GAME_TYPE_MAPAI_1			0x00000800		   //马牌1  红桃A      11
#define GAME_TYPE_MAPAI_2			0x00001000		   //马牌2  红桃5      12
#define GAME_TYPE_MAPAI_3			0x00002000		   //马牌3  红桃10     13
 
#define GAME_TYPE_CHONG3			0x00004000		   //冲三
#define GAME_TYPE_CUTIN				0x00008000		   //中途禁入

//自动准备
#define GAME_TYPE_TIME_1			0x00100000							//10秒 20
#define GAME_TYPE_TIME_2			0x00200000							//20秒 21

#define GAME_TYPE_SPEACIAL			0x00400000							//特殊牌型

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define SUB_S_GAME_START				101									//游戏开始
#define SUB_S_SHOW_CARD					102									//玩家摊牌
#define SUB_S_COMPARE_CARD				103									//比较扑克
#define SUB_S_GAME_END					106									//游戏结束
#define SUB_S_CUT_USER					109
#define	SUB_S_CUT_CARD					110
#define SUB_S_AUTO_PUT					114
#define SUB_S_RE_SET					115
#define SUB_S_WASH_CARD					116									//洗牌
//////////////////////////////////////////////////////////////////////////
struct CMD_S_GameStart
{
	WORD								wBanker;							//庄家玩家
	BYTE								bCardData[MAX_COUNT];				//手上扑克
	BYTE								cbSpecialType;
	bool								bPlayStatus[GAME_PLAYER];			//游戏状态
};

//玩家摊牌
struct CMD_S_ShowCard
{
	WORD								wCurrentUser;						//当前玩家
	BYTE								bFrontCard[3];						//前墩扑克
	BYTE								bMidCard[5];						//中墩扑克
	BYTE								bBackCard[5];						//后墩扑克
};

//开始比牌
struct CMD_S_CompareCard
{
	WORD								wBankUser;							//庄家玩家
	BYTE								bSegmentCard[GAME_PLAYER][MAX_COUNT];//分段扑克
	SCORE								llScoreTimes[GAME_PLAYER][3];		//分段倍数
	BYTE								bSpecialType[GAME_PLAYER];			//特殊牌型
	SCORE                               llFinalScore[GAME_PLAYER];          //
	bool								bGun[GAME_PLAYER][GAME_PLAYER];
};

struct CMD_S_CutUser
{
	WORD								wChairID;
};

struct CMD_S_CutCard
{
	WORD								wChairID;
	double								dValue;
};

struct CMD_S_AutoPut
{
	BYTE								bSegmentCard[MAX_COUNT];	//分段扑克
};

struct CMD_S_ReSet
{
	BYTE								bCardData[ MAX_COUNT ];				//手上扑克
	BYTE								cbSpecialType;
};

struct CMD_S_WashCard
{
	BYTE								cbWash;
	WORD								wWashUser;
	BYTE								cbWashCount;
};

//游戏结束
struct CMD_S_GameEnd
{
	SCORE								lGameScore[GAME_PLAYER];			//游戏积分
	SCORE								lScoreTimes[GAME_PLAYER];			//输赢倍数
	SCORE								lCompareResult[GAME_PLAYER][3];		//比牌结果
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];
	BYTE								cbSpecialType[GAME_PLAYER];
	BYTE								cbEndType;
	bool								bShowBigResult;
};

//大结算
struct CMD_S_GameCustomInfo
{
	SCORE							llTotalScore[GAME_PLAYER];			//总分数
};

//空闲状态
struct CMD_S_StatusFree
{	
	//游戏属性
	bool								bGameStatus[GAME_PLAYER];			//游戏状态
	//时间定义
	BYTE								cbTimeStartGame;					//开始时间
	BYTE								cbWashCount;						//洗牌次数
};


//游戏状态
struct CMD_S_StatusPlay
{
	bool								bGameStatus[GAME_PLAYER];			//游戏状态
	BYTE								cbHandCardData[MAX_COUNT];	    	//扑克数据
	BYTE								cbSegmentCard[GAME_PLAYER][MAX_COUNT];	//分段扑克
	bool								bFinishSegment[GAME_PLAYER];		//完成分段
	WORD								wBanker;							//庄家玩家
	WORD								wCurUser;
	SCORE								lScoreTimes[GAME_PLAYER][3];		//分段倍数
	BYTE								cbSpecialType;						//特殊牌型
	bool								bCompared;							//是否比牌
	WORD								wWaitTime;					    	//理牌时间

};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define SUB_C_SHOWCARD					1									//玩家摊牌
#define SUB_C_COMPLETE_COMPARE			2									//完成比较
#define	SUB_C_CUT_USER					6
#define SUB_C_CUT_CARD					7
#define SUB_C_AUTO						9
#define SUB_C_RESET						10
#define SUB_C_WASH_CARD					11									//洗牌
//分段信息
struct CMD_C_ShowCard
{
	BYTE								cbFrontCard[3];						//前墩扑克
	BYTE								cbMidCard[5];						//中墩扑克
	BYTE								cbBackCard[5];						//后墩扑克
	bool								bSpecial;							//特殊牌型
};

struct CMD_C_CutCard
{
	double								dValue;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif
