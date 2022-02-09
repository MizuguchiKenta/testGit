/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c                                                */
/*  DATE        :                                                      */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"
#include 	"r_cg_macrodriver.h"
#include	"pfdl_user.h"
#include 	"r_cg_timer.h"

void 	mainSVC(void);
void 	mainWaitMode(void);
void	mainCourseMode(void);				/* ����ݒ�Ӱ��				*/
void	mainWarmMode(void);					/* �g�C�^�]Ӱ��				*/
void	mainDriveMode(void);				/* �^�]Ӱ��				*/
void	mainStanbyMode(void);				/* �X���[�v���[�h*/	
void	mainErrorMode(void);				/* �װ���oӰ��				*/

void	mainStanbyCheck(void);			/*���A�m�F*/
void mainPackCountDisplay_Start(void);	/*�J�E���g���\���J�n*/
void mainPackCountDisplay_End(void);	/*�J�E���g���\���I��*/	

void	mainVacuumElectromangCheck(void);
UBYTE	glbGetReadKey(void);
UBYTE   glbWaitKeyOff(UBYTE,UWORD);
void 	mainDisplay7Seg(UWORD);


void 	mainDisplayWarning7Seg(UWORD);
void	glbGetDipSW(void);							/* DIPSWذ��						*/
void	mainPortOutputInitial(void);
void	mainGlobalClear(void);							/* ������ظر						*/
void	mcmLocalClear(void);
UBYTE	mcmFlashDataRead(void);
UBYTE	mcmFlashDataInitial(void);
void	mainFlashClear(void);
void	mcmFlashInitialClear(void);
void	mcmStartBuzzer(UWORD);
UBYTE	mcmFlashWriteCheck(void);							/* �ޯ�������ؕύX����				*/
void	mainCheckPumpthermal(void);						/* ����߻��ق̴װ�Ď�		*/
void	mcmSetBuzzerEnd(void);
void	mcmSetBuzzerError(void);
void	mcmSetBuzzerEffective(void);						/* �L���޻ް��炷			*/
void	mcmSetBuzzerInvalid(void);						/* �����޻ް��炷			*/
void	mcmBuzzerOff(UWORD);						/* 170ms OFF*/
UBYTE	mcmCheckDrawerOpenToClose(void);
void	mcmCheckVacuumSensorError(void);					/* �^��ݻ��װ�̌��o		*/
void	mcmCheckVacuumSensorError(void);					/* �^��ݻ��װ�̌��o		*/
void	mcmWaitTimer(UWORD);
void	mcmSetSystemData(void);
void	mcmSetLed(UBYTE);


void	mainStartVacuum(void);

UBYTE mcmDataSettingRangeCheck(void);	/*�f�[�^���͔͈͗L���m�F*/	

void	(*mainProcTbl[])(void);


extern	void	VacuumEndControl(void);
extern	void R_TAU0_Channel0_Start(void);
extern	void R_TAU0_Channel0_Stop(void);
extern	void R_LVD_InterruptMode_Start(void);
extern	void R_INTC6_Start(void);

extern void powerLedControl(uint8_t ctl,uint8_t onTime,uint8_t offTime );	


extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);	
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);	
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);	

extern	UBYTE	DrawerSwRead(void);
extern	UBYTE	iodInKey(void);

extern	UBYTE	stiFlKydt;
extern	UBYTE	stiFlFlashing;								/* ĸ�ٍX�V								*/
extern	UWORD	stiCtFlashing;								/* 500ms��������						*/
extern	UBYTE	stiFlFlashing2;								/* ĸ�ٍX�V								*/
extern	UWORD	stiCtFlashing2;								/* 500ms��������						*/
extern	UBYTE	stiFlFlashing3;								/* ĸ�ٍX�V								*/
extern	UWORD	stiCtFlashing3;								/* 500ms��������						*/
extern	UBYTE	stiFlFlashing4;								/* ĸ�ٍX�V								*/
extern	UWORD	stiCtFlashing4;								/* 500ms��������						*/
extern	const	UBYTE	DefaultCourseData[4][8];	 		/* 4�����			*/
extern	const	UBYTE	DefaultSystemData[5][3];	 		/* 4�����			*/

extern	UWORD	pfdlFlashDataSize(void);
extern	void	pldCopyBackupData(void);	
extern	UBYTE	pldVerifyCheck(void);	
extern	UBYTE	pdlReadFlash(void);											/* �ׯ�������ޯ������ް���ǂݍ���	*/
extern	void	stiInitialSamplingADC(void);
extern	R_WDT_Restart(void);										/* �����ޯ����ؾ��							*/
extern	UBYTE	pdlWriteFlash(void);
extern	void	mainSystemMode(void);
extern	void	mainWarmStart(void);

extern void powerOn_GuruGuru7Seg(void);

UBYTE	mainFlDrawer;

uint8_t mainFlPackCountDisplay_Busy;

UBYTE	mainDtBit[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,0xff};

										// �װ�\��No.ð��� [Ex]	
UBYTE	mainTblError[8] = {
						    0 ,			/* ����߻��ٴװ			*/
							5 ,			/* *****				*/							    
							2 ,			/* �^��װ				*/
							3 ,			/* �^��x��				*/
							6 ,			/* �^��ݻ��װ			*/
							7 ,			/* CPU�װ				*/
							8 ,			/* �����ޯ��װ			*/
							9 			/* �ׯ���ް��װ			*/
							};

// Ҳ�ٰ�ݏ�ԑJ�ڊ֐�ð���	
void	(*mainProcTbl[])() =
{
	mainWaitMode,			/* �ҋ@Ӱ�ޏ���				*/
	mainCourseMode,			/* ����ݒ�Ӱ��	�i���g�p)	*/
	mainWarmMode,			/* �g�C�^�]Ӱ��				*/
	mainDriveMode,			/* �^�]Ӱ��					*/
	mainErrorMode,			/* �װ���oӰ��				*/
	mainStanbyMode,			/* �d���X�^���o�CӰ��*/
	mcmSetSystemData		/* �����ް��ݒ�Ӱ��			*/

};

/***********************************************************************************************************************
*    �֐���    mainSVC
*    ��  ��    
*    �߂�l    
*    �֐��@�\  Ҳݏ���
*    �������e  �޷��Ѹ�����6��Ӱ�ނł���y�ҋ@Ӱ�ށz,�y�^��^�]Ӱ�ށz�A�y�g�C�^�]Ӱ�ށz�A�y�װ�\��Ӱ�ށz�A
*              �y�\��Ӱ�ށz�A�y���ѐݒ�Ӱ�ށz����ԑJ���׸ށiglbFlMainMode�j�ɂ��J�ڂ���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void mainSVC(void)
{
	UBYTE 	data;
	UBYTE err;
	

	mainPortOutputInitial();														/* �o���߰ĲƼ��					*/
	mainGlobalClear();																/* ������ظر						*/
	mcmLocalClear();																/* ۰����ظر						*/
	glbGetDipSW();																	/* DIPSWذ��						*/
	R_INTC6_Start();
	R_TAU0_Channel0_Start();														/* 1ms��ϊ��荞�ݽ���				*/
	

	err = mcmFlashDataRead();								/* �ׯ��������ް��ǂݍ��݂ƲƼ��	*/
#if 0
	if(err)
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;
		glbFlMainMode = GCN_ERROR_MODE;						/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
		(*mainProcTbl[glbFlMainMode])();					/* �G���[Ӱ�ނ֑J��		*/
	}
#endif	

	mainSystemMode();																/* ý�Ӱ�ނɑJ��				*/
																					/* ý�Ӱ�ނɑJ�ڂ����ꍇ�́AmainSystemMode����߂��Ă��Ȃ�	*/
	
	powerOn_GuruGuru7Seg();
	glbDt7Seg1 = 0;
	glbDt7Seg2 = 0;
	
	glbFlContinue = 1;										/* �A��Ӱ��	*/	
	ledControl_Course(geefFlCourse,1,0);					/* LED			*/
	ledControl_Proc(glbFlProc,1,0);											
	
	powerLedControl(1,0,0);	
	

	glbFlMainMode = GCN_WAIT_MODE;													/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* �^��H����[�b]�\���̏ꍇ						*/
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];								/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
	}
	else																			/* �^��H����[%]�\���̏ꍇ						*/
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];							/* �I������Ă��麰�1�̐^������					*/

	}
	glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?			/* �^��H����[�b][%]�w���LED���f				*/
					GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;							
	ledControl_Sec(glbFlSecLevel,1,0);
	mainDisplay7Seg(data);															/* �^��H�����Ԃ܂������ق�7SEG�ɕ\��			*/

	mainFlDrawer = 0;	


	glbCtWaitTimerE6Cansel = GCN_TIMER_4_9S;									/* �^�]�I����WAIT����(T5)	*/
					   
	stiInitialSamplingADC();														/* �^��xA/D�l�Ƽ�ُ���				*/

	while(1)																		/* Ҳݏ���							*/
	{	
		if(!glbFlStanby)
		{
			
			mainCheckPumpthermal();														/* ����߻��ق̴װ�Ď�		*/
			mcmCheckVacuumSensorError();												/* �^��ݻ��װ[E6]�̌��o		*/

			if(glbFlMainMode != GCN_ERROR_MODE)
			{
				mainStanbyCheck();
			}
		}

		
		mainVacuumElectromangCheck();												/* �^��d����OFF�̊Ď�		*/
		
			
		(*mainProcTbl[glbFlMainMode])();											/* ����Ԃ�Ӱ�ނ֑J��		*/
	}
	

}


