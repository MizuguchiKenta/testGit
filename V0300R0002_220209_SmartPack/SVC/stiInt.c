#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"
#include "r_cg_timer.h"

UBYTE	stiFlKydt;
UBYTE	stiFlFlashing;								/* ĸ�ٍX�V								*/
UWORD	stiCtFlashing;								/* 500ms��������						*/
UBYTE	stiFlFlashing2;								/* ĸ�ٍX�V								*/
UWORD	stiCtFlashing2;								/* 500ms��������						*/
UBYTE	stiFlFlashing3;								/* ĸ�ٍX�V								*/
UWORD	stiCtFlashing3;								/* 500ms��������						*/
UBYTE	stiFlFlashing4;								/* ĸ�ٍX�V								*/
UWORD	stiCtFlashing4;								/* 500ms��������						*/
long int stiCtP124=0;
long int stiCtP124Check=0;
long int stiCtChildError=0;
UWORD	stiBfAdc[4];
UWORD	stiCtMinute;


#define	LCN_KEY_SW1_COURSE_SET		0x02			/* �����		*/
#define	LCN_KEY_SW2_PROC_SET		0x04			/* �H����		*/
#define	LCN_KEY_SW3_UP_SET			0x08			/* ����			*/
#define	LCN_KEY_SW4_DOWN_SET		0x10			/* ����			*/
#define	LCN_KEY_SW5_START_SET		0x20			/* ���ķ�		*/
#define	LCN_KEY_SW6_STOP_SET		0x40			/* �į�߷�(�B���j*/
#define	LCN_KEY_SW7_PUSH_SET		0x80			/* PUSH��		*/

#define	LCN_LCD_FLASH_TIME			500				/* LED�_�Ŏ����i�_��500m�����500ms)	*/
#define	LCN_LCDCOM_NUM				4				/* LEDCOM����M���̌�			)	*/
#define	LCN_LCD_FLASH_CNT			(LCN_LCD_FLASH_TIME/LCN_LCDCOM_NUM)

#define	LCN_LCD_FLASH_TIME2			1000				/* LED�_�Ŏ����i�_��500m�����500ms)	*/
#define	LCN_LCD_FLASH_CNT2			(LCN_LCD_FLASH_TIME/LCN_LCDCOM_NUM)

struct	tagLEDCOM	LedCom[LCN_LCDCOM_NUM] = {{&P5,	P50_LEDCOM1_ON,P50_LEDCOM1_OFF},			/* LEDCOM1���ڽ�AON,OFF		*/
							   				  {&P5,	P51_LEDCOM2_ON,P51_LEDCOM2_OFF},			/* LEDCOM2���ڽ�AON,OFF		*/
										      {&P7,  P70_LEDCOM4_ON,P70_LEDCOM4_OFF},			/* LEDCOM4���ڽ�AON,OFF		*/
										      {&P3,  P30_LEDCOM3_ON,P30_LEDCOM3_OFF},			/* LEDCOM3���ڽ�AON,OFF		*/
                                };	

UBYTE	stiDt7Seg[]	=		{	0x3f,					/* ����:'0'	*/
								0x06,					/* ����:'1'	*/
								0x5b,					/* ����:'2'	*/
								0x4f,					/* ����:'3'	*/
								0x66,					/* ����:'4'	*/
								0x6d,					/* ����:'5'	*/
								0x7d,					/* ����:'6'	*/
								0x27,					/* ����:'7'	*/
								0x7f,					/* ����:'8'	*/
								0x6f,					/* ����:'9'	*/
								0x77,					/* �p��:'A'	*/
								0x7c,					/* �p��:'b'	*/
								0x39,					/* �p��:'C'	*/
								0x5e,					/* �p��:'d'	*/
								0x79,					/* �p��:'E'	*/
								0x71,					/* �p��:'F'	*/
								0x40,					/* �L��:'-'	*/
								0x00,					/* ALL OFF	*/
								0xff					/* ALL ON	*/

							};
UBYTE	stiDtBit[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,0xff};	


UBYTE stiLedOut_Com2 = 0x00;
UBYTE stiLedOut_Com3 = 0x00;
UBYTE stiLedOut_Com3_Sec = 0x00;

