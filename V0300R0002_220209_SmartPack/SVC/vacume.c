#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include	"geefFlash.h"




UBYTE	vacFlHeaterBlock;										/* ˰���ۯ��㏸�v���׸�		*/
UBYTE	glbFlSaveVacuumType;
SWORD	vacCtStartAdLevel;
UBYTE	vacFlEndAdLevel;
UBYTE	vacFlRestart;

uint8_t vacFlAbnormalEnd; 										//�񐳏�^�]�I���׸� 

void 	mainDriveMode(void);
void 	DriveStartMode(void);
void 	VacuumPullMode(void);
void	GusInclusionMode(void);
void	GusStubilityMode(void);
void 	SeelMode(void);
void	SeelHeaterBlockWait(void);
void 	SeelCoolingMode(void);
void	DriveEndMode(void);
void 	VacuumStopMode(void);
void 	VacuumStart(void);
void 	VacuumAllStop(void);
void	VacuumEmergencyStop(void);
UBYTE	DrawerSwRead(void);
SWORD	GetVacuumLevel(void);
void	VacuumEndControl(void);
void	VacuumSetWaitMode(uint8_t);
void	(*DriveProcTbl[])(void);

extern	UBYTE   glbWaitKeyOff(UBYTE,UWORD);
extern	void	mainDisplay7Seg(UWORD);							/* 7SEG�\��							*/
extern	void	mainDisplayWarning7Seg(void);					/* 7SEG�\��							*/
extern	UBYTE	glbGetReadKey(void);
extern	UBYTE   glbWaitKeyOn(UBYTE KeyNo);
extern	UWORD	R_ADC_Get_Result2(void);
extern	void	mcmSetBuzzerEnd(void);
extern	void	mcmSetBuzzerInvalid(void);						/* �����޻ް			*/
extern	void	mcmSetBuzzerEffective(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

/***********************************************************************************************************************
����ް��̏����l�FRAM�ر���A�����ް����ׯ���ɏ������܂��
***********************************************************************************************************************/

const	UBYTE	DefaultCourseData[4][8] = 		/* 4�����			*/
								{		/* ��̫�Ă̺���ް��@�����^�󎞊ԒP�ʂ�1.0�b�ŁA����ȊO��0.1�b�P�ʁ���	*/
										/*                  �����^�����قɊւ��ẮA1%�P��				����	*/
														/* �y���1�z		*/
										{ 20,			/* �^�󎞊�		*/
									  	  25,			/* ��َ���		*/
						                  40,			/* ��ٗ�p����	*/
									   	   0,			/* �޽����		*/
                                           0,			/* �޽���莞��	*/
                                           0,			/* �޽����		*/
									       30,			/* ��ĉ������	*/
										  80			/* �^������		*/
									     },
														/* �y���2�z		*/
										{ 35,			/* �^�󎞊�		*/
									  	  25,			/* ��َ���		*/
						                  40,			/* ��ٗ�p����	*/
									   	   0,			/* �޽����		*/
                                           0,			/* �޽���莞��	*/
                                           0,			/* �޽����		*/
									       30,			/* ��ĉ������	*/
										  90			/* �^������		*/
									     },
														/* �y���3�z		*/
										{ 15,			/* �^�󎞊�		*/
									  	  25,			/* ��َ���		*/
						                  40,			/* ��ٗ�p����	*/
									   	   0,			/* �޽����		*/
                                           0,			/* �޽���莞��	*/
                                           0,			/* �޽����		*/
									       30,			/* ��ĉ������	*/
										  70			/* �^������		*/
									     },
														/* �y���4�z		*/
										{ 10,			/* �^�󎞊�		*/
									  	  25,			/* ��َ���		*/
						                  40,			/* ��ٗ�p����	*/
									   	   0,			/* �޽����		*/
                                           0,			/* �޽���莞��	*/
                                           0,			/* �޽����		*/
									       30,			/* ��ĉ������	*/
										  40			/* �^������		*/
									     }
								};


/***********************************************************************************************************************
�����ް��̏����l�FRAM�ر���A�����ް����ׯ���ɏ������܂��
***********************************************************************************************************************/
const	UBYTE	DefaultSystemData[5][3] = {  {60,0,99},					/* T1:�^���~����	�i�P�ʁF1.0�b)*/
									    	 {99,0,99},					/* T2:�g�C�^�]����	�i�P�ʁF1.0�b)*/
									 		 {90,0,99},					/* T3:�^����ѱ��	�i�P�ʁF1.0�b)*/
									 		 { 5,0,99},					/* T4:�\��^�]��WAIT���ԁi�P�ʁF0.1�b)*/
									 		 {20,0,99}					/* T5:�^�]���I��WAIT���ԁi�P�ʁF1�b)*/

								            };

struct		tagSYSTEMMAXMIN	geefBfCourseDataMaxMin[8] = {{  5,99},		/* �^�󎞊�(MIN,MAX)(�P��:1�b)		*/
													     {  0,40},		/* ��َ���(MIN,MAX)(�P��:0.1�b)		*/
													     {  0,99},		/* ��ٗ�p����(MIN,MAX)(�P��:0.1�b)	*/
													     {  0,99},		/* �޽����(MIN,MAX)(�P��:0.1�b)		*/
													     {  0,99},		/* �޽���莞��(MIN,MAX)�P��:0.1�b	*/
													     {  0,90},		/* �޽����(MIN,MAX)(�P��:%)		*/
													     {  0,90},		/* ��ĉ������(MIN,MAX)�P��:0.1�b	*/
													     { 40,99},		/* �^�����فi�P��:1%)				*/
													 };


/***********************************************************************************************************************
�^��^�]�����e�[�u��
***********************************************************************************************************************/
void	(*DriveProcTbl[])() =
{
	DriveStartMode,				/* �^�]�J�n��					*/
	VacuumPullMode,				/* �^������H��					*/
	GusInclusionMode,			/* �޽�����H��					*/
	GusStubilityMode,			/* �޽����҂��H��				*/
	SeelMode,					/* ��ٍH��						*/
	SeelCoolingMode,			/* ��ٗ�p�H���i�\�t�g�J���H���j*/
	VacuumStopMode,				/* �^������ꎞ��~Ӱ��			*/
	SeelHeaterBlockWait,		/* ˰����ۯ��㏸�҂�����		*/
	DriveEndMode,				/* �H���I��						*/
};

/***********************************************************************************************************************
*    �֐���    mainDriveMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]���C������
*    �������e  �^��H���˃V�[���H���˗�p�H���ː^��^�]�I���܂ł�{�����ōs���B
*    ���Ӂ@�@�@�^��^�]���Ɉ��o���X�C�b�`���J�����ꍇ�́A�������Ɂu�H���I��������v�Ɉڍs���邱�ƁB
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void mainDriveMode(void)
{
	uint8_t driveEndFlag;

	driveEndFlag = 0;												//�d��SW�ɂ��^�]�I��
	vacFlAbnormalEnd = 0;
	
	glbFlSaveMode = glbFlMainMode;
	
	/* �^��^�]�@�H���I�����쒆�ȊO*/
	if(glbFlDriveMode != GCN_DRIVE_END_MODE)						/* �^��^�]�H���I�����쒆�͈��o��SW�̏�Ԃ����o���Ȃ�	*/
	{
		if(glbFlPowerSw)	//�d��SW�ɂ��I��ON�H
		{
			driveEndFlag = 1;
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/	
		}
		
		if( (DrawerSwRead())|| driveEndFlag)						/* �^�]���Ɉ��o��SW���u�J�v��ԂɂȂ����ꍇ	*/
		{
			/* �^��^�]�𒆎~���A�ҋ@Ӱ�ނ֑J�ڂ���		*/
			P0 &= P01_SEEL_ELECTROMAG_OFF;							/* ��ٓd����OFF							*/
			VacuumAllStop();										/* �^��^�]�o�͐M�������ׂ�OFF			*/
			VacuumEndControl();										/* �^��^�]�I�����̐M������i�^��d����=10�b ON,̧�=3���� ON*/
			
			vacFlAbnormalEnd = 1;
			
			glbFlDriveMode = GCN_DRIVE_END_MODE;					/* �^��^�]�H���I������֑J��	*/
		}	
	}
	
	(*DriveProcTbl[glbFlDriveMode])();								/* ����Ԃ�Ӱ�ނ֑J��		*/
}

