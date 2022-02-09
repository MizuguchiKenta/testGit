#include "r_cg_macrodriver.h"

#define	UBYTE	unsigned char
#define	SBYTE			 char
#define	UWORD	unsigned int
#define	SWORD			 int

#define	GCN_ERROR_PUMP_THERMAL_0	0x01				/* �|���v�T�[�}���g���b�v�̌��o	*/
#define	GCN_ERROR_MASTER_5			0x02				/* �e�@�װ						*/
#define	GCN_ERROR_VACUUM_LEVEL_2	0x04				/* �^��G���[					*/
														/* �^������J�n����Q�O�b�o�ߌ�A�^�󃌃x�����P�O���ɒB���Ă��Ȃ��ꍇ	*/

#define	GCN_ERROR_VACUUM_WARNING_3	0x08				/* �^��x��						*/
														/* �^������J�n����R�O�b�o�ߌ�A�^�󃌃x�����X�O���ɒB���Ă��Ȃ��ꍇ	*/

#define	GCN_ERROR_VACUUM_ADC_6		0x10				/* �^��Z���T�[�G���[			*/
														/* �P�O�r�b�g�`�c�f�[�^�̒l���P�O������������ꍇ						*/

#define	GCN_ERROR_R78_CPU_7			0x20				/* �b�o�t�G���[					*/
														/* ��O�����ݑ�															*/
#define	GCN_ERROR_WATCHDOG_CPU_8	0x40				/* �v�c�n�f�G���[					*/
														/* �v�c�n�f�^�C���A�b�v													*/
#define	GCN_ERROR_FLASH_DATA_9		0x80				/* �f�[�^�G���[						*/
														/* �ۑ����Ă����f�[�^���j�����Ă���										*/


#define	GCN_WAIT_MODE	0		/* �ҋ@Ӱ��			*/
#define	GCN_COURSE_MODE	1		/* ����ݒ�Ӱ��		*/
#define	GCN_WARM_MODE	2		/* �g�C�^�]Ӱ��		*/
#define	GCN_DRIVE_MODE	3		/* �^�]Ӱ��			*/
#define	GCN_ERROR_MODE	4		/* �װ���oӰ��		*/
#define	GCN_STANBY_MODE	5		/* ��Ӱ��			*/
#define	GCN_SYSTEM_MODE	6		/* �����ް��ݒ�Ӱ��	*/

#define	GCN_PROC_VACUUM	0		/* �^��H��			*/
#define	GCN_PROC_SEEL	1		// ��ٍH��
#define	GCN_PROC_COOL	2		// ��p�H��

#define	GCN_KEY_COURSE_SET		1			/* �����		*/
#define	GCN_KEY_PROC_SET		2			/* �H����		*/
#define	GCN_KEY_UP_SET			3			/* ����			*/
#define	GCN_KEY_DOWN_SET		4			/* ����			*/
#define	GCN_KEY_START_SET		5			/* ���ķ�		*/
#define	GCN_KEY_STOP_SET		6			/* �į�߷�(�B���j*/
#define	GCN_KEY_PUSH_SET		7			/* PUSH��		*/
#define	GCN_KEY_UPDOWN_SET		8			/* ����������	*/

#define	GCN_SYSTEM_VACUUM				0
#define	GCN_SYSTEM_WARM					1

#define	GCN_SYSTEM_VACUUM_TIMEOUT		2
#define	GCN_SYSTEM_SPAR					3
#define	GCN_SYSTEM_DRIVE_AFTER_WAIT		4

#define	GCN_VACUUM_SECONDS			0
#define	GCN_VACUUM_LEVEL			1
#define	GCN_VACUUM_LEDALLOFF		2
#define	GCN_VACUUM_LEDALLON			3

#define	GCN_INT_TIME				5											/* 5ms							*/
#define	GCN_TIME_1S					1000										/* 1000ms = 1S					*/
#define	GCN_TIME_4_9S				4900										/* 4900ms = 4.9S				*/
#define	GCN_TIME_5S					5000										/* 5000ms = 5S					*/
#define	GCN_TIME_10S				10000										/* 10000ms = 10S					*/
#define	GCN_TIME_20S				20000										/* 20000ms = 20S					*/
#define	GCN_TIME_30S				30000										/* 30000ms = 30S					*/
#define	GCN_TIME_100MS				100											/* 100ms  = 0.1S				*/
#define	GCN_TIME_500MS				500											/* 500ms  = 0.1S				*/
#define	GCN_TIME_700MS				700											/* 700ms  = 0.7S				*/
#define	GCN_TIME_1MN				12000                                       /* (GCN_TIME_1S * 60 * 1)/GCN_INT_TIME;		 1��*/
#define	GCN_TIME_3MN				36000                                       /* (GCN_TIME_1S * 60 * 3)/GCN_INT_TIME;		 3��*/
#define	GCN_TIME_9999HR				9999 * 60                                   /* 9999���Ԃ𕪂ɕϊ������l						*/