/***********************************************************************************************************************
*    �֐���    mainVacuumElectromangCheck
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^�]�H���i�^��j/�g�C�^�]�I�����̃|���v��~��10�b�Ԑ^��d����ON�̊Ď�
*    �������e  �c�C���`�����o�[���쎞�̏ꍇ�A���肪�^�]���ł����10�b�Ԑ^��d���ق�ON�̓L�����Z��
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainVacuumElectromangCheck(void)
{
	volatile	UBYTE	in1;

	if(glbFlVacuumElectromangReq == 1)			/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v���L��H*/
	{	
		if(glbCtVacuumElectromang == 0)			/* ���=0?							*/
		{
			P2 &= P20_VACUUM_ELECTROMAG_OFF;	/* �^��d����OFF					*/
			glbFlVacuumElectromangReq = 0;		/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��ؾ��	*/
		}
	}
}


/***********************************************************************************************************************
*    �֐���    mainWaitMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �ҋ@���[�h����
*    �������e  �y����iSW1)�z�A�y�H��(SW2)�z�A�y��(SW3)�z�A�y��(SW4)�z�A�ySTART/STOP(SW5)�z�A�y�^���~(SW6)�z
*               �̊Ď��Ƃ��̏������s���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void mainWaitMode(void)
{
	UBYTE	key,sts,data,*pt;
	UWORD	MaxKeyOnTime,Max,Min,Level;
	SWORD	Time;

	glbFlSaveMode = glbFlMainMode;										/* ���݂̑ҋ@Ӱ�ނ�ۑ�		*/
	
	/*======�p�b�N���J�E���g�\���@�\======*/
	if(glbFlPackCountDisplay_Start)
	{
		if(mcmFlashWriteCheck())										/* �ޯ�������ؕύX����				*/
		{
			glbFlError |= GCN_ERROR_FLASH_DATA_9;
			glbFlMainMode = GCN_ERROR_MODE;						/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
			return;
		}
	
		glbFlPackCountDisplay_Start = 0;
		
		mainPackCountDisplay_Start();
	}
	//�\�����ԏI���H
	if(glbCtPackCountDisplayTime == 0)
	{
		mainPackCountDisplay_End();
			
	}
	/*=====================================*/
	
	if(glbFlContinue == 1)												/* �A���^�]Ӱ�ނ̏ꍇ		*/
	{
		if(mcmCheckDrawerOpenToClose())									/* ���o��SW �u�J���v�ˁu����v�ɕω������ꍇ	*/
		{
			mainPackCountDisplay_End();
			
			mainStartVacuum();											/* �^��^�]Ӱ�ނ֑J��		*/
			mcmWaitTimer(500);											/* ���o���u�J�v�ˁu�v��500ms���Ă��Ă���^��^�]���J�n����*/
			return;
		}
	}
	if(!(key = glbGetReadKey()))										/* SW�̓�������			*/
	{
		return;															/* ���͂��Ȃ��������		*/
	}
	
	/*======�p�b�N���J�E���g�\������@�\======*/
	if(mainFlPackCountDisplay_Busy)
	{
		mainPackCountDisplay_End();
		return;		
	}
	/*=====================================*/
	
	MaxKeyOnTime = GCN_TIME_1S;											/* SW�������ʒm����	�i1�b)			*/
	if(key == GCN_KEY_COURSE_SET)										/* �y����iSW1�j�z�̏ꍇ				*/
	{

		mcmSetBuzzerEffective();										/* �L���޻ް��炷			*/
		glbFlProc = GCN_PROC_VACUUM;									/* �H�����u�^��v�ɂ���	*/
		ledControl_Proc(glbFlProc,1,0);									
			
		geefFlCourse++;													/* ���No.�X�V					*/
		geefFlCourse &= 3;												/* ���No.�������				*/
		
		ledControl_Course(geefFlCourse,1,0);					/* LED			*/
				
		glbFlReqDot = 0;												/* 7SEG��ص�ޔ�\��				*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)		/* "�b"(LED8)�\���̏ꍇ						*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];				/* �I������Ă��麰��̐^�󎞊�ذ��			*/
			glbFlSecLevel = GCN_VACUUM_SECONDS;							/* �u�b�v�\��					*/
		}
		else															/* "%"(LED9)�\���̏ꍇ			*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* �I������Ă��麰��̐^������ذ��				*/
			glbFlSecLevel = GCN_VACUUM_LEVEL;							/* �u%�v�\��					*/
		}
		ledControl_Sec(glbFlSecLevel,1,0);
		mainDisplay7Seg(data);											/* �^�󎞊Ԃ܂������ق�7SEG�\��					*/
	}
	else if(key == GCN_KEY_PROC_SET)									/* �H��(SW2)�̏ꍇ				*/
	{
		mcmSetBuzzerEffective();										/* �L���޻ް��炷			*/
		glbFlProc++;													/* �H��UP�X�V				*/
		if(glbFlProc > GCN_PROC_COOL)									/* �H��MAX����				*/
		{
			glbFlProc = GCN_PROC_VACUUM;								/* �H�����u�^��v�Ɉړ�		*/
		}
		ledControl_Proc(glbFlProc,1,0);									
			
		if(glbFlProc == GCN_PROC_VACUUM)								/* �u�^��v�H���̏ꍇ		*/
		{
			glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?	/* */
							GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
		}
		else
		{
			glbFlSecLevel = GCN_VACUUM_SECONDS;
		}
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;				/* �u�^��v�ȊO����ص�ޕ\��				*/
		if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) || 	/* '�b'�\���܂��͍H���I�����^��ȊO�i��فA��p�̏ꍇ)		*/
           (glbFlProc > GCN_PROC_VACUUM))
		{
			pt = &geefBfCourseData[geefFlCourse][Vacuum];				/* �I������Ă��麰��̐擪���ڽذ��		*/
			data = *(pt + glbFlProc );									/* ����̍H������ذ��					*/
		}
		else															/* '%'�ł��H���I�����^��̏ꍇ			*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* �I������Ă��麰��̐^������ذ��				*/
		}
		ledControl_Sec(glbFlSecLevel,1,0);		
		mainDisplay7Seg(data);											/* 7SEG�\��								*/
	}
	else if(key == GCN_KEY_UP_SET)										/* ���iSW3)�̏ꍇ						*/
	{
		sts = 0;
		MaxKeyOnTime = GCN_TIME_1S;										/* ���������ʒm����	�i1�b)				*/
		do
		{
			if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) ||/* '�b'(LED8)�\���܂��͍H�����^��ȊO�i��فA��p�̏ꍇ)		*/
                (glbFlProc > GCN_PROC_VACUUM))
			{
				pt = &geefBfCourseData[geefFlCourse][Vacuum];			/* �I������Ă��麰��̐擪���ڽذ��		*/
				Time = *(pt + glbFlProc);								/* ���No.�ɑ΂��Ă̑I�����ꂽ�H�����Ԃ�ذ��				*/
				Time++;													/* ���ԍX�V�i+)											*/
				Max = geefBfCourseDataMaxMin[glbFlProc].Max;			/* �I������Ă���H���l��MAX�lذ��						*/
				if(Time > Max)											/* Max�𒴂����ꍇ										*/
				{
					Time = Max;											/* Max�ɕ␳											*/
					mcmSetBuzzerInvalid();								/* �����޻ް��炷			*/
				}
				else
				{
					if(sts == 0)										/* KEY�������̎����޻ް��炳�Ȃ�*/
					{
						mcmSetBuzzerEffective();						/* �L���޻ް��炷			*/
					}
				}
				*(pt + glbFlProc) = Time;								/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�				*/
				mainDisplay7Seg(Time);									/* 7SEG�\��								*/
			}
			else														/* '%'�\���ł��H�����^��w��̏ꍇ	*/
			{
				Level = geefBfCourseData[geefFlCourse][VacuumLevel];	/* �I������Ă��麰��̐^������ذ��		*/
				Level++;
				Max = geefBfCourseDataMaxMin[VacuumLevel].Max;			/* �I������Ă���H���l��MAX�lذ��		*/
				if(Level > Max)											/* MAX�𒴂����ꍇ						*/
				{
					Level = Max;
					mcmSetBuzzerInvalid();								/* �����޻ް��炷			*/
				}
				else
				{
					if(sts == 0)										/* SW�������ꂽ�ŏ������޻ް��炷	*/
					{
						mcmSetBuzzerEffective();						/* �L���޻ް��炷			*/
					}
				}
				geefBfCourseData[geefFlCourse][VacuumLevel] = Level;	/* �I������Ă��麰��̐^�����پ��		*/
				mainDisplay7Seg(Level);									/* 7SEG�\��								*/
			}
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEY���������̂�҂�				*/
			MaxKeyOnTime = GCN_TIME_100MS;								/* SW�������ʒm����	�i0.1�b)			*/
																		/* SW��������Ă���Ԃ͑�����ɂ��邽�߁A�����Œ��������ԕύX�@*/
		}while(sts == 1);
		DI();
		glbFlKydt = 0;													/* ��߰Ē��ɓ��͂��������͎̂Ă�		*/
		EI();
	}
	else if(key == GCN_KEY_DOWN_SET)									/* ��(SW4)�̏ꍇ						*/
	{
		sts = 0;
		MaxKeyOnTime = GCN_TIME_1S;										/* ���������ʒm����	�i1�b)				*/
		do																/* KEY��������Ă���Ԃ͑�����							*/
		{
			if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) || /* '�b'(LED8)�\���܂��͍H�����^��ȊO�i��فA��p�̏ꍇ)		*/
                (glbFlProc > GCN_PROC_VACUUM))
			{
				pt = &geefBfCourseData[geefFlCourse][Vacuum];			/* �I������Ă��麰��̐擪���ڽذ��						*/
				Time = *(pt + glbFlProc);								/* ���No.�ɑ΂��Ă̑I�����ꂽ�H�����Ԃ�ذ��				*/
				Time--;													/* ���ԍX�V�i-)											*/
				Min = geefBfCourseDataMaxMin[glbFlProc].Min;			/* �I������Ă���H���l��MIN�lذ��						*/
				if(Time < (SWORD)Min)									/* Min����������ꍇ									*/
				{
					Time = Min;
					mcmSetBuzzerInvalid();								/* �����޻ް��炷					*/
				}
				else
				{
					if(sts == 0)										/* SW�������ꂽ�ŏ������޻ް��炷	*/
					{
						mcmSetBuzzerEffective();						/* �L���޻ް��炷					*/
					}
				}

				*(pt + glbFlProc) = Time;								/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�				*/
				mainDisplay7Seg(Time);									/* 7SEG�\��								*/
			}
			else														/* '%'�\���ł��H�����^��w��̏ꍇ	*/
			{
				Level = geefBfCourseData[geefFlCourse][VacuumLevel];	/* �I������Ă��麰��̐^������ذ��		*/
				Level--;
				Min = geefBfCourseDataMaxMin[VacuumLevel].Min;			/* �I������Ă���H���l��MAX�lذ��						*/
				if(Level < Min)											/* Min����������ꍇ									*/
				{
					Level = Min;
					mcmSetBuzzerInvalid();								/* �����޻ް��炷			*/
				}
				else
				{
					if(sts == 0)									    /* SW�������ꂽ�ŏ������޻ް��炷	*/
					{
						mcmSetBuzzerEffective();						/* �L���޻ް��炷			*/
					}
				}
				geefBfCourseData[geefFlCourse][VacuumLevel] = Level;	/* �I������Ă��麰��̐^�����پ��						*/
				mainDisplay7Seg(Level);									/* 7SEG�\��								*/
			}
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEY���������̂�҂�				*/
			MaxKeyOnTime = GCN_TIME_100MS;								/* SW�������ʒm����	�i0.1�b)			*/
																		/* SW��������Ă���Ԃ͑�����ɂ��邽�߁A�����Œ��������ԕύX�@*/
		}while(sts == 1);
		DI();
		glbFlKydt = 0;													/* ��߰Ē��ɓ��͂��������͎̂Ă�		*/
		EI();
	}
	else if(key == GCN_KEY_UPDOWN_SET)									/* ����(SW3,SW4)����					*/
	{
		if(glbFlProc > GCN_PROC_VACUUM)									/* �^��H��(LED5)�ȊO�̏ꍇ			*/
		{
			mcmSetBuzzerInvalid();										/* �����޻ް��炷			*/
			return;
		}
		mcmSetBuzzerEffective();										/* �L���޻ް��炷			*/
		geefFlVacuumType[geefFlCourse]++;								/* �^��������ߍX�V(0:�b,1:%)	*/
		geefFlVacuumType[geefFlCourse] &= 1;							/* �������						*/
		glbFlSecLevel       = geefFlVacuumType[geefFlCourse] ;			/* '�b'(LED8)�@or '%'(LED9) ذ��	*/

		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)		/*'�b'(LED8)�\��				*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];				/* �I������Ă��麰��̐^�󎞊�ذ��		*/
		}
		else															/*'%'(LED9)�\��				*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* �I������Ă��麰��̐^������			*/

		}
		ledControl_Sec(glbFlSecLevel,1,0);
		mainDisplay7Seg(data);											/* 7SEG�\��								*/
	}
	else if(key == GCN_KEY_START_SET)									/* START/STOP(SW5)�̏ꍇ				*/
	{

		if((DrawerSwRead() == 1))								/* ���o��SW���u�v�̏ꍇ			*/
		{
			mcmSetBuzzerEffective();									/* �L���޻ް��炷			*/
			glbFlMainMode = GCN_WARM_MODE;						/* �g�C�^�]Ӱ�ނ֑J��		*/
			mainWarmStart();									/* �g�C�^�]�J�n						*/
		}
	}
	else if(key == GCN_KEY_PUSH_SET)									/* 				*/
	{
		sts = 1;
		MaxKeyOnTime = GCN_TIME_1S;										/* ���������ʒm����	�i1�b)				*/
		while(sts == 1)													/* SW���P�b�ȓ��ɗ����ꂽ��ٰ�߂��甲����	*/
		{
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEY���������̂�҂�				*/
			if(sts == 1)												/* 1�b�Ԉȏ�KEY��������Ă���ꍇ		*/
			{
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
				glbFlMainMode = GCN_SYSTEM_MODE;						/* Ҳ�Ӱ�ނ����ް��ݒ�Ӱ�ނ�	*/

				break;
			}
		}
	}
	else if(key == GCN_KEY_STOP_SET)									/* PUSH�i�B��SW7)�̏ꍇ		*/
	{
		sts = 1;
		MaxKeyOnTime = GCN_TIME_1S;										/* ���������ʒm����	�i1�b)				*/
		while(sts == 1)													/* KEY��������Ă���Ԃ͑�����							*/
		{
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEY���������̂�҂�				*/
			if(sts == 1)												/* 1�b�Ԉȏ�KEY��������Ă���ꍇ		*/
			{
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
				
				geefCtPack = 0;
				break;
			}
			else
			{
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
				mainPackCountDisplay_Start();
				
			}
		}

	
	}
	else																/* ��L�ȊO��SW�̏ꍇ			*/
	{
		mcmSetBuzzerInvalid();											/* �����޻ް��炷			*/

	}

}

