#ifndef CMD_THIRTEEN_HEAD_FILE
#define CMD_THIRTEEN_HEAD_FILE

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID							63000								//��Ϸ I D
#define GAME_PLAYER						6									//��Ϸ����
#define GAME_NAME						TEXT("����ʮ����")				    //��Ϸ����
#define DLL_NAME						TEXT("ThirteenFJ.dll")				

#define VERSION_SERVER					PROCESS_VERSION(1,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(1,0,1)				//����汾

//��������
#define MAX_COUNT						13									//�˿���Ŀ
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
//���� ��4��
#define GAME_TYPE_1_PLAYER			0x00010000		   //3��             
#define GAME_TYPE_2_PLAYER			0x00020000		   //2��             
#define GAME_TYPE_3_PLAYER			0x00040000		   //6��      
#define GAME_TYPE_4_PLAYER			0x00080000		   //5��      

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
//���Ʒ�ʽ
#define GAME_TYPE_COMPARECOLORFIRST	0x00000080		   //���ȱȻ�ɫ       7 
//����
#define GAME_TYPE_KING_1			0x00000100					//����1  ˫��
#define GAME_TYPE_KING_2			0x00000200					//����2  ����
#define GAME_TYPE_KING_3			0x00000400					//����3  ����
//����
#define GAME_TYPE_MAPAI_1			0x00000800		   //����1  ����A      11
#define GAME_TYPE_MAPAI_2			0x00001000		   //����2  ����5      12
#define GAME_TYPE_MAPAI_3			0x00002000		   //����3  ����10     13
 
#define GAME_TYPE_CHONG3			0x00004000		   //����
#define GAME_TYPE_CUTIN				0x00008000		   //��;����

//�Զ�׼��
#define GAME_TYPE_TIME_1			0x00100000							//10�� 20
#define GAME_TYPE_TIME_2			0x00200000							//20�� 21

#define GAME_TYPE_SPEACIAL			0x00400000							//��������

//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_GAME_START				101									//��Ϸ��ʼ
#define SUB_S_SHOW_CARD					102									//���̯��
#define SUB_S_COMPARE_CARD				103									//�Ƚ��˿�
#define SUB_S_GAME_END					106									//��Ϸ����
#define SUB_S_CUT_USER					109
#define	SUB_S_CUT_CARD					110
#define SUB_S_AUTO_PUT					114
#define SUB_S_RE_SET					115
#define SUB_S_WASH_CARD					116									//ϴ��
//////////////////////////////////////////////////////////////////////////
struct CMD_S_GameStart
{
	WORD								wBanker;							//ׯ�����
	BYTE								bCardData[MAX_COUNT];				//�����˿�
	BYTE								cbSpecialType;
	bool								bPlayStatus[GAME_PLAYER];			//��Ϸ״̬
};

//���̯��
struct CMD_S_ShowCard
{
	WORD								wCurrentUser;						//��ǰ���
	BYTE								bFrontCard[3];						//ǰ���˿�
	BYTE								bMidCard[5];						//�ж��˿�
	BYTE								bBackCard[5];						//����˿�
};

//��ʼ����
struct CMD_S_CompareCard
{
	WORD								wBankUser;							//ׯ�����
	BYTE								bSegmentCard[GAME_PLAYER][MAX_COUNT];//�ֶ��˿�
	SCORE								llScoreTimes[GAME_PLAYER][3];		//�ֶα���
	BYTE								bSpecialType[GAME_PLAYER];			//��������
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
	BYTE								bSegmentCard[MAX_COUNT];	//�ֶ��˿�
};

struct CMD_S_ReSet
{
	BYTE								bCardData[ MAX_COUNT ];				//�����˿�
	BYTE								cbSpecialType;
};

struct CMD_S_WashCard
{
	BYTE								cbWash;
	WORD								wWashUser;
	BYTE								cbWashCount;
};

//��Ϸ����
struct CMD_S_GameEnd
{
	SCORE								lGameScore[GAME_PLAYER];			//��Ϸ����
	SCORE								lScoreTimes[GAME_PLAYER];			//��Ӯ����
	SCORE								lCompareResult[GAME_PLAYER][3];		//���ƽ��
	BYTE								cbCardData[GAME_PLAYER][MAX_COUNT];
	BYTE								cbSpecialType[GAME_PLAYER];
	BYTE								cbEndType;
	bool								bShowBigResult;
};

//�����
struct CMD_S_GameCustomInfo
{
	SCORE							llTotalScore[GAME_PLAYER];			//�ܷ���
};

//����״̬
struct CMD_S_StatusFree
{	
	//��Ϸ����
	bool								bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	//ʱ�䶨��
	BYTE								cbTimeStartGame;					//��ʼʱ��
	BYTE								cbWashCount;						//ϴ�ƴ���
};


//��Ϸ״̬
struct CMD_S_StatusPlay
{
	bool								bGameStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE								cbHandCardData[MAX_COUNT];	    	//�˿�����
	BYTE								cbSegmentCard[GAME_PLAYER][MAX_COUNT];	//�ֶ��˿�
	bool								bFinishSegment[GAME_PLAYER];		//��ɷֶ�
	WORD								wBanker;							//ׯ�����
	WORD								wCurUser;
	SCORE								lScoreTimes[GAME_PLAYER][3];		//�ֶα���
	BYTE								cbSpecialType;						//��������
	bool								bCompared;							//�Ƿ����
	WORD								wWaitTime;					    	//����ʱ��

};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_SHOWCARD					1									//���̯��
#define SUB_C_COMPLETE_COMPARE			2									//��ɱȽ�
#define	SUB_C_CUT_USER					6
#define SUB_C_CUT_CARD					7
#define SUB_C_AUTO						9
#define SUB_C_RESET						10
#define SUB_C_WASH_CARD					11									//ϴ��
//�ֶ���Ϣ
struct CMD_C_ShowCard
{
	BYTE								cbFrontCard[3];						//ǰ���˿�
	BYTE								cbMidCard[5];						//�ж��˿�
	BYTE								cbBackCard[5];						//����˿�
	bool								bSpecial;							//��������
};

struct CMD_C_CutCard
{
	double								dValue;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif
