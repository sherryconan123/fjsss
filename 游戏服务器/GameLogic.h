#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once
#include "afxtempl.h"
//////////////////////////////////////////////////////////////////////////	

//�˿�����
#define CT_INVALID					    	0								//��������
#define CT_SINGLE					    	1								//��������
#define CT_DOUBLE			    			2								//ֻ��һ��
#define CT_DOUBLE_X2			    		3								//��������
#define CT_THREE_HEAD				     	4								//ͷ������
#define CT_THREE_KING				     	5								//ͷ������
#define CT_THREE				     		6								//��������
#define CT_FIVE_LINE	    				7								//��ͨ˳��
#define CT_FIVE_COLOR				    	8								//ͬ��
#define CT_THREE_TWO			     		9								//��«����
#define CT_FOUR_SAME				    	10								//��ͬ
#define CT_FIVE_COLOR_LINE		    		11								//ͬ��˳��
#define CT_FIVE_SAME				    	12								//��ͬ
#define CT_FOUR_KING				    	13								//����
#define CT_FIVE_KING				    	14								//����

//��������
#define CT_EX_INVALID				    	0								//��������
#define CT_EX_COLOR_X3			    		20								//��ͬ��
#define CT_EX_HALF_XIAO			    		21								//��С ***
#define CT_EX_HALF_DA			    		22								//��� ***
#define CT_EX_LINE_X3				    	23								//��˳��
#define CT_EX_LIUDUIBAN				    	24								//���԰�
#define CT_EX_LEAST_6				    	25								//6�� ***
#define CT_EX_LEAST_7				    	26								//7�� ***
#define CT_EX_DUI_5_THREE			    	27								//�������
#define CT_EX_SITAOSANTIAO			       	28								//��������
#define CT_EX_ALL_BLACK			 	    	29								//ȫ��
#define CT_EX_ALL_RED			 	    	30								//ȫ��
#define CT_EX_ONE_COLOR			 	    	31								//��һɫ
#define CT_EX_ALL_XIAO				    	32								//ȫС
#define CT_EX_ALL_DA                        33                              //ȫ��
#define CT_EX_LEAST_9				    	34								//9��
#define CT_EX_THREE_TDX2				    35								//�������ͷ ***
#define CT_EX_ALL_RED_B1                    36                              //ȫ��һ��� ***
#define CT_EX_ALL_BLACK_R1                  37                              //ȫ��һ��� ***
#define CT_EX_LEAST_10						38                              //10�� ***
#define CT_EX_SEVEN_SAME					39                              //7ͬ
#define CT_EX_EIGHT_SAME					40                              //8ͬ
#define CT_EX_NINE_SAME 					41                              //9ͬ
#define CT_EX_SANFENGTIANXIA		    	42								//��������(��ը��)
#define CT_EX_SANTONGHUASHUN				43						    	//��ͬ��˳
#define CT_EX_SHIERHUANGZU			    	44						    	//ʮ������
#define CT_EX_YITIAOLONG			    	45								//һ����
#define CT_EX_YITIAOLONG_EX			    	46								//һ������
#define CT_EX_ZHIZUNQINGLONG		     	47								//��������

//��ֵ����
#define	LOGIC_MASK_COLOR		    		0xF0						 	//��ɫ����
#define	LOGIC_MASK_VALUE			    	0x0F							//��ֵ����

#define	WEIGHT_TYPE			    			44							
						
//�����ṹ
struct  tagAnalyseCard 
{
	//��������	
	BYTE							cbCardCnt;
	BYTE							cbMinLogicValue;
	BYTE							cbMaxLogicValue;
	BYTE							cbMaxCardValue;
	BYTE							cbColorKind;		//������ɫ���� 1-4	
	BYTE							cbColorCount[ 5 ];	//��ɫ��Ŀ ��Ƭ-�ݻ�-����-����-��		
	BYTE 							cbBlockCount[ 8 ];	//ͬ����Ŀ ��-��-��ͬ-ը��-��ͬ~8ͬ		
	/**
	 *�˿��б� [8] ��-��-��ͬ-ը��-��ͬ~8ͬ	
	 *�˿��б� [MAX_COUNT] ��ֵ
	**/
	BYTE							cbCardData[ 8 ][ MAX_COUNT ];	
	BYTE							cbTempCardData[ MAX_COUNT ];		//�˿��б�
	/**
	 *	�˿��б�[5]	��Ƭ-÷��-����-����-�ܼ�
	 *	�˿��б�[MAX_COUNT]	A-2-3~K
	**/
	BYTE							cbValueCnt[ 5 ][ MAX_COUNT ];		
};