/********************************************//*
	�J�E���g�\���J�n
************************************************/
void mainPackCountDisplay_Start(void)
{
	uint16_t data,hundredsDigit;
	
	mainFlPackCountDisplay_Busy = 1;	//�\����
	
	glbCtPackCountDisplayTime = 800;	//4000ms
	
	glbFlReqDot = 0;	
	
	hundredsDigit = geefCtPack/100;								/* �p�b�N�� 100�̈�			*/
	

	ledControl_Sec(ALL_LED_SELECT,0,0);	

	
	if(!hundredsDigit)		//2���\���p
	{
		data = geefCtPack%100;
		mainDisplay7Seg(data);										/* �p�b�N���\��			*/
	}
	else					//3���@1/10�ŕ\���p
	{
		glbFlReqDot = 2;	
		data = geefCtPack/10;
		mainDisplay7Seg(data);										/* �p�b�N���\��			*/
		
	}
}


/********************************************//*
	�J�E���g�\���I��
************************************************/
void mainPackCountDisplay_End(void)	
{
	UWORD	data;
	
	if(!mainFlPackCountDisplay_Busy)
	{
		return;
	}
	
	mainFlPackCountDisplay_Busy = 0;
	
	glbFlReqDot = 0;											/* �u�^��v�H���̂��߁A��ص�ޕ\���Ȃ�					*/
	glbFlProc 		= GCN_PROC_VACUUM;							/* �H�����u�^��v�ɂ���	*/
	ledControl_Proc(glbFlProc,1,0);											
		
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];			/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
	}
	else
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/

	}
	
	ledControl_Sec(glbFlSecLevel,1,0);	
	mainDisplay7Seg(data);										/* 7SEG�\��					*/
}

