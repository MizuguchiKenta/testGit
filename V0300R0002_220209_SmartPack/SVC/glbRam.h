#include	"common.h"

#if 0
struct 	tagCOURSE {
	UBYTE	Vacuum;				/* �^�󎞊�				(1�b�P��)	*/
	UBYTE	Seel;				/* ��َ���				(0.1�b�P�ʁj*/
	UBYTE	SeelCooling;		/* ��ٗ�p����			(0.1�b�P��)	*/
	UBYTE	Gus;				/* �޽����				(0.1�b�P��)	*/
	UBYTE	GusStability;		/* �޽���莞��			(0.1�b�P��)	*/
	UBYTE	SoftRelease;		/* ��ĉ������			(0.1�b�P��)	*/
	UBYTE	VacuumLevel;		/* �^�󓞒B����			(1%�P��)	*/
	
};
#endif

enum	PROSESS
{
	Vacuum,				/* �^�󎞊�				(1�b�P��)	*/
	Seel,				/* ��َ���				(0.1�b�P�ʁj*/
	SeelCooling,		/* ��ٗ�p����			(0.1�b�P��)	*/
	Gus,				/* �޽����				(0.1�b�P��)	*/
	GusStability,		/* �޽���莞��			(0.1�b�P��)	*/
	GusLevel,			/* �޽����				(0.1�b�P��)	*/
	SoftRelease,		/* ��ĉ������			(0.1�b�P��)	*/
	VacuumLevel			/* �^�󓞒B����			(1%�P��)	*/
};

struct tagSYSTEMMAXMIN	{
	UWORD	Min;				/* MIN.					*/
	UWORD	Max;				/* MAX.					*/
};

struct tagLEDCOM	{
	volatile UBYTE	*pIO;				/* LEDCOMx�o�ͱ��ڽ	*/
	UBYTE	ON;					/* LED�o���ް�		*/
	UBYTE	OFF;				/* LED�o���ް�		*/
};

struct tagSYSTEM	{
	UWORD	Default;			/* �����l				*/
	UWORD	Min;				/* MIN.					*/
	UWORD	Max;				/* MAX.					*/
};

enum	SYSTEM
{
	Default,					/* �����l				*/
	Min,						/* MIN.					*/
	Max,						/* MAX.					*/
};

struct tagOUTPUTTEST	{
	volatile UBYTE	*pIO;				/* �o�͋@����ڽ	*/
	UBYTE	BitON;				/* ON�ް�			*/
	UBYTE	BitOFF;				/* OFF�ް�			*/
};

extern	UBYTE	geefFlCourse;								/* ����w���׸ށi0:���1,1:���2,2:���3,3:���4)	*/
extern	UBYTE	glbFlReqDot;								/* 7SEG ��ص�ޕ\���v���׸�							*/

extern	UBYTE	glbFlLedCom;								/* LED COM1�`COM4�̶����i1�`4�̂ǂ�COM�ɏo�͂��邩	*/
extern	UWORD	glbCtKeyOffTimer;
extern	UBYTE	glbFlError;									/* �װ�ڍ��ޯ��׸�			*/
extern	UBYTE	glbFlKydt;									/* KEY�����ް�				*/
															/* bit0:����ύX/�ݒ跰		*/
															/* bit1:����				*/
															/* bit2:����				*/
															/* bit3:����/�į�߷�		*/
extern	UBYTE	glbFlMainMode;								/* Ҳ�Ӱ���׸�				*/
extern	UBYTE	glbDt7Seg2;									/* 7SEG 2�����ް�		*/
extern	UBYTE	glbDt7Seg1;									/* 7SEG 1�����ް�		*/




extern	UBYTE	glbFlDipSW;									/* Dip SW				*/

extern UBYTE	glbFlSoftOpen;				/* �\�t�g�J���L��Fl	*/	

extern	UBYTE	glbFlVacuumElectromangReq;					/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��*/
extern	UBYTE	glbFlSysytem7Seg;							/* ����Ӱ��7SEG�o��ý��׸�	*/
extern	UBYTE	glbFlSystemMode;							/* ����Ӱ��7SEG�o��ý��׸�	*/
extern	UBYTE	glbFlSysHeaterReq;							/* ����Ӱ�ށ@˰��Ď��v��=2�b		*/
extern	UBYTE	glbFlVaccumeTips;
extern	UBYTE	glbFlWarmCansel;							/* �g�C�^�]���׸�			*/