void    stiIntMain(void);
void 	GlobalTimerReneal(void);
void 	stiInKey(void);
void 	stiChkLEDCOM(void);
UBYTE	iodInKey(void);
extern	UBYTE	DrawerSwRead(void);
extern	UWORD	R_ADC_Get_Result2(void);
void	stiInitialSamplingADC(void);
void	stiReadSamplingADC(void);
void	stiCheckPump(void);											/* ����߉ғ����ԐώZ		*/
UBYTE	stiCheckPumpthermal(void);						/* ����߻��ق̴װ�Ď�		*/

void stiPowerSwCheck(void);
void stiPowerSwLong(void);
void powerLedControl(uint8_t ctl,uint8_t onTime,uint8_t offTime );
void stiPowerLedOut(void);

void powerOn_GuruGuru7Seg(void);
extern	void	R_WDT_Restart(void);

void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

/***********************************************************************************************************************
*    �֐���    stiIntMain
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �P�����^�C�}���荞�ݏ���
*    �������e  �P���������̏����ƂT���������̏���������B
*			   �y�P���������z
*			   		�E�k�d�c����
*                   �E�b�m�S�M�����͊Ď��i�c�C���`�����o�[�p)
*                   �E�b�m�V�M���o�͐���i�c�C���`�����o�[�p)
*					�E�e�@�ɂĎq�@�^�]���M���Ď��ɂ��|���v����
*              �y�T���������z
*              �@�@ �E�|���v�ғ����ԐώZ����	
*                   �E���ʃ^�C�}�[���ԊǗ�
*                   �E�r�v���͏���	
*                   �E�^��Z���T�[�`�^�c�l���ω�����
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void stiIntMain(void)
{
	R_WDT_Restart();											/* �����ޯ����ؾ��							*/

	if(glbCtIntTimer >= 5)									/* 5ms�o�߁H					*/
	{
		stiCheckPump();										/* ����߉ғ����ԐώZ			*/
		GlobalTimerReneal();								/* ������ύX�V					*/
		stiInKey();											/* KEY ���͊Ď�					*/
		stiReadSamplingADC();								/* �^��ݻ�A/D�l�̓��͂ƕ��ω�	*/
		
		stiPowerSwCheck();
		
		if(glbFlSystemMode  != 1)						/* ����Ӱ�ވȊO�̏ꍇ							*/
		{
			stiPowerLedOut();
		}
		
		glbCtIntTimer=0;									/* 5ms��ϰؾ��					*/
	}
	glbCtIntTimer++;
	glbCtCN4Timer++;
	
	
	stiChkLEDCOM();											/* LED����					*/


}