/***********************************************************************************************************************
*    �֐���    mcmSetSystemData
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �V�X�e���f�[�^�ݒ胂�[�h
*    �������e  [�s�P]:�^���~����,[�s�Q]:�g�@�^�]����,[�s�R]:�^��^�C���A�E�g,[�s�S]:�\��^�]��WAIT����
*              [�s�T]:�^�]�I����WAIT����
*    �쐬��    2018�N6��16��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetSystemData(void)
{
	UBYTE	saveFlCourse,saveFlSecLevel,saveFlContinue ;
	UBYTE	ctLed=0;
	UWORD	data;
	UBYTE	key,sts,*pt;
	UWORD	MaxKeyOnTime,MaxTime,MinTime;
	SWORD	Time;

	saveFlCourse = geefFlCourse;										/* ���No.����			*/
	saveFlSecLevel = glbFlSecLevel;										/* �u�b�vor �u%�v�w�辰��	*/
	saveFlContinue = glbFlContinue;										/* �A���^�]Ӱ���׸޾���		*/
	glbFlReqDot = 0;													/* �u�^��v�ȊO����ص�ޕ\��				*/
	Time = geefBfSystemData[ctLed][Default];							/* �����ް�ذ��								*/
	mainDisplay7Seg(Time);												/* 7SEG�\��					*/
	mcmSetLed(ctLed);													/* �w���LED�_��							*/
	
	while(1)
	{
		if((key = glbGetReadKey()))										/* SW�̓�������			*/
		{
			if(key == GCN_KEY_STOP_SET)									/* STOP�i�B��SW6)�̏ꍇ					*/
			{
				ctLed++;												/* T1�`T11��ذ�޶����X�V					*/
				if(ctLed > 10)											/* T11�𒴂����ꍇ							*/
				{
					ctLed = 0;											/* ذ�޶�����T1�Ɉړ�						*/
				}
				mcmSetLed(ctLed);										/* �w���LED�_��							*/
				if(ctLed < 5)											/* T1�`T5�͈̔͂̏ꍇ						*/
				{
					Time = geefBfSystemData[ctLed][Default];			/* �����ް�ذ��								*/
					mainDisplay7Seg(Time);								/* 7SEG�\��					*/
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]��0.1�b�P�ʂ̂��߁A7SEG��ص�ޕ\��	*/
				}
				else
				{
					glbDt7Seg2 = 										/* 7SEG��2���ڂ�'-'�\��		*/
					glbDt7Seg1 = GCN_7SEG_HAIPHONG;						/* 7SEG��2���ڂ�'-'�\��		*/
				    glbFlReqDot = 0;									/* 7SEG��ص�ޕ\������		*/
				}
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
			}
			else if(key == GCN_KEY_UP_SET)								/* ��(SW3)�̏ꍇ						*/
			{
				if(ctLed < 5)											/* T1�`T5�̏ꍇ							*/
				{
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]��0.1�b�P�ʂ̂��߁A7SEG��ص�ޕ\��	*/
					sts = 0;
					MaxKeyOnTime = GCN_TIME_1S;							/* ���������ʒm����	�i1�b)				*/
					do
					{
						pt = &geefBfSystemData[ctLed][Default];			/* �I������Ă��鼽���ް��̐擪���ڽذ��		*/
						Time = *pt;										/* �I�����ꂽ���ю��Ԃ�ذ��				*/
						Time++;											/* ���ԍX�V�i+)											*/
						MaxTime = geefBfSystemData[ctLed][Max];			/* �I������Ă���H���l��MAX�lذ��						*/
						if(Time > MaxTime)								/* Max�𒴂����ꍇ										*/
						{
							Time = MaxTime;								/* Max�ɕ␳											*/
							mcmSetBuzzerInvalid();						/* �����޻ް��炷			*/
						}
						else
						{
							if(sts == 0)								/* KEY�������̎����޻ް��炳�Ȃ�*/
							{
								mcmSetBuzzerEffective();				/* �L���޻ް��炷			*/
							}
						}
						*pt = Time;										/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�								*/
						mainDisplay7Seg(Time);							/* 7SEG�\��								*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* KEY���������̂�҂�				*/
						MaxKeyOnTime = GCN_TIME_100MS;					/* ���������ʒm����	�i0.1�b)			*/
					}while(sts == 1);
				}
				else													/* T6�`T11�̏ꍇ							*/
				{
					mcmSetBuzzerInvalid();								/* �����޻ް��炷			*/
				}
			}
			else if(key == GCN_KEY_DOWN_SET)							/* ��(SW4)�̏ꍇ						*/
			{
				if(ctLed < 5)											/* T1�`T5�̏ꍇ							*/
				{
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]��0.1�b�P�ʂ̂��߁A7SEG��ص�ޕ\��	*/
					sts = 0;
					MaxKeyOnTime = GCN_TIME_1S;							/* ���������ʒm����	�i1�b)				*/
					do
					{
						pt = &geefBfSystemData[ctLed][Default];			/* �I������Ă��鼽���ް��̐擪���ڽذ��		*/
						Time = *pt;										/* �I�����ꂽ���ю��Ԃ�ذ��				*/
						Time--;											/* ���ԍX�V�i-)											*/
						MinTime = geefBfSystemData[ctLed][Min];			/* �I������Ă���H���l��MAX�lذ��						*/
						if(Time < (SWORD)MinTime)						/* Min�𒴂����ꍇ										*/
						{
							Time = MinTime;								/* Max�ɕ␳											*/
							mcmSetBuzzerInvalid();						/* �����޻ް��炷			*/
						}
						else
						{
							if(sts == 0)								/* KEY�������̎����޻ް��炳�Ȃ�*/
							{
								mcmSetBuzzerEffective();				/* �L���޻ް��炷			*/
							}
						}
						*pt = Time;										/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�								*/
						mainDisplay7Seg(Time);							/* 7SEG�\��								*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* KEY���������̂�҂�				*/
						MaxKeyOnTime = GCN_TIME_100MS;					/* ���������ʒm����	�i0.1�b)			*/
					}while(sts == 1);
				}
				else													/* T6�`T11�̏ꍇ							*/
				{
					mcmSetBuzzerInvalid();								/* �����޻ް��炷			*/
				}
			}
			else if(key == GCN_KEY_START_SET)							/* START/STOP(SW5)�̏ꍇ							*/
			{
				glbFlProc =  GCN_PROC_VACUUM;							/* �H����^���							*/
				ledControl_Proc(glbFlProc,1,0);							
					
				geefFlCourse = saveFlCourse;							/* ���LED�����̏�Ԃ֖߂�				*/
				ledControl_Course(geefFlCourse,1,0);					/* LED			*/
				glbFlSecLevel = saveFlSecLevel;							/* '�b'(LED8) or '%'(LED9)�����̏�Ԃ֖߂�	*/
				glbFlContinue = saveFlContinue;							/* �A��(LED10)�����̏�Ԃ֖߂�				*/
			    glbFlReqDot = 0;										/* 7SEG��ص�ޔ�\��	*/
				if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)/* "�b"�\���̏ꍇ						*/
				{
					data = geefBfCourseData[geefFlCourse][Vacuum];		/* �I������Ă��麰��̐^�󎞊�ذ��				*/
					glbFlSecLevel = GCN_VACUUM_SECONDS;					/* �u�b�v�\��					*/
				}
				else													/* "%"�\���̏ꍇ								*/
				{
					data = geefBfCourseData[geefFlCourse][VacuumLevel];	/* �I������Ă��麰��̐^������ذ��				*/
					glbFlSecLevel = GCN_VACUUM_LEVEL;					/* �u%�v�\��					*/
				}
				ledControl_Sec(glbFlSecLevel,1,0);
				mainDisplay7Seg(data);									/* �^�󎞊Ԃ܂������ق�7SEG�\��					*/
				if(mcmFlashWriteCheck())									/* �ޯ�������ؕύX���ׯ��������������				*/
				{
					glbFlError |= GCN_ERROR_FLASH_DATA_9;
					glbFlMainMode = GCN_ERROR_MODE;						/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
					return;
				}
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
				glbFlMainMode = GCN_WAIT_MODE;							/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/
				return;
			}
			else if(key == GCN_KEY_PUSH_SET)									
			{		
				geefCtPack += 10;
				if(geefCtPack > 999)		/* 999��𒴂����ꍇ			*/
				{
					geefCtPack = 999	;	/* 999�������Ƃ���			*/
				}
				mcmSetBuzzerEffective();								/* �L���޻ް��炷			*/
			}
			else				
			{
				mcmSetBuzzerInvalid();									/* �����޻ް��炷			*/
			}	
		}
	}
	
}

/***********************************************************************************************************************
*    �֐���    mcmSetLed
*    ��  ��    LED No.
*    �߂�l    
*    �֐��@�\  �I�����ꂽ�s�P�`�s�P�P�̍��ڂɑ΂��ĔC�ӂ̂k�d�c��_��������
*    �������e  
* 
*    �쐬��    2018�N6��16��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetLed(UBYTE	ctLed)
{
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Course(ALL_LED_SELECT,0,0);					/* LED			*/
	ledControl_Sec(ALL_LED_SELECT,1,0);	
	glbFlContinue = 0;
	glbFlLed11 = 0;								/* T11*/

	switch(ctLed)	{
		case	0:								/* T1�`T3�͈͍̔͂H��LED��_��	*/
		case	1:
		case	2:
			ledControl_Proc(ctLed,1,0);										
			break;
		case	3:								/* T4�`T7�͈̔͂ͺ��LED��_��	*/
		case	4:
		case	5:
		case	6:
			ledControl_Course((ctLed - 3),1,0);					/* LED			*/
			break;
		case	7:								/* T8�`T9�͈̔͂́u�b�vLED or �u%�vLED��_��	*/
		case	8:
			ledControl_Sec(ctLed - 7,1,0);
			break;
		case	9:								/* T10�́u�A���vLED��_��	*/
			glbFlContinue = 1;
			break;
		case   10:
			glbFlLed11 = 1;						/* T11*/
			break;
	}
}