#define	GCN_TIMER_100MS				GCN_TIME_100MS/GCN_INT_TIME					/* 100ms��5ms�P�ʂɊ��Z			*/
#define	GCN_TIMER_500MS				GCN_TIME_500MS/GCN_INT_TIME				/* 500ms						*/
#define	GCN_TIMER_700MS				GCN_TIME_700MS/GCN_INT_TIME					/* 700ms��5ms�P�ʂɊ��Z			*/
#define	GCN_TIMER_1S				(GCN_TIME_1S/GCN_INT_TIME)					/* 1000ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_2S				(2*(GCN_TIME_1S/GCN_INT_TIME))				/* 2000ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_5S				(GCN_TIME_5S/GCN_INT_TIME))					/* 5000ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_4_9S				(GCN_TIME_4_9S/GCN_INT_TIME)				/* 4900ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_10S				(GCN_TIME_10S/GCN_INT_TIME)					/* 10000ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_20S				(GCN_TIME_20S/GCN_INT_TIME)					/* 20000ms��5ms�P�ʂɊ��Z		*/
#define	GCN_TIMER_30S				(GCN_TIME_30S/GCN_INT_TIME)					/* 20000ms��5ms�P�ʂɊ��Z		*/

#define	GCN_DRIVE_START_MODE		0							/* �^�]�J�n��					*/
#define	GCN_VACUUM_PULL_MODE		1							/* �^������H��					*/
#define	GCN_GUS_INCLUSIOM_MODE		2							/* �޽�����H��					*/
#define	GCN_GUS_STUBILITY_MODE		3							/* �޽����҂��H��				*/
#define	GCN_SEEL_MODE				4							/* ��ٍH��						*/
#define	GCN_SEEL_COOLING_MODE		5							/* ��ٗ�p�H���i�\�t�g�J���H���j*/
#define	GCN_VACUUM_STOP_MODE		6							/* �^������ꎞ��~Ӱ��			*/
#define	GCN_HEATER_BLOCK_MODE		7							/* ˰���ۯ��㏸�҂������i�O�D�V�b�j*/
#define	GCN_DRIVE_END_MODE			8							/* �H���I��						*/

#define	P20_VACUUM_ELECTROMAG_ON	0x01						/* �^��d����ON(H->Act	)	bit0*/
#define	P20_VACUUM_ELECTROMAG_OFF	~P20_VACUUM_ELECTROMAG_ON	/* �^��d����OFF(L->��Act)	bit0*/
#define	P130_GUS_ELECTROMAG_ON		0x01						/* �޽�d����ON(H->Act	)	bit1*/
#define	P130_GUS_ELECTROMAG_OFF		~P130_GUS_ELECTROMAG_ON		/* �޽�d����OFF(L->��Act)	bit1*/
#define	P01_SEEL_ELECTROMAG_ON		0x02						/* ��ٓd����ON(H->Act	)	bit2*/
#define	P01_SEEL_ELECTROMAG_OFF		~P01_SEEL_ELECTROMAG_ON		/* ��ٓd����OFF(L->��Act)	bit2*/
#define	P41_VACUUM_RELEASE_ON		0x02						/* �^������ON �iH->Act)	bit3*/
#define	P41_VACUUM_RELEASE_OFF		~P41_VACUUM_RELEASE_ON		/* �^������OFF�iL->��Act) bit3*/

#define	P72_POWER_LED_ON			0x04						/* �d���ƌ�SW	(H->Act)	bit2*/
#define	P72_POWER_LED_OFF			~P72_POWER_LED_ON			/* �d���ƌ�SW	(L->NonAct)	bit2*/
#define	P73_HEATER_START_ON			0x08						/* ˰�����	(H->Act)	bit3*/
#define	P73_HEATER_START_OFF		~P73_HEATER_START_ON		/* ˰��į��	(L->NonAct)	bit3*/
#define	P74_PUMP_START_ON			0x10						/* ����߉ғ��iH->Act)	bit4*/
#define	P74_PUMP_START_OFF			~P74_PUMP_START_ON			/* ����ߒ�~�iL->NonAct)bit4*/
#define	P75_SOFTRELEASE_START_ON	0x20						/* ��ĉ���iH->Act)		bit5*/
#define	P75_SOFTRELEASE_START_OFF	~P75_SOFTRELEASE_START_ON	/* ��Ĕ����iL->NonAct)bit5*/