/***********************************************************************************************************************
*    �֐���    
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^�]�J�n����O����
*    �������e  �E�^��|���v��ON,�t�@��->ON,�^�����فi��)->ON
*              �E�R�[�X�f�[�^���̐^�󎞊ԃZ�b�g�i�b�w��j
*              �E�^��J�n���A���݂̐^�󃌃x���{�T����ۑ����A�����^��^�]�I�����Ɏg�p����B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
********************************************************************************************************************/

void DriveStartMode(void)
{
	SWORD	Level;

	if(vacFlRestart == 0)															/* �^���~����̍ĊJ�łȂ��ꍇ		*/
	{
																					/* �^���~����̍ĊJ�̏ꍇ�́A�ŏ��̐^��J�n���̐^�����ق��g�p����	*/
		Level = GetVacuumLevel();													/* �^��J�n���̐^�����ق��Z�o�i����:�P��%)		*/
		vacCtStartAdLevel = Level + 5;												/* �^��^�]�I�����̏I����������		*/
	}																				/* �^�]�J�n�O��+5%(AD�l��0x1f���x)���I���^�����قƂ���	*/
	vacFlRestart = 0;
	VacuumStart();																	/* �^��^�]�J�n						*/
	glbFlDriveMode = GCN_VACUUM_PULL_MODE;											/* �^������H���֑J��				*/

	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* �^������H����b�ŕ\��			*/
	{
		glbCtDriveTimer = (geefBfCourseData[geefFlCourse][Vacuum])* GCN_TIMER_1S;		/* �b��5ms�P�ʂɕϊ�				*/
	}
	glbCtVacuumTimeout1 = geefBfSystemData[GCN_SYSTEM_VACUUM_TIMEOUT][Default]			/* �^�������ѱ�Ď���ذ��(T3)		*/
						 * GCN_TIMER_1S;
		
	glbCtVacuumTimeout2 = GCN_TIMER_10S;												/* 10.0�b�o�߂��Ă��^�����ق�20%�ɒB���Ȃ����Ƃ��������鎞��	*/
	glbCtVacuumTimeout3 = GCN_TIMER_30S;											/* 30�b�o�ߌ�A�^�����ق�90%����	*/
	glbFlVacuumWarning = 0;															/* �^��x���׸޾��				*/

}