/***********************************************************************************************************************
*    �֐���    mainErrorMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �G���[���o���[�h
*    �������e  �G���[�������ɖ{���[�h�֑J�ځB
*              �v�c�s�G���[�ȊO�͂����Ŗ������[�v�B
*              �G���[�����t���b�V���������֏������݁B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainErrorMode(void)
{
	SWORD	i;
	UBYTE	checkBit;
	UBYTE	ErrorKind = 0,Proc=0,ErrorNo=0;
	volatile UBYTE	err;

	glbFlChildBusy = 0;
	DI();
	P7 &= P74_PUMP_START_OFF;									/* ����߉ғ���~						*/ 
	

	P7 &= P74_PUMP_START_OFF;									/* ����߉ғ���~						*/ 
	P7 &= P73_HEATER_START_OFF;									/* ˰�OFF								*/
	P7 &= P75_SOFTRELEASE_START_OFF;							/* ��ĉ����OFF							*/
	EI();

	P2 &= P20_VACUUM_ELECTROMAG_OFF;							/* �^��d����OFF						*/
	P4 &= P41_VACUUM_RELEASE_OFF;								/* �^������OFF						*/ 

	P0 &= P01_SEEL_ELECTROMAG_OFF;								/* ��ٓd����OFF							*/
	P13 &= P130_GUS_ELECTROMAG_OFF;								/* �޽�d���� OFF						*/

	i=0;
	checkBit = 0x01;											/* �װ�����ޯď����� 					*/
	while(i<8)													/* �Y������װNo.������					*/
	{
		if((glbFlError & checkBit) != 0)						/* �C�ӂ̴װ�ޯĂ���Ă���Ă��邩��������	*/
		{
			break;												/*�ޯĂ���Ă���Ă����ٰ�߂��甲����	*/
		}
		i++;
		checkBit <<= 1;
	}
	glbFlReqDot         = 0;									/* ��ص�ޕ\������					*/
	glbDt7Seg2 = 'E' - 0x37;									/* 7SEG��2���ڂ�'E'��\��				*/
	ErrorNo    = 
	glbDt7Seg1 = (UBYTE)(mainTblError[i]);					/* 7SEG��1���ڂɴװNo��\��				*/

	glbCtKeyOffTimer = 0;										/* KEY OFF�����X�V			*/
	glbCtDriveTimer = 0;										/* ��ϒl�X�V					*/
	glbCtVacuumEmergncyTimer = 0;								/* ��ϒl�X�V					*/
	glbCtGusInclusionTimer   = 0;								/* �޽�����H�����{���H			*/
	glbCtGusStubilityTimer   = 0;								/* �޽���莞�Զ��Ē��H			*/
	glbCtSeelTimer           = 0;								/* ��ٍH�����{���H			*/
	glbCtWarmTimer           = 0;								/* �g�C�^�]���{���H				*/
	glbCtVacuumeWarm         = 0;								/* ��ϒl�X�V					*/
	glbCtFunTimer            = 0;								/* ��ϒl�X�V					*/
	glbFlSysHeaterReq        = 0;								/* ����Ӱ��˰��Ď��v���L?		*/
	glbCtSysHeaterOffTimer    = 0;								/* 2�b�Ď���϶����X�V			*/
	glbFlVacuumElectromangReq = 0;								/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v���L��H*/
	glbCtVacuumElectromang   = 0;								/* 10�b��ύX�V					*/
	glbCtHeaterBlockTimer    = 0;								/* 0.7�b��ύX�V					*/	
	glbCtVacuumTimeout1       = 0;								/* ��ύX�V						*/
	glbCtVacuumTimeout2       = 0;								/* ��ύX�V						*/
	glbCtVacuumTimeout3       = 0;								/* ��ύX�V						*/

	if((glbFlSaveMode == GCN_DRIVE_MODE) || 					/* �^��^�]���܂��͒g�C�^�]���̏ꍇ	*/
       (glbFlSaveMode == GCN_WARM_MODE))							
	{
		switch(glbFlDriveMode)	{
			case	GCN_DRIVE_START_MODE:						/* �^������J�n��		*/
			case	GCN_VACUUM_PULL_MODE:						/* �^������H��			*/
			case	GCN_VACUUM_STOP_MODE:						/* �^������ꎞ��~		*/
				Proc = 1;										/* �^��H�����ɴװ����	*/
				break;
			case	GCN_GUS_INCLUSIOM_MODE:						/* �޽�����H��			*/
			case	GCN_GUS_STUBILITY_MODE:						/* �޽����҂��H��		*/
				Proc = 2;										/* �޽�H�����ɴװ����	*/
				break;
			case	GCN_SEEL_MODE:								/* ��ٍH��				*/
			case	GCN_HEATER_BLOCK_MODE:						/*  ˰���ۯ��㏸�҂������i�O�D�V�b�j*/
				Proc = 3;										/* ��ٍH�����ɴװ����	*/
				break;
			case	GCN_SEEL_COOLING_MODE:								
				Proc = 4;										/* ��ٗ�p�H�����ɴװ����*/
				break;
			default:
				Proc = 0;										/* �H���^�]�ȊO			*/
		}
		
		ErrorKind = ((geefFlCourse+1) << 4) | Proc;				/* ���4�ޯāF�װ�������̺���ԍ�,����4�ޯ�:���̎��̍H��		*/
	}
	for(i=8;i>=0;i--)											/* �װ����ð��ق��P�Â�����	*/
	{
		geefBfErrorHistory[i+1][0] = geefBfErrorHistory[i][0];	/* �װ�v���ړ�			*/
		geefBfErrorHistory[i+1][1] = geefBfErrorHistory[i][1];	/* �װ�������̺��No�ƍH���ړ�	*/
	}
	geefBfErrorHistory[0][0] = ErrorNo; 						/* �ŐV�̴װ�������e�ۑ�				*/
	geefBfErrorHistory[0][1] = ErrorKind; 						/* �ŐV�̴װ�������e�ۑ�				*/
	err = pdlWriteFlash();										/* �ޯ������ް����ׯ���֏�������				*/

	mcmSetBuzzerError();
	
	powerLedControl(2,5,3);		//�G���[�_��
	
	if(glbFlError & GCN_ERROR_WATCHDOG_CPU_8)					/* �����ޯ���ϴװ�̏ꍇ������				*/
	{
		while(1)
		{
			NOP();
		}
	}
	while(1)
	{	
		if(!(glbFlError & GCN_ERROR_R78_CPU_7))
		{
			if(glbFlPowerSwLongPalse)	//ResetSw�ő���\
			{
				WDTE = 0xFFU;  				/* ����ؾ�Ă������邽�߂ɁAWDT��*/
			}
		}
	
	}
	
}


/***********************************************************************************************************************
*    �֐���    mainStanbyMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  
*    �������e  
*    �쐬��    
*    �쐬��   
***********************************************************************************************************************/
void	mainStanbyMode(void)
{
	UBYTE 	data;
		
	if(glbFlPowerSwLongPalse)
	{
		glbFlStanby = 0;
		glbFlPowerSwLongPalse = 0;	
	}
	if(!glbFlStanby)
	{
		powerLedControl(1,0,0);
		
		glbFlReqDot = 0;
		
		mcmSetBuzzerEffective();									/* ���ݗL���޻ް					*/	
	
		ledControl_Course(geefFlCourse,1,0);					/* LED			*/
		glbFlProc = GCN_PROC_VACUUM;
		ledControl_Proc(glbFlProc,1,0);										

		glbFlMainMode = GCN_WAIT_MODE;													/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* �^��H����[�b]�\���̏ꍇ						*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];								/* �I������Ă��麰�1�̐^�󎞊�ذ��				*/
		}
		else																			/* �^��H����[%]�\���̏ꍇ						*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];							/* �I������Ă��麰�1�̐^������					*/

		}
		glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?			/* �^��H����[�b][%]�w���LED���f				*/
						GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;							

		ledControl_Sec(glbFlSecLevel,1,0);		
		mainDisplay7Seg(data);															/* �^��H�����Ԃ܂������ق�7SEG�ɕ\��			*/

		mainFlDrawer = 0;	
	}
}
/***********************************************************************************************************************
*    �֐���    mainStanbyCheck
*    ��  ��    
*    �߂�l    
*    �֐��@�\  
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainStanbyCheck(void)
{
	if(glbFlPowerSwLongPalse)
	{
		glbFlStanby = 1;
		glbFlPowerSwLongPalse = 0;	
	}
	if(glbFlStanby)
	{
		mainPackCountDisplay_End();	
		
		powerLedControl(0,0,0);
		
		glbFlMainMode = GCN_STANBY_MODE;																				
		glbFlStanby = 1;
		
		ledControl_Course(ALL_LED_SELECT,0,0);					/* LED			*/
		ledControl_Proc(ALL_LED_SELECT,0,0);											
		ledControl_Sec(ALL_LED_SELECT,0,0);	
		
		glbDt7Seg1 = 17;
		glbDt7Seg2 = 17;

		glbFlReqDot = 2;	//�d�����Ă���̂��H�̏�Ȃ̂�������Ȃ�����SEG�̃h�b�gLED�\��	
	}
}


