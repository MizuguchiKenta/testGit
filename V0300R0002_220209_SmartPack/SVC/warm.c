#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"



void	mainWarmMode(void);
void	mainWarmStart(void);
void	mainWarmEnd(void);
void	mainWarmDrive(void);

void	(*WarmProcTbl[])(void);

extern	void	mainDisplay7Seg(UWORD);										/* 7SEG�\��							*/
extern	UBYTE	glbGetReadKey(void);
extern	UBYTE	DrawerSwRead(void);
extern	void	mcmSetBuzzerEnd(void);										/* �I���޻ް��炷			*/
extern	UBYTE	mcmCheckDrawerOpenToClose(void);
extern	UBYTE	mcmFlashWriteCheck(void);							/* �ޯ�������ؕύX����				*/
extern	void	mcmSetBuzzerInvalid(void);
extern	void	mcmSetBuzzerEffective(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

void	(*WarmProcTbl[])() =
{
	mainWarmDrive				/* �g�@�^�]�Ď�						*/
};
/***********************************************************************************************************************
*    �֐���    mainDriveMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �g�C�^�]���[�h���C������
*    �������e  �g�@�^�]�ƏI�����T5���ԊĎ�����
*    ���Ӂ@�@�@
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void mainWarmMode(void)
{
	(*WarmProcTbl[glbFlWarmMode])();								/* ����Ԃ�Ӱ�ނ֑J��		*/

}
/***********************************************************************************************************************
*    �֐���    mainWarmDrive
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �g�C�^�]����
*    �������e  
*    ���Ӂ@�@�@�g�@�^�]���ɂr�s�`�q�s�^�r�s�n�o�X�C�b�`�������ꂽ�ꍇ�́A�g�@�^�]�𒆎~����
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainWarmDrive(void)
{
	UBYTE	key;
	UWORD	data;

	glbFlSaveMode = glbFlDriveMode;										/* Ҳ�Ӱ�ނ̏�Ԃ�ޔ�			*/
	data = ((glbCtWarmTimer * GCN_INT_TIME)/GCN_TIME_1S);				/* 5ms�P�ʂ̶�����b�ɕϊ�			*/
	mainDisplay7Seg((UWORD)data);										/* 7SEG�\��							*/

	if((key = glbGetReadKey()))											/* KEY�̓�������					*/
	{		
		if(key == GCN_KEY_START_SET)									/* ����/�į�߷��������ꂽ�ꍇ		*/
		{
			mcmSetBuzzerEffective();									/* ���ݗL���޻ް					*/
			mainWarmEnd();												/* �g�C�^�]�I������					*/
			if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
			{
				data = geefBfCourseData[geefFlCourse][Vacuum];			/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
			}
			else
			{
				data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
			}
			mainDisplay7Seg(data);										/* 7SEG�\��					*/
		}
		else if(key == GCN_KEY_STOP_SET)								/* �į�߷��i�B���j�������ꂽ�ꍇ		*/
		{
																		/* �޻ް��炳�Ȃ�					*/
		}
		else 
		{
			mcmSetBuzzerInvalid();										/* ���ݖ����޻ް			*/
		}
	}
	
	if(glbFlPowerSw)
	{	
		mcmSetBuzzerEffective();									/* ���ݗL���޻ް					*/
		mainWarmEnd();												/* �g�C�^�]�I������					*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];			/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
		}
		else
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
		}
		mainDisplay7Seg(data);										/* 7SEG�\��					*/	
	}
				

	if(glbCtVacuumElectromang == 0)										/* �^��d����ON�o�߁H(0.5�b)		*/
	{
		P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* �^��d����OFF					*/
	}

	if(glbCtWarmTimer == 0)												/* �g�C�^�]�I�����Ԍo��?			*/
	{
		mainWarmEnd();													/* �g�C�^�]�I������					*/
		glbFlWarmMode = 1;												/* �g�@�^�]Ӱ�ނ�T5���ԊĎ���		*/
	}

}

/***********************************************************************************************************************
*    �֐���    mainWarmStart
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �g�C�^�]�J�n����
*    �������e  
*    ���Ӂ@�@�@�g�C�^�]�ĊJ�̏ꍇ�́A�O�D�T�b�Ԃ̐^��d���ق�ON���Ȃ����ƁB�B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainWarmStart(void)
{
	unsigned long int	data;
	volatile	int	i;
	
	DI();

	P7 |= P74_PUMP_START_ON;										/* ����߉ғ��J�n					*/ 

	EI();
	
	ledControl_Proc(ALL_LED_SELECT,0,0);									

	glbFlReqDot         = 0;											/* ��ص�ޕ\������					*/
	glbFlWarmMode = 0;													/* �g�@�^�]Ҳ��׸ށ˒g�@�^�]����	*/
	if(glbFlWarmCansel == 0)											/* �g�C�^�]�r����ݾفː^��^�]�ˍēx�A�g�C�^�]�̏ꍇ�́A0.5�b�Ԃ̐^��d��ON�͍s��Ȃ�	*/
	{
		P2 |= P20_VACUUM_ELECTROMAG_ON;									/* �^��d����ON						*/
		glbCtVacuumElectromang = GCN_TIMER_500MS;						/* �^��d���ق�0.5�b�J�����߂���Ͼ��*/
		glbFlVacuumElectromangReq = 1;									/* �^��d���ق�0.5�b�J�����߂̗v�����	*/
	}
	data = geefBfSystemData[GCN_SYSTEM_WARM][Default];
	mainDisplay7Seg((UWORD)data);										/* 7SEG�\��							*/

	glbFlSecLevel   =  GCN_VACUUM_SECONDS;								/* �u�b�v�\���ɂ���					*/
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbCtWarmTimer = (geefBfSystemData[GCN_SYSTEM_WARM][Default] *		/* �g�C�^�]���Ծ�āi�b�j			*/
					 GCN_TIMER_1S);

}

/***********************************************************************************************************************
*    �֐���    mainWarmEnd
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �g�C�^�]�I������
*    �������e  
*    ���Ӂ@�@�@
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainWarmEnd(void)
{
	UWORD	data;

	DI();
	P7 &= P74_PUMP_START_OFF;											/* ����߉ғ���~					*/ 
	EI();
	P2 |= P20_VACUUM_ELECTROMAG_ON;										/* �^��d����ON						*/
	glbFlVacuumElectromangReq = 1;										/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v�����	*/
	glbCtVacuumElectromang = GCN_TIMER_10S;								/* �g�C�^�]�I����A�^��d���ق�10�b�ԊJ���iON�j���邽�߂���Ͼ��	*/
// ��20180630
//	glbFlFunReq = 1;													/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��,3����̧݂��쓮�����邽�߂̗v�����	*/
// ��	20180630
	glbCtFunTimer = GCN_TIME_3MN;										/* �g�C�^�]�I����A̧݂�3���ԉ񂷂��߂���Ͼ��	*/
	glbFlWarmCansel = 0;
	mcmSetBuzzerEnd();													/* �I���޻ް			*/
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)			/* '�b'�\���̏ꍇ								*/
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];					/* �I������Ă��麰��̐^�󎞊�ذ��				*/
	}
	else																/* '%'�\���̏ꍇ								*/
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];				/* �I������Ă��麰��̐^�󎞊�ذ��				*/

	}
	mainDisplay7Seg(data);												/* 7SEG�\��					*/
	
	glbFlProc 		= GCN_PROC_VACUUM;						/* �H�����u�^��v�ɂ���	*/
	ledControl_Proc(glbFlProc,1,0);											
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbFlMainMode = GCN_WAIT_MODE;					/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/

}


