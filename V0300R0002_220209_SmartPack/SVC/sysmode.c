#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"
#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"


/*============�o�[�W����============*/
/* �X�}�[�g�p�b�N�ذ��@220201		
�E�����o��?�X�}�[�g�p�b�N��
�E�����A�i���O������̃o�O�C��
�E�f�[�^�t���b�V���j���H�f�[�^�ݒ�͈͊O��ǉ�
�E�f�[�^�t���b�V���̏������݃^�C�~���O�ǉ�
�@���d���؂莞�̏������݂͕s�B����͓d���̗��������ɂ���āA�������ݎ��s���鋰�ꂠ��
�E�f�[�^�t���b�V���̏������ݎ��ɌŒ�END�R�[�h�ǉ�
�E�eLED������֐��ɂčs���悤�ɏC���B
�@�����͏o�͗p�ϐ��𒼐ژM���Ă������z��w��ł��g���Ă��邽�߁A�͈͊O�ɂȂ鋰�ꂠ����
 ���d�����o���̃��Z�b�g�͎g��Ȃ����ƁB�n�[�h�̃��Z�b�g��H�Ƃ̊֌W�̌��ؖ��̂���

*/	
const	UBYTE	Version[] = {"Ver03.00"};

//const	UBYTE	Revision[] = {"Rev00.00"};
//const	UBYTE	Revision[] = {"Rev00.01"};	//�o�͌������̃t�@��Out�s��C������޼ޮݒǉ�
const	UBYTE	Revision[] = {"Rev00.02"};	//�\���������̘A���Ɨ\��LED�̕s��C��


/*=======================================*/


void	sysSoftVersionMode(void);					/* ����ް�ޮ�Ӱ�ޏ���				*/
void	sysRamClearMode(void);						/* RAM�ر�ݒ�Ӱ��					*/
void	sysAnalogAdjustMode(void);					/* ��۸ޒ���Ӱ��					*/
void	sysPanelTestMode(void);						/* ����ý�Ӱ��						*/
void	sysInputTestMode(void);						/* ����ý�Ӱ��						*/
void	sysOutputTestMode(void);					/* �o��ý�Ӱ��						*/
void	sysErrorLogMode(void);					/* �װ۸�Ӱ��						*/

void	mainSystemMode(void);
UBYTE	glbGetDipSW(void);
void	sys7SegLedAllOff(void);						/* 7SEG/LED ALL OFF			*/
void	sys7SegLedAllOn(void);						/* 7SEG/LED ALL ON			*/
void	sysLedAllOff(void);							/* LED ALL OFF			*/
void	sysLedAllOn(void);							/* LED ALL ON				*/
void	sysAnalogAdjustAuto(void);
void	sysRamClearMode(void);
void	sysErrorLogMode(void);					/* �װ۸�Ӱ��						*/
void	sysPumpTimeMode(void);

UWORD	R_ADC_Get_Result2(void);
void	sysGetVacuumLevel(UBYTE ,UBYTE *,UBYTE *);
void	sysGetPumpTime(UBYTE ,UBYTE *,UBYTE *);
void	sysFlashWrite(void);
void	(*sysProcTbl[])(void);
void	sysVacuumStart(void);						/* �^������J�n				*/

UBYTE	sysFlMode;
UBYTE	sysFlCourse;
extern	UBYTE   glbWaitKeyOff(UBYTE,UWORD);
extern	UBYTE	glbGetReadKey(void);
extern	void	mainDisplay7Seg(UWORD);			/* 7SEG�\��							*/
extern	void	VacuumStart(void);						/* �^������J�n				*/
extern	UBYTE   glbWaitKeyOn(UBYTE KeyNo);
extern	void	mainVacuumElectromangCheck(void);
//extern	void	mainFlashClear(void);
extern	UBYTE   glbWaitKeyOff(UBYTE,UWORD);
extern	void	mcmSetBuzzerInvalid(void);					/* �����޻ް			*/
extern	void	mcmSetBuzzerEffective(void);
extern	void	mainErrorMode(void);				/* �װ���oӰ��				*/
extern	UBYTE	pdlWriteFlash(void);
extern	void	pldRestoreBackupData(void);
extern	UBYTE	DrawerSwRead(void);
extern	void	VacuumEndControl(void);
extern	UBYTE	mcmFlashDataInitial(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

void	(*sysProcTbl[])() =
{
	sysSoftVersionMode,					/* ����ް�ޮ�Ӱ�ޏ���				*/
	sysRamClearMode,					/* RAM�ر�ݒ�Ӱ��					*/
	sysAnalogAdjustMode,				/* ��۸ޒ���Ӱ��					*/
	sysPanelTestMode,					/* ����ý�Ӱ��						*/
	sysInputTestMode,					/* ����ý�Ӱ��						*/
	sysOutputTestMode,					/* �o��ý�Ӱ��						*/
	sysErrorLogMode,					/* �װ۸�Ӱ��						*/
	sysPumpTimeMode						/* ����߉ғ����ԕ\��Ӱ��			*/

};

#define	LCN_SOFTWARE_VERSION		0						/* ��ĳ���ް�ޮݕ\��		*/
#define	LCN_RAM_CLEAR				1						/* RAM�ر					*/
#define	LCN_ANALOG_ADJUST			2						/* ��۸ޒ���Ӱ��			*/
#define	LCN_PANEL_TEST				3						/* ����ý�Ӱ��				*/
#define	LCN_INPUT_TEST				4						/* ����ý�Ӱ��				*/
#define	LCN_OUTPUT_TEST				5						/* �o��ý�Ӱ��				*/
#define	LCN_ERROR_LOG				6						/* �װ۸ޕ\��Ӱ��			*/
#define	LCN_PUMP_TIME				7						/* ����߉ғ����ԕ\��Ӱ��	*/
#define	LCN_NON_SYSTEM_MODE			8						/* bit0��0�̏ꍇ�͒ʏ�Ӱ�ނ�	*/

UBYTE	DipSwToIndexTBL[16] = {
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_SOFTWARE_VERSION	,					/* ��ĳ���ް�ޮݕ\��		*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_INPUT_TEST		,					/* ����ý�Ӱ��				*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_ANALOG_ADJUST		,					/* ��۸ޒ���Ӱ��			*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_ERROR_LOG			,					/* �װ۸ޕ\��Ӱ��			*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_RAM_CLEAR			,					/* RAM�ر					*/
						      LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_OUTPUT_TEST		,					/* �o��ý�Ӱ��				*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
						      LCN_PANEL_TEST		,					/* ����ý�Ӱ��				*/
							  LCN_NON_SYSTEM_MODE	,					/* �ʏ�Ӱ�ނ�				*/
							  LCN_PUMP_TIME								/* ����߉ғ����ԕ\��		*/
							};



/***********************************************************************************************************************
*    �֐���    mainSystemMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �e�X�g���[�h���C������
*    �������e  DIPSW�̐ݒ�ɂ��A�e�e�X�g���ڂɑJ�ڂ���
*              �Y������e�X�g���ڂ�����DIPSW�̐ݒ�̏ꍇ�͒ʏ�Ӱ�ނɑJ�ڂ���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void mainSystemMode(void)
{
	UBYTE	key;
	
	sysFlMode = DipSwToIndexTBL[glbFlDipSW];				/* DipSW��Ԃɂ��ý�No.��ǂݍ���			*/
	if(sysFlMode == LCN_NON_SYSTEM_MODE)					/* �Y������ý�Ӱ�ނ��Ȃ��ꍇ�́A�ʏ�Ӱ�ނ֑J��	*/
	{
		if( glbFlDipSW & 0x08 )	//�@�\�t�g�J���L��
		{
			//�\�t�g�J����
			glbFlSoftOpen = 0;
		}
		else
		{
			//�\�t�g�J���L
			glbFlSoftOpen = 1;
		}
		return;
	}
	
	glbFlSystemMode = 1;									/* ����Ӱ�ޒ��׸�			*/
	sysFlCourse = geefFlCourse;								/* ���No.����					*/
	sys7SegLedAllOff();										/* 7SEG/LED ALL OFF			*/
	glbDt7Seg2 = 'D' - 0x37;								/* 7SEG��2���ڂ�'d'��\��		*/
	glbDt7Seg1 = sysFlMode;									/* 7SEG��1���ڂ�ýč���No.��\��		*/
	while(1)												/* PUSH(SW7)�i��՗����j���������܂ų���	*/
	{
		if(key = glbGetReadKey())							/* KEY�̓�������			*/
		{
			if( (key == GCN_KEY_PUSH_SET)||(key == GCN_KEY_START_SET) )	//�X�^�[�gSW�ł��ǂ�
			{
				break;
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ���ݖ����޻ް			*/
			}
		}
	}
	mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/
	while(1)
	{
		(*sysProcTbl[sysFlMode])();							/* ����Ԃ�Ӱ�ނ֑J��		*/
	}
	

}