/***********************************************************************************************************************
*    �֐���    mainStartVacuum
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]�O����
*    �������e  �^��^�]�ɑJ�ځB���̑O�Ƀo�b�N�A�b�v�f�[�^�ύX������΃t���b�V���������֏�������
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainStartVacuum(void)
{	
	if(mcmFlashWriteCheck())										/* �ޯ�������ؕύX����				*/
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;
		glbFlMainMode = GCN_ERROR_MODE;						/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
		return;
	}		

	glbFlMainMode 		= GCN_DRIVE_MODE;						/* �^�]Ӱ�ނ֑J��		*/
	glbFlDriveMode 		= GCN_DRIVE_START_MODE;					/* �^�]Ӱ�ގ��̏�Ԃ��^�]�J�n�ɐݒ�	*/
	glbFlProc 			= GCN_PROC_VACUUM;						/* �H�����u�^��v�Ɉړ�				*/
	ledControl_Proc(glbFlProc,1,0);											
	
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)	/* '�b'(LED8)�w��̏ꍇ				*/
	{
		glbFlSecLevel       = GCN_VACUUM_SECONDS;
	}
	else														/* '%'(LED9)�w��̏ꍇ				*/
	{
		glbFlSecLevel       = GCN_VACUUM_LEVEL;
	}
	
	ledControl_Sec(glbFlSecLevel,1,0);
	glbFlReqDot         = 0;									/* ��ص�ޕ\������					*/
}
/***********************************************************************************************************************
*    �֐���    glbGetReadKey
*    ��  ��    
*    �߂�l    �r�v���
*    �֐��@�\  �r�v���͏���
*    �������e  �T�����^�C�}���荞�݂Ō��o����7�̂r�v�̂`�b�s�h�u�d���o�ʒm����͂���
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE	glbGetReadKey(void)
{
	UBYTE	key=0;

	DI();
	R_WDT_Restart();											/* �����ޯ����ؾ��							*/
	if(glbFlKydt & 0x02) 										/* ���(SW1)�̏ꍇ				*/
	{
		key = GCN_KEY_COURSE_SET;								/* ���(SW1)ON�F��				*/
	}
	else if(glbFlKydt & 0x04)									/* �H��(SW2)�̏ꍇ				*/
	{
		key = GCN_KEY_PROC_SET;									/* �H��(SW2)ON�F��				*/
	}
	else if(glbFlKydt & 0x08)									/* ��(SW3)�̏ꍇ				*/
	{
		key = GCN_KEY_UP_SET;									/* ��(SW3)ON�F��				*/
		if(glbFlKydt & 0x10)									/* �X�Ɂ�(SW4)��������Ă���ꍇ	*/
		{
			key = GCN_KEY_UPDOWN_SET;							/* ���������F��					*/
		}
	}
	else if(glbFlKydt & 0x10)									/* ��(SW4)�̏ꍇ				*/
	{
		key = GCN_KEY_DOWN_SET;									/* ��(SW4)ON�F��					*/
	}

	else if(glbFlKydt & 0x20)									/* START/STOP(SW5)�̏ꍇ		*/
	{
		key = GCN_KEY_START_SET;								/* START/STOP(SW5)ON�F��		*/
	}
	else if(glbFlKydt & 0x40)									/* �^���~(SW6)�̏ꍇ			*/
	{
		key = GCN_KEY_STOP_SET;									/* �^���~(SW6)�F��			*/
	}
	else if(glbFlKydt & 0x80)									/* PUSH�i��՗���)�̏ꍇ		*/
	{
		key = GCN_KEY_PUSH_SET;									/* PUSH�i��՗���)ON�F��		*/
	}
	glbFlKydt ^= glbFlKydt;										/* SW�����ޯĸر	*/
	EI();
	return(key);
}

/***********************************************************************************************************************
*    �֐���    glbWaitKeyOff
*    ��  ��    SW�@No.
*    �߂�l    0:�w�莞�ԓ��ɗ����ꂽ�A1:�w�莞�ԉ�����Ă���
*    �֐��@�\  
*    �������e  �j�d�x���w�莞�ԉ�����Ă��邩�Ď�
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE   glbWaitKeyOff(UBYTE KeyNo,UWORD KeyOnMax)
{
	UBYTE	keydata[7] = {P21_KEY_SW1_COURSE_OFF,				/* ���(SW1)		*/
						  P22_KEY_SW2_PROCESS_OFF,				/* �H��(SW2)	*/
						  P23_KEY_SW3_UP_OFF,					/* ��(SW3)		*/
						  P24_KEY_SW4_DOWN_OFF,					/* ��(SW4)		*/
						  P25_KEY_SW5_START_OFF,				/* START/STOP(SW5)	*/
						  P26_KEY_SW6_STOP_OFF,					/* �^���~(SW6)	*/
						  P27_KEY_SW7_PUSH_OFF					/* PUSH*/
						};

	DI();
	glbCtKeyOffTimer = 0;										/* ����ؾ��								*/
	EI();
	KeyNo--;													/* SW No. �␳								*/
	KeyOnMax /= GCN_INT_TIME;									/* ���Ԃ�5ms�P�ʂɕϊ�						*/
	while(1)
	{
		if(glbCtKeyOffTimer >= KeyOnMax)						/* �w�莞��KEY�������ꂽ�ꍇ				*/
		{
			return	1;
		}
		if(iodInKey() & (keydata[KeyNo]))						/* �w�莞�ԓ���KEY�������ꂽ�ꍇ�@(H�̎���KEY OFF)	*/
		{
			return 0;
		}
	}
}

/***********************************************************************************************************************
*    �֐���    glbWaitKeyOn
*    ��  ��    
*    �߂�l    0:SW��������Ă���A1:SW�������ꂽ
*    �֐��@�\  �w��̂r�v��������Ă��邩�ۂ�������
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE   glbWaitKeyOn(UBYTE KeyNo)
{
	UBYTE	keydata[7] = {P21_KEY_SW1_COURSE_OFF,
						  P22_KEY_SW2_PROCESS_OFF,
						  P23_KEY_SW3_UP_OFF,
						  P24_KEY_SW4_DOWN_OFF,
						  P25_KEY_SW5_START_OFF,
						  P26_KEY_SW6_STOP_OFF,
						  P27_KEY_SW7_PUSH_OFF
						};

	while(1)
	{
		if(!(iodInKey() & (keydata[KeyNo])))	                /* SW��������Ă��邩	*/
		{
			return 0;											/* SW��������Ă���ꍇ	*/
		}
		else
		{	
			return 1;											/* SW�������ꂽ�ꍇ	*/
		}
	}
}

/***********************************************************************************************************************
*    �֐���    mainDisplay7Seg
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �V�r�d�f�ɕ\�������鐔�����Q�P�^�ɂ���
*    �������e  �����ŃZ�b�g�����ϐ��̒l���P�����^�C�}���荞�݂œǂݍ��݁A�V�r�d�f�ɕ\������B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void mainDisplay7Seg(UWORD data)
{
		
	glbDt7Seg2 = data/10;					/* ���No.�ɑ΂��Ă̑I�����ꂽ�H������ذ�ނ��A2���ڂ��	*/
	glbDt7Seg1 = data%10;					/* ���No.�ɑ΂��Ă̑I�����ꂽ�H������ذ�ނ��A1���ڂ��	*/
}


/***********************************************************************************************************************
*    �֐���    mainDisplayWarning7Seg
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��x���G���[[�d�R]�̂V�r�d�f�\���f�[�^�쐬
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/


void mainDisplayWarning7Seg(UWORD data)
{

	glbDt7Seg2 = 'E' - 0x37;				/* 'E'�\��	*/
	glbDt7Seg1 = 3;							/* '3'�\��	*/
}
	
/***********************************************************************************************************************
*    �֐���    glbGetDipSW()
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �c�h�o�r�v�̓ǂݍ���
*    �������e  �k�łc�h�o�r�v �n�m
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	glbGetDipSW(void)
{
	UBYTE	dipsw=0;

	if(!(P12 & 0x01))
	{
		dipsw |= 0x01;
	}
	if(!(P12 & 0x02))
	{
		dipsw |= 0x02;
	}
	if(!(P12 & 0x04))
	{
		dipsw |= 0x04;
	}
	if(!(P12 & 0x08))
	{
		dipsw |= 0x08;
	}

	glbFlDipSW = dipsw;											/* DIPSW�̏�Ծ��				*/
	return;
}

/***********************************************************************************************************************
*    �֐���    mainPortOutputInitial
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �o�̓|�[�g������
*    �������e  �o�̓|�[�g���A�N�e�B�u�ɂ���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainPortOutputInitial(void)
{
	P1 = 0x00;										/* 7SEG 8bit All Off	*/
	P5  |= (P50_LEDCOM1_OFF | P51_LEDCOM2_OFF);		/* LEDCOM1,2,3,4 OFF	*/
	P3  |= (P30_LEDCOM3_OFF);
	P7  |= (P70_LEDCOM4_OFF);
	P7  &= (P72_POWER_LED_OFF);						/* �d��SW				*/
	P7  &= (P73_HEATER_START_OFF);					/* ˰�OFF			*/
	P7  &= (P74_PUMP_START_OFF);					/* �����OFF			*/
	P7  &= (P75_SOFTRELEASE_START_OFF);				/* ��ĉ����OFF		*/
	P2  &= (P20_VACUUM_ELECTROMAG_OFF);				/* �^��d����OFF	*/
	P13  &= (P130_GUS_ELECTROMAG_OFF);				/* �޽�d����OFF		*/
	P0  &= (P01_SEEL_ELECTROMAG_OFF);				/* ��ٓd����OFF		*/
	P4  &= (P41_VACUUM_RELEASE_OFF);				/* �^������OFF	*/
	
}


