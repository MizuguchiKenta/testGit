#include	"common.h"

#if 0
struct 	tagCOURSE {
	UBYTE	Vacuum;				/* 真空時間				(1秒単位)	*/
	UBYTE	Seel;				/* ｼｰﾙ時間				(0.1秒単位）*/
	UBYTE	SeelCooling;		/* ｼｰﾙ冷却時間			(0.1秒単位)	*/
	UBYTE	Gus;				/* ｶﾞｽ時間				(0.1秒単位)	*/
	UBYTE	GusStability;		/* ｶﾞｽ安定時間			(0.1秒単位)	*/
	UBYTE	SoftRelease;		/* ｿﾌﾄ解放時間			(0.1秒単位)	*/
	UBYTE	VacuumLevel;		/* 真空到達ﾚﾍﾞﾙ			(1%単位)	*/
	
};
#endif

enum	PROSESS
{
	Vacuum,				/* 真空時間				(1秒単位)	*/
	Seel,				/* ｼｰﾙ時間				(0.1秒単位）*/
	SeelCooling,		/* ｼｰﾙ冷却時間			(0.1秒単位)	*/
	Gus,				/* ｶﾞｽ時間				(0.1秒単位)	*/
	GusStability,		/* ｶﾞｽ安定時間			(0.1秒単位)	*/
	GusLevel,			/* ｶﾞｽﾚﾍﾞﾙ				(0.1秒単位)	*/
	SoftRelease,		/* ｿﾌﾄ解放時間			(0.1秒単位)	*/
	VacuumLevel			/* 真空到達ﾚﾍﾞﾙ			(1%単位)	*/
};

struct tagSYSTEMMAXMIN	{
	UWORD	Min;				/* MIN.					*/
	UWORD	Max;				/* MAX.					*/
};

struct tagLEDCOM	{
	volatile UBYTE	*pIO;				/* LEDCOMx出力ｱﾄﾞﾚｽ	*/
	UBYTE	ON;					/* LED出力ﾃﾞｰﾀ		*/
	UBYTE	OFF;				/* LED出力ﾃﾞｰﾀ		*/
};

struct tagSYSTEM	{
	UWORD	Default;			/* 初期値				*/
	UWORD	Min;				/* MIN.					*/
	UWORD	Max;				/* MAX.					*/
};

enum	SYSTEM
{
	Default,					/* 初期値				*/
	Min,						/* MIN.					*/
	Max,						/* MAX.					*/
};

struct tagOUTPUTTEST	{
	volatile UBYTE	*pIO;				/* 出力機器ｱﾄﾞﾚｽ	*/
	UBYTE	BitON;				/* ONﾃﾞｰﾀ			*/
	UBYTE	BitOFF;				/* OFFﾃﾞｰﾀ			*/
};

extern	UBYTE	geefFlCourse;								/* ｺｰｽ指定ﾌﾗｸﾞ（0:ｺｰｽ1,1:ｺｰｽ2,2:ｺｰｽ3,3:ｺｰｽ4)	*/
extern	UBYTE	glbFlReqDot;								/* 7SEG ﾋﾟﾘｵﾄﾞ表示要求ﾌﾗｸﾞ							*/

extern	UBYTE	glbFlLedCom;								/* LED COM1～COM4のｶｳﾝﾀ（1～4のどのCOMに出力するか	*/
extern	UWORD	glbCtKeyOffTimer;
extern	UBYTE	glbFlError;									/* ｴﾗｰ詳細ﾋﾞｯﾄﾌﾗｸﾞ			*/
extern	UBYTE	glbFlKydt;									/* KEY入力ﾃﾞｰﾀ				*/
															/* bit0:ｺｰｽ変更/設定ｷｰ		*/
															/* bit1:▲ｷｰ				*/
															/* bit2:▼ｷｰ				*/
															/* bit3:ｽﾀｰﾄ/ｽﾄｯﾌﾟｷｰ		*/
extern	UBYTE	glbFlMainMode;								/* ﾒｲﾝﾓｰﾄﾞﾌﾗｸﾞ				*/
extern	UBYTE	glbDt7Seg2;									/* 7SEG 2桁目ﾃﾞｰﾀ		*/
extern	UBYTE	glbDt7Seg1;									/* 7SEG 1桁目ﾃﾞｰﾀ		*/




extern	UBYTE	glbFlDipSW;									/* Dip SW				*/

extern UBYTE	glbFlSoftOpen;				/* ソフト開放有りFl	*/	