/***********************************************************************************************************************
*    �֐���    sysSoftVersionMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ̧�ѳ�����ް�ޮ݂�\������
*    �������e  �E�p�l���X�C�b�`��������Ă��Ȃ��ꍇ�o�[�W�����̏�ʂQ���y��[.]��\��
*			   �E ���̃X�C�b�`�������Ă���ԁA�o�[�W�����̉��ʂQ����\���i���X�C�b�`������ƃo�[�W������ʕ\���ɖ߂�j
*    ���l�@�@�@�E���̃X�C�b�`����͑S�Ė����Ƃ���B
*              �E��d���ؒf�ŏI���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void sysSoftVersionMode(void)
{
	UBYTE	key,data,sts;
	UWORD	MaxKeyOnTime;
	
	static UBYTE   turnFl = 0;


	while(1)
	{
		if(turnFl == 0)	//Ver�\��
		{
			ledControl_Sec(ALL_LED_SELECT,0,0);	
			
			data = (Version[3] - '0')*10;
			data = data + (Version[4] - '0');
			glbFlReqDot = 2;											/* 1���̈ʒu����ص�ޕ\��	*/
			mainDisplay7Seg(data);										/* �ް�ޮ݂̏��2����\��	*/

			if((key = glbGetReadKey()))									/* KEY�̓�������			*/
			{
				if(key == GCN_KEY_DOWN_SET)								/* ��(SW4)���́H				*/
				{
					MaxKeyOnTime = GCN_TIME_100MS;						/* SW�������ʒm����	�i0.1�b)			*/
					sts = 1;
					mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
					while(sts == 1)										/* �����������܂��ް�ޮ݂̉���2����\��	*/
					{
						data = (Version[6] - '0')*10;
						data = data + (Version[7] - '0');
						glbFlReqDot = 0;								/* ��ص�ޕ\������			*/
						mainDisplay7Seg(data);							/* �ް�ޮ݂̉���2����\��	*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* SW���������̂�҂�				*/
					}
				}
				else if(key == GCN_KEY_PUSH_SET)
				{
					turnFl = 1;
				}
				else
				{
					mcmSetBuzzerInvalid();								/* ���ݖ����޻ް			*/
				}
			}
		}
		else	//Rev�\��
		{
			ledControl_Sec(ALL_LED_SELECT,1,0);	
			
			data = (Revision[3] - '0')*10;
			data = data + (Revision[4] - '0');
			glbFlReqDot = 2;											/* 1���̈ʒu����ص�ޕ\��	*/
			mainDisplay7Seg(data);										/* �ް�ޮ݂̏��2����\��	*/

			if((key = glbGetReadKey()))									/* KEY�̓�������			*/
			{
				if(key == GCN_KEY_DOWN_SET)								/* ��(SW4)���́H				*/
				{
					MaxKeyOnTime = GCN_TIME_100MS;						/* SW�������ʒm����	�i0.1�b)			*/
					sts = 1;
					mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
					while(sts == 1)										/* �����������܂��ް�ޮ݂̉���2����\��	*/
					{
						data = (Revision[6] - '0')*10;
						data = data + (Revision[7] - '0');
						glbFlReqDot = 0;								/* ��ص�ޕ\������			*/
						mainDisplay7Seg(data);							/* �ް�ޮ݂̉���2����\��	*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* SW���������̂�҂�				*/
					}
				}
				else if(key == GCN_KEY_PUSH_SET)
				{
					turnFl = 0;
				}
				else
				{
					mcmSetBuzzerInvalid();								/* ���ݖ����޻ް			*/
				}
			}
			
			
		}
	}
}