/***********************************************************************************************************************
*    �֐���    VacuumPullMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^������H��
*    �������e  �E�^����c�莞�Ԃ܂��͓x����7SEG��1�b�P�ʂŕ\��
*              �E�{�H�����ɁuSTART/STOP�v�������ꂽ�ꍇ�A�^��d���ق���āA�^���~���ԁmT1�n�b�Ԑ^��������~����B
*                �^��d���قn�e�e�A�|���v�n�e�e
*			   �E�^���~���ԁi�mT1�n�b�j���o�߂����ꍇ�A�܂��́A�ēx����p�l���́uSTART/STOP�v�X�C�b�`
*                �������ꂽ�ꍇ�A�^��d���ق��J���āA�^��H�����ĊJ����B
*              �E�ĊJ�̏ꍇ�̎��Ԃ̓R�[�X�f�[�^�̎��Ԃ���ēx�ĊJ�B
*              �E�^��������Ԃ��o�߂�����A�^��d���قn�e�e�B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void VacuumPullMode(void)
{
	UBYTE	sts,key,VacuumEnd=0;
	UWORD	MaxKeyOnTime,VacuumLebelMax;
	SWORD	Level;
	UWORD	data;

	Level = GetVacuumLevel();												/* �^�����ق��Z�o�i����:�P��%)	*/

	glbFlProc 		= GCN_PROC_VACUUM;										/* �H����^���					*/
	ledControl_Proc(glbFlProc,1,0);											
	glbFlReqDot 	= (glbFlProc > GCN_PROC_VACUUM)?1:0;					/* �u�^��v�ȊO����ص�ޕ\��				*/
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_LEVEL)					/* �^��H���\�����u%�v�̏ꍇ			*/
	{
		if(glbCtVacuumTimeout1 == 0)										/* 90�b�o�߂��Ă��A�ݒ�^��x�܂œ��B���Ȃ��ꍇ�A���̍H���ɐi�܂Ȃ���Ԃ�����邽�߁A*/
																			/* �^����ѱ�Ĵװ�Ƃ��Ă������A�����װ�ɂ������H���i��فj�֑J�ڂ���悤�ɂ����B	 */
		{
			VacuumEnd = 1;													/* �^������I���׸޾��				*/
		}
	}
	if(glbCtVacuumTimeout2 == 0)											/* �^��J�n����5�b�o�߁H		*/
	{
		if(Level < 20)														/* 5�b�o�߂��Ă��^�����ق�20%�ɒB���Ă��Ȃ��ꍇ					*/
		{
			glbFlError |= GCN_ERROR_VACUUM_LEVEL_2;							/* �^��װ[E2]					*/
			glbFlMainMode = GCN_ERROR_MODE;									/* Ҳ�Ӱ�ނ�װ���oӰ�ނ�		*/
			return;
		}
	}
	

	if(glbCtVacuumTimeout3 == 0)											/* �^��J�n����30�b�o�߁H		*/
	{
		if(Level < 70)														/* 30�b�o�߂��Ă��^�����ق�90%�ɒB���Ă��Ȃ��ꍇ					*/
		{
			glbFlVacuumWarning = 1;											/* �^��x���׸޾��				*/
		}
	}


	if(glbFlVacuumWarning == 1)												/* �^��x�����H					*/
	{
		mainDisplayWarning7Seg();											/* �^��x����7SEG�\��([E3])		*/

	}
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)				/* �^��H���\�����b�̏ꍇ			*/
	{
		if(glbFlVacuumWarning == 0)											/* �^��x�����H					*/
		{
			data = ((glbCtDriveTimer * GCN_INT_TIME)/GCN_TIME_1S);			/* 5ms�P�ʂ̶�����b�ɕϊ�			*/
			mainDisplay7Seg((UWORD)data);									/* 7SEG�\��							*/
		}
		if(glbCtDriveTimer == 0)											/* �^������H���I��	?				*/
		{
			VacuumEnd = 1;													/* �^������I���׸޾��				*/
		}																
	}	
	else 																	/* �^��H���\����%�̏ꍇ			*/
	{
		VacuumLebelMax = (geefBfCourseData[geefFlCourse][VacuumLevel]);		/* ������̐^�󓞒B���ق�ذ��	*/
		if(glbFlVacuumWarning == 0)											/* �^��x�����H					*/
		{
			mainDisplay7Seg(Level);											/* �^������(%)7SEG�\��			*/
		}
		if(Level >= VacuumLebelMax )										/* ����ް��ɐݒ肳�ꂽ�^�����قɒB�����ꍇ*/
		{
			VacuumEnd = 1;													/* �^������H���I�����			*/
		}
	}
	if(VacuumEnd == 1)														/* �^������H���I���̏ꍇ			*/
	{
		P2 &= P20_VACUUM_ELECTROMAG_OFF;									/* �^��d����OFF					*/
																			/* ���޽�����H���J�n				*/
		glbCtGusInclusionTimer = (geefBfCourseData[geefFlCourse][Gus] *		/* �޽�������ԁi�P�ʁF0.1�b�j		*/
							      GCN_TIMER_100MS);
		glbFlDriveMode = GCN_GUS_INCLUSIOM_MODE	;							/* �^�]Ӱ�ނ�޽�����H���֑J��	*/
		glbFlVacuumWarning  = 0;											/* �^��x��(E3�j�ر				*/
		glbFlSecLevel  = GCN_VACUUM_SECONDS;								/* �u�b�v�\��					*/
		ledControl_Sec(glbFlSecLevel,1,0);									
		return;

	}
	if((key = glbGetReadKey()))												/* KEY�̓�������					*/
	{
		if(key == GCN_KEY_START_SET)										/* ���Ă������ꂽ�ꍇ(�^��ެ��ߋ@�\)	*/
		{
			Level = GetVacuumLevel();										/* �^�����ق��Z�o				*/
			if(Level < 40)													/* �^�����ق�40%�ɒB���Ă��Ȃ��ꍇ�͐^��ެ��ߋ@�\�����Ƃ���	*/
			{
				mcmSetBuzzerInvalid();										/* ���ݖ����޻ް			*/
				return;
			}	
			mcmSetBuzzerEffective();										/* ���ݗL���޻ް					*/
			P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* �^��d����OFF					*/
																			/* ���޽�����H���J�n				*/
			glbCtGusInclusionTimer = (geefBfCourseData[geefFlCourse][Gus] *	/* �޽�������ԁi�P�ʁF0.1�b�j		*/
                                      GCN_TIMER_100MS);
			glbFlVacuumWarning  = 0;										/* �^��x��(E3�j�ر				*/
			glbFlSecLevel  = GCN_VACUUM_SECONDS;							/* �u�b�v�\��					*/
			ledControl_Sec(glbFlSecLevel,1,0);								
					
			glbFlDriveMode = GCN_GUS_INCLUSIOM_MODE	;						/* �^�]Ӱ�ނ�޽�����H���֑J��	*/
		}	
		else if(key == GCN_KEY_STOP_SET)												/* �į�߷��i�B���j�������ꂽ�ꍇ		*/
		{	

			MaxKeyOnTime = GCN_TIME_1S;													/* ���������ʒm����	�i1�b)				*/
			sts = glbWaitKeyOff(key,MaxKeyOnTime);										/* KEY��1�b�ȏ㉟���ꂽ������			*/
			if(sts == 1)																/* KEY��1�b�ȏエ���ꂽ�ꍇ				*/
			{
				mcmSetBuzzerEffective();												/* ���ݗL���޻ް					*/
				VacuumEmergencyStop();													/* �^��H���ꎞ��~					*/

				glbCtVacuumEmergncyTimer = (geefBfSystemData[GCN_SYSTEM_VACUUM][Default]) 	/* T1:�^��ꎞ��~���Ծ�āi�b�j			*/
										     * GCN_TIMER_1S;
				glbFlSecLevel = GCN_VACUUM_SECONDS;										/* �u�b�v�\���ɂ���				*/
				ledControl_Sec(glbFlSecLevel,1,0);
				
				glbFlDriveMode = GCN_VACUUM_STOP_MODE;									/* �^�]ӰĂ��ꎞ��~�֑J��			*/
		
			}
		}
		else
		{
			mcmSetBuzzerInvalid();														/* ���ݖ����޻ް			*/
		}
	}

}