/***********************************************************************************************************************
*    �֐���    mainGlobalClear
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���ʃ������N���A
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainGlobalClear(void)
{
	geefFlCourse = 0;										/* ����w���׸ށi0:���1,1:���2,2:���3,3:���4)	*/
	glbFlReqDot=0;										/* 7SEG ��ص�ޕ\���v���׸�							*/
	glbFlLedCom=0;										/* LED COM1�`COM4�̶����i1�`4�̂ǂ�COM�ɏo�͂��邩	*/
	glbCtKeyOffTimer=0;
	glbFlKydt=0;										/* KEY�����ް�				*/
														/* bit0:����ύX/�ݒ跰		*/
														/* bit1:����				*/
														/* bit2:����				*/
														/* bit3:����/�į�߷�		*/
	glbFlMainMode=0;									/* Ҳ�Ӱ���׸�			*/
	glbDt7Seg2=GCN_7SEG_ALLOFF;							/* 7SEG 2�����ް� (ALL OFF)		*/
	glbDt7Seg1=GCN_7SEG_ALLOFF;							/* 7SEG 1�����ް� (ALL OFF)		*/
	glbFlDipSW=0;										/* Dip SW				*/
	glbFlSysytem7Seg=0;									/* ����Ӱ��7SEG�o��ý��׸�	*/
	glbFlSystemMode=0;									/* ����Ӱ�ޒ��׸�			*/
	glbFlVacuumElectromangReq=0;						/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��*/
	glbFlSysHeaterReq=0;								/* ����Ӱ�ށ@˰��Ď��v��=2�b		*/

	glbFlVaccumeTips=0;
	glbFlWarmCansel=0;										/* �g�C�^�]���׸�			*/
	glbCtVacuumElectromang=0;							/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~���10�b����	*/
	glbCtHeaterBlockTimer=0;							/* ˰���ۯ��㏸���Զ���		*/
	glbCtWarmTimer=0;									/* �g�C�^�]����				*/
	glbFlDriveMode=0;									/* �^�]Ӱ�ގ���Ӱ���׸�		*/
	geefFlVacuumeDisp=0;								/* �^��H���c��[���� or %]�̐؊���	*/
	glbCtDriveTimer=0;									/* �^��������Զ���			*/
	glbCtGusInclusionTimer=0;							/* �޽�������Զ���			*/
	glbCtVacuumEmergncyTimer=0;							/* �^��ꎞ��~����			*/
	glbCtGusStubilityTimer=0;							/* �޽����҂����Զ���		*/
	glbCtSeelTimer=0;									/* ��ٍH�����Զ���			*/
	glbCtSeelCoolingTimer=0;							/* ��ٗ�p�H�����Զ���		*/
	glbCtSoftReleaseTimer=0;							/* ��ĉ������				*/
	glbCtSysHeaterOffTimer=0;							/* ����Ӱ�� ˰�2�b�Ď���϶���		*/
	glbCtFunTimer=0;
	glbCtVacuumeWarm=0;									/* �g�C�^�]��ϰ				*/
	glbFlProc = GCN_PROC_VACUUM;									/* �H��LED���u�^��v�\���ɂ���	*/


	glbCtVacuumTimeout1=0;								/* �^����ѱ����������		*/
	glbCtVacuumTimeout2=0;								/* �^����ѱ����������		*/
	glbCtVacuumTimeout3=0;								/* �^����ѱ����������		*/
	glbCtIntTimer = 0;
	glbFlSaveMode = glbFlDriveMode;									/* Ҳ�Ӱ�ނ̏�Ԃ�ޔ�			*/
	glbFlError = 0;										/* �װ�׸�							*/
	glbFlVacuumWarning = 0;

	glbFlPowerSw = 0;
	glbFlPowerSwLongPalse = 0;
	glbFlPowerSwLong = 0;
	glbCntPowerSwOnChk = 0;
	glbCntPowerSwLongChk = 0;
	glbCntPowerSwOffChk = 0;
	glbPowerLed = 0;
	

	glbCtPackCountDisplayTime = 0;
	glbFlPackCountDisplay_Start = 0;

			
	glbFlContinue = 0;									/* �A��Ӱ��	*/	
	
	glbFlMainSystem = 0;
	mainFlDrawer = 0;									/* ���o��SW��Ծ���			*/
	glbCtWaitTimer = 0;
	glbCtBuzzerOnTimer = 0;
	glbCtBuzzerOffTimer = 0;

	glbCtWaitTimerT5 = 0;
		
	glbFlLed11 = 0;
	glbFlWarmMode = 0;

}

/***********************************************************************************************************************
*    �֐���    mcmLocalClear
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ���[�J���������N���A
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmLocalClear(void)
{
	stiFlKydt = 0xff;								/* KEY ON��L�̂��߁AFFH�Ƃ���			*/
	stiFlFlashing = 0;								/* ĸ�ٍX�V								*/
	stiCtFlashing = 0;								/* 500ms��������						*/
	stiFlFlashing2 = 0;								/* ĸ�ٍX�V								*/
	stiCtFlashing2 = 0;								/* 500ms��������						*/
	stiFlFlashing3 = 0;								/* ĸ�ٍX�V								*/
	stiCtFlashing3 = 0;								/* 500ms��������						*/
	stiFlFlashing4 = 0;								/* ĸ�ٍX�V								*/
	stiCtFlashing4 = 0;								/* 500ms��������						*/

}


/***********************************************************************************************************************
*    �֐���    mcmFlashDataRead
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �t���b�V������������o�b�N�A�b�v�f�[�^��ǂݍ���
*    �������e  �t���b�V������������̐擪����Q�o�C�g���T�`�g�y�т`�T�g�łȂ���΃t���b�V�����������������Ɣ��f���A
*              �f�t�H���g�̃o�b�N�f�[�^���t���b�V���������ɏ������ށB
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	mcmFlashDataRead(void)
{
	volatile pfdl_status_t fdlResult;	
	volatile UBYTE	err=0;
	UBYTE	retry = 0;

	/*�擪/END �Œ�f�[�^�Ǎ�*/	
	R_FDL_Open();
	retry = 0;
	while(1)
	{
		fdlResult = R_FDL_IVerify( PFDL_FIXDATA_ADDR, sizeof(geefFlFixData));
		if(fdlResult == PFDL_OK )
		{
			if((R_FDL_Read( PFDL_FIXDATA_ADDR, (__near pfdl_u08*)geefFlFixData, sizeof(geefFlFixData) ) == PFDL_OK ))								
		   	{
				retry = 0;
				break;	
			}
		}

		retry++;
		if(retry > 3)
		{
			err= 2;												/* �ׯ��ذ�޴װ						*/
			goto TAG_END;
		}
		
		continue;
	}
	while(1)
	{
		fdlResult = R_FDL_IVerify( PFDL_FIXEND_ADDR, sizeof(geefFlFixEndData));
		if(fdlResult == PFDL_OK )
		{			
		    if((R_FDL_Read( PFDL_FIXEND_ADDR, (__near pfdl_u08*)geefFlFixEndData, sizeof(geefFlFixEndData) ) == PFDL_OK ))								
		   	{
				retry = 0;
				break;	
			}
		}
		retry++;
		if(retry > 3)
		{
			err= 2;												/* �ׯ��ذ�޴װ						*/
			goto TAG_END;
		}
		
		continue;
	}
	
	R_FDL_Close();
	
	/*�擪/END �Œ�f�[�^�m�F�@�t���b�V�����������H���͔j���H*/	
	if( ((geefFlFixData[0] != 0x5a) && (geefFlFixData[1] != 0xa5))	/* �ׯ�����������̏ꍇ				*/
		|| ((geefFlFixEndData[0] != 0x5a) && (geefFlFixEndData[1] != 0xa5)) )	/* �ׯ�����������̏ꍇ				*/
	{
		//�Ǎ��f�[�^NG�̂��ߏ�����
		err = mcmFlashDataInitial();								/* �ׯ���ޯ������ް��������Ə�������*/
		if(err)	
		{
			//�����ݎ��s
			return(err);
		}
	}
	
	/*�������݃f�[�^�̓Ǎ��ƍ�*/
	err = pdlReadFlash();											/* �ׯ�������ޯ������ް���ǂݍ���	*/
	if(err == 0)													/* �װ�����̏ꍇ					*/
	{
		//����f�[�^�Ȃ��ƃG���A�֓o�^
		pldCopyBackupData();										/* �ޯ������ް����r�ް��ر�ֺ�߰	*/
	}

	TAG_END:
	return(err);
}

/***********************************************************************************************************************
*    �֐���    mcmFlashDataInitial
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �t���b�V���������C�j�V��������
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	mcmFlashDataInitial(void)
{
	UBYTE	err = 0,i = 0;																																																				

	mcmFlashInitialClear();							/* �ޯ����ߗ̈揉����						*/
     
	for(i=0;i<3;i++)
	{
		err = pdlWriteFlash();								/* �ް����ׯ���֏�������	*/
		if(err == 0)	
		{
			break;		
		}
	}
																									
   return(err);
}

/***********************************************************************************************************************
*    �֐���    mcmFlashInitialClear
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �t���b�V���������̃o�b�N�A�b�v�f�[�^������
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmFlashInitialClear(void)
{
	UBYTE	i,j;																											

	geefFlFixData[0] = geefFlFixEndData[0] = 0x5a;								/* �ׯ���������ް����		*/
	geefFlFixData[1] = geefFlFixEndData[1] = 0xa5;								/* �ׯ���������ް����		*/
	geefFlVacuumType[0] = GCN_VACUUM_LEVEL;				/* ����P�F�^������H����%�ŕ\��	*/		
	geefFlVacuumType[1] = GCN_VACUUM_LEVEL;				/* ����Q�F�^������H����%�ŕ\��	*/		
	geefFlVacuumType[2] = GCN_VACUUM_LEVEL;				/* ����R�F�^������H����%�ŕ\��	*/	
	geefFlVacuumType[3] = GCN_VACUUM_LEVEL	;				/* ����S�F�^������H����%�ŕ\��	*/	
	geefCtAnalogAdjust = 0;									/* ��۸ޒ����l				*/
	geefCtPumpRunning = 0;
	geefCtPack = 0;
	geefFlCourse = 0;	/* ���No.������			*/
	for(i=0;i<10;i++)										/* �װ���o����ð��ٸر		*/
	{
		for(j=0;j<2;++j)
		{
			geefBfErrorHistory[i][j] = 0xff;
		}
	}
	for(i=0;i<4;++i)										/* ��̫�Ă̺���ް����		*/
	{
		for(j=0;j<8;++j)
		{
			geefBfCourseData[i][j] = DefaultCourseData[i][j];
		}
	}
	for(i=0;i<5;++i)										/* ��̫�Ă̼����ް����		*/
	{
		for(j=0;j<3;++j)
		{

			geefBfSystemData[i][j] = DefaultSystemData[i][j];
		}
	}


}