/***********************************************************************************************************************
*    �֐���    sysAnalogAdjustMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �A�i���O�������[�h
*    �������e  ���̓Z���T�[�^�n�[�h�E�G�A�^�`�����o�[���e�ʂɂ����āA�����Ȍ̍�����������B
*			   ���ׁ̈A�P�O�O���̏ꍇ�̊�l�ɉ��Z����ׂ̕␳�l�i�A�i���O�����l�j�����߂�B
*              �A�i���O�����l��,�|�X�X�`�{�X�X�ł���B
*              ���蓮�������@��
*					�m���n�m���n�ŕ\������Ă��钲���l���}�P���t�o�^�c�n�v�m����B�i�͈́F�|�X�X�`�{�X�X�j
*                    �]�̏ꍇ�́A�b��LED��_�������A�u�H���v�X�C�b�`�������ƁA�ύX��̒l���L������B
*              �������������@��
*					 sysAnalogAdjustAuto�֐��Q�ƁB
*    ���l�@�@�@
*             
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysAnalogAdjustMode(void)
{
	UBYTE	key,Flag=0,sts;
	UBYTE	data;
	SWORD	sdata;
	UWORD	MaxKeyOnTime;

	glbFlAdjust = 1;

	data = geefCtAnalogAdjust;								/* ��۸ޒ����lذ��		*/
	if(data & 0x80	)										/* ���̏ꍇ				*/
	{
		data = ~data+1;										/* �����l��2�̕␔		*/
		Flag = 1;											/* ����\�����߂�LED�̕b��_��	*/
	}
	glbFlReqDot = 0;										/* ��ص�ޖ�						*/
	mainDisplay7Seg(data);									/* �����l��7SEG�\��					*/
	glbFlSecLevel = (Flag)?GCN_VACUUM_SECONDS:				/* ���̏ꍇ�́A'�b'��_��			*/
						   GCN_VACUUM_LEDALLOFF;
	ledControl_Sec(glbFlSecLevel,1,0);
			
						   
	sdata = (SWORD)((Flag)?~data+1:data);					/* ���̏ꍇ�Ȃ�A�ēx�A�l�𕉂ɂ���	*/
	while(1)
	{
		if((key = glbGetReadKey()))							/* SW�̓�������			*/
		{
			if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)���́H				*/
			{
				if(DrawerSwRead())							/* ���o��SW���u�J�v��Ԃ̏ꍇ	*/
				{
					mcmSetBuzzerInvalid();					/* ���ݖ����޻ް			*/
					return;
				}
				else
				{
					mcmSetBuzzerEffective();				/* ���ݗL���޻ް					*/
				}
				sysAnalogAdjustAuto();						/* ��������Ӱ��				*/
			}
			else if((key == GCN_KEY_UP_SET) ||				/* ��(SW3)�������ꂽ�ꍇ		*/
				    (key == GCN_KEY_DOWN_SET))
			{
				sts = 0;
				MaxKeyOnTime = GCN_TIME_1S;					/* ���������ʒm����	�i1�b)				*/
				do
				{
					if(key ==  GCN_KEY_UP_SET)				/* ��(SW3)						*/
					{
						sdata++;							/* �����l+1					*/
					}
					else									/* ��(SW4)						*/
					{
						sdata--;							/* �����l-1					*/
					}
					if(sdata >= 0)
					{
						ledControl_Sec(ALL_LED_SELECT,0,0);
					}
					if(sdata > 99)							/* �������						*/
					{
						sdata = 99;
						mcmSetBuzzerInvalid();				/* �����޻ް��炷			*/
					}
					else
					{
						if(sts == 0)
						{
							mcmSetBuzzerEffective();		/* ���ݗL���޻ް					*/
						}
					}
					if(sdata < 0)
					{
						glbFlSecLevel = GCN_VACUUM_SECONDS;	/* �b ���				*/
						ledControl_Sec(glbFlSecLevel,1,0);	
					}
					if(sdata < -99)							/* ��������						*/
					{
						sdata = -99;
						mcmSetBuzzerInvalid();				/* �����޻ް��炷			*/
					}
					else
					{
						if(sts == 0)
						{
							mcmSetBuzzerEffective();		/* ���ݗL���޻ް					*/
						}
					}

					if(glbFlSecLevel == GCN_VACUUM_SECONDS)	/* ���̏ꍇ?			*/
					{
						data = (UBYTE)(~sdata + 1);
					}
					else
					{
						data = (UBYTE)(sdata);
					}
					mainDisplay7Seg((UBYTE)data);			/* �����l��7SEG�\��					*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);	/* SW���������̂�҂�				*/
					MaxKeyOnTime = GCN_TIME_100MS;			/* ���������ʒm����	�i0.1�b)			*/
				}while(sts == 1);
				DI();
				glbFlKydt = 0;								/* ��߰Ē��ɓ��͂��������͎̂Ă�		*/
				EI();
			}
			else if(key == GCN_KEY_PROC_SET)				/* �H��(SW2)�������ꂽ�ꍇ		*/
			{
				geefFlCourse = sysFlCourse;
				geefCtAnalogAdjust = sdata;					/* �����l���ޯ�������؂־��	*/
				sysFlashWrite();
				
				ledControl_Course(ALL_LED_SELECT,0,0);							/* 7SEG/LED ALL OFF			*/
				mcmSetBuzzerEffective();					/* ���ݗL���޻ް					*/
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ���ݖ����޻ް			*/
			}
		}
	}
}