/***********************************************************************************************************************
*    �֐���    GusInclusionMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�K�X�����H������
*    �������e  �E�m�R�[�X�f�[�^�n�ɐݒ肳�ꂽ���ԁi�b���j���o�߂���܂ŃK�X�������s���B
*              �E�K�X�d���ق��J���i�n�m�j�K�X�𕕓�����B
*				 �K�X�d���قn�m
*              �E�ݒ莞�Ԃ��o�߂�����K�X�d���ق���A���̍H���i�K�X����H���j�ֈڍs����B
*              �E�A���A�m�R�[�X�f�[�^�n�̐ݒ莞�ԁi�K�X���ԁj���O�D�O�b�̏ꍇ�A�K�X�����H���͎��s�����A���̍H��
*�@�@�@�@�@�@�@�i�V�[���H���j�ֈڍs����
*    ���Ӂ@�@�@�Q�O�P�W�N�W���Q�W�����݁A�R�[�X�f�[�^���̂��̎��Ԃ͂O�D�O�b�ƂȂ��Ă���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	GusInclusionMode(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEY�̓�������					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* �į�߷��i�B���j�ȊO�̏ꍇ		*/
		{
			mcmSetBuzzerInvalid();										/* �����޻ް			*/
		}
	}
	if(geefBfCourseData[geefFlCourse][Gus] == 0)							/* �޽�����ݒ莞�Ԃ�0�b�̏ꍇ		*/
	{
		vacFlHeaterBlock = 1;											/* ˰���ۯ��㏸�v���׸޾��	*/
		glbFlDriveMode = GCN_SEEL_MODE;									/* �޽�����H���͎��s�����A���̍H���i��ٍH���j�֑J�ڂ���	*/
		return;
	}
	P13 |= P130_GUS_ELECTROMAG_ON;										/* �޽�d���� ON						*/
	if(glbCtGusInclusionTimer == 0)									 	/* �޽�������Ԃ�0�b�ɂȂ����ꍇ		*/
	{
		P13 &= P130_GUS_ELECTROMAG_OFF;									/* �޽�d���� OFF					*/
		glbCtGusStubilityTimer = (geefBfCourseData[geefFlCourse][GusStability] *	/* �޽����҂����ԁi�P�ʁF0.1�b�jذ��		*/
								  GCN_TIMER_100MS);						/* 0.1s�P�ʂ�5ms�P�ʂɕϊ�					*/
		glbFlDriveMode = GCN_GUS_STUBILITY_MODE	;						/* �^�]Ӱ�ނ�޽����҂��H���֑J��	*/
	}
}

/***********************************************************************************************************************
*    �֐���    GusStubilityMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�K�X����҂��H��
*    �������e  �E[�R�[�X�f�[�^�n�ɐݒ肳�ꂽ�K�X����҂����Ԃ̊ԃE�G�C�g����B
*    ���Ӂ@�@�@�Q�O�P�W�N�W���Q�W�����݁A�R�[�X�f�[�^���̂��̎��Ԃ͂O�D�O�b�ƂȂ��Ă���B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	GusStubilityMode(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEY�̓�������					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* �į�߷��i�B���j�ȊO�̏ꍇ		*/
		{
			mcmSetBuzzerInvalid();										/* �����޻ް			*/
		}
	}

	if(glbCtGusStubilityTimer == 0)										/* �޽����҂����ԂɒB�����ꍇ	*/
	{
		glbFlDriveMode = GCN_SEEL_MODE	;								/* �^�]Ӱ�ނ�ٍH���֑J��	*/
		vacFlHeaterBlock = 1;											/* ˰���ۯ��㏸�v���׸޾��	*/
	}
}

