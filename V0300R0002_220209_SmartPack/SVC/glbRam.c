#include	"common.h"
#include    "glbRam.h"
UBYTE	glbFlReqDot;			/* 7SEG ��ص�ޕ\���v���׸�							*/
UBYTE	glbFlLedCom;			/* LED COM1�`COM4�̶����i1�`4�̂ǂ�COM�ɏo�͂��邩	*/
UWORD	glbCtKeyOffTimer;
UBYTE	glbFlKydt;				/* KEY�����ް�				*/
								/* bit0:����ύX/�ݒ跰		*/
								/* bit1:����				*/
								/* bit2:����				*/
								/* bit3:����/�į�߷�		*/
UBYTE	glbFlMainMode;			/* Ҳ�Ӱ���׸�			*/
UBYTE	glbDt7Seg2;					/* 7SEG 2�����ް�		*/
UBYTE	glbDt7Seg1;					/* 7SEG 1�����ް�		*/

UBYTE	glbFlDipSW;					/* Dip SW				*/

UBYTE	glbFlSoftOpen;				/* �\�t�g�J���L��Fl	*/


UBYTE	glbFlSysytem7Seg;			/* ����Ӱ��7SEG�o��ý��׸�	*/
UBYTE	glbFlSystemMode;			/* ����Ӱ��7SEG�o��ý��׸�	*/
UBYTE	glbFlVacuumElectromangReq;	/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~��10�b�ԁA�^��d����ON�̗v��*/
UBYTE	glbFlSysHeaterReq;			/* ����Ӱ�ށ@˰��Ď��v��=2�b		*/
UBYTE	glbFlVaccumeTips;
UBYTE	glbFlWarmCansel;					/* �g�C�^�]���׸�			*/
UBYTE	glbFlProcError;				/* �^��H�����ɴװ�����̔F���׸�		*/
UWORD	glbCtVacuumElectromang;		/* �^�]�H���i�^��j/�g�C�^�]�I����������ߒ�~���10�b����	*/
UWORD	glbCtHeaterBlockTimer;		/* ˰���ۯ��㏸���Զ���		*/
unsigned long int glbCtWarmTimer;				/* �g�C�^�]����				*/
UBYTE	glbFlDriveMode;				/* �^�]Ӱ�ގ���Ӱ���׸�		*/
UBYTE	geefFlVacuumeDisp;			/* �^��H���c��[���� or %]�̐؊���	*/
unsigned long int	glbCtDriveTimer;			/* �^��������Զ���			*/
UWORD	glbCtGusInclusionTimer;		/* �޽�������Զ���			*/
UWORD	glbCtVacuumEmergncyTimer;	/* �^��ꎞ��~����			*/
UWORD	glbCtGusStubilityTimer;		/* �޽����҂����Զ���		*/
UWORD	glbCtSeelTimer;				/* ��ٍH�����Զ���			*/
UWORD	glbCtSeelCoolingTimer;		/* ��ٗ�p�H�����Զ���		*/
UWORD	glbCtSoftReleaseTimer;		/* ��ĉ������				*/
UWORD	glbCtKeyOffTimer;			/* ������1�b���				*/
UWORD	glbCtSysHeaterOffTimer;		/* ����Ӱ�� ˰�2�b�Ď���϶���		*/
UWORD	glbCtFunTimer;
UWORD	glbCtVacuumeWarm;			/* �g�C�^�]��ϰ				*/
SWORD	glbFlProc;					/* �H���w���׸ށi0:�^��A1:��فA2:��p)	*/
UWORD	glbCtVacuumTimeout1;		/* �^����ѱ����������		*/
UWORD	glbCtVacuumTimeout2;		/* �^����ѱ����������(20�b�o�ߌ�A�^�����ق�10%����)		*/
UWORD	glbCtVacuumTimeout3;		/* �^����ѱ����������(30�b�o�ߌ�A�^�����ق�90%����)		*/
UWORD	glbCtWaitTimerT5;

UWORD glbCntPowerSwOffChk;
UWORD glbCntPowerSwOnChk;
UWORD glbFlPowerSw;
UWORD glbCntPowerSwLongChk;
UWORD glbFlPowerSwLong;
UWORD glbFlPowerSwLongPalse;
UWORD glbFlStanby;
uint8_t glbPowerLed;
uint8_t glbPowerLedBlinkTogle;
uint16_t glbCntPowerLedBlink_onTime;
uint16_t glbCntSetPowerLedBlink_onTime;
uint16_t glbCntPowerLedBlink_offTime;
uint16_t glbCntSetPowerLedBlink_offTime;


UWORD	glbCtWaitTimerE6Cansel;
UWORD	glbCtOutputTimer;
UBYTE	glbFlError;					/* �װ�ڍ��ޯ��׸�			*/
volatile	UWORD	glbCtBuzzerOnTimer;
volatile	UWORD	glbCtBuzzerOffTimer;
volatile	UWORD	glbCtWaitTimer;
UBYTE	stiCtKey=0;
UBYTE	ctTest=0;
UBYTE	ctTest2=0;
UBYTE	glbFlFunReq;
UWORD	glbCtIntTimer;
UWORD	glbCtRealAD;
UBYTE	glbFlSaveMode;
UBYTE	glbFlVacuumWarning;
UBYTE	glbFlContinue;
UBYTE	glbFlSecLevel;
UBYTE	glbFlMainSystem;
UBYTE	glbFlAdjust;

UBYTE	glbFlWriteModeCN7;				/* CN7�M���F������ٽ�o�͏�ԊĎ��׸ށi�e�@�j/ �Őf��ٽ�o�͊Ď�����׸ށi�q�@�j				*/
UBYTE	glbFlReadModeCN4;				/* CN4�M��:�Őf��ٽ���͏�ԊĎ��׸�	(�e�@)  / ������ٽ���͏�ԊĎ��׸ށi�q�@�j				*/
UBYTE	glbFlGetACK;					/* �Őf��ٽ�ɑ΂��ĉ�����ٽ��M�׸ށi�q�@�j*/
UBYTE	glbFlGetENQ;					/* �Őf��ٽ��M�׸ށi�e�@�j*/
UBYTE	glbFlConnection;				/* �e�@�Ǝq�@�̐ڑ�����׸�			*/
UBYTE	glbFlReserveMode;
UBYTE	glbFlChildStartReq;				/* �q�@�^�]���׸�					*/
UBYTE	glbFlLed11;
UBYTE	glbFlChildBusy;
UBYTE	glbFlWarmMode;
unsigned long	int	glbCtCN4Timer;

uint8_t glbFlPackCountDisplay_Start;
uint16_t glbCtPackCountDisplayTime;

UBYTE glbFlLedCourse;