/***********************************************************************************************************************
*    �֐���    sysAnalogAdjustMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �����������[�h
*    �������e  �@�m����/�ެ��߁n�X�C�b�`�������ƁA�^��������J�n����B�i�m����/�ެ��߁n�X�C�b�`�����������_��
*			   �@ ���������Ɣ��f����j
*             			�|���v		�n�m	�ғ�
*                       �t�@��		�n�m
*                       �^��J����	�n�m	����
*                       �^��d����	�n�m	�J��
*			   �A�m����/�ެ��߁n�X�C�b�`�������ƁA�^��������~����B
*						�^��d����	�n�e�e	����
* 			   �B�m����/�ެ��߁n�X�C�b�`�������ƁA�^��������ĊJ����B
*                  		�^��d����	�n�m	�J��
*			   �C�^�������~���ɁA�m���n�X�C�b�`�������ꂽ��A������Ă���Ԃ����^��J�����s���B
*                       �^��J����	�n�e�e	�J��
*                       �^��d����	�n�e�e	����
*                �m���n�X�C�b�`���������A�B�^�������~���̏�Ԃɖ߂��B
*						�^��J����	�n�m	����
*                       �^��d����	�n�e�e	����
*			�@ �D�@�A�`�C�𑀍삵�āA���o�����̐^��v�̒l���u�P�D�Q���o���v�ɂȂ�l�ɒ�������B
*    ���l�@�@�@
*             
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	sysAnalogAdjustAuto(void)
{
	UBYTE	key,sts,togle=0,data;
	SWORD	adData,MaxKeyOnTime ;

	sysVacuumStart();											/* �^������J�n				*/
	while(1)
	{
		if(DrawerSwRead())							/* ���o��SW���u�J�v��Ԃ̏ꍇ	*/
		{
			DI();
			P7 &= P74_PUMP_START_OFF;					/* �����OFF 					*/
			EI();
			P4 &= P41_VACUUM_RELEASE_OFF;				/* �^������OFF�i�J���j		*/ 
			VacuumEndControl();							/* �^��^�]�I�����̐M������i�^��d����=10�b ON,̧�=3���� ON)*/
			
			break;
		}
		
		mainVacuumElectromangCheck();
		if((key = glbGetReadKey()))							/* SW�̓�������			*/
		{
			if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)���́H				*/
			{
				mcmSetBuzzerEffective();					/* ���ݗL���޻ް					*/
				DI();
				P7 |= P74_PUMP_START_ON;					/* �����ON (�ғ�)			*/
				EI();
				P4 |= P41_VACUUM_RELEASE_ON;				/* �^������ON�i����j	*/ 
				togle++;									/* ĸ�ٍX�V					*/
				if(togle & 1)								/*							*/
				{
					P2 &= P20_VACUUM_ELECTROMAG_OFF;		/* �^��d����OFF�i�^�������~)	*/
				}
				else
				{
					P2 |= P20_VACUUM_ELECTROMAG_ON;			/* �^��d����ON	�i�^������ĊJ�j*/
				}
			}
			else if(key == GCN_KEY_PROC_SET)				/* �H��(SW2)�����ꂽ�ꍇ			*/
			{
				adData = R_ADC_Get_Result2();				/* ���;ݻ���A/D�l����		*/
				adData = GCN_BASE_AD - adData;				/* AD�l�␳(�A/D(110 - A/D�l)		*/
				if(adData > 99 || adData < -99)				/* �͈�����								*/
				{
					mcmSetBuzzerInvalid();					/* ���ݖ����޻ް			*/

				}
				else
				{
				
					if(adData < 0)							/* ���̏ꍇ					*/
					{
						glbFlSecLevel = GCN_VACUUM_SECONDS;	/* �b ���				*/
						ledControl_Sec(glbFlSecLevel,1,0);	
							
						data = (UBYTE)(~adData + 1);
					}
					else
					{
						ledControl_Sec(glbFlSecLevel,1,0);	
						data = (UBYTE)(adData);
					}
					geefFlCourse = sysFlCourse;				/* �������							*/			
					geefCtAnalogAdjust = data;				/* �����l���ޯ�������؂־��	*/
					sysFlashWrite();
					
					mainDisplay7Seg((UBYTE)data);			/* �����l��7SEG�\��					*/
					ledControl_Course(ALL_LED_SELECT,0,0);							/* 7SEG/LED ALL OFF			*/ 
					mcmSetBuzzerEffective();				/* ���ݗL���޻ް					*/
				}
			}
			else if(key == GCN_KEY_DOWN_SET)				/* ��(SW4)							*/
			{
															/* �^��������̏ꍇ	�A������������Ă���ԁA�^�������s��	*/
				if(togle & 1)								/* �^��������̏ꍇ�H				*/
				{
					if(!(P7 & P74_PUMP_START_ON))			/* ����ߒ�~���H 					*/
					{
						mcmSetBuzzerInvalid();				/* ���ݖ����޻ް			*/
						continue;
					}
					mcmSetBuzzerEffective();				/* ���ݗL���޻ް					*/
					P2 &= P20_VACUUM_ELECTROMAG_OFF;		/* �^��d����OFF�i�^�������~)	*/
					sts = 1;
					MaxKeyOnTime = GCN_TIME_100MS;			/* ���������ʒm����	�i0.1�b)			*/
					while(sts == 1)
					{
						sts = glbWaitKeyOff(key,MaxKeyOnTime);	/* KEY���������̂�҂�				*/
						P4 &= P41_VACUUM_RELEASE_OFF;		/* �^������OFF�i�J���j		*/ 
					}
					P4 |= P41_VACUUM_RELEASE_ON;			/* �^������ON�i����j		*/ 
				}
				else										/* �^��������łȂ��ꍇ			*/
				{
					mcmSetBuzzerInvalid();					/* ���ݖ����޻ް			*/
				}
			}
			else if(key == GCN_KEY_UP_SET)					/* ��(SW3)						*/
			{
				if(!(P7 & P74_PUMP_START_ON))				/* ����ߒ�~���H 					*/
				{
					mcmSetBuzzerInvalid();					/* ���ݖ����޻ް			*/
					continue;
				}
				mcmSetBuzzerEffective();					/* ���ݗL���޻ް					*/
				DI();
				P7 &= P74_PUMP_START_OFF;					/* �����OFF 					*/
				EI();
				P4 &= P41_VACUUM_RELEASE_OFF;				/* �^������OFF�i�J���j		*/ 
				VacuumEndControl();							/* �^��^�]�I�����̐M������i�^��d����=10�b ON,̧�=3���� ON)*/
				
				break;
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ���ݖ����޻ް			*/
			}
		}
	}
}


#define	LCN_7SEG_ALL_OFF	0x00
#define	LCN_7SEG_ALL_ON		0xff

#define	LCN_LED_ALL_OFF		8
#define	LCN_LED_ALL_ON		9