extern	UBYTE	glbFlVacuumElectromangReq;					/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求*/
extern	UBYTE	glbFlSysytem7Seg;							/* ｼｽﾃﾑﾓｰﾄﾞ7SEG出力ﾃｽﾄﾌﾗｸﾞ	*/
extern	UBYTE	glbFlSystemMode;							/* ｼｽﾃﾑﾓｰﾄﾞ7SEG出力ﾃｽﾄﾌﾗｸﾞ	*/
extern	UBYTE	glbFlSysHeaterReq;							/* ｼｽﾃﾑﾓｰﾄﾞ　ﾋｰﾀ監視要求=2秒		*/
extern	UBYTE	glbFlVaccumeTips;
extern	UBYTE	glbFlWarmCansel;							/* 暖気運転中ﾌﾗｸﾞ			*/

extern	UWORD	glbCtVacuumElectromang;	
extern	UWORD	glbCtHeaterBlockTimer;						/* ﾋｰﾀﾌﾞﾛｯｸ上昇時間ｶｳﾝﾀ		*/
extern	unsigned long int glbCtWarmTimer;
extern	UBYTE	glbFlDriveMode;								/* 運転ﾓｰﾄﾞ時のﾓｰﾄﾞﾌﾗｸﾞ		*/
extern	UBYTE	geefFlVacuumeDisp;							/* 真空工程残を[時間 or %]の切換え	*/
extern	UBYTE	glbFlProcError;								/* 真空工程中にｴﾗｰ発生の認識ﾌﾗｸﾞ		*/
extern	unsigned long int glbCtDriveTimer;					/* 真空引き時間ｶｳﾝﾀ			*/
extern	UWORD	glbCtGusInclusionTimer;						/* ｶﾞｽ封入時間ｶｳﾝﾀ			*/
extern	UWORD	glbCtVacuumEmergncyTimer;					/* 真空一時停止ｶｳﾝﾀ			*/
extern	UWORD	glbCtGusStubilityTimer;						/* ｶﾞｽ安定待ち時間ｶｳﾝﾀ		*/
extern	UWORD	glbCtSeelTimer;								/* ｼｰﾙ工程時間ｶｳﾝﾀ			*/
extern	UWORD	glbCtSeelCoolingTimer;						/* ｼｰﾙ冷却工程時間ｶｳﾝﾀ		*/
extern	UWORD	glbCtSoftReleaseTimer;						/* ｿﾌﾄ解放時間				*/
extern	UWORD	glbCtKeyOffTimer;							/* ｷｰ入力1秒ﾀｲﾏ				*/
extern	UWORD	glbCtSysHeaterOffTimer;						/* ｼｽﾃﾑﾓｰﾄﾞ ﾋｰﾀ2秒監視ﾀｲﾏｶｳﾝﾀ		*/
extern	UWORD	glbCtFunTimer;
extern	UWORD	glbCtVacuumeWarm;							/* 暖気運転ﾀｲﾏｰ				*/
extern	SWORD	glbFlProc;									/* 工程指定ﾌﾗｸﾞ（0:真空、1:ｼｰﾙ、2:冷却)	*/
extern	UWORD	glbCtVacuumTimeout1;							/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ		*/
extern	UWORD	glbCtVacuumTimeout2;						/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ(20秒経過後、真空ﾚﾍﾞﾙが10%未満)		*/
extern	UWORD	glbCtVacuumTimeout3;						/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ(20秒経過後、真空ﾚﾍﾞﾙが10%未満)		*/
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

extern	UBYTE	glbFlWriteModeCN7;				/* CN7信号：応答ﾊﾟﾙｽ出力状態監視ﾌﾗｸﾞ（親機）/ 打診ﾊﾟﾙｽ出力監視状態ﾌﾗｸﾞ（子機）				*/
extern	UBYTE	glbFlReadModeCN4;				/* CN4信号:打診ﾊﾟﾙｽ入力状態監視ﾌﾗｸﾞ	(親機)  / 応答ﾊﾟﾙｽ入力状態監視ﾌﾗｸﾞ（子機）				*/
extern	UBYTE	glbFlGetACK;					/* 打診ﾊﾟﾙｽに対して応答ﾊﾟﾙｽ受信ﾌﾗｸﾞ（子機）*/
extern	UBYTE	glbFlGetENQ;					/* 打診ﾊﾟﾙｽ受信ﾌﾗｸﾞ（親機）*/
extern	UBYTE	glbFlConnection;				/* 親機と子機の接続状態ﾌﾗｸﾞ			*/
extern	UBYTE	glbFlReserveMode;
extern	UBYTE	glbFlChildStartReq;				/* 子機運転中ﾌﾗｸﾞ					*/
extern	UBYTE	glbFlLed11;
extern	UBYTE	glbFlChildBusy;
extern	UBYTE	glbFlWarmMode;
extern	unsigned long	int	glbCtCN4Timer;


extern	uint8_t glbFlPackCountDisplay_Start;
extern uint16_t glbCtPackCountDisplayTime;

extern UBYTE glbFlLedCourse;