/***********************************************************************************************************************
*    �֐���    GlobalTimerReneal
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �e��A�Ď��^�C�}�̊Ď�
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void GlobalTimerReneal(void)
{

	glbCtKeyOffTimer++;									/* KEY OFF�����X�V			*/
	if(glbCtDriveTimer != 0)							/* �^��H���c�莞�ԗL�̏ꍇ	*/
	{
		glbCtDriveTimer--;								/* ��ϒl�X�V					*/
	}

	if(glbCtVacuumEmergncyTimer != 0)					/* �^��ꎞ��~���Ԏc�̏ꍇ			*/
	{
		glbCtVacuumEmergncyTimer--;						/* ��ϒl�X�V					*/
	}

	if(glbCtGusInclusionTimer != 0)						/* �޽�����H�����{���H			*/
	{
		glbCtGusInclusionTimer--;						/* ��ϒl�X�V					*/
	}

	if(glbCtGusStubilityTimer != 0)						/* �޽���莞�Զ��Ē��H			*/
	{
		glbCtGusStubilityTimer--;						/* ��ϒl�X�V					*/
	}

	if(glbCtSeelTimer != 0)								/* ��ٍH�����{���H			*/
	{
		glbCtSeelTimer--;								/* ��ϒl�X�V					*/
	}

	if(glbCtSeelCoolingTimer != 0)						/* */
	{
		glbCtSeelCoolingTimer--;
	}
	
	if(glbCtSoftReleaseTimer != 0)						/* */
	{
		glbCtSoftReleaseTimer--;
	}
	
	
	if(glbCtWarmTimer != 0)								/* �g�C�^�]���{���H				*/
	{
		glbCtWarmTimer--;								/* ��ϒl�X�V					*/
	}
	if(glbCtVacuumeWarm != 0)							/* �g�C�^�]Ӱ�ނɂĐ^��d���يJ���H(0.5�b�j*/
	{	
		glbCtVacuumeWarm--;								/* ��ϒl�X�V					*/
	}
	if(glbCtFunTimer != 0)								/* ̧݋N������ύX�V��			*/
	{	
		glbCtFunTimer--;								/* ��ϒl�X�V					*/
	}

	if(glbFlSysHeaterReq == 1)							/* ����Ӱ��˰��Ď��v���L?		*/
	{
		glbCtSysHeaterOffTimer--;						/* 2�b�Ď���϶����X�V			*/
		
	}
	if(glbFlVacuumElectromangReq == 1)					/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v���L��H*/
	{
		if(glbCtVacuumElectromang != 0)					/* ���=0?						*/
		{
			glbCtVacuumElectromang--;					/* 10�b��ύX�V					*/
		}
	}
	if(glbCtHeaterBlockTimer != 0)						/*  ˰���ۯ��㏸���Զ��Ē��H�i0.7�b�j	*/
	{
		glbCtHeaterBlockTimer--;						/* 0.7�b��ύX�V					*/
	}

	if(glbCtVacuumEmergncyTimer == 0)					/* �^��ꎞ��~���łȂ��ꍇ�̂ݐ^�����ٓ��B����ѱ�Ď��Ԃ��X�V����			*/
	{
		if(glbCtVacuumTimeout1 != 0)						/* �^��x��ѱ�ĊĎ����H			*/
		{
			glbCtVacuumTimeout1--;							/* ��ύX�V						*/
		}
		if(glbCtVacuumTimeout2 != 0)						/* �^��x��ѱ�ĊĎ����H			*/
		{
			glbCtVacuumTimeout2--;							/* ��ύX�V						*/
		}
		if(glbCtVacuumTimeout3 != 0)						/* �^��x���Ď����H			*/
		{
			glbCtVacuumTimeout3--;							/* ��ύX�V						*/
		}
	}
	if(glbCtBuzzerOnTimer != 0)							/* �޻ް�炵��?				*/
	{
		glbCtBuzzerOnTimer--;								/* �޻ްON��ύX�V				*/
		if(glbCtBuzzerOnTimer == 0)						/* �޻ްON���=0?				*/
		{
			R_TAU0_Channel2_Stop();						/* �޻ް��~					*/
		}
	}

	if(glbCtBuzzerOffTimer != 0)						/* �޻ްOFF��?				*/
	{
		glbCtBuzzerOffTimer--;								/* �޻ްOFF��ύX�V				*/
	}

	if(glbCtWaitTimer != 0)								/* �w�莞�Ԃ̳��ĊĎ���?			*/
	{
		glbCtWaitTimer--;		
		
		
		
		/* ��ύX�V							*/
	}
	
	if(glbCtWaitTimerT5 != 0)							/* T5:�\��^�]��WAIT���ԊĎ����H*/
	{
		glbCtWaitTimerT5--;								/* ��ύX�V						*/
	}
	

	if(glbCtWaitTimerE6Cansel != 0)						/* �d����������T5���Ԃ�[E6]�װ���o���Ȃ��Ď���?*/
	{
		glbCtWaitTimerE6Cansel--;						/* ��ύX�V						*/
	}
	
	if(glbCtPackCountDisplayTime != 0)
	{
		glbCtPackCountDisplayTime--;
	}
	
	
}	

uint8_t stiGuruGuruFlag = 0;
UBYTE Data_GuruGuru7Seg[2];

void powerOn_GuruGuru7Seg(void)
{
	const UBYTE   stiDtGuruGuru7Seg[8][2] =
	{
		{0b0110000,	0b0},
		{0b0100001,	0b0},
		{0b0000001,	0b0000001},
		{0b0,	0b0000011},
		{0b0,	0b0000110},
		{0b0,	0b0001100},
		{0b0001000,	0b0001000},
		{0b0011000,	0b0000000}	
	};
	
	uint16_t countData;
	uint8_t i;
	
	stiGuruGuruFlag = 1;

	/*�N�������邮��*/
	Data_GuruGuru7Seg[1] = stiDtGuruGuru7Seg[0][0];
	Data_GuruGuru7Seg[0] = stiDtGuruGuru7Seg[0][1];
	glbCtPackCountDisplayTime =  600;	//3000ms
	

	countData = glbCtPackCountDisplayTime;
	while(glbCtPackCountDisplayTime)
	{
		if((countData-glbCtPackCountDisplayTime)>10)
		{
			countData = glbCtPackCountDisplayTime;
			i++;
			if(i>7)i=0;
			Data_GuruGuru7Seg[1] = stiDtGuruGuru7Seg[i][0];
			Data_GuruGuru7Seg[0] = stiDtGuruGuru7Seg[i][1];
			
		}
		
		NOP();
	}
	
	stiGuruGuruFlag = 0;
}