/***********************************************************************************************************************
*    �֐���    sysPanelTestMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �p�l���e�X�g���[�h
*    �������e  �p�l����̕\�����i�V�Z�O�A�k�d�c�j�A�X�C�b�`�����̓���e�X�g���s���B
*				����P�F�m����/�ެ��߁n�X�C�b�`�������ƁA�u�S�Ă̂k�d�c�v�y�сu�V�Z�O�S�Z�O�����g�\���v��_���B
*					     �ēx�����Ɓu�S�Ă̂k�d�c�v�y�сu�V�Z�O�S�Z�O�����g�v�������B
*                       �m����/�ެ��߁n�X�C�b�`�������тɑS�_���̑S�������J��Ԃ��B
*    			����Q�F�m�R�[�X�n�r�v���������ƂɁA�e�u�R�[�X�k�d�c�v��_���ړ�����B
*               ����R�F�m�H���n�r�v���������ƂɁA�e�u�H���k�d�c�v�_���ړ�����B
*               ����S�F�m�į�߁n�r�v�i�B���X�C�b�`�j���������ƂɁA�u�b�v�u���v�̂k�d�c���_���ړ�����B
*               ����T�F�m���n�X�C�b�`���������ƂɁA�V�Z�O�k�d�c�́u�Z�O�����g�v�P�ʂŌʂɓ_���ړ�����B
*
*    ���l�@�@�@�E���̃X�C�b�`����͑S�Ė����Ƃ���B
*              �E��d���ؒf�ŏI���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysPanelTestMode(void)
{
	SWORD	ctcourse = -1;
	SWORD	ctproc   = -1;
	SWORD	cttips   = -1;
	SWORD	ct7seg 	 = -1;
	UBYTE	key,togle=0;
	UBYTE	operation = 1;
	UBYTE	tbl7seg2[16] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UBYTE	tbl7seg1[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


	glbFlSysytem7Seg = 1;											/* ����ý�7SEG�p�\��Ӱ�޾��	*/
	sys7SegLedAllOff();												/* 7SEG/LED ALL OFF			*/
	while(1)
	{
		if((key = glbGetReadKey()))									/* KEY�̓�������			*/
		{
			if(key == GCN_KEY_START_SET)							/* ���ķ����́H				*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				togle++;											/* �\���؊����׸ލX�V		*/
				togle &= 1;											/* �\���؊����׸�ĸ��		*/
				if(togle)
				{
					sys7SegLedAllOn();								/* 7SEG/LED ALL ON			*/
				}
				else
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
				}

				operation = 1;
				ctcourse = -1;
				ctproc   = -1;
				cttips   = -1;
				ct7seg 	 = -1;
			}
			else if(key == GCN_KEY_COURSE_SET)						/* ��������́H				*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				if(operation == 1)									/* ����1����I�����ꂽ�ꍇ	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				ctcourse++;											/* ����X�V					*/
				ctcourse &= 0x03;									/* ����������				*/
				geefFlCourse =  ctcourse;							/* ���No.���				*/	
				ledControl_Course(geefFlCourse,1,0);					/* 7SEG/LED ALL OFF			*/
			}
			else if(key == GCN_KEY_PROC_SET)						/* �H��������́H			*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				if(operation == 1)									/* ����1����I�����ꂽ�ꍇ	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				ctproc++;											/* �H���X�V					*/
				ctproc = (ctproc > GCN_PROC_COOL)?					/* �H���������				*/
						   GCN_PROC_VACUUM:ctproc;
				
				ledControl_Proc(ctproc,1,0);						
			}
			else if(key == GCN_KEY_STOP_SET)						/* �į�߷��i�B�����j���́H	*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				if(operation == 1)									/* ����1����I�����ꂽ�ꍇ	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				cttips++;											/* �b or % �؊����X�V	*/
				cttips &= 1;										
				glbFlSecLevel  = cttips;							/* �b or % ���				*/
				ledControl_Sec(glbFlSecLevel,1,0);					
			}	
			else if(key == GCN_KEY_UP_SET)							/* �������́H				*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				if(operation == 1)									/* ����1����I�����ꂽ�ꍇ	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
					ct7seg 	 = -1;
				}
				ct7seg++;											/* ����1�����čX�V			*/
				ct7seg &= 15;										/* �������					*/
				glbDt7Seg2 = tbl7seg2[ct7seg];						/* 2���ڂ�7SEG��1�ӏ�ON		*/
				glbDt7Seg1 = tbl7seg1[ct7seg];						/* 1���ڂ�7SEG��1�ӏ�ON		*/
			}
			else if(key == GCN_KEY_DOWN_SET)						/* �������́H				*/
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
				if(operation == 1)									/* ����1����I�����ꂽ�ꍇ	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
					ct7seg 	 = -1;
				}
				ct7seg--;											/* ����1�����čX�V			*/
				if(ct7seg < 0)										/* ϲŽ�Ɉڍs�H				*/
				{
					ct7seg = 15;
				}
				glbDt7Seg2 = tbl7seg2[ct7seg];						/* 2���ڂ�7SEG��1�ӏ�ON		*/
				glbDt7Seg1 = tbl7seg1[ct7seg];						/* 1���ڂ�7SEG��1�ӏ�ON		*/
			}
			else
			{
				mcmSetBuzzerInvalid();								/* ���ݖ����޻ް			*/
			}
		}
	}
}

/***********************************************************************************************************************
*    �֐���    sysInputTestMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���̓e�X�g���[�h
*    �������e  �E�u�V�Z�O�v�Ɉ��̓Z���T�[�̒l�i�g�d�w�j��\������B
*			   �E  �\���͈́F�O�O�O���`�R�e�e��
*    ���l�@�@�@�E���̃X�C�b�`����͑S�Ė����Ƃ���B
*              �E��d���ؒf�ŏI���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void 	sysInputTestMode(void)
{
	UWORD	sts,key,MaxKeyOnTime ;
	UBYTE	seg1,seg2;

	while(1)
	{

		if((key = glbGetReadKey()))								/* SW�̓�������			*/
		{
			if(key == GCN_KEY_DOWN_SET)							/* ��(SW4)�̏ꍇ				*/
			{
				sts = 1;
				MaxKeyOnTime = GCN_TIME_1S;						/* SW�������ʒm����	�i1�b)				*/
				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				while(sts == 1)									/* SW��������Ă���Ԃ�AD�̉���1�޲Ă�\��	*/
				{
					sysGetVacuumLevel(0,&seg2,&seg1);			/* ����1�޲Ă�AD�l��2���œ���				*/
																/* ��j0x3B7��seg1=0x07,seg2=0x0b			*/
 					glbDt7Seg2 = seg2;							/* 2���ڂ��								*/
 					glbDt7Seg1 = seg1;							/* 1���ڂ��								*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);		/* SW���������̂�҂�					*/
					MaxKeyOnTime = GCN_TIME_100MS;				/* SW�������ʒm����	�i0.1�b)				*/
				}
			}
			else
			{
				mcmSetBuzzerInvalid();							/* ���ݖ����޻ް			*/
			}
		}
		else
		{
			sysGetVacuumLevel(1,&seg2,&seg1);					/* ���1�޲Ă�AD�l��2���œ���				*/
																/* ��j0x3B7��seg1=0x03,seg2=0x00			*/
			glbDt7Seg2 = seg2;									/* 2���ڂ��								*/
			glbDt7Seg1 = seg1;									/* 1���ڂ��								*/
		}
	}
}