extern	UWORD	glbCtVacuumElectromang;	
extern	UWORD	glbCtHeaterBlockTimer;						/* ˰���ۯ��㏸���Զ���		*/
extern	unsigned long int glbCtWarmTimer;
extern	UBYTE	glbFlDriveMode;								/* �^�]Ӱ�ގ���Ӱ���׸�		*/
extern	UBYTE	geefFlVacuumeDisp;							/* �^��H���c��[���� or %]�̐؊���	*/
extern	UBYTE	glbFlProcError;								/* �^��H�����ɴװ�����̔F���׸�		*/
extern	unsigned long int glbCtDriveTimer;					/* �^��������Զ���			*/
extern	UWORD	glbCtGusInclusionTimer;						/* �޽�������Զ���			*/
extern	UWORD	glbCtVacuumEmergncyTimer;					/* �^��ꎞ��~����			*/
extern	UWORD	glbCtGusStubilityTimer;						/* �޽����҂����Զ���		*/
extern	UWORD	glbCtSeelTimer;								/* ��ٍH�����Զ���			*/
extern	UWORD	glbCtSeelCoolingTimer;						/* ��ٗ�p�H�����Զ���		*/
extern	UWORD	glbCtSoftReleaseTimer;						/* ��ĉ������				*/
extern	UWORD	glbCtKeyOffTimer;							/* ������1�b���				*/
extern	UWORD	glbCtSysHeaterOffTimer;						/* ����Ӱ�� ˰�2�b�Ď���϶���		*/
extern	UWORD	glbCtFunTimer;
extern	UWORD	glbCtVacuumeWarm;							/* �g�C�^�]��ϰ				*/
extern	SWORD	glbFlProc;									/* �H���w���׸ށi0:�^��A1:��فA2:��p)	*/
extern	UWORD	glbCtVacuumTimeout1;							/* �^����ѱ����������		*/
extern	UWORD	glbCtVacuumTimeout2;						/* �^����ѱ����������(20�b�o�ߌ�A�^�����ق�10%����)		*/
extern	UWORD	glbCtVacuumTimeout3;						/* �^����ѱ����������(20�b�o�ߌ�A�^�����ق�10%����)		*/
extern	UWORD	glbCtWaitTimerT5;

extern	UWORD	glbCtOutputTimer;
extern	UWORD	glbCtWaitTimerE6Cansel;



extern	UWORD glbCntPowerSwOffChk;
extern	UWORD glbCntPowerSwOnChk;
extern	UWORD glbFlPowerSw;
extern	UWORD glbCntPowerSwLongChk;
extern	UWORD glbFlPowerSwLong;
extern	UWORD glbFlPowerSwLongPalse;
extern UWORD glbFlStanby;
extern uint8_t glbPowerLed;
extern uint8_t glbPowerLedBlinkTogle;
extern uint16_t glbCntPowerLedBlink_onTime;
extern uint16_t glbCntSetPowerLedBlink_onTime;
extern uint16_t glbCntPowerLedBlink_offTime;
extern uint16_t glbCntSetPowerLedBlink_offTime;


extern	volatile	UWORD	glbCtBuzzerOnTimer;
extern	volatile	UWORD	glbCtBuzzerOffTimer;
extern	volatile	UWORD	glbCtWaitTimer;
extern	struct	tagCOURSE	tmpCourseData[4];
extern	struct	tagSYSTEMMAXMIN	geefBfCourseDataMaxMin[8];
extern	UBYTE	stiCtKey;
extern	UBYTE	ctTest;
extern	UBYTE	ctTest2;
extern	UBYTE	glbFlFunReq;
extern	UWORD	glbCtIntTimer;
extern	UWORD	glbCtRealAD;
extern	UBYTE	glbFlSaveMode;
extern	UBYTE	glbFlVacuumWarning;
extern	UBYTE	glbFlContinue;
extern	UBYTE	glbFlSecLevel;
extern	UBYTE	glbFlMainSystem;
extern	UBYTE	glbFlAdjust;

extern	UBYTE	glbFlWriteModeCN7;				/* CN7�M���F������ٽ�o�͏�ԊĎ��׸ށi�e�@�j/ �Őf��ٽ�o�͊Ď�����׸ށi�q�@�j				*/
extern	UBYTE	glbFlReadModeCN4;				/* CN4�M��:�Őf��ٽ���͏�ԊĎ��׸�	(�e�@)  / ������ٽ���͏�ԊĎ��׸ށi�q�@�j				*/
extern	UBYTE	glbFlGetACK;					/* �Őf��ٽ�ɑ΂��ĉ�����ٽ��M�׸ށi�q�@�j*/
extern	UBYTE	glbFlGetENQ;					/* �Őf��ٽ��M�׸ށi�e�@�j*/
extern	UBYTE	glbFlConnection;				/* �e�@�Ǝq�@�̐ڑ�����׸�			*/
extern	UBYTE	glbFlReserveMode;
extern	UBYTE	glbFlChildStartReq;				/* �q�@�^�]���׸�					*/
extern	UBYTE	glbFlLed11;
extern	UBYTE	glbFlChildBusy;
extern	UBYTE	glbFlWarmMode;
extern	unsigned long	int	glbCtCN4Timer;


extern	uint8_t glbFlPackCountDisplay_Start;
extern uint16_t glbCtPackCountDisplayTime;

extern UBYTE glbFlLedCourse;