/***********************************************************************************************************************
*    �֐���    stiInKey
*    ��  ��    
*    �߂�l    
*    �֐��@�\  SW���͂̊Ď�
*    �������e  �y����iSW1)�z�A�y�H��(SW2)�z�A�y��(SW3)�z�A�y��(SW4)�z�A�ySTART/STOP(SW5)�z�A�y�^���~(SW6)�z
               �yPUSH�iSW7)�z��7��SW���͂�ACTIVE�ω����Ď�
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void stiInKey(void)
{
	UWORD	postd = 0;												
	UBYTE	keydt;												

	keydt = iodInKey();				 				/* ���ް����́iL�̎���KEY ON)					    */
	if( keydt != stiFlKydt)							/* �O��Ɠ��͒l���قȂ�ꍇ							*/
	{

		stiCtKey++;									/* SW ON �A�������X�V								*/
		if(stiCtKey <= 14)							/*70ms�ȏ�A���I�ɷ��������ꂽ�ꍇ�A�L���Ƃ���	*/
		{
			return;
		}
		stiCtKey = 0;								/* �A��SW ON�����ر					*/
		keydt = ~keydt;								/* SW�ް����]						*/
		if((keydt & P21_KEY_SW1_COURSE_OFF))		/* ����iSW1�j�̏ꍇ					*/
		{											
			postd |= LCN_KEY_SW1_COURSE_SET;				
		}											
		else if((keydt & P22_KEY_SW2_PROCESS_OFF))	/* �H��(SW2)�̏ꍇ					*/
		{											
			postd |= LCN_KEY_SW2_PROC_SET;				
		}											
		else if((keydt & P23_KEY_SW3_UP_OFF))		/* ���iSW3)�̏ꍇ					*/
		{											
			postd |= LCN_KEY_SW3_UP_SET;			/* �������							*/
			if((keydt & P24_KEY_SW4_DOWN_OFF))		/* �X�Ɂ����������ɉ�����Ă���ꍇ	*/
			{
				postd |= LCN_KEY_SW4_DOWN_SET;		/* �������������					*/
			}
		}											
		else if((keydt & P24_KEY_SW4_DOWN_OFF))		/* ��(SW4)�̏ꍇ					*/
		{
			postd |= LCN_KEY_SW4_DOWN_SET;				
		}
		else if((keydt & P25_KEY_SW5_START_OFF ))	/* START/STOP(SW5)�̏ꍇ			*/
		{
			postd |= LCN_KEY_SW5_START_SET;				
		}
		else if((keydt & P26_KEY_SW6_STOP_OFF))		/* �^���~(SW6)�̏ꍇ				*/
		{
			postd |= LCN_KEY_SW6_STOP_SET;				
		}
		else if((keydt & P27_KEY_SW7_PUSH_OFF))		/* PUSH�i�B��SW7)�̏ꍇ				*/
		{
			postd |= LCN_KEY_SW7_PUSH_SET;				
		}
		keydt = ~keydt;								/* SW�ް����]		*/
		stiFlKydt = keydt;							/* ���ް�����		*/
		if(stiFlKydt != 0xff)						/* �ǂꂩ��SW��������Ă�ꍇ		*/
		{
			if(glbFlKydt == 0)						/* Ҳݑ��őO���SW���͊����ς̏ꍇ	*/
			{
				glbFlKydt = postd;					/* Ҳ݂֒ʒm����	*/
			}
		}
	}
}