/***********************************************************************************************************************
*    �֐���    sysGetVacuumLevel
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^�󈳗̓Z���T�[��AD�l�����
*    �������e  AD�l����͂��A�P�U�i���̕�����ɕϊ�
*    ���l�@�@�@
*              
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysGetVacuumLevel(UBYTE culum,UBYTE *pSeg2,UBYTE *pSeg1)
{
	UWORD	iadData;
	UBYTE	data,answer[4],j,k,i;
	UBYTE	henkan[16] = "0123456789ABCDEF";

	for(i=0;i<4;++i)
	{
		answer[i] = '0';
	}
	iadData = R_ADC_Get_Result2();						/* ���;ݻ���A/D�l����		*/
//	iadData = 0x3ff - iadData;							/* AD�l�𔽓]������			*/
	i=0;
	while(iadData > 0) {								/* 10�i����16�i������ɕϊ�	*/
		j = iadData % 16;
   		iadData = iadData / 16;
   		answer[i] = henkan[j];
   		i++;
	}
														/* ��jAD�l�F0x3b7�̏ꍇ		*/
														/* answer[0] = '7',answer[1] = 'B',answer[2] = '3',answer[3] = '0'	*/
	k = culum * 2;										/* k=0 answer[0]��answer[1] / k=2 nswer[2]��answer[3] */

	if(answer[k] >= '0' && answer[k] <= '9')				
	{
		data = answer[k] - '0';
	}
	else
	{
		data = answer[k] - 0x37;
	}
	*pSeg1 = data;										/* 7SEG��1���ڂɕ\��		*/
	if(answer[k+1] >= '0' && answer[k+1] <= '9')
	{
		data = answer[k+1] - '0';
	}
	else
	{
		data = answer[k+1] - 0x37;
	}
	*pSeg2 = data;										/* 7SEG��2���ڂɕ\��		*/

}



/***********************************************************************************************************************
*    �֐���    sysOutputTestMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �o�̓e�X�g���[�h
*    �������e  �E�p�l����̊e�X�C�b�`�𑀍삵�Ċe�o�͋@��̃e�X�g���s���B
*			   �E�m���n�m���n�X�C�b�`�������āA�o�͂��郆�j�b�g��I��
*              �E�m�X�^�[�g�^�X�g�b�v�n���������тɁA�\������Ă���ԍ��Ή��̋@����n�m�^�n�e�e����B
*              �E[�R�[�X�n�X�C�b�`�������ꂽ��A�S�Ă̏o�͂��n�e�e����B
*    ���l�@�@�@�E���̃X�C�b�`����͑S�Ė����Ƃ���B
*              �E��d���ؒf�ŏI���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
#define	LCN_SYS_OUTPUT_NUM		8

struct	tagOUTPUTTEST	sysOutputTest[LCN_SYS_OUTPUT_NUM] = {
											{&P7,P74_PUMP_START_ON          ,P74_PUMP_START_OFF			},			/* ����ߏo��I/O,ON�ް�	*/
											{&P2,P20_VACUUM_ELECTROMAG_ON   ,P20_VACUUM_ELECTROMAG_OFF	},			/* �^��d����I/O,ON�ް�	*/
											{&P13,P130_GUS_ELECTROMAG_ON    ,P130_GUS_ELECTROMAG_OFF     },			/* �޽�d����I/O,ON�ް�	*/
											{&P0,P01_SEEL_ELECTROMAG_ON		,P01_SEEL_ELECTROMAG_OFF	},			/* ��ٓd����I/O,ON�ް�	*/
											{&P4,P41_VACUUM_RELEASE_ON		,P41_VACUUM_RELEASE_OFF		},			/* �^������I/O,ON�ް�	*/
											{&P7,P75_SOFTRELEASE_START_ON	,P75_SOFTRELEASE_START_OFF	},			/* ��ĉ����I/O,ON�ް�	*/
											{&P7,P73_HEATER_START_ON        ,P73_HEATER_START_OFF		},			/* ˰�,ON�ް�			*/
											{&P7,P72_POWER_LED_ON          ,P72_POWER_LED_OFF 			}			/* ̧�,ON�ް�			*/
										   };
void	sysOutputTestMode(void)
{
	SWORD	OutputNo=0;
	UBYTE	key,i;
	volatile	UBYTE	*pIO;

	while(1)
	{
		if(glbFlSysHeaterReq == 1)								/* ˰��Ď��v���L��H		*/
		{
			if(glbCtSysHeaterOffTimer == 0)						/* 2�b�o�߁H				*/
			{
				DI();
				*pIO &= sysOutputTest[6].BitOFF;				/* ˰�OFF					*/
				EI();
			}
		}
		mainDisplay7Seg(OutputNo);								/* �o�͋@��No.��\��		*/
		if((key = glbGetReadKey()))								/* KEY�̓�������			*/
		{
			if(key == GCN_KEY_UP_SET)							/* ��(SW3)���́H				*/
			{
				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				OutputNo++;										/* �o��No.�X�V				*/
				if(OutputNo >= LCN_SYS_OUTPUT_NUM)
				{
					OutputNo = 0;
				}
			}
			else if(key == GCN_KEY_DOWN_SET)					/*��(SW4)���́H				*/
			{
				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				OutputNo--;										/* �o��No.�X�V				*/
				if(OutputNo < 0)
				{
					OutputNo = LCN_SYS_OUTPUT_NUM - 1;
				}
			}
			else if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)���́H				*/
			{
				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				pIO = sysOutputTest[OutputNo].pIO;				/* �o�͋@���߰ı��ڽذ��	*/
				DI();
				if(*pIO & sysOutputTest[OutputNo].BitON)		/* �o�͋@���߰Ă̎w���ޯĂ�H�̏�Ԃ̏ꍇ	*/
				{
					DI();
					*pIO &= sysOutputTest[OutputNo].BitOFF;		/* �w��I/O���ޯĂ�L�ɂ���	*/
					EI();
				}
				else
				{
					DI();


					*pIO |= sysOutputTest[OutputNo].BitON;		/* �w��I/O���ޯĂ�L�ɂ���	*/
					EI();
					if(OutputNo == 6)							/* ˰��o�͐M���̏ꍇ		*/
					{
						glbFlSysHeaterReq = 1;					/* ˰��Ď��v�����			*/
						glbCtSysHeaterOffTimer = GCN_TIMER_2S;	/* 2�b�Ď���Ͼ��			*/
					}
				}
				EI();
			}
			else if(key == GCN_KEY_COURSE_SET)					/*���(SW1)����́H				*/
			{
				mcmSetBuzzerInvalid();							/* ���ݖ����޻ް			*/
//				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				for(i=0;i<LCN_SYS_OUTPUT_NUM;i++)
				{
					pIO = sysOutputTest[i].pIO;					/* �o�͋@���߰ı��ڽذ��	*/
					DI();
					*pIO &= sysOutputTest[i].BitOFF;			/* OFF�o��					*/
					EI();
				}
				glbFlSysHeaterReq = 0;							/* ˰��Ď��v��ؾ��			*/
			}
			else
			{
				mcmSetBuzzerInvalid();														/* ���ݖ����޻ް			*/
			}

		}
	}		
}
/***********************************************************************************************************************
*    �֐���    sysRamClearMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  RAM�ر�i�ޯ�������؂̏������j
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysRamClearMode(void)
{
	mcmFlashDataInitial();						/* �ޯ������ް��̏�����			*/
	glbDt7Seg2 = 'E' - 0x37;					/* 2���ڂ�7SEG��'E'��\��		*/
	glbDt7Seg1 = 'D' - 0x37;					/* 1���ڂ�7SEG��'d'��\��		*/
	sysLedAllOff();								/* ���ׂĂ�LED�@OFF				*/
	
	while(1);
}