/***********************************************************************************************************************
*    �֐���    mcmDataSettingRangeCheck	
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �ݒ�f�[�^�����͔͈͓��i�L���f�[�^�j���m�F����
*    �������e 
*    �쐬��    2022�N1��14��
*    �쐬��    
***********************************************************************************************************************/
UBYTE mcmDataSettingRangeCheck(void)
{
	volatile UBYTE i,j;
	volatile UBYTE sts = 0;
	
	for(i=0;i<4;i++)
	{
		if(geefFlVacuumType[i] > GCN_VACUUM_LEDALLON )
		{
			sts = 1;
			goto TAG_END;	
		}
	}
	
	if( (geefCtAnalogAdjust > 99)
		|| (geefCtAnalogAdjust < 0) )
	{
			sts = 1;
			goto TAG_END;	
	}

	for(i=0;i<4;++i)										/* ����ް����m�F		*/
	{
		for(j=0;j<8;++j)
		{
			if( (geefBfCourseData[i][j] < geefBfCourseDataMaxMin[j].Min)
				||(geefBfCourseData[i][j] > geefBfCourseDataMaxMin[j].Max) )
			{
				sts = 1;
				goto TAG_END;	
			}
		}
	}
	
	
	for(i=0;i<5;++i)										/* �����ް����m�F*/
	{
		if( (geefBfSystemData[i][j] < DefaultSystemData[i][1])
			||(geefBfSystemData[i][j] > DefaultSystemData[i][2]) )
	
		{
				sts = 1;
				goto TAG_END;	
		}
	}
	
TAG_END:

	return(sts);

	
}

/***********************************************************************************************************************
*    �֐���    mcmStartBuzzer
*    ��  ��    ON���ԁims�P��)
*    �߂�l    
*    �֐��@�\  �w�莞�ԃu�U�[��炷
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	mcmStartBuzzer(UWORD Timer)
{
	glbCtBuzzerOnTimer = Timer / GCN_INT_TIME;				/* �޻ްON���Ԃ�5ms�P�ʂɕϊ�			*/
	R_TAU0_Channel2_Start();								/* PWM ON								*/
}				


/***********************************************************************************************************************
*    �֐���    mcmFlashWriteCheck
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �o�b�N�A�b�v�f�[�^�̍X�V�`�F�b�N
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	mcmFlashWriteCheck(void)							/* �ޯ�������ؕύX����				*/
{
	volatile UBYTE	err=0;
	volatile UBYTE i;

	if(!pldVerifyCheck())									/* �ޯ�������؂ɕύX�ް��L��H		*/
	{
		return(err);	//�ύX����
	}
	
	//����������ײ�ǉ�
	for(i=0;i<3;i++)
	{
		err = pdlWriteFlash();								/* �ޯ������ް����ׯ���֏�������	*/
		if(err == 0)	
		{
			pldCopyBackupData();								/* ��r�p�ޯ̧�ֺ�߰				*/
			break;		
		}
	}
	
	return(err);	
}

/***********************************************************************************************************************
*    �֐���    mainCheckPumpthermal
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �|���v�T�[�}���G���[�`�F�b�N([E0])
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mainCheckPumpthermal(void)						/* ����߻��ق̴װ�Ď�		*/
{
	volatile UBYTE	in1,in2;

	in1 = P14;
	in2 = P14;
	if(in1 != in2)
	{
		in1 = P14;
	}
	if(!(in1 & P146_PUMP_THERMAL))							/* �װ�L��H					*/
	{
		glbFlError |= GCN_ERROR_PUMP_THERMAL_0;
		glbFlMainMode = GCN_ERROR_MODE;						/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
		return;
	}
}

/***********************************************************************************************************************
*    �֐���    mcmSetBuzzerEffective
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �{�^���L��
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetBuzzerEffective(void)						/* �L���޻ް��炷			*/
{
	mcmStartBuzzer(100);								/* 100ms*/

}

/***********************************************************************************************************************
*    �֐���    mcmSetBuzzerInvalid
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �{�^�������u�U�[
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetBuzzerInvalid(void)						/* �����޻ް��炷			*/
{
	mcmStartBuzzer(400);								/* 400ms*/

}

/***********************************************************************************************************************
*    �֐���    mcmSetBuzzerEnd
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]�H��/�g�C�^�]�I������
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetBuzzerEnd(void)						
{
	UBYTE	i;

	for(i=0;i<3;++i)
	{
		mcmStartBuzzer(130);							/* 130ms ON*/
		mcmBuzzerOff(170);								/* 170ms OFF*/
	}
}	


/***********************************************************************************************************************
*    �֐���    mcmSetBuzzerError
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �G���[�������̃u�U�[�x��
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmSetBuzzerError(void)						
{
	UBYTE	i;

	for(i=0;i<10;++i)
	{
		mcmStartBuzzer(300);							/* 300ms ON*/
		mcmBuzzerOff(300);								/* 300ms OFF*/
	}
}	


/***********************************************************************************************************************
*    �֐���    mcmBuzzerOff
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �u�U�[���J��Ԃ����̃u�U�[�n�e�e���ԊĎ�
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmBuzzerOff(UWORD	Timer)						
{
	while(glbCtBuzzerOnTimer != 0);						/* �޻ްON���Ȃ��OFF�ɂȂ�܂ő҂�		*/
	glbCtBuzzerOffTimer = Timer / GCN_INT_TIME;			/* �޻ްOFF���Ԃ�5ms�P�ʂɕϊ�			*/
	while(glbCtBuzzerOffTimer != 0);					/* �޻ްOFF���Ԃ̊Ď�					*/
}				


/***********************************************************************************************************************
*    �֐���    mcmWaitTimer
*    ��  ��    �v�`�h�s����(ms)
*    �߂�l    
*    �֐��@�\  �w�莞�Ԃ̂v�`�h�s
*    �������e 
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmWaitTimer(UWORD	Timer)
{
	glbCtWaitTimer = Timer /  GCN_INT_TIME;				/* ���Ď��Ԃ�5ms�ϊ�					*/
	while(glbCtWaitTimer != 0)
	{
		NOP();
	};							/* �w�莞�Ԍo�߂̊Ď�					*/

}

/***********************************************************************************************************************
*    �֐���    mcmCheckContinue
*    ��  ��    
*    �߂�l    0:��ԕω��Ȃ��A1:�u�J�v����u�v�̏�ԕω��L��
*    �֐��@�\  ���o��SW���u�J�v����u�v�̏������
*    �������e  �A���^�]���[�h���A���o��SW���u�J�v����u�v�̕ω�������΁A�^��^�]�H���ɑJ�ڂ���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	mcmCheckDrawerOpenToClose(void)
{
	UBYTE	sens;

	if(((sens = DrawerSwRead()) == 0))					/* ���o��SW���u�v�̏ꍇ			*/
	{
		if(mainFlDrawer == 1)							/* �O��̈��o��SW���u�J�v�̏ꍇ		*/
		{
			mainFlDrawer = sens;						/* ���o��SW�̏�ԕۑ�				*/
			return(1);									/* ���o��SW�̏�Ԃ��u�J�v�ˁu�v�ɕω��������߁A�^��H���֑J��	*/
		}
	}
	mainFlDrawer = sens;								/* ���o��SW�̏�ԕۑ�				*/
	return(0);
}


/***********************************************************************************************************************
*    �֐���    mcmCheckVacuumSensorError
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��Z���T�[�G���[�`�F�b�N���s��
*    �������e  �^��^�]���Ƃ���ȊO�ŃG���[��臒l���قȂ�B
*              �܂��A�^�]�I����WAIT���ԁi�s�T)�̊Ԃʹװ���o���Ȃ��B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	mcmCheckVacuumSensorError(void)					/* �^��ݻ��װ�̌��o		*/
{
	long int	adc;


	if(glbCtWaitTimerE6Cansel  != 0)					/* �d����������^�]�I����WAIT���ԁiT5)�̊Ԃ�[E6]�װ�����o���Ȃ�	*/
	{
		return;											
	}
	adc = glbCtRealAD;									/* ���ω����ꂽAD�l���́@			*/
	if(glbFlMainMode == GCN_DRIVE_MODE)					/* �^��^�]���̏ꍇ					*/
	{
		if(adc >= 0x3c1 || adc <= 0x20)					/* ���ω���AD�ް���3C1[hex]�ȏ�A�܂���0x20[hex]�ȉ�	*/
		{
			glbFlError |= GCN_ERROR_VACUUM_ADC_6;		/* �^��ݻ��װ[E6]					*/
			glbFlMainMode = GCN_ERROR_MODE;				/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
		}
	}
	else												/* �^��^�]���ȊO�i�ҋ@���ƒg�C�^�]���j*/
	{
		if(adc >= 0x3c1 || adc <= 0x1ff)				/* ���ω���AD�ް���3C1[hex]�ȏ�A�܂���1ff[hex]�ȉ�	*/
		{
			glbFlError |= GCN_ERROR_VACUUM_ADC_6;		/* �^��ݻ��װ[E6]					*/
			glbFlMainMode = GCN_ERROR_MODE;				/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
		}

	}
}