/***********************************************************************************************************************
*    �֐���    stiChkLEDCOM
*    ��  ��    
*    �߂�l    
*    �֐��@�\  LEDCOM�P�`4�̐���
*    �������e  LEDCOM1:7SEG_2���ځALEDCOM2:7SEG_1���ځALEDCOM3:�H��LED [�^��][���][��p]
*              LEDCOM4:���LED [���1][���2][���3][���4]
			   Ҳ�ٰ�݂Őݒ肵��LED�������I��LEDCOMx�߰Ăɏo��
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void stiChkLEDCOM(void)
{
	UBYTE	i,data=0;
	volatile UBYTE	*pIO;


	P1 = 0x00;										/* �o���ް�ALL OFF			*/
	for(i=0;i<4;i++)								/* LEDCOM1�`4 ALL OFF		*/
	{
		pIO   = LedCom[i].pIO;						/* �w���LCDCOMx���ڽذ��	*/
		*pIO  |= LedCom[i].OFF;						/* LEDCOMx <- OFF			*/
	}
	pIO  = LedCom[glbFlLedCom].pIO;					/* �w���LCDCOMx���ڽذ��	*/
	*pIO &= LedCom[glbFlLedCom].ON;					/* ��L���߰Ă�LEDCOM ON (Active Low)	*/

	if(glbFlLedCom == 0)							/* LEDCOM1:7Seg��2���ڕ\���v��		*/
	{
		if(glbFlSysytem7Seg  == 1)					/* ����Ӱ�ނ̏ꍇ							*/
		{
			P1 = glbDt7Seg2;						/* 1SEG�݂̂̏o�͂Ƃ���				*/
		}
		else
		{
			if(!stiGuruGuruFlag)
			{
				data  = (glbFlReqDot == 1)?0x80:0x00;	/* ��ص�ޕ\���v������		*/
				data  |= stiDt7Seg[glbDt7Seg2];			/* �\����������ص�ލ���		*/
			}
			else
			{
				data = Data_GuruGuru7Seg[1];
				
			}
			
			P1    = data;			 				/* �\��������7SEG�o���ް��ɕϊ������߰ďo��	*/

		}
	}
	else if(glbFlLedCom == 1)							/* LEDCOM2:7Seg��1���ڕ\���v��		*/
	{
		if(glbFlSysytem7Seg  == 1)						/* ����Ӱ�ނ̏ꍇ							*/
		{
			P1 = glbDt7Seg1;							/* 1SEG�݂̂̏o�͂Ƃ���				*/
		}
		else
		{
			if(!stiGuruGuruFlag)
			{
				data  = (glbFlReqDot == 2)?0x80:0x00;	/* ��ص�ޕ\���v������		*/
				data  |= stiDt7Seg[glbDt7Seg1];			/* �\����������ص�ލ���		*/
			}
			else
			{
				data = Data_GuruGuru7Seg[0];
			}
			
			P1 = data;			 					/* �\��������7SEG�o���ް��ɕϊ������߰ďo��	*/
		}
	}
	else if(glbFlLedCom == 2)							/* LEDCOM3:�H��LED�̕\���v��	*/
	{
		if(glbFlSysytem7Seg  == 1)						/* ����Ӱ�ނ̏ꍇ							*/
		{
			if((stiCheckPumpthermal()))					/* ����߻��وُ�̏ꍇ			*/
			{
				stiCtFlashing2++;						/* 500ms�����X�V						*/
				if(stiCtFlashing2 > LCN_LCD_FLASH_CNT)	/* 500ms�o��							*/
				{
					stiCtFlashing2 = 0;					/* 500ms��������						*/
					stiFlFlashing2++;					/* ĸ�ٍX�V								*/
					stiFlFlashing2 &= 1;				/* �_�Őؑ�ĸ���׸�						*/
				}
				if(stiFlFlashing2)						/* �_�����ݸނ̏ꍇ						*/
				{
					P1 |= stiLedOut_Com2;			/* �w��̍H��LED�_��				*/
				}
				else									/* �������ݸނ̏ꍇ					*/
				{
					P1  &= ~0x07;	/* 3�̍H��LED����					*/
				}
			}
			else
			{
				P1 |= stiLedOut_Com2;			/* �w��̍H��LED�_��				*/
			}
		}
		else
		{
			if(glbFlMainMode == GCN_WARM_MODE)			/* �g�@�^�]���́u�^��vLED��_�ł�����	*/
			{
				stiCtFlashing2++;						/* 500ms�����X�V						*/
				if(stiCtFlashing2 > LCN_LCD_FLASH_CNT)	/* 500ms�o��							*/
				{
					stiCtFlashing2 = 0;					/* 500ms��������						*/
					stiFlFlashing2++;					/* ĸ�ٍX�V								*/
					stiFlFlashing2 &= 1;				/* �_�Őؑ�ĸ���׸�						*/
				}
				if(stiFlFlashing2)
				{
					P1 |= stiLedOut_Com2;			/* �w��̍H��LED�_��				*/
				}
				else
				{
					P1  = (stiLedOut_Com2 & 0xfe);
				}
			}
			else
			{
				P1 = stiLedOut_Com2;				/* �w��̍H��LED�\��			*/
			}
		}
	
	}
	else if(glbFlLedCom == 3)							/* LEDCOM4:���LED�Ɓu�b�vor �u%�v�\���v��	*/
	{
		/* courseLed */
		if(glbFlAdjust == 1)							/* ý�Ӱ�ނ̱�۸ޒ���Ӱ�ނ̏ꍇ				*/
		{
			P1  = stiDtBit[8];							/* LED1�`LED4,LED8,LED9����					*/
		}
		else
		{
			if(DrawerSwRead())							/* ���o��SW���u�J�v�̏ꍇ�́A�_��	*/
			{
				stiCtFlashing++;						/* 500ms�����X�V						*/
				if(stiCtFlashing > LCN_LCD_FLASH_CNT)	/* 500ms�o��							*/
				{
					stiCtFlashing = 0;					/* 500ms��������						*/
					stiFlFlashing++;					/* ĸ�ٍX�V								*/
					stiFlFlashing &= 1;					/* �_�Őؑ�ĸ���׸�						*/
				}
				if(stiFlFlashing)						/* �_�����ݸނ̏ꍇ						*/
				{
					P1  |= stiLedOut_Com3;		/* �w��̺��LED�_��						*/
				}
				else									/* �������ݸނ̏ꍇ						*/
				{
					P1  &= ~stiLedOut_Com3;
				}
			}
			else
			{
				P1  |= stiLedOut_Com3;		/* �w��̺��LED�_��						*/
			}
		}

		/* Sec/% */
		P1 |= stiLedOut_Com3_Sec;	
		
		/* spear */
		if(glbFlLed11 == 1)								/* LED11�_���v��?				*/
		{
			P1 |= 0x80;									/* �u�b & %�v�_��	*/
		}
		
		/* cycleModeLed */
		if(glbFlSystemMode  == 0)						/* ����Ӱ�ވȊO�̏ꍇ							*/
		{
			if( (glbFlContinue == 1)&&(!glbFlStanby) )						/* �A���^�]Ӱ��?			*/
			{
				P1 |= 0x40;								/* LED10�_���i�A��Ӱ�� ON) 	*/
			}
			else										
			{
				P1 &= ~0x40;							/* LED10�����i�A��Ӱ�� OFF) 	*/
			}
		}
		else
		{
			 if(glbFlContinue == 1)
			 {
				 P1 |= 0x40;								/* LED10�_���i�A��Ӱ�� ON) 	*/	 
			 }
		}

	}
	


	glbFlLedCom++;										/* LEDCOMx�o�Ͷ����X�V		*/
	glbFlLedCom &= 0x03;								/* �������					*/
}


