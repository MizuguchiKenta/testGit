#include	"common.h"
#include    "glbRam.h"
UBYTE	glbFlReqDot;			/* 7SEG ﾋﾟﾘｵﾄﾞ表示要求ﾌﾗｸﾞ							*/
UBYTE	glbFlLedCom;			/* LED COM1～COM4のｶｳﾝﾀ（1～4のどのCOMに出力するか	*/
UWORD	glbCtKeyOffTimer;
UBYTE	glbFlKydt;				/* KEY入力ﾃﾞｰﾀ				*/
								/* bit0:ｺｰｽ変更/設定ｷｰ		*/
								/* bit1:▲ｷｰ				*/
								/* bit2:▼ｷｰ				*/
								/* bit3:ｽﾀｰﾄ/ｽﾄｯﾌﾟｷｰ		*/
UBYTE	glbFlMainMode;			/* ﾒｲﾝﾓｰﾄﾞﾌﾗｸﾞ			*/
UBYTE	glbDt7Seg2;					/* 7SEG 2桁目ﾃﾞｰﾀ		*/
UBYTE	glbDt7Seg1;					/* 7SEG 1桁目ﾃﾞｰﾀ		*/

UBYTE	glbFlDipSW;					/* Dip SW				*/

UBYTE	glbFlSoftOpen;				/* ソフト開放有りFl	*/


UBYTE	glbFlSysytem7Seg;			/* ｼｽﾃﾑﾓｰﾄﾞ7SEG出力ﾃｽﾄﾌﾗｸﾞ	*/
UBYTE	glbFlSystemMode;			/* ｼｽﾃﾑﾓｰﾄﾞ7SEG出力ﾃｽﾄﾌﾗｸﾞ	*/
UBYTE	glbFlVacuumElectromangReq;	/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求*/
UBYTE	glbFlSysHeaterReq;			/* ｼｽﾃﾑﾓｰﾄﾞ　ﾋｰﾀ監視要求=2秒		*/
UBYTE	glbFlVaccumeTips;
UBYTE	glbFlWarmCansel;					/* 暖気運転中ﾌﾗｸﾞ			*/
UBYTE	glbFlProcError;				/* 真空工程中にｴﾗｰ発生の認識ﾌﾗｸﾞ		*/
UWORD	glbCtVacuumElectromang;		/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後の10秒ｶｳﾝﾀ	*/
UWORD	glbCtHeaterBlockTimer;		/* ﾋｰﾀﾌﾞﾛｯｸ上昇時間ｶｳﾝﾀ		*/
unsigned long int glbCtWarmTimer;				/* 暖気運転時間				*/
UBYTE	glbFlDriveMode;				/* 運転ﾓｰﾄﾞ時のﾓｰﾄﾞﾌﾗｸﾞ		*/
UBYTE	geefFlVacuumeDisp;			/* 真空工程残を[時間 or %]の切換え	*/
unsigned long int	glbCtDriveTimer;			/* 真空引き時間ｶｳﾝﾀ			*/
UWORD	glbCtGusInclusionTimer;		/* ｶﾞｽ封入時間ｶｳﾝﾀ			*/
UWORD	glbCtVacuumEmergncyTimer;	/* 真空一時停止ｶｳﾝﾀ			*/
UWORD	glbCtGusStubilityTimer;		/* ｶﾞｽ安定待ち時間ｶｳﾝﾀ		*/
UWORD	glbCtSeelTimer;				/* ｼｰﾙ工程時間ｶｳﾝﾀ			*/
UWORD	glbCtSeelCoolingTimer;		/* ｼｰﾙ冷却工程時間ｶｳﾝﾀ		*/
UWORD	glbCtSoftReleaseTimer;		/* ｿﾌﾄ解放時間				*/
UWORD	glbCtKeyOffTimer;			/* ｷｰ入力1秒ﾀｲﾏ				*/
UWORD	glbCtSysHeaterOffTimer;		/* ｼｽﾃﾑﾓｰﾄﾞ ﾋｰﾀ2秒監視ﾀｲﾏｶｳﾝﾀ		*/
UWORD	glbCtFunTimer;
UWORD	glbCtVacuumeWarm;			/* 暖気運転ﾀｲﾏｰ				*/
SWORD	glbFlProc;					/* 工程指定ﾌﾗｸﾞ（0:真空、1:ｼｰﾙ、2:冷却)	*/
UWORD	glbCtVacuumTimeout1;		/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ		*/
UWORD	glbCtVacuumTimeout2;		/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ(20秒経過後、真空ﾚﾍﾞﾙが10%未満)		*/
UWORD	glbCtVacuumTimeout3;		/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ(30秒経過後、真空ﾚﾍﾞﾙが90%未満)		*/
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
UBYTE	glbFlError;					/* ｴﾗｰ詳細ﾋﾞｯﾄﾌﾗｸﾞ			*/
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

UBYTE	glbFlWriteModeCN7;				/* CN7信号：応答ﾊﾟﾙｽ出力状態監視ﾌﾗｸﾞ（親機）/ 打診ﾊﾟﾙｽ出力監視状態ﾌﾗｸﾞ（子機）				*/
UBYTE	glbFlReadModeCN4;				/* CN4信号:打診ﾊﾟﾙｽ入力状態監視ﾌﾗｸﾞ	(親機)  / 応答ﾊﾟﾙｽ入力状態監視ﾌﾗｸﾞ（子機）				*/
UBYTE	glbFlGetACK;					/* 打診ﾊﾟﾙｽに対して応答ﾊﾟﾙｽ受信ﾌﾗｸﾞ（子機）*/
UBYTE	glbFlGetENQ;					/* 打診ﾊﾟﾙｽ受信ﾌﾗｸﾞ（親機）*/
UBYTE	glbFlConnection;				/* 親機と子機の接続状態ﾌﾗｸﾞ			*/
UBYTE	glbFlReserveMode;
UBYTE	glbFlChildStartReq;				/* 子機運転中ﾌﾗｸﾞ					*/
UBYTE	glbFlLed11;
UBYTE	glbFlChildBusy;
UBYTE	glbFlWarmMode;
unsigned long	int	glbCtCN4Timer;

uint8_t glbFlPackCountDisplay_Start;
uint16_t glbCtPackCountDisplayTime;

UBYTE glbFlLedCourse;