#define	P50_LEDCOM1_OFF				0x01						/* LEDCOM1 OFF (H->��Act)	bit0	*/
#define	P50_LEDCOM1_ON				~P50_LEDCOM1_OFF			/* LEDCOM1 ON  (L->Act)		bit0	*/
#define	P51_LEDCOM2_OFF				0x02						/* LEDCOM2 OFF (H->��Act)	bit1	*/
#define	P51_LEDCOM2_ON				~P51_LEDCOM2_OFF			/* LEDCOM2 ON  (L->Act)		bit1	*/
#define	P30_LEDCOM3_OFF				0x01						/* LEDCOM3 OFF (H->��Act)	bit0	*/
#define	P30_LEDCOM3_ON				~P30_LEDCOM3_OFF			/* LEDCOM3 ON  (L->Act)		bit0	*/
#define	P70_LEDCOM4_OFF				0x01						/* LEDCOM4 OFF (H->��Act)	bit0	*/
#define	P70_LEDCOM4_ON				~P70_LEDCOM4_OFF			/* LEDCOM4 ON  (L->Act)		bit0	*/

#define	P137_DRAWER_SW				0x80						/* ���o��SW�@�iL->�AH->�J)*/
#define	P146_PUMP_THERMAL			0x40						/* ����߻��فiL���ُ�ߔM	)*/

#if 0
//#define	P22_KEY_SW1_COURSE_OFF		0x04						
//#define	P22_KEY_SW1_COURSE_ON		~P22_KEY_SW1_COURSE_OFF
//#define	P23_KEY_SW2_PROCESS_OFF 	0x08
//#define	P23_KEY_SW2_PROCESS_ON	 	~P23_KEY_SW2_PROCESS_OFF
//#define	P24_KEY_SW3_UP_OFF			0x10
//#define	P24_KEY_SW3_UP_ON			~P24_KEY_SW3_UP_OFF
//#define	P25_KEY_SW4_DOWN_OFF		0x20
//#define	P25_KEY_SW4_DOWN_ON			~P25_KEY_SW4_DOWN_OFF
//#define	P26_KEY_SW5_START_OFF		0x40
//#define	P26_KEY_SW5_START_ON		~P26_KEY_SW5_START_OFF

//#define	P27_KEY_SW6_STOP_OFF		0x80
//#define	P27_KEY_SW6_STOP_ON			~P27_KEY_SW6_STOP_OFF
//#define	P21_KEY_SW7_PUSH_OFF		0x02
//#define	P21_KEY_SW7_PUSH_ON			~P21_KEY_SW7_PUSH_OFF
#endif

#define	P21_KEY_SW1_COURSE_OFF		0x02						
#define	P21_KEY_SW1_COURSE_ON		~P21_KEY_SW1_COURSE_OFF
#define	P22_KEY_SW2_PROCESS_OFF 	0x04
#define	P22_KEY_SW2_PROCESS_ON	 	~P22_KEY_SW2_PROCESS_OFF
#define	P23_KEY_SW3_UP_OFF			0x08
#define	P23_KEY_SW3_UP_ON			~P23_KEY_SW3_UP_OFF
#define	P24_KEY_SW4_DOWN_OFF		0x10
#define	P24_KEY_SW4_DOWN_ON			~P24_KEY_SW4_DOWN_OFF
#define	P25_KEY_SW5_START_OFF		0x20
#define	P25_KEY_SW5_START_ON		~P25_KEY_SW5_START_OFF
#define	P26_KEY_SW6_STOP_OFF		0x40
#define	P26_KEY_SW6_STOP_ON			~P26_KEY_SW6_STOP_OFF
#define	P27_KEY_SW7_PUSH_OFF		0x80
#define	P27_KEY_SW7_PUSH_ON			~P27_KEY_SW7_PUSH_OFF



#define	GCN_7SEG_HAIPHONG			16
#define	GCN_7SEG_ALLOFF				17
#define	GCN_7SEG_ALLON				18

#define	GCN_BASE_AD					110								/* �AD�l							*/
#define	GCN_NORMAL_LEVEL			724								/* �W����C���̎���AD�l				*/

#define ALL_LED_SELECT				0xFF