//��������
enum enSortCardType
{
	enDescend,								//�������� 
	enAscend,								//��������
	enColor									//��ɫ����
};

//////////////////////////////////////////////////////////////////////////	

//��Ϸ�߼���
class CGameLogic
{
	//��������
public:
	static const BYTE				m_bCardListData[ FULL_COUNT ];				//�˿�����
	ITableFrame						*m_pITableFrame;									//��ܽӿ�
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();
	//�����˿�
	void RandCardList( BYTE cbRandBuffer[ ], BYTE cbCardBuffer[ ], BYTE cbBufferCount );

	//���ͺ���
public:
	//��ȡ����
	void AnalysebCardData( BYTE bCardData[ ], BYTE bCardCount, tagAnalyseCard & AnalyseCard );
	//��ȡ����
	BYTE GetCardSpecialType( BYTE bCardData[ ], BYTE bCardCount, BYTE bType );
	//��ȡ����
	BYTE GetCardType( BYTE bCardData[ ], BYTE bCardCount, tagAnalyseCard & AnalyseCard );
	//��ȡȨ��
	SCORE GetCardWeight(BYTE bCardData[], BYTE bCardCount, bool bColor);
	//��ȡȨ��
	SCORE GetLineWeight( tagAnalyseCard AnalyseCard, BYTE bType, bool bColor );
	//��ȡȨ��
	SCORE GetUnLineWeight(tagAnalyseCard AnalyseCard, BYTE bType, bool bColor);
	//��ȡȨ��
	SCORE GetColorWeight(tagAnalyseCard AnalyseCard, BYTE bType, bool bColor);
	//��ȡ����
	BYTE GetTypeByWeight( SCORE lWeight ){ return ( BYTE ) ( lWeight >> WEIGHT_TYPE ); };
	
	//��������
public:
	//�Ƿ�ʮ������
	bool IsPelpleX12( tagAnalyseCard AnalyseCard );
	//�Ƿ���˳��
	bool IsLineX3( tagAnalyseCard AnalyseCard, bool bSameColor );
	//�Ƿ�3ͬ��
	bool IsColorX3( tagAnalyseCard AnalyseCard );
	//�����
	bool IsSameGroup( tagAnalyseCard AnalyseCard, BYTE bySame1, BYTE byCnt1, BYTE bySame2=0, BYTE byCnt2=0 );
	//��ȡ��
	bool GetSameCnt(tagAnalyseCard &AnalyseCard, BYTE bySame, BYTE byCnt, BYTE byCard[] = NULL);
	//��ȡ��
	BYTE GetSameCard( tagAnalyseCard &AnalyseCard, BYTE bySame );
	//˳�ӳ���
	BYTE GetLineLen(tagAnalyseCard AnalyseCard, bool bSameColor = false);
	//��ȡ��
	bool GetLine( tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byStart = INVALID_BYTE, BYTE byCard[ ] = NULL );
	//��ȡ��
	bool GetMaxLine( tagAnalyseCard AnalyseCard, bool bSameColor, BYTE byCard[] = NULL );
	//��ȡը��
	bool GetBomb(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//��ȡ��«
	bool GetHuLu(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//��ȡͬ��
	bool GetSameColor(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//��ȡ����
	bool GetDoubleX2(tagAnalyseCard AnalyseCard, BYTE cbCard[] = NULL);
	//��ȡ����
	bool GetSingleCard(tagAnalyseCard &AnalyseCard, BYTE cbCnt, BYTE cbCard[] = NULL);

	//�߼�����
public:
	//�Զ�����
	void AutoSortCard( BYTE bCardData[], BYTE bPutCardData[] );
	//����������
	void GetMaxCard( tagAnalyseCard AnalyseCard, BYTE cbPutCardData[ ] );

	//�߼�����
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE bCardData);
	BYTE SwitchCardToIndex( BYTE bCardData );
	int GetSpecialWeights(BYTE bType);

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE bCardData) { return bCardData&LOGIC_MASK_VALUE; } //ʮ������ǰ����λ��ʾ�Ƶ���ֵ
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE bCardData) { return (bCardData&LOGIC_MASK_COLOR) >> 4; } //ʮ�����ƺ�����λ��ʾ�ƵĻ�ɫ 

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE bCardData[], BYTE bCardCount, enSortCardType SortCardType = enDescend);
	//�����˿�
	void RandCardList(BYTE bCardBuffer[], BYTE bBufferCount);
	//ɾ���˿�
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount);

public:
	CString GetCardString( BYTE cbCardData );
	CString GetCardListString( BYTE cbCardData[], BYTE cbCardCount );
};

//////////////////////////////////////////////////////////////////////////

#endif