/***********************************************************************************************************************
*    �֐���    SeelMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�V�[���H��
*    �������e  �E�V�[���d���ق��J���i�n�m�j���ƂŁA�q�[�^�[�u���b�N���㏸�����A�V�[���d����ʓd���A�܌��𖧕���B
*                �A���A�V�[�����Ԃ��O�b�̏ꍇ�͎��̍H���i�V�[����p�H���j�ֈڍs����B
*              �ySTEP1�z�V�[���d���ق��J���i�n�m�j�q�[�^�[�u���b�N���㏸������B
*                       ���V�[���d���� �n�m	�J��
*              �ySTEP2�z�q�[�^�[�u���b�N�㏸�҂����ԁi�O�D�V�b�j���o�߂���܂ő҂B
*			   �ySTEP3�z�q�[�^�[�u���b�N�㏸�҂����Ԃ��o�߂�����A�q�[�^�[�d����ʓd�i�n�m�j���A
*                       �m�R�[�X�f�[�^�n�̃V�[�����Ԃɏ]���ăV�[�������s����B
*                       ���q�[�^�[�M�� �n�m �ʓd
*			   �ySTEP4�z�V�[�����Ԍo�ߌ�q�[�^�[���n�e�e����B
*						���q�[�^�[�M��	�n�e�e	�ʓd��~
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void 	SeelMode(void)
{	
	UWORD	data;

	volatile	UBYTE	key;

	if((key = glbGetReadKey()))													/* KEY�̓�������					*/
	{
		if(key != GCN_KEY_STOP_SET)												/* �į�߷��i�B���j�ȊO�̏ꍇ		*/
		{
			mcmSetBuzzerInvalid();												/* �����޻ް			*/
		}
	}

	glbFlProc 		= GCN_PROC_SEEL;											/* �H����ق�				*/
	ledControl_Proc(glbFlProc,1,0);											
		
	glbFlSecLevel   = GCN_VACUUM_SECONDS;
	ledControl_Sec(glbFlSecLevel,1,0);
			
	if(geefBfCourseData[geefFlCourse][Seel] == 0)								/* ��ِݒ莞�Ԃ�0�b�̏ꍇ		*/
	{
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;						/* �u�^��v�ȊO����ص�ޕ\��				*/
		DI();
		P7 &= P73_HEATER_START_OFF;												/* ˰�OFF						*/
		EI();
		glbCtSeelCoolingTimer = (geefBfCourseData[geefFlCourse][SeelCooling] *	/* ��ٗ�p�H�����ԁi�P�ʁF0.1�b�j		*/
							      GCN_TIMER_100MS);
		glbCtSoftReleaseTimer = (geefBfCourseData[geefFlCourse][SoftRelease] *	/* ��ĉ�����ԁi�P�ʁF0.1�b�j		*/
							      GCN_TIMER_100MS);
		glbFlDriveMode = GCN_SEEL_COOLING_MODE;									/* ��ٍH���͎��s�����A���̍H���i��ٗ�p�H���j�֑J�ڂ���	*/
		vacFlHeaterBlock = 0;													/* ˰���ۯ��㏸�v���׸�ؾ��	*/
		return;
	}
	P0 |= P01_SEEL_ELECTROMAG_ON;												/* ��ٓd���� ON						*/
	if(vacFlHeaterBlock == 1)													/* ˰���ۯ��㏸�v���׸޾��	*/
	{
		data = geefBfCourseData[geefFlCourse][Seel];
		mainDisplay7Seg((UWORD)data);											/* 7SEG�\��							*/
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;						/* �u�^��v�ȊO����ص�ޕ\��				*/
		glbFlDriveMode = GCN_HEATER_BLOCK_MODE;									/* ˰���ۯ��㏸�҂�����wait�֑J�ځi0.7�b�j*/
		
		glbCtHeaterBlockTimer = GCN_TIMER_500MS*3;								/* 1500ms(1.5�b�j5ms���Ăɕϊ������l		*/
		return;
	}
	data = ((glbCtSeelTimer * GCN_INT_TIME)/GCN_TIME_100MS);					/* 5ms�P�ʂ̶�����0.1�b�P�ʂɕϊ�			*/
	mainDisplay7Seg((UWORD)data);												/* 7SEG�\��							*/
	glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;							/* �u�^��v�ȊO����ص�ޕ\��				*/
	if(glbCtSeelTimer == 0)														/* ��ٍH�����ԏI��?					*/
	{
		DI();
		P7 &= P73_HEATER_START_OFF;												/* ˰�OFF						*/
		EI();
		glbCtSeelCoolingTimer = (geefBfCourseData[geefFlCourse][SeelCooling] *	/* ��ٗ�p�H�����ԁi�P�ʁF0.1�b�j		*/
							      GCN_TIMER_100MS);
								  
		if(glbFlSoftOpen)
		{
			glbCtSoftReleaseTimer = (geefBfCourseData[geefFlCourse][SoftRelease] *	/* ��ĉ�����ԁi�P�ʁF0.1�b�j		*/
								      GCN_TIMER_100MS);
			if(glbCtSoftReleaseTimer)
			{
			  P7 |= P75_SOFTRELEASE_START_ON;									/* ��ĉ����ON							*/
			}
		}
		else
		{
			glbCtSoftReleaseTimer = 0;
		}

		glbFlDriveMode = GCN_SEEL_COOLING_MODE;									/* ��ٗ�p�H���֑J�ڂ���	*/
	}
	
}