/***********************************************************************************************************************
*    �֐���    iodInKey
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �V�̃L�[SW����
*    �������e  �M�����ق�L��KEY�@ON�ƂȂ�B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	iodInKey(void)
{
	UBYTE	keydt1,keydt2;

	keydt1 = P2;
	keydt2 = P2;
	if(keydt1 != keydt2)
	{
		keydt1 = P2;
	}
	keydt1 |= 0x01;									/* �ޯ0�͖��g�p�̂���KEY���͖����i1)�Ƃ���	*/
	return(keydt1);
}

/***********************************************************************************************************************
*    �֐���    stiReadSamplingADC
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��xAD�l�̕��ω�
*    �������e  5ms����AD�l����͂��A�ŏ��l�ƍő�l����菜���A�c��Q��AD�l�̕��ϒl���Z�o
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	stiReadSamplingADC(void)
{
	UWORD	adc,min1,min2,mid1,mid2,max1,max2,adave,i;
	
	adc = R_ADC_Get_Result2();								/* AD�l���́@ANI18					*/


	for (i = 0; i < 3; i++){								/* AD�l۰ٱ���				*/
		stiBfAdc[i] = stiBfAdc[i + 1];						
	}															
	stiBfAdc[3] = adc;										/* �ŐVAD�l���				*/
	if (stiBfAdc[0] < stiBfAdc[1]){							
		min1 = stiBfAdc[0];									
		max1 = stiBfAdc[1];									
	} else {													
		max1 = stiBfAdc[0];									
		min1 = stiBfAdc[1];									
	}															
	if (stiBfAdc[2] < stiBfAdc[3]){							
		min2 = stiBfAdc[2];									
		max2 = stiBfAdc[3];									
	} else {													
		max2 = stiBfAdc[2];									
		min2 = stiBfAdc[3];									
	}															
																
	if (max1 < max2){											
		mid1 = max1;											
	}
	else {														
		mid1 = max2;											
	}
	if (min1 < min2){											
		mid2 = min2;											
	}
	else {														
		mid2 = min1;											
	}															
	adave = (UWORD)mid1 + (UWORD)mid2;						
	adave >>= 1;											
	glbCtRealAD = adave;
}