/***********************************************************************************************************************
*    �֐���    sysErrorLogMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �G���[����\�����[�h
*    �������e �P�O�񕪂̃G���[������\������B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysErrorLogMode(void)					
{
	UBYTE	key,ErrorCourse,ErrorProcess,ErrorCode,flContent=0 ;
	SWORD	i = 1;

	mainDisplay7Seg(i);												/* 7SEG�\��					*/
	while(1)
	{
		while(!(key = glbGetReadKey()))								/* SW�̓�������			*/
		{
																	/* ���͂��Ȃ��������		*/
		}
		if(key == GCN_KEY_UP_SET)									/* ��(SW3)�̏ꍇ				*/
		{
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/
			i++;													/* �װ����No.�X�V(+)		*/
			if(i > 10)												/* �������					*/
			{
				i = 1;
			}
			flContent = 0;											/* START/STOP(SW5)�������ꂽ���Ƃ�F�������׸�ؾ��	*/
			mainDisplay7Seg(i);										/* 7SEG�\��					*/
			sysLedAllOff();											/* LED ALL OFF	*/
		}
		else if(key == GCN_KEY_DOWN_SET)							/* ��(SW4)�̏ꍇ				*/
		{
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/
			i--;													/* �װ����No.�X�V(-)		*/
			if(i < 1)												/* ��������					*/
			{
				i = 10;
			}
			flContent = 0;											/* START/STOP(SW5)�������ꂽ���Ƃ�F�������׸�ؾ��	*/
			mainDisplay7Seg(i);										/* 7SEG�\��					*/
			sysLedAllOff();											/* LED ALL OFF	*/
		}
		else if(key == GCN_KEY_START_SET)							/* START/STOP(SW5)					*/
		{
			if(flContent == 1)										/* START/STOP(SW5)�������ĉ����ꂽ�ꍇ		*/
			{
				mcmSetBuzzerInvalid();								/* ���ݖ����޻ް			*/
			}
			else
			{
				mcmSetBuzzerEffective();							/* ���ݗL���޻ް					*/
			}
			ErrorCourse		= geefBfErrorHistory[i-1][1] >> 4;		/* �װ�������̺��No.ذ��	*/
			ErrorProcess    = geefBfErrorHistory[i-1][1] & 0x0f;	/* �װ�������̍H��ذ��		*/
 			if(ErrorCourse == 0)									/* ����^�]���ȊO�̴�-�̏ꍇ	*/
			{
				ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL OFF			*/
			}
			else
			{
				geefFlCourse = ErrorCourse - 1;						/* ����ɑΉ�����LED�_��		*/
				ledControl_Course(geefFlCourse,1,0);					/* 7SEG/LED ALL			*/
			}
			if(ErrorProcess == 0)									/* �H���^�]�ȊO�̏ꍇ		*/
			{
				ledControl_Proc(ALL_LED_SELECT,0,0);									
			}
			else
			{
				ErrorProcess = (ErrorProcess > 1)?(ErrorProcess - 2):(ErrorProcess - 1);	/* ����ɑΉ�����LED�_��		*/				   
			   ledControl_Proc(ErrorProcess,1,0);								
			}
			ErrorCode = geefBfErrorHistory[i-1][0];					/* �I�����ꂽ�װ����No.�̴װ����ذ��	*/
			if(ErrorCode == 0xff)									/* �װ�������̏ꍇ			*/
			{
				
				ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
				ledControl_Proc(ALL_LED_SELECT,0,0);											
				glbDt7Seg2 = 										/* 7SEG��2���ڂ�'-'�\��		*/
				glbDt7Seg1 = GCN_7SEG_HAIPHONG;						/* 7SEG��2���ڂ�'-'�\��		*/
				ledControl_Sec(ALL_LED_SELECT,0,0);		
			}
			else
			{
				glbDt7Seg2 = 'E' - 0x37;							/* 7SEG��2���ڂ�'E'�\��		*/
				glbDt7Seg1 = ErrorCode;								/* 7SEG��1���ڂɴװ���ޕ\��	*/
				ledControl_Sec(ALL_LED_SELECT,0,0);	
			}
			flContent = 1;											/* START/STOP(SW5)�������ꂽ���Ƃ�F�������׸޾��	*/
		}
		else
		{
			mcmSetBuzzerInvalid();									/* ���ݖ����޻ް			*/
		}
	}

}