/***********************************************************************************************************************
*    �֐���    SeelHeaterBlockWait
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �q�[�^�[�u���b�N�㏸�҂������i�O�D�V�b�j
*    �������e  �E�q�[�^�[�u���b�N�㏸�҂����ԁi�O�D�V�b�j���o�߂���܂ő҂B
*              �E�q�[�^�[�u���b�N�㏸�҂����Ԃ��o�߂�����A�q�[�^�[�d����ʓd�i�n�m�j���A
*                �R�[�X�f�[�^�̃V�[�����Ԃɏ]���ăV�[�������s����B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	SeelHeaterBlockWait(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEY�̓�������					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* �į�߷��i�B���j�ȊO�̏ꍇ		*/
		{
			mcmSetBuzzerInvalid();										/* �����޻ް			*/
		}
	}

	if(glbCtHeaterBlockTimer == 0)										/* 0.7�b�o�߁H					*/
	{
		DI();
		P7 &= P74_PUMP_START_OFF;										/* �|���v��~					*/
		P7 |= P73_HEATER_START_ON;										/* ˰�ON						*/
		EI();
		glbFlDriveMode = GCN_SEEL_MODE	;								/* �^�]Ӱ�ނ��ēx��ٍH���֑J��	*/
		glbCtSeelTimer = (geefBfCourseData[geefFlCourse][Seel] *			/* ��ٍH�����ԁi�P�ʁF0.1�b�j		*/
						  GCN_TIMER_100MS);
		vacFlHeaterBlock = 0;											/* ˰���ۯ��㏸�v���׸�ؾ��		*/
	}

}
/***********************************************************************************************************************
*    �֐���    SeelCoolingMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �V�[����p�H���i�\�t�g�J���j
*    �������e  �E�m�R�[�X�f�[�^�n�̃V�[����p���Ԑݒ�ɏ]���ăV�[�����p����B
*              �E�\�t�g�J�����Ԃ��ݒ肳��Ă���ꍇ�A�\�t�g�J�����s���B
*              �E�\�t�g�J�����Ԃ��ݒ肳��Ă��Ȃ��ꍇ�m�R�[�X�f�[�^�n�̃V�[����p���Ԃ̊ԁA�E�G�C�g��
*                ���Ԍo�ߌ�A�H���I������ֈڍs����B
*                 ���V�[���d����->ON �i�J���j
*              �E�m�R�[�X�f�[�^�n�̃\�t�g�J�����Ԃ��ݒ肳��Ă���ꍇ�A�m�R�[�X�f�[�^�n�̃V�[����p����
*                 �ݒ莞�ԃE�G�C�g�Ɠ����Ƀ\�t�g�J���ق��J���i�n�m�j
*                 ���\�t�g�����->ON �i�J���j
*              �E�\�t�g�J�����Ԃ��o�߂�����A�\�t�g�J���ق����B
*                 ���\�t�g�����->OFF �i����j
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void 	SeelCoolingMode(void)
{
	UWORD	data;

	glbFlProc = GCN_PROC_COOL;											/* �H�����p��				*/
	ledControl_Proc(glbFlProc,1,0);		
		
	glbFlSecLevel   = GCN_VACUUM_SECONDS;
	ledControl_Sec(glbFlSecLevel,1,0);
		
	if(geefBfCourseData[geefFlCourse][SeelCooling]  == 0) 					/* ��ٗ�p�H�����Ԃ�0�b�̏ꍇ	*/
	{
		if(geefBfCourseData[geefFlCourse][SoftRelease] == 0)				/* ��ĉ���H�����Ԃ�0�b�̏ꍇ	*/
		{
			P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ��ٓd����OFF							*/
			VacuumAllStop();												/* �^��^�]�o�͐M�������ׂ�OFF			*/
			VacuumEndControl();												/* �^��^�]�I�����̐M������i�^��d����=10�b ON,̧�=3���� ON*/
			glbFlDriveMode = GCN_DRIVE_END_MODE;							/* �^��^�]�H���I������֑J��	*/
			return;
		}
	}


	if(glbCtSoftReleaseTimer == 0)										/* ��ĉ����ON�w�莞�Ԍo�߁H			*/
	{
		P7 &= P75_SOFTRELEASE_START_OFF;								/* ��ĉ����OFF							*/
	}

	data = ((glbCtSeelCoolingTimer * GCN_INT_TIME)/GCN_TIME_100MS);		/* 5ms�P�ʂ̶�����0.1�b�P�ʂɕϊ�		*/
	mainDisplay7Seg((UWORD)data);										/* 7SEG�\��								*/
	if((glbCtSeelCoolingTimer == 0)&&(glbCtSoftReleaseTimer == 0))										/* ��ٗ�p�w�莞�Ԍo�߁H				*/
	{
		P7 &= P75_SOFTRELEASE_START_OFF;								/* ��ĉ����OFF							*/
		
		P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ��ٓd����OFF							*/
		VacuumAllStop();												/* �^��^�]�o�͐M�������ׂ�OFF			*/
		VacuumEndControl();												/* �^��^�]�I�����̐M������i�^��d����=10�b ON,̧�=3���� ON*/
		glbFlDriveMode = GCN_DRIVE_END_MODE;							/* �^��^�]�H���I������֑J��	*/

	}
}