/***********************************************************************************************************************
*    �֐���    stiInitialSamplingADC
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �^��AD�l�̏�����
*    �������e  ���݂�AD�l����͂��ĂS�̃o�b�t�@�ɓW�J
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	stiInitialSamplingADC(void)
{
	UWORD	i,adc;

	adc = R_ADC_Get_Result2();									/* AD�l���́@ANI18					*/
	for(i=0;i<4;++i)
	{
		stiBfAdc[i] = adc;										/* �ŐVAD�l���				*/
	}
	glbCtRealAD = adc;
}

/***********************************************************************************************************************
*    �֐���    stiCheckPump
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ����߉ғ����Ԃ̐ώZ
*    �������e  1���P�ʂ�����߉ғ����Ԃ�ώZ
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	stiCheckPump(void)													
{

	if(P7 & P74_PUMP_START_ON)												/* ����߉ғ���				*/ 
	{
		stiCtMinute++;														/* �������X�V				*/
		if(stiCtMinute > GCN_TIME_1MN)										/* 1���o�߁H				*/
		{
			geefCtPumpRunning++;											/* ����߉ғ����ԍX�V�i1���P��)	*/
			if(geefCtPumpRunning >(unsigned long int) GCN_TIME_9999HR)		/* 9999���Ԃ𒴂����ꍇ			*/
			{
				geefCtPumpRunning = (unsigned long int) GCN_TIME_9999HR	;	/* 9999���Ԃ�����Ƃ���			*/
			}
			stiCtMinute = 0;												/* 1����ϰؾ��					*/
		}
	}
}


/***********************************************************************************************************************
*    �֐���    stiCheckPumpthermal
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ����߂̏�Ԍ��o
*    �������e  0:����,1:�ُ�
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	stiCheckPumpthermal(void)						/* ����߻��ق̴װ�Ď�		*/
{
	volatile UBYTE	in1,in2;

	in1 = P14;
	in2 = P14;
	if(in1 != in2)
	{
		in1 = P14;
	}
	if(!(in1 & P146_PUMP_THERMAL))
	{
		return(1);
	}
	return(0);
}


/*============================================
* �d��SW���͏���		5ms�X�V
*===========================================*/
void stiPowerSwCheck(void)	
{
	if(P12_bit.no4 != 1)
	{
		glbCntPowerSwOffChk = 0;
		
		glbCntPowerSwOnChk++;
		if( glbCntPowerSwOnChk >= 10)
		{
			glbCntPowerSwOnChk = 10;
			
			glbFlPowerSw = 1;
			
			stiPowerSwLong();
		}
	}
	else
	{
		glbCntPowerSwOnChk = 0;
		
		
		glbCntPowerSwOffChk++;

		if(glbCntPowerSwOffChk >= 10)
		{
			glbCntPowerSwOffChk = 10;
			
			glbFlPowerSw = 0;
			
			glbCntPowerSwLongChk = 0;
			glbFlPowerSwLongPalse = 0;
			glbFlPowerSwLong = 0;

		}
	}
	
	
	
}

/*============================================
* �d��SW���͒���������		5ms�X�V
*===========================================*/
void stiPowerSwLong(void)	
{
	glbCntPowerSwLongChk++;
	if(glbCntPowerSwLongChk >=  400)
	{
		glbCntPowerSwLongChk = 400;

		if(glbFlPowerSwLong != 1)
		{
			glbFlPowerSwLongPalse = 1;
		}
		glbFlPowerSwLong = 1;

	}

}