/***********************************************************************************************************************
*    �֐���    sysPumpTimeMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �u�V�Z�O�v�\����Ɂu�|���v�ώZ�l�i�O�`�X�X�X�X���ԁj�v��\������B
*    �������e  �p�l���X�C�b�`��������Ă��Ȃ��ꍇ�u�|���v�ώZ�l�v�̏�ʂQ����\��
*			   ���̃X�C�b�`�������Ă���ԁA�u�|���v�ώZ�l�v�̉��ʂQ����\��
*             �i���X�C�b�`������Ɓu�|���v�ώZ�l�v�̏�ʕ\��)
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	sysPumpTimeMode(void)
{
	UWORD	sts,key,MaxKeyOnTime ;
	UBYTE	seg1,seg2;



	while(1)
	{

		if((key = glbGetReadKey()))								/* SW�̓�������			*/
		{
			if(key == GCN_KEY_DOWN_SET)							/* ���iSW4)�̏ꍇ				*/
			{
				mcmSetBuzzerEffective();						/* ���ݗL���޻ް					*/
				sts = 1;
				MaxKeyOnTime = GCN_TIME_1S;						/* SW�������ʒm����	�i1�b)				*/
				while(sts == 1)									/* KEY��������Ă���Ԃ�AD�̉���1�޲Ă�\��	*/
				{
					sysGetPumpTime(0,&seg2,&seg1);				/* ����1�޲Ă�����߉ғ����Ԃ�2���œ���		*/
																/* ��j1234��seg1=0x04,seg2=0x03					*/
 					glbDt7Seg2 = seg2;							/* 2���ڂ��								*/
 					glbDt7Seg1 = seg1;							/* 1���ڂ��								*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);		/* KEY���������̂�҂�					*/
					MaxKeyOnTime = GCN_TIME_100MS;				/* ���������ʒm����	�i0.1�b)				*/
				}
			}
			else
			{
				mcmSetBuzzerInvalid();							/* ���ݖ����޻ް			*/
			}
		}
		else
		{
			sysGetPumpTime(1,&seg2,&seg1);						/* ���1�޲Ă�AD�l��2���œ���				*/
																/* ��j1234��seg1=0x02,seg2=0x01			*/
			glbDt7Seg2 = seg2;									/* 2���ڂ��								*/
			glbDt7Seg1 = seg1;									/* 1���ڂ��								*/
		}
	}
}

/***********************************************************************************************************************
*    �֐���    sysGetPumpTime
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �|���v�ғ����Ԃ��P�O�i������ɕϊ�
*    �������e 
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	sysGetPumpTime(UBYTE culum,UBYTE *pSeg2,UBYTE *pSeg1)
{
	UBYTE	data;
	UWORD	iadData;
	UBYTE	answer[4],j,k,i;
	UBYTE	henkan[10] = "0123456789";


	for(i=0;i<4;++i)
	{
		answer[i] = '0';
	}
	iadData = geefCtPumpRunning / 60;								/* ����߉ғ����ԁigeefCtPumpRunning)�͕��P�ʂ� 	*/
																	/* �i�[����Ă��邽�߁A��������ԁiH)�ɕϊ�����	*/
	i=0;
	while(iadData > 0) {											/* 10�i����10�i������ɕϊ�	*/
		j = iadData % 10;
   		iadData = iadData / 10;
   		answer[i] = henkan[j];
   		i++;
	}
																	/* ��j���ԁF1234���Ԃ̏ꍇ		*/
																	/* answer[0] = '4',answer[1] = '3',answer[2] = '2',answer[3] = '1'	*/
	k = culum * 2;													/* k=0 answer[0]��answer[1] / k=2 nswer[2]��answer[3] */

	data = answer[k] - '0';
	*pSeg1 = data;													/* 7SEG��1���ڂɕ\��		*/
	data = answer[k+1] - '0';
	*pSeg2 = data;													/* 7SEG��2���ڂɕ\��		*/

}

/***********************************************************************************************************************
*    �֐���    sys7SegLedAllOff
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���ׂĂ̂V�r�d�f�A�k�d�c�@�n�e�e
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sys7SegLedAllOff(void)
{
	glbDt7Seg2 = 0x00;			/* 2���ڂ�7SEG��ALL OFF		*/
	glbDt7Seg1 = 0x00;			/* 1���ڂ�7SEG��ALL OFF		*/
	
	ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Sec(ALL_LED_SELECT,0,0);		
	
	glbFlLed11 = 0;
	glbFlContinue = 0;									/* �A��Ӱ��	*/	
}

/***********************************************************************************************************************
*    �֐���    sysLedAllOff
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���ׂĂ̂V�r�d�f�A�k�d�c�@�n�e�e
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysLedAllOff(void)
{
	
	ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Sec(ALL_LED_SELECT,0,0);		
	
	glbFlLed11 = 0;
	glbFlContinue = 0;									/* �A��Ӱ��	*/	
}

/***********************************************************************************************************************
*    �֐���    sys7SegLedAllOn
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���ׂĂ̂V�r�d�f�A�k�d�c�@�n�m
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sys7SegLedAllOn(void)				/* 7SEG/LED ALL ON			*/
{
	glbDt7Seg2 = 0xff;			/* 2���ڂ�7SEG��ALL ON		*/
	glbDt7Seg1 = 0xff;			/* 1���ڂ�7SEG��ALL ON		*/
	ledControl_Course(ALL_LED_SELECT,1,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,1,0);									
	ledControl_Sec(ALL_LED_SELECT,1,0);	
	
	glbFlLed11 = 1;
	glbFlContinue = 1;									/* �A��Ӱ��	*/	
	
}

/***********************************************************************************************************************
*    �֐���    sysLedAllOn
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���ׂĂ̂k�d�c�@�n�m
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysLedAllOn(void)				/* 7SEG/LED ALL ON			*/
{
	
	ledControl_Course(ALL_LED_SELECT,1,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,1,0);											
	ledControl_Sec(ALL_LED_SELECT,1,0);
	
	glbFlLed11 = 1;
	glbFlContinue = 1;									/* �A��Ӱ��	*/	
}

/***********************************************************************************************************************
*    �֐���    R_ADC_Get_Result2
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �`�c�l����
*    �������e  ANI18����ǂݍ���
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UWORD	R_ADC_Get_Result2(void)
{
    return((UWORD)(ADCR >> 6U));
}

/***********************************************************************************************************************
*    �֐���    sysFlashWrite
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �ݒ��ް����ׯ���֏�������
*    �������e  �Ώۂ̓A�i���O�␳�l�A�|���v�ғ�����
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	sysFlashWrite(void)
{
	UBYTE	err;

	err = pdlWriteFlash();						/* �ޯ������ް����ׯ���֏�������				*/
	if(err != 0)								/* �װ�H							*/
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;	/* �ׯ��ذ�޴װ						*/
		mainErrorMode();						/* �װӰ�ނ�						*/
	}
	
}


/***********************************************************************************************************************
*    �֐���    sysVacuumStar
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�^������J�n���̐���M���o��
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void sysVacuumStart(void)
{
	DI();

	P7 |= P74_PUMP_START_ON;									/* ����߉ғ��J�n					*/ 

	EI();
	P4 |= P41_VACUUM_RELEASE_ON;									/* �^������ON						*/ 
	P2 |= P20_VACUUM_ELECTROMAG_ON;									/* �^��d����ON						*/
	glbFlVacuumElectromangReq = 0;									/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��ؾ��	*/

}