/***********************************************************************************************************************
*    �֐���    DriveEndMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�H���I��������
*    �������e  �E�`�����o�[�����C�J�����A�^��^�]���I������B
*               �V�[���d����	�n�e�e	����
*				�^��J����	    �n�e�e	�J��
*               �|���v	        �n�e�e	��~(*1)
*               �t�@��	        �n�m	�|���v��~��R���ԋN��
*               �^��d����	    �n�m	�|���v��~��P�O�b�ԊJ��
*               �K�X�d����	    �n�e�e	����
*               �\�t�g�J����	�n�e�e	����
*               �q�[�^�[�M��	�n�e�e	�ʓd��~
*              �E�|���v��~���A�z�Ǔ����C�J������ׁA�P�O�b�Ԑ^��ق��J���B�^��ق��J���Ă���r���ŁA
*                ���̉^�]���n�܂����ꍇ�A���̎��_�Ŏ��̉^�]���J�n����B(DriveStartMode�ցj
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	DriveEndMode(void)
{
	SWORD	Level;
	UBYTE	key;


	if((key = glbGetReadKey()))							/* SW�̓�������					*/
	{
		if(key != GCN_KEY_STOP_SET)						/* �^���~(SW6)�ȊO�̏ꍇ		*/
		{
			mcmSetBuzzerInvalid();						/* �����޻ް			*/
		}
	}
	
	if(glbCtWaitTimerT5 == 0)							/* �^�]�I�����WAIT���Ԍo�߁H	*/
	{
		vacFlAbnormalEnd = 1;
		if(vacFlEndAdLevel == 0)						/* T5���ԓ��ɐ^��x���J�n����ϲŽ5%�ɒB���ĂȂ��ꍇ	*/
		{
			VacuumSetWaitMode(vacFlAbnormalEnd);		/* �ҋ@Ӱ�ނ֑J�ڂ��邽�߁A7SEG�y��LED��^��H���̏����l�ɖ߂�*/
														/* �޻ް��炳���ɑҋ@Ӱ�ނ֑J��					*/
														/* ϲŽ5%�ɒB�����ꍇ���޻ް��炵�đҋ@Ӱ�ނ֑J��	*/
		}
		glbFlMainMode = GCN_WAIT_MODE;					/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/
		return;
	}

	Level = GetVacuumLevel();							/* �^�����ق��Z�o										*/
	if(Level > vacCtStartAdLevel)						/* �^�����ق��^��J�n����(�^������ + 5%)�ȉ��ɂȂ�܂Ő^��H�����I�����Ȃ�		*/
	{
		return;											/* �B����܂ŁA����Ӱ�ނɗ��܂�													*/
	}
	
	if(!DrawerSwRead())	//�t�^���J������I��
	{
		return;	
	}
	
	vacFlEndAdLevel = 1;								/* �^�����ق��^��J�n����(�^������ + 5%)�ȉ��ɒB����							*/
	
	mcmSetBuzzerEnd();									/* �L���޻ް						*/
	
	glbFlMainMode = GCN_WAIT_MODE;						/* Ҳ�Ӱ�ނ�ҋ@Ӱ�ނ�		*/
	

	VacuumSetWaitMode(vacFlAbnormalEnd);				/* �ҋ@Ӱ�ނ֑J�ڂ��邽�߁A7SEG�y��LED��^��H���̏����l�ɖ߂�*/
	
	glbFlPackCountDisplay_Start = 1;
	
	return;

}
/***********************************************************************************************************************
*    �֐���    VacuumStart
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]�I������ҋ@Ӱ�ޑJ�ڎ��̂V�r�d�f�^�k�d�c�����\��
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	VacuumSetWaitMode(uint8_t endType)
{
	UWORD	data;
	
	/*�H���I��������*/
	glbFlReqDot = 0;											/* �u�^��v�H���̂��߁A��ص�ޕ\���Ȃ�					*/
	glbFlProc 		= GCN_PROC_VACUUM;							/* �H�����u�^��v�\���ɂ���	*/
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
		
	/*����Ƀp�b�N����*/
	if(endType == 0)
	{
		geefCtPack++;				/* �p�b�N��	*/
		if(geefCtPack > 999)		/* 999��𒴂����ꍇ			*/
		{
			geefCtPack = 999	;	/* 999�������Ƃ���			*/
		}
	}
	else
	{	
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
	ledControl_Proc(glbFlProc,1,0);			

}
/***********************************************************************************************************************
*    �֐���    VacuumStart
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]�̗�p�H���I����̏I��������
*    �������e  
*    ���Ӂ@�@�@�u�����O�Q�D�O�O���t�@���̎g�p�͎~�߁A�c�C���`�����o�[���̐e�@�E�q�@�Ԃ̐���M���Ƃ��Ďg�p�B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	VacuumEndControl(void)
{

	P2 |= P20_VACUUM_ELECTROMAG_ON;						/* �^��d����ON						*/
	glbFlVacuumElectromangReq = 1;						/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v�����	*/
	glbCtVacuumElectromang = GCN_TIMER_10S;				/* 10�b��Ͼ��(5ms�P�ʂɕϊ������l�j					*/

	glbCtFunTimer = GCN_TIME_3MN;						/* �g�C�^�]�I����A̧݂�3���ԉ񂷂��߂���Ͼ��	*/
	
	glbCtWaitTimerT5 = geefBfSystemData[GCN_SYSTEM_DRIVE_AFTER_WAIT]
                       [Default] * GCN_TIMER_1S;		/* �^�]�I����WAIT����(T5)	*/
					   
	vacFlEndAdLevel = 0;								/* ��p�H���I����̐^��x���J�n���̐^��xϲŽ5%�ɒB�������ۂ����׸޸ر	*/

}
																
/***********************************************************************************************************************
*    �֐���    VacuumStopMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �@�^���~�@�\
*    �������e  �E�^������H�����ɑ���p�l���́u�^���~�v�X�C�b�`�i�B���X�C�b�`�j���P�b���������ꂽ�ꍇ�A�^��d���ق���āA
*                �^���~���ԁmT1�n�b�Ԑ^��������~����B
*                	�^��d����	�n�e�e	����
*		            �|���v	    �n�e�e	��~
*              �E�^���~���ԁi�mT1�n�b�j���o�߂����ꍇ�A�܂��́A�ēx����p�l���́u����/�į�߁v�X�C�b�`
*                �������ꂽ�ꍇ�A�^��d���ق��J���āA�^��H�����ĊJ����B
*                   �^��d����	�n�m	�J��
*                   �|���v	    �n�m	�ғ�
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void VacuumStopMode(void)
{
	UBYTE	restart=0,key;
	UWORD	data;


	VacuumEmergencyStop();											/* �^��H���ꎞ��~					*/
	if(glbFlSecLevel  == GCN_VACUUM_SECONDS )						/* �u�b�v�\���̏ꍇ					*/
	{
		data = ((glbCtVacuumEmergncyTimer * GCN_INT_TIME)/GCN_TIME_1S);	/* 5ms�P�ʂ̶�����b�ɕϊ�			*/
	}
	else
	{
		data = GetVacuumLevel();									/* �^�����ق��Z�o										*/
	}
	mainDisplay7Seg((UWORD)data);									/* 7SEG�\��							*/
	if(glbCtVacuumEmergncyTimer == 0)								/* �^��ꎞ��~���Ԃ��o�߂������H	*/
	{
		restart = 1;												/* �^������H���̍ĊJ				*/
	}
	if((key = glbGetReadKey()))										/* SW�̓�������					*/
	{
		if(key == GCN_KEY_START_SET)								/* START/STOP(SW5)�������ꂽ�ꍇ	*/
		{
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/
			restart = 1;											/* �^������H���̍ĊJ				*/
		}
		else if(key == GCN_KEY_UP_SET)								/* ��(SW3)�̏ꍇ						*/
		{
			glbFlSecLevel = GCN_VACUUM_SECONDS;						/* �u�b�v�\���ɂ���				*/
			ledControl_Sec(glbFlSecLevel,1,0);
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/

		}
		else if(key == GCN_KEY_DOWN_SET)							/* ��(SW4)�̏ꍇ						*/
		{
			glbFlSecLevel = GCN_VACUUM_LEVEL;						/* �u%�v�\���ɂ���				*/
			ledControl_Sec(glbFlSecLevel,1,0);
			mcmSetBuzzerEffective();								/* ���ݗL���޻ް					*/
		}
		else if(key == GCN_KEY_STOP_SET)							/* �^���~(SW6)�������ꂽ�ꍇ	*/
		{
		}
		else
		{
			mcmSetBuzzerInvalid();									/* �����޻ް			*/
		}
	}
	if(restart == 1)												/* �^������H���̍ĊJ?				*/
	{
		DI();
		glbCtVacuumEmergncyTimer = 0;								/* �^���~����ؾ��					*/
		glbCtVacuumTimeout1 = 0;									/* �^�������ѱ�Ď��Զ����ر			*/
		
		glbCtVacuumTimeout2 = 0;									/* 5�b�o�߂��Ă��^�����ق�20%�ɒB���Ȃ����Ƃ��������鶳���ر	*/
		glbCtVacuumTimeout3 = 0;									/* 30�b�o�ߌ�A�^�����ق�90%�������鶳���ر	*/

		EI();
		glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
	                           GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;		/* �u�b�vor �u%�v��^��H���O�̕\���ɖ߂�			*/
		ledControl_Sec(glbFlSecLevel,1,0);
									
		vacFlRestart = 1;
		glbFlDriveMode = GCN_DRIVE_START_MODE;						/* �^������H���֑J��				*/
																	/* �^������H���̏�������ĊJ�i�r������ł͂Ȃ��j	*/
	}
}