/*============================================
* �d��LED�o�͐���
*	[ctl]0:OFF,1:ON,2:BLINK
*	[onTime]:on time when blinking (*.*s)
*	[offTime]:off time when blinking(*.*s)
*===========================================*/
void powerLedControl(uint8_t ctl,uint8_t onTime,uint8_t offTime )
{
	switch(ctl)
	{
		case 0:
			glbPowerLed = 0;
			break;
		
		case 1:
			glbPowerLed = 1;
			break;
		
		case 2:
			glbPowerLed = 2;
			glbPowerLedBlinkTogle = 0;
			glbCntPowerLedBlink_onTime = glbCntSetPowerLedBlink_onTime = onTime*20; 
			glbCntSetPowerLedBlink_offTime = offTime*20;
			glbCntPowerLedBlink_offTime = 0;
			break;
			
		default:
			break;
	}
}

/*============================================
* �d��LED�o�͐���
*	[ctl]0:OFF,1:ON,2:BLINK
*	[onTime]:on time when blinking
*	[offTime]:off time when blinking
*===========================================*/
void stiPowerLedOut(void)
{
	switch(glbPowerLed)
	{
		case 0:
			P7 &= P72_POWER_LED_OFF;
			break;
		
		case 1:
			P7 |= P72_POWER_LED_ON;
			break;
		
		case 2:
			if(!glbPowerLedBlinkTogle)
			{
				glbCntPowerLedBlink_onTime--;
				if(!glbCntPowerLedBlink_onTime)
				{
					glbPowerLedBlinkTogle = 1;
					P7 &= P72_POWER_LED_OFF;
					glbCntPowerLedBlink_offTime = glbCntSetPowerLedBlink_offTime;
				}
			}
			else
			{
				glbCntPowerLedBlink_offTime--;
				if(!glbCntPowerLedBlink_offTime)
				{
					glbPowerLedBlinkTogle = 0;
					P7 |= P72_POWER_LED_ON;
					glbCntPowerLedBlink_onTime = glbCntSetPowerLedBlink_onTime;
				}	
			}
			
			break;
			
		default:
			break;
	}
}



/*============================================
*�@�R�[�XLed����
*   [course]0:1���,1:2���,2:3���,3:4���,0xFF:�S��
*	[ctl]0:OFF,1:ON
*	[mode]0:�A��,1:�ʐ���
*===========================================*/
void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode)
{	
	if(ctl == 0)	//����
	{
		if(course != 0xFF)
		{
			stiLedOut_Com3 &= ~stiDtBit[course];
		}
		else
		{
			stiLedOut_Com3 &= ~0xF;
		}
	}
	else			//�_��
	{
		if(course != 0xFF)
		{
			if(!mode)
			{
				stiLedOut_Com3 = stiDtBit[course];
			}
			else
			{
				stiLedOut_Com3 |= stiDtBit[course];
			}
		}
		else
		{
			stiLedOut_Com3 = 0xF;
		}
	}
}


/*============================================
*�@�H��Led����
*   [proc]0:1���,1:2���,2:3���,3:4���,0xFF:�S��
*	[ctl]0:OFF,1:ON
*	[mode]0:�A��,1:�ʐ���
*===========================================*/
void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode)
{	

	if(ctl == 0)	//����
	{
		if(proc != 0xFF)
		{
			stiLedOut_Com2 &= ~stiDtBit[proc];
		}
		else
		{
			stiLedOut_Com2 &= ~0x7;
		}
	}
	else			//�_��
	{
		if(proc != 0xFF)
		{
			if(!mode)
			{
				stiLedOut_Com2 = stiDtBit[proc];
			}
			else
			{
				stiLedOut_Com2 |= stiDtBit[proc];
			}
		}
		else
		{
			stiLedOut_Com2 = 0x7;
		}
	}

}

/*============================================
*�@�b/��Led����
*   [toggle]0:�b,1:��
*	[ctl]0:OFF,1:ON
*	[mode]0:�A��,1:�ʐ���
*===========================================*/
void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode)
{	
	if(ctl == 0)	//����
	{
		if(toggle != 0xFF)
		{
			stiLedOut_Com3_Sec &= ~stiDtBit[4+toggle];
		}
		else
		{
			stiLedOut_Com3_Sec &= ~0x30;
		}
	}
	else			//�_��
	{
		if(toggle != 0xFF)
		{
			if(!mode)
			{
				stiLedOut_Com3_Sec = stiDtBit[4+toggle];
			}
			else
			{
				stiLedOut_Com3_Sec |= stiDtBit[4+toggle];
			}
		}
		else
		{
			stiLedOut_Com3_Sec = 0x30;
		}
	}
}