/***********************************************************************************************************************
*    �֐���    VacuumStart
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^������J�n���̐���M���o��
*    �������e  
*    ���Ӂ@�@�@�c�C���`�����o�[�d�l���̎q�@�w��̏ꍇ�̓|���v�ғ��͂��Ȃ��B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void VacuumStart(void)
{
	DI();

	P7 |= P74_PUMP_START_ON;									/* ����߉ғ��J�n					*/ 

	EI();
	P4 |= P41_VACUUM_RELEASE_ON;									/* �^������ON						*/ 
	P2 |= P20_VACUUM_ELECTROMAG_ON;									/* �^��d����ON						*/
	glbFlVacuumElectromangReq = 0;									/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��ؾ��	*/

}

/***********************************************************************************************************************
*    �֐���    VacuumAllStop
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��^�]�i�^��H���`�V�[����p�j�Ɋւ�邷�ׂĂ̐���o�͐M����OFF����
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void VacuumAllStop(void)
{
	if(glbFlWarmCansel == 0)										/* �^�]�O�Ɂu�g�C�^�]�v���N������Ă����ꍇ�A�|���v�͒�~�����|���v�^�]���p������B	*/
																	/* ����āA�g�C�^�]�̓r���Ő^��H���ɓ������ꍇ������߂��~�����Ȃ��iOFF���Ȃ�)�@	*/
	{
		DI();
		P7 &= P74_PUMP_START_OFF;									/* ����߉ғ���~					*/ 
		EI();
	}
	
	P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* �^��d����OFF					*/
	P4 &= P41_VACUUM_RELEASE_OFF;									/* �^������OFF						*/ 
	P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ��ٓd����OFF							*/
	DI();
	P7 &= P74_PUMP_START_OFF;										/* ����߉ғ���~						*/ 
	P7 &= P73_HEATER_START_OFF;										/* ˰�OFF								*/
	P7 &= P75_SOFTRELEASE_START_OFF;								/* ��ĉ����OFF							*/
	EI();
	P13 &= P130_GUS_ELECTROMAG_OFF;									/* �޽�d���� OFF						*/

}

/***********************************************************************************************************************
*    �֐���    VacuumEmergencyStop
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^������ꎞ��~����OFF����o�͐M��
*    �������e  �^��d����	�n�e�e	����
*			   �|���v	    �n�e�e	��~
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void VacuumEmergencyStop(void)
{
	DI();
	P7 &= P74_PUMP_START_OFF;										/* ����߉ғ���~					*/ 
	EI();
	P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* �^��d����OFF						*/
}

/***********************************************************************************************************************
*    �֐���    DrawerSwRead
*    ��  ��    
*    �߂�l    0:�A1:�J
*    �֐��@�\  ���o��SW�̊J���o
*    �������e  �^��d����	�n�e�e	����
*			   �|���v	    �n�e�e	��~
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE	DrawerSwRead(void)
{
	UBYTE	data1,data2,sts;

	data1 = P13;											/* �ڰ�u�v���			*/
	data2 = P13;
	if(data1 != data2)
	{
		data1 =  P13;
	}
	if(!(data1 & P137_DRAWER_SW))							/* �ڰ�u�v���?			*/
	{
		sts = 0;											/* �u�v*/
	}
	else
	{
		sts = 1;											/* �u�J�v*/
	}
	return(sts);

}

/***********************************************************************************************************************
*    �֐���    GetVacuumLevel
*    ��  ��    
*    �߂�l    �^�����فi�������ʂ܂Łj�܂��͐^��װ�i-1)
*    �֐��@�\  �^�����َZ�o
*    �������e  �^�����ق�0%����99%�܂ŎZ�o�B
*    ���l�F�@�@�^��������n�܂�ƁAADC���ǂݍ���AD�l�͏������Ȃ�
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

SWORD	GetVacuumLevel(void)
{
	long int	adc;
	long int	y;

	adc = glbCtRealAD;												/* ���ω����ꂽAD�l���́@			*/
	y =(GCN_NORMAL_LEVEL - (adc +geefCtAnalogAdjust)) *1000 / 		/* �i�W����C���̎���AD�l�|�iAD�l+�␳�l�j�j���i�W����C����AD�l�|��l�j*/
       (GCN_NORMAL_LEVEL - GCN_BASE_AD	);							
	y = (y + 5)/10;													/* �l�̌ܓ����āA�����_�ȉ��͐؂�̂ĂĐ����ɂ���	*/

	
	if(y <= 0)														/* 0%�ȉ��̏ꍇ						*/
	{
		y = 0;														/* 0.0%�ɕ␳						*/
	}
	if(y >= 99)														/* 99%�ȏ�̏ꍇ					*/
	{
		y = 99;														/* 99���ɕ␳						*/
	}
	return((SWORD)y);
}
