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
void	mainCourseMode(void);				/* ｺｰｽ設定ﾓｰﾄﾞ				*/
void	mainWarmMode(void);					/* 暖気運転ﾓｰﾄﾞ				*/
void	mainDriveMode(void);				/* 運転ﾓｰﾄﾞ				*/
void	mainStanbyMode(void);				/* スリープモード*/	
void	mainErrorMode(void);				/* ｴﾗｰ検出ﾓｰﾄﾞ				*/

void	mainStanbyCheck(void);			/*復帰確認*/
void mainPackCountDisplay_Start(void);	/*カウント数表示開始*/
void mainPackCountDisplay_End(void);	/*カウント数表示終了*/	

void	mainVacuumElectromangCheck(void);
UBYTE	glbGetReadKey(void);
UBYTE   glbWaitKeyOff(UBYTE,UWORD);
void 	mainDisplay7Seg(UWORD);


void 	mainDisplayWarning7Seg(UWORD);
void	glbGetDipSW(void);							/* DIPSWﾘｰﾄﾞ						*/
void	mainPortOutputInitial(void);
void	mainGlobalClear(void);							/* 共通ﾒﾓﾘｸﾘｱ						*/
void	mcmLocalClear(void);
UBYTE	mcmFlashDataRead(void);
UBYTE	mcmFlashDataInitial(void);
void	mainFlashClear(void);
void	mcmFlashInitialClear(void);
void	mcmStartBuzzer(UWORD);
UBYTE	mcmFlashWriteCheck(void);							/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更ﾁｪｯｸ				*/
void	mainCheckPumpthermal(void);						/* ﾎﾟﾝﾌﾟｻｰﾏﾙのｴﾗｰ監視		*/
void	mcmSetBuzzerEnd(void);
void	mcmSetBuzzerError(void);
void	mcmSetBuzzerEffective(void);						/* 有効ﾌﾞｻﾞｰを鳴らす			*/
void	mcmSetBuzzerInvalid(void);						/* 無効ﾌﾞｻﾞｰを鳴らす			*/
void	mcmBuzzerOff(UWORD);						/* 170ms OFF*/
UBYTE	mcmCheckDrawerOpenToClose(void);
void	mcmCheckVacuumSensorError(void);					/* 真空ｾﾝｻｰｴﾗｰの検出		*/
void	mcmCheckVacuumSensorError(void);					/* 真空ｾﾝｻｰｴﾗｰの検出		*/
void	mcmWaitTimer(UWORD);
void	mcmSetSystemData(void);
void	mcmSetLed(UBYTE);


void	mainStartVacuum(void);

UBYTE mcmDataSettingRangeCheck(void);	/*データ入力範囲有効確認*/	

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
extern	UBYTE	stiFlFlashing;								/* ﾄｸﾞﾙ更新								*/
extern	UWORD	stiCtFlashing;								/* 500ms周期ｶｳﾝﾀ						*/
extern	UBYTE	stiFlFlashing2;								/* ﾄｸﾞﾙ更新								*/
extern	UWORD	stiCtFlashing2;								/* 500ms周期ｶｳﾝﾀ						*/
extern	UBYTE	stiFlFlashing3;								/* ﾄｸﾞﾙ更新								*/
extern	UWORD	stiCtFlashing3;								/* 500ms周期ｶｳﾝﾀ						*/
extern	UBYTE	stiFlFlashing4;								/* ﾄｸﾞﾙ更新								*/
extern	UWORD	stiCtFlashing4;								/* 500ms周期ｶｳﾝﾀ						*/
extern	const	UBYTE	DefaultCourseData[4][8];	 		/* 4ｺｰｽ分			*/
extern	const	UBYTE	DefaultSystemData[5][3];	 		/* 4ｺｰｽ分			*/

extern	UWORD	pfdlFlashDataSize(void);
extern	void	pldCopyBackupData(void);	
extern	UBYTE	pldVerifyCheck(void);	
extern	UBYTE	pdlReadFlash(void);											/* ﾌﾗｯｼｭからﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀを読み込む	*/
extern	void	stiInitialSamplingADC(void);
extern	R_WDT_Restart(void);										/* ｳｫｯﾁﾄﾞｯｸﾀｲﾏﾘｾｯﾄ							*/
extern	UBYTE	pdlWriteFlash(void);
extern	void	mainSystemMode(void);
extern	void	mainWarmStart(void);

extern void powerOn_GuruGuru7Seg(void);

UBYTE	mainFlDrawer;

uint8_t mainFlPackCountDisplay_Busy;

UBYTE	mainDtBit[10] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,0xff};

										// ｴﾗｰ表示No.ﾃｰﾌﾞﾙ [Ex]	
UBYTE	mainTblError[8] = {
						    0 ,			/* ﾎﾟﾝﾌﾟｻｰﾏﾙｴﾗｰ			*/
							5 ,			/* *****				*/							    
							2 ,			/* 真空ｴﾗｰ				*/
							3 ,			/* 真空警告				*/
							6 ,			/* 真空ｾﾝｻｰｴﾗｰ			*/
							7 ,			/* CPUｴﾗｰ				*/
							8 ,			/* ｳｫｯﾁﾄﾞｯｸｴﾗｰ			*/
							9 			/* ﾌﾗｯｼｭﾃﾞｰﾀｴﾗｰ			*/
							};

// ﾒｲﾝﾙｰﾁﾝ状態遷移関数ﾃｰﾌﾞﾙ	
void	(*mainProcTbl[])() =
{
	mainWaitMode,			/* 待機ﾓｰﾄﾞ処理				*/
	mainCourseMode,			/* ｺｰｽ設定ﾓｰﾄﾞ	（未使用)	*/
	mainWarmMode,			/* 暖気運転ﾓｰﾄﾞ				*/
	mainDriveMode,			/* 運転ﾓｰﾄﾞ					*/
	mainErrorMode,			/* ｴﾗｰ検出ﾓｰﾄﾞ				*/
	mainStanbyMode,			/* 電源スタンバイﾓｰﾄﾞ*/
	mcmSetSystemData		/* ｼｽﾃﾑﾃﾞｰﾀ設定ﾓｰﾄﾞ			*/

};

/***********************************************************************************************************************
*    関数名    mainSVC
*    引  数    
*    戻り値    
*    関数機能  ﾒｲﾝ処理
*    処理内容  ﾊﾞｷｭｰﾑｸｯｶｰの6つのﾓｰﾄﾞである【待機ﾓｰﾄﾞ】,【真空運転ﾓｰﾄﾞ】、【暖気運転ﾓｰﾄﾞ】、【ｴﾗｰ表示ﾓｰﾄﾞ】、
*              【予約ﾓｰﾄﾞ】、【ｼｽﾃﾑ設定ﾓｰﾄﾞ】を状態遷移ﾌﾗｸﾞ（glbFlMainMode）により遷移する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void mainSVC(void)
{
	UBYTE 	data;
	UBYTE err;
	

	mainPortOutputInitial();														/* 出力ﾎﾟｰﾄｲﾆｼｬﾙ					*/
	mainGlobalClear();																/* 共通ﾒﾓﾘｸﾘｱ						*/
	mcmLocalClear();																/* ﾛｰｶﾙﾒﾓﾘｸﾘｱ						*/
	glbGetDipSW();																	/* DIPSWﾘｰﾄﾞ						*/
	R_INTC6_Start();
	R_TAU0_Channel0_Start();														/* 1msﾀｲﾏ割り込みｽﾀｰﾄ				*/
	

	err = mcmFlashDataRead();								/* ﾌﾗｯｼｭからのﾃﾞｰﾀ読み込みとｲﾆｼｬﾙ	*/
#if 0
	if(err)
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;
		glbFlMainMode = GCN_ERROR_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
		(*mainProcTbl[glbFlMainMode])();					/* エラーﾓｰﾄﾞへ遷移		*/
	}
#endif	

	mainSystemMode();																/* ﾃｽﾄﾓｰﾄﾞに遷移				*/
																					/* ﾃｽﾄﾓｰﾄﾞに遷移した場合は、mainSystemModeから戻ってこない	*/
	
	powerOn_GuruGuru7Seg();
	glbDt7Seg1 = 0;
	glbDt7Seg2 = 0;
	
	glbFlContinue = 1;										/* 連続ﾓｰﾄﾞ	*/	
	ledControl_Course(geefFlCourse,1,0);					/* LED			*/
	ledControl_Proc(glbFlProc,1,0);											
	
	powerLedControl(1,0,0);	
	

	glbFlMainMode = GCN_WAIT_MODE;													/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* 真空工程が[秒]表示の場合						*/
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];								/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/
	}
	else																			/* 真空工程が[%]表示の場合						*/
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];							/* 選択されているｺｰｽ1の真空ﾚﾍﾞﾙ					*/

	}
	glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?			/* 真空工程が[秒][%]指定をLED反映				*/
					GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;							
	ledControl_Sec(glbFlSecLevel,1,0);
	mainDisplay7Seg(data);															/* 真空工程時間またはﾚﾍﾞﾙを7SEGに表示			*/

	mainFlDrawer = 0;	


	glbCtWaitTimerE6Cansel = GCN_TIMER_4_9S;									/* 運転終了後WAIT時間(T5)	*/
					   
	stiInitialSamplingADC();														/* 真空度A/D値ｲﾆｼｬﾙ処理				*/

	while(1)																		/* ﾒｲﾝ処理							*/
	{	
		if(!glbFlStanby)
		{
			
			mainCheckPumpthermal();														/* ﾎﾟﾝﾌﾟｻｰﾏﾙのｴﾗｰ監視		*/
			mcmCheckVacuumSensorError();												/* 真空ｾﾝｻｰｴﾗｰ[E6]の検出		*/

			if(glbFlMainMode != GCN_ERROR_MODE)
			{
				mainStanbyCheck();
			}
		}

		
		mainVacuumElectromangCheck();												/* 真空電磁弁OFFの監視		*/
		
			
		(*mainProcTbl[glbFlMainMode])();											/* 現状態のﾓｰﾄﾞへ遷移		*/
	}
	

}


/***********************************************************************************************************************
*    関数名    mainVacuumElectromangCheck
*    引  数    
*    戻り値    
*    関数機能  運転工程（真空）/暖気運転終了時のポンプ停止後10秒間真空電磁弁ONの監視
*    処理内容  ツインチャンバー動作時の場合、相手が運転中であれば10秒間真空電磁弁のONはキャンセル
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainVacuumElectromangCheck(void)
{
	volatile	UBYTE	in1;

	if(glbFlVacuumElectromangReq == 1)			/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求有り？*/
	{	
		if(glbCtVacuumElectromang == 0)			/* ﾀｲﾏ=0?							*/
		{
			P2 &= P20_VACUUM_ELECTROMAG_OFF;	/* 真空電磁弁OFF					*/
			glbFlVacuumElectromangReq = 0;		/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求ﾘｾｯﾄ	*/
		}
	}
}


/***********************************************************************************************************************
*    関数名    mainWaitMode
*    引  数    
*    戻り値    
*    関数機能  待機モード処理
*    処理内容  【ｺｰｽ（SW1)】、【工程(SW2)】、【▲(SW3)】、【▼(SW4)】、【START/STOP(SW5)】、【真空停止(SW6)】
*               の監視とその処理を行う。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void mainWaitMode(void)
{
	UBYTE	key,sts,data,*pt;
	UWORD	MaxKeyOnTime,Max,Min,Level;
	SWORD	Time;

	glbFlSaveMode = glbFlMainMode;										/* 現在の待機ﾓｰﾄﾞを保存		*/
	
	/*======パック数カウント表示機能======*/
	if(glbFlPackCountDisplay_Start)
	{
		if(mcmFlashWriteCheck())										/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更ﾁｪｯｸ				*/
		{
			glbFlError |= GCN_ERROR_FLASH_DATA_9;
			glbFlMainMode = GCN_ERROR_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
			return;
		}
	
		glbFlPackCountDisplay_Start = 0;
		
		mainPackCountDisplay_Start();
	}
	//表示時間終了？
	if(glbCtPackCountDisplayTime == 0)
	{
		mainPackCountDisplay_End();
			
	}
	/*=====================================*/
	
	if(glbFlContinue == 1)												/* 連続運転ﾓｰﾄﾞの場合		*/
	{
		if(mcmCheckDrawerOpenToClose())									/* 引出しSW 「開き」⇒「閉じる」に変化した場合	*/
		{
			mainPackCountDisplay_End();
			
			mainStartVacuum();											/* 真空運転ﾓｰﾄﾞへ遷移		*/
			mcmWaitTimer(500);											/* 引出し「開」⇒「閉」で500msｳｪｲﾄしてから真空運転を開始する*/
			return;
		}
	}
	if(!(key = glbGetReadKey()))										/* SWの入力ﾁｪｯｸ			*/
	{
		return;															/* 入力がなければﾘﾀｰﾝ		*/
	}
	
	/*======パック数カウント表示取消機能======*/
	if(mainFlPackCountDisplay_Busy)
	{
		mainPackCountDisplay_End();
		return;		
	}
	/*=====================================*/
	
	MaxKeyOnTime = GCN_TIME_1S;											/* SW長押し通知時間	（1秒)			*/
	if(key == GCN_KEY_COURSE_SET)										/* 【ｺｰｽ（SW1）】の場合				*/
	{

		mcmSetBuzzerEffective();										/* 有効ﾌﾞｻﾞｰを鳴らす			*/
		glbFlProc = GCN_PROC_VACUUM;									/* 工程を「真空」にする	*/
		ledControl_Proc(glbFlProc,1,0);									
			
		geefFlCourse++;													/* ｺｰｽNo.更新					*/
		geefFlCourse &= 3;												/* ｺｰｽNo.上限ﾁｪｯｸ				*/
		
		ledControl_Course(geefFlCourse,1,0);					/* LED			*/
				
		glbFlReqDot = 0;												/* 7SEGﾋﾟﾘｵﾄﾞ非表示				*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)		/* "秒"(LED8)表示の場合						*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];				/* 選択されているｺｰｽの真空時間ﾘｰﾄﾞ			*/
			glbFlSecLevel = GCN_VACUUM_SECONDS;							/* 「秒」表示					*/
		}
		else															/* "%"(LED9)表示の場合			*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* 選択されているｺｰｽの真空ﾚﾍﾞﾙﾘｰﾄﾞ				*/
			glbFlSecLevel = GCN_VACUUM_LEVEL;							/* 「%」表示					*/
		}
		ledControl_Sec(glbFlSecLevel,1,0);
		mainDisplay7Seg(data);											/* 真空時間またはﾚﾍﾞﾙを7SEG表示					*/
	}
	else if(key == GCN_KEY_PROC_SET)									/* 工程(SW2)の場合				*/
	{
		mcmSetBuzzerEffective();										/* 有効ﾌﾞｻﾞｰを鳴らす			*/
		glbFlProc++;													/* 工程UP更新				*/
		if(glbFlProc > GCN_PROC_COOL)									/* 工程MAXﾁｪｯｸ				*/
		{
			glbFlProc = GCN_PROC_VACUUM;								/* 工程を「真空」に移動		*/
		}
		ledControl_Proc(glbFlProc,1,0);									
			
		if(glbFlProc == GCN_PROC_VACUUM)								/* 「真空」工程の場合		*/
		{
			glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?	/* */
							GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
		}
		else
		{
			glbFlSecLevel = GCN_VACUUM_SECONDS;
		}
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;				/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
		if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) || 	/* '秒'表示または工程選択が真空以外（ｼｰﾙ、冷却の場合)		*/
           (glbFlProc > GCN_PROC_VACUUM))
		{
			pt = &geefBfCourseData[geefFlCourse][Vacuum];				/* 選択されているｺｰｽの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ		*/
			data = *(pt + glbFlProc );									/* ｺｰｽの工程時間ﾘｰﾄﾞ					*/
		}
		else															/* '%'でかつ工程選択が真空の場合			*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* 選択されているｺｰｽの真空ﾚﾍﾞﾙﾘｰﾄﾞ				*/
		}
		ledControl_Sec(glbFlSecLevel,1,0);		
		mainDisplay7Seg(data);											/* 7SEG表示								*/
	}
	else if(key == GCN_KEY_UP_SET)										/* ▲（SW3)の場合						*/
	{
		sts = 0;
		MaxKeyOnTime = GCN_TIME_1S;										/* ｷｰ長押し通知時間	（1秒)				*/
		do
		{
			if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) ||/* '秒'(LED8)表示または工程が真空以外（ｼｰﾙ、冷却の場合)		*/
                (glbFlProc > GCN_PROC_VACUUM))
			{
				pt = &geefBfCourseData[geefFlCourse][Vacuum];			/* 選択されているｺｰｽの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ		*/
				Time = *(pt + glbFlProc);								/* ｺｰｽNo.に対しての選択された工程時間をﾘｰﾄﾞ				*/
				Time++;													/* 時間更新（+)											*/
				Max = geefBfCourseDataMaxMin[glbFlProc].Max;			/* 選択されている工程値のMAX値ﾘｰﾄﾞ						*/
				if(Time > Max)											/* Maxを超えた場合										*/
				{
					Time = Max;											/* Maxに補正											*/
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす			*/
				}
				else
				{
					if(sts == 0)										/* KEY長押しの時はﾌﾞｻﾞｰを鳴らさない*/
					{
						mcmSetBuzzerEffective();						/* 有効ﾌﾞｻﾞｰを鳴らす			*/
					}
				}
				*(pt + glbFlProc) = Time;								/* 変更時間をTMPﾊﾞｯﾌｧに保存				*/
				mainDisplay7Seg(Time);									/* 7SEG表示								*/
			}
			else														/* '%'表示でかつ工程が真空指定の場合	*/
			{
				Level = geefBfCourseData[geefFlCourse][VacuumLevel];	/* 選択されているｺｰｽの真空ﾚﾍﾞﾙﾘｰﾄﾞ		*/
				Level++;
				Max = geefBfCourseDataMaxMin[VacuumLevel].Max;			/* 選択されている工程値のMAX値ﾘｰﾄﾞ		*/
				if(Level > Max)											/* MAXを超えた場合						*/
				{
					Level = Max;
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす			*/
				}
				else
				{
					if(sts == 0)										/* SWが押された最初だけﾌﾞｻﾞｰを鳴らす	*/
					{
						mcmSetBuzzerEffective();						/* 有効ﾌﾞｻﾞｰを鳴らす			*/
					}
				}
				geefBfCourseData[geefFlCourse][VacuumLevel] = Level;	/* 選択されているｺｰｽの真空ﾚﾍﾞﾙｾｯﾄ		*/
				mainDisplay7Seg(Level);									/* 7SEG表示								*/
			}
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEYが離されるのを待つ				*/
			MaxKeyOnTime = GCN_TIME_100MS;								/* SW長押し通知時間	（0.1秒)			*/
																		/* SWが押されている間は早送りにするため、ここで長押し時間変更　*/
		}while(sts == 1);
		DI();
		glbFlKydt = 0;													/* ﾘﾋﾟｰﾄ中に入力した他ｷｰは捨てる		*/
		EI();
	}
	else if(key == GCN_KEY_DOWN_SET)									/* ▼(SW4)の場合						*/
	{
		sts = 0;
		MaxKeyOnTime = GCN_TIME_1S;										/* ｷｰ長押し通知時間	（1秒)				*/
		do																/* KEYが押されている間は早送り							*/
		{
			if((geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS) || /* '秒'(LED8)表示または工程が真空以外（ｼｰﾙ、冷却の場合)		*/
                (glbFlProc > GCN_PROC_VACUUM))
			{
				pt = &geefBfCourseData[geefFlCourse][Vacuum];			/* 選択されているｺｰｽの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ						*/
				Time = *(pt + glbFlProc);								/* ｺｰｽNo.に対しての選択された工程時間をﾘｰﾄﾞ				*/
				Time--;													/* 時間更新（-)											*/
				Min = geefBfCourseDataMaxMin[glbFlProc].Min;			/* 選択されている工程値のMIN値ﾘｰﾄﾞ						*/
				if(Time < (SWORD)Min)									/* Minを下回った場合									*/
				{
					Time = Min;
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす					*/
				}
				else
				{
					if(sts == 0)										/* SWが押された最初だけﾌﾞｻﾞｰを鳴らす	*/
					{
						mcmSetBuzzerEffective();						/* 有効ﾌﾞｻﾞｰを鳴らす					*/
					}
				}

				*(pt + glbFlProc) = Time;								/* 変更時間をTMPﾊﾞｯﾌｧに保存				*/
				mainDisplay7Seg(Time);									/* 7SEG表示								*/
			}
			else														/* '%'表示でかつ工程が真空指定の場合	*/
			{
				Level = geefBfCourseData[geefFlCourse][VacuumLevel];	/* 選択されているｺｰｽの真空ﾚﾍﾞﾙﾘｰﾄﾞ		*/
				Level--;
				Min = geefBfCourseDataMaxMin[VacuumLevel].Min;			/* 選択されている工程値のMAX値ﾘｰﾄﾞ						*/
				if(Level < Min)											/* Minを下回った場合									*/
				{
					Level = Min;
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす			*/
				}
				else
				{
					if(sts == 0)									    /* SWが押された最初だけﾌﾞｻﾞｰを鳴らす	*/
					{
						mcmSetBuzzerEffective();						/* 有効ﾌﾞｻﾞｰを鳴らす			*/
					}
				}
				geefBfCourseData[geefFlCourse][VacuumLevel] = Level;	/* 選択されているｺｰｽの真空ﾚﾍﾞﾙｾｯﾄ						*/
				mainDisplay7Seg(Level);									/* 7SEG表示								*/
			}
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEYが離されるのを待つ				*/
			MaxKeyOnTime = GCN_TIME_100MS;								/* SW長押し通知時間	（0.1秒)			*/
																		/* SWが押されている間は早送りにするため、ここで長押し時間変更　*/
		}while(sts == 1);
		DI();
		glbFlKydt = 0;													/* ﾘﾋﾟｰﾄ中に入力した他ｷｰは捨てる		*/
		EI();
	}
	else if(key == GCN_KEY_UPDOWN_SET)									/* ▲▼(SW3,SW4)同時					*/
	{
		if(glbFlProc > GCN_PROC_VACUUM)									/* 真空工程(LED5)以外の場合			*/
		{
			mcmSetBuzzerInvalid();										/* 無効ﾌﾞｻﾞｰを鳴らす			*/
			return;
		}
		mcmSetBuzzerEffective();										/* 有効ﾌﾞｻﾞｰを鳴らす			*/
		geefFlVacuumType[geefFlCourse]++;								/* 真空引きﾀｲﾌﾟ更新(0:秒,1:%)	*/
		geefFlVacuumType[geefFlCourse] &= 1;							/* 上限ﾁｪｯｸ						*/
		glbFlSecLevel       = geefFlVacuumType[geefFlCourse] ;			/* '秒'(LED8)　or '%'(LED9) ﾘｰﾄﾞ	*/

		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)		/*'秒'(LED8)表示				*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];				/* 選択されているｺｰｽの真空時間ﾘｰﾄﾞ		*/
		}
		else															/*'%'(LED9)表示				*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];			/* 選択されているｺｰｽの真空ﾚﾍﾞﾙ			*/

		}
		ledControl_Sec(glbFlSecLevel,1,0);
		mainDisplay7Seg(data);											/* 7SEG表示								*/
	}
	else if(key == GCN_KEY_START_SET)									/* START/STOP(SW5)の場合				*/
	{

		if((DrawerSwRead() == 1))								/* 引出しSWが「閉」の場合			*/
		{
			mcmSetBuzzerEffective();									/* 有効ﾌﾞｻﾞｰを鳴らす			*/
			glbFlMainMode = GCN_WARM_MODE;						/* 暖気運転ﾓｰﾄﾞへ遷移		*/
			mainWarmStart();									/* 暖気運転開始						*/
		}
	}
	else if(key == GCN_KEY_PUSH_SET)									/* 				*/
	{
		sts = 1;
		MaxKeyOnTime = GCN_TIME_1S;										/* ｷｰ長押し通知時間	（1秒)				*/
		while(sts == 1)													/* SWが１秒以内に離されたらﾙｰﾌﾟから抜ける	*/
		{
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEYが離されるのを待つ				*/
			if(sts == 1)												/* 1秒間以上KEYが押されている場合		*/
			{
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
				glbFlMainMode = GCN_SYSTEM_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｼｽﾃﾑﾃﾞｰﾀ設定ﾓｰﾄﾞへ	*/

				break;
			}
		}
	}
	else if(key == GCN_KEY_STOP_SET)									/* PUSH（隠しSW7)の場合		*/
	{
		sts = 1;
		MaxKeyOnTime = GCN_TIME_1S;										/* ｷｰ長押し通知時間	（1秒)				*/
		while(sts == 1)													/* KEYが押されている間は早送り							*/
		{
			sts = glbWaitKeyOff(key,MaxKeyOnTime);						/* KEYが離されるのを待つ				*/
			if(sts == 1)												/* 1秒間以上KEYが押されている場合		*/
			{
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
				
				geefCtPack = 0;
				break;
			}
			else
			{
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
				mainPackCountDisplay_Start();
				
			}
		}

	
	}
	else																/* 上記以外のSWの場合			*/
	{
		mcmSetBuzzerInvalid();											/* 無効ﾌﾞｻﾞｰを鳴らす			*/

	}

}

/********************************************//*
	カウント表示開始
************************************************/
void mainPackCountDisplay_Start(void)
{
	uint16_t data,hundredsDigit;
	
	mainFlPackCountDisplay_Busy = 1;	//表示中
	
	glbCtPackCountDisplayTime = 800;	//4000ms
	
	glbFlReqDot = 0;	
	
	hundredsDigit = geefCtPack/100;								/* パック数 100の位			*/
	

	ledControl_Sec(ALL_LED_SELECT,0,0);	

	
	if(!hundredsDigit)		//2桁表示用
	{
		data = geefCtPack%100;
		mainDisplay7Seg(data);										/* パック数表示			*/
	}
	else					//3桁　1/10で表示用
	{
		glbFlReqDot = 2;	
		data = geefCtPack/10;
		mainDisplay7Seg(data);										/* パック数表示			*/
		
	}
}


/********************************************//*
	カウント表示終了
************************************************/
void mainPackCountDisplay_End(void)	
{
	UWORD	data;
	
	if(!mainFlPackCountDisplay_Busy)
	{
		return;
	}
	
	mainFlPackCountDisplay_Busy = 0;
	
	glbFlReqDot = 0;											/* 「真空」工程のため、ﾋﾟﾘｵﾄﾞ表示なし					*/
	glbFlProc 		= GCN_PROC_VACUUM;							/* 工程を「真空」にする	*/
	ledControl_Proc(glbFlProc,1,0);											
		
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];			/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/
	}
	else
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/

	}
	
	ledControl_Sec(glbFlSecLevel,1,0);	
	mainDisplay7Seg(data);										/* 7SEG表示					*/
}

/***********************************************************************************************************************
*    関数名    mcmSetSystemData
*    引  数    
*    戻り値    
*    関数機能  システムデータ設定モード
*    処理内容  [Ｔ１]:真空停止時間,[Ｔ２]:暖機運転時間,[Ｔ３]:真空タイムアウト,[Ｔ４]:予約運転時WAIT時間
*              [Ｔ５]:運転終了後WAIT時間
*    作成日    2018年6月16日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmSetSystemData(void)
{
	UBYTE	saveFlCourse,saveFlSecLevel,saveFlContinue ;
	UBYTE	ctLed=0;
	UWORD	data;
	UBYTE	key,sts,*pt;
	UWORD	MaxKeyOnTime,MaxTime,MinTime;
	SWORD	Time;

	saveFlCourse = geefFlCourse;										/* ｺｰｽNo.ｾｰﾌﾞ			*/
	saveFlSecLevel = glbFlSecLevel;										/* 「秒」or 「%」指定ｾｰﾌﾞ	*/
	saveFlContinue = glbFlContinue;										/* 連続運転ﾓｰﾄﾞﾌﾗｸﾞｾｰﾌﾞ		*/
	glbFlReqDot = 0;													/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
	Time = geefBfSystemData[ctLed][Default];							/* ｼｽﾃﾑﾃﾞｰﾀﾘｰﾄﾞ								*/
	mainDisplay7Seg(Time);												/* 7SEG表示					*/
	mcmSetLed(ctLed);													/* 指定のLED点灯							*/
	
	while(1)
	{
		if((key = glbGetReadKey()))										/* SWの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_STOP_SET)									/* STOP（隠しSW6)の場合					*/
			{
				ctLed++;												/* T1～T11のﾘｰﾄﾞｶｳﾝﾀ更新					*/
				if(ctLed > 10)											/* T11を超えた場合							*/
				{
					ctLed = 0;											/* ﾘｰﾄﾞｶｳﾝﾀをT1に移動						*/
				}
				mcmSetLed(ctLed);										/* 指定のLED点灯							*/
				if(ctLed < 5)											/* T1～T5の範囲の場合						*/
				{
					Time = geefBfSystemData[ctLed][Default];			/* ｼｽﾃﾑﾃﾞｰﾀﾘｰﾄﾞ								*/
					mainDisplay7Seg(Time);								/* 7SEG表示					*/
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]は0.1秒単位のため、7SEGﾋﾟﾘｵﾄﾞ表示	*/
				}
				else
				{
					glbDt7Seg2 = 										/* 7SEGの2桁目に'-'表示		*/
					glbDt7Seg1 = GCN_7SEG_HAIPHONG;						/* 7SEGの2桁目に'-'表示		*/
				    glbFlReqDot = 0;									/* 7SEGﾋﾟﾘｵﾄﾞ表示無し		*/
				}
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
			}
			else if(key == GCN_KEY_UP_SET)								/* ▲(SW3)の場合						*/
			{
				if(ctLed < 5)											/* T1～T5の場合							*/
				{
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]は0.1秒単位のため、7SEGﾋﾟﾘｵﾄﾞ表示	*/
					sts = 0;
					MaxKeyOnTime = GCN_TIME_1S;							/* ｷｰ長押し通知時間	（1秒)				*/
					do
					{
						pt = &geefBfSystemData[ctLed][Default];			/* 選択されているｼｽﾃﾑﾃﾞｰﾀの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ		*/
						Time = *pt;										/* 選択されたｼｽﾃﾑ時間をﾘｰﾄﾞ				*/
						Time++;											/* 時間更新（+)											*/
						MaxTime = geefBfSystemData[ctLed][Max];			/* 選択されている工程値のMAX値ﾘｰﾄﾞ						*/
						if(Time > MaxTime)								/* Maxを超えた場合										*/
						{
							Time = MaxTime;								/* Maxに補正											*/
							mcmSetBuzzerInvalid();						/* 無効ﾌﾞｻﾞｰを鳴らす			*/
						}
						else
						{
							if(sts == 0)								/* KEY長押しの時はﾌﾞｻﾞｰを鳴らさない*/
							{
								mcmSetBuzzerEffective();				/* 有効ﾌﾞｻﾞｰを鳴らす			*/
							}
						}
						*pt = Time;										/* 変更時間をTMPﾊﾞｯﾌｧに保存								*/
						mainDisplay7Seg(Time);							/* 7SEG表示								*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* KEYが離されるのを待つ				*/
						MaxKeyOnTime = GCN_TIME_100MS;					/* ｷｰ長押し通知時間	（0.1秒)			*/
					}while(sts == 1);
				}
				else													/* T6～T11の場合							*/
				{
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす			*/
				}
			}
			else if(key == GCN_KEY_DOWN_SET)							/* ▼(SW4)の場合						*/
			{
				if(ctLed < 5)											/* T1～T5の場合							*/
				{
				    glbFlReqDot = (ctLed > 2)?1:0;						/* [T3][T4]は0.1秒単位のため、7SEGﾋﾟﾘｵﾄﾞ表示	*/
					sts = 0;
					MaxKeyOnTime = GCN_TIME_1S;							/* ｷｰ長押し通知時間	（1秒)				*/
					do
					{
						pt = &geefBfSystemData[ctLed][Default];			/* 選択されているｼｽﾃﾑﾃﾞｰﾀの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ		*/
						Time = *pt;										/* 選択されたｼｽﾃﾑ時間をﾘｰﾄﾞ				*/
						Time--;											/* 時間更新（-)											*/
						MinTime = geefBfSystemData[ctLed][Min];			/* 選択されている工程値のMAX値ﾘｰﾄﾞ						*/
						if(Time < (SWORD)MinTime)						/* Minを超えた場合										*/
						{
							Time = MinTime;								/* Maxに補正											*/
							mcmSetBuzzerInvalid();						/* 無効ﾌﾞｻﾞｰを鳴らす			*/
						}
						else
						{
							if(sts == 0)								/* KEY長押しの時はﾌﾞｻﾞｰを鳴らさない*/
							{
								mcmSetBuzzerEffective();				/* 有効ﾌﾞｻﾞｰを鳴らす			*/
							}
						}
						*pt = Time;										/* 変更時間をTMPﾊﾞｯﾌｧに保存								*/
						mainDisplay7Seg(Time);							/* 7SEG表示								*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* KEYが離されるのを待つ				*/
						MaxKeyOnTime = GCN_TIME_100MS;					/* ｷｰ長押し通知時間	（0.1秒)			*/
					}while(sts == 1);
				}
				else													/* T6～T11の場合							*/
				{
					mcmSetBuzzerInvalid();								/* 無効ﾌﾞｻﾞｰを鳴らす			*/
				}
			}
			else if(key == GCN_KEY_START_SET)							/* START/STOP(SW5)の場合							*/
			{
				glbFlProc =  GCN_PROC_VACUUM;							/* 工程を真空へ							*/
				ledControl_Proc(glbFlProc,1,0);							
					
				geefFlCourse = saveFlCourse;							/* ｺｰｽLEDを元の状態へ戻す				*/
				ledControl_Course(geefFlCourse,1,0);					/* LED			*/
				glbFlSecLevel = saveFlSecLevel;							/* '秒'(LED8) or '%'(LED9)を元の状態へ戻す	*/
				glbFlContinue = saveFlContinue;							/* 連続(LED10)を元の状態へ戻す				*/
			    glbFlReqDot = 0;										/* 7SEGﾋﾟﾘｵﾄﾞ非表示	*/
				if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)/* "秒"表示の場合						*/
				{
					data = geefBfCourseData[geefFlCourse][Vacuum];		/* 選択されているｺｰｽの真空時間ﾘｰﾄﾞ				*/
					glbFlSecLevel = GCN_VACUUM_SECONDS;					/* 「秒」表示					*/
				}
				else													/* "%"表示の場合								*/
				{
					data = geefBfCourseData[geefFlCourse][VacuumLevel];	/* 選択されているｺｰｽの真空ﾚﾍﾞﾙﾘｰﾄﾞ				*/
					glbFlSecLevel = GCN_VACUUM_LEVEL;					/* 「%」表示					*/
				}
				ledControl_Sec(glbFlSecLevel,1,0);
				mainDisplay7Seg(data);									/* 真空時間またはﾚﾍﾞﾙを7SEG表示					*/
				if(mcmFlashWriteCheck())									/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更とﾌﾗｯｼｭ書き込みﾁｪｯｸ				*/
				{
					glbFlError |= GCN_ERROR_FLASH_DATA_9;
					glbFlMainMode = GCN_ERROR_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
					return;
				}
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
				glbFlMainMode = GCN_WAIT_MODE;							/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/
				return;
			}
			else if(key == GCN_KEY_PUSH_SET)									
			{		
				geefCtPack += 10;
				if(geefCtPack > 999)		/* 999回を超えた場合			*/
				{
					geefCtPack = 999	;	/* 999回を上限とする			*/
				}
				mcmSetBuzzerEffective();								/* 有効ﾌﾞｻﾞｰを鳴らす			*/
			}
			else				
			{
				mcmSetBuzzerInvalid();									/* 無効ﾌﾞｻﾞｰを鳴らす			*/
			}	
		}
	}
	
}

/***********************************************************************************************************************
*    関数名    mcmSetLed
*    引  数    LED No.
*    戻り値    
*    関数機能  選択されたＴ１～Ｔ１１の項目に対して任意のＬＥＤを点灯させる
*    処理内容  
* 
*    作成日    2018年6月16日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmSetLed(UBYTE	ctLed)
{
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Course(ALL_LED_SELECT,0,0);					/* LED			*/
	ledControl_Sec(ALL_LED_SELECT,1,0);	
	glbFlContinue = 0;
	glbFlLed11 = 0;								/* T11*/

	switch(ctLed)	{
		case	0:								/* T1～T3の範囲は工程LEDを点灯	*/
		case	1:
		case	2:
			ledControl_Proc(ctLed,1,0);										
			break;
		case	3:								/* T4～T7の範囲はｺｰｽLEDを点灯	*/
		case	4:
		case	5:
		case	6:
			ledControl_Course((ctLed - 3),1,0);					/* LED			*/
			break;
		case	7:								/* T8～T9の範囲は「秒」LED or 「%」LEDを点灯	*/
		case	8:
			ledControl_Sec(ctLed - 7,1,0);
			break;
		case	9:								/* T10は「連続」LEDを点灯	*/
			glbFlContinue = 1;
			break;
		case   10:
			glbFlLed11 = 1;						/* T11*/
			break;
	}
}

/***********************************************************************************************************************
*    関数名    mainErrorMode
*    引  数    
*    戻り値    
*    関数機能  エラー検出モード
*    処理内容  エラー発生時に本モードへ遷移。
*              ＷＤＴエラー以外はここで無限ループ。
*              エラー情報をフラッシュメモリへ書き込み。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainErrorMode(void)
{
	SWORD	i;
	UBYTE	checkBit;
	UBYTE	ErrorKind = 0,Proc=0,ErrorNo=0;
	volatile UBYTE	err;

	glbFlChildBusy = 0;
	DI();
	P7 &= P74_PUMP_START_OFF;									/* ﾎﾟﾝﾌﾟ稼働停止						*/ 
	

	P7 &= P74_PUMP_START_OFF;									/* ﾎﾟﾝﾌﾟ稼働停止						*/ 
	P7 &= P73_HEATER_START_OFF;									/* ﾋｰﾀOFF								*/
	P7 &= P75_SOFTRELEASE_START_OFF;							/* ｿﾌﾄ解放弁OFF							*/
	EI();

	P2 &= P20_VACUUM_ELECTROMAG_OFF;							/* 真空電磁弁OFF						*/
	P4 &= P41_VACUUM_RELEASE_OFF;								/* 真空解放弁OFF						*/ 

	P0 &= P01_SEEL_ELECTROMAG_OFF;								/* ｼｰﾙ電磁弁OFF							*/
	P13 &= P130_GUS_ELECTROMAG_OFF;								/* ｶﾞｽ電磁弁 OFF						*/

	i=0;
	checkBit = 0x01;											/* ｴﾗｰ判定ﾋﾞｯﾄ初期化 					*/
	while(i<8)													/* 該当するｴﾗｰNo.を検索					*/
	{
		if((glbFlError & checkBit) != 0)						/* 任意のｴﾗｰﾋﾞｯﾄがｾｯﾄされているかﾁｪｯｸする	*/
		{
			break;												/*ﾋﾞｯﾄがｾｯﾄされていればﾙｰﾌﾟから抜ける	*/
		}
		i++;
		checkBit <<= 1;
	}
	glbFlReqDot         = 0;									/* ﾋﾟﾘｵﾄﾞ表示無し					*/
	glbDt7Seg2 = 'E' - 0x37;									/* 7SEGの2桁目に'E'を表示				*/
	ErrorNo    = 
	glbDt7Seg1 = (UBYTE)(mainTblError[i]);					/* 7SEGの1桁目にｴﾗｰNoを表示				*/

	glbCtKeyOffTimer = 0;										/* KEY OFFｶｳﾝﾀ更新			*/
	glbCtDriveTimer = 0;										/* ﾀｲﾏ値更新					*/
	glbCtVacuumEmergncyTimer = 0;								/* ﾀｲﾏ値更新					*/
	glbCtGusInclusionTimer   = 0;								/* ｶﾞｽ封入工程実施中？			*/
	glbCtGusStubilityTimer   = 0;								/* ｶﾞｽ安定時間ｶｳﾝﾄ中？			*/
	glbCtSeelTimer           = 0;								/* ｼｰﾙ工程実施中？			*/
	glbCtWarmTimer           = 0;								/* 暖気運転実施中？				*/
	glbCtVacuumeWarm         = 0;								/* ﾀｲﾏ値更新					*/
	glbCtFunTimer            = 0;								/* ﾀｲﾏ値更新					*/
	glbFlSysHeaterReq        = 0;								/* ｼｽﾃﾑﾓｰﾄﾞﾋｰﾀ監視要求有?		*/
	glbCtSysHeaterOffTimer    = 0;								/* 2秒監視ﾀｲﾏｶｳﾝﾀ更新			*/
	glbFlVacuumElectromangReq = 0;								/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求有り？*/
	glbCtVacuumElectromang   = 0;								/* 10秒ﾀｲﾏ更新					*/
	glbCtHeaterBlockTimer    = 0;								/* 0.7秒ﾀｲﾏ更新					*/	
	glbCtVacuumTimeout1       = 0;								/* ﾀｲﾏ更新						*/
	glbCtVacuumTimeout2       = 0;								/* ﾀｲﾏ更新						*/
	glbCtVacuumTimeout3       = 0;								/* ﾀｲﾏ更新						*/

	if((glbFlSaveMode == GCN_DRIVE_MODE) || 					/* 真空運転中または暖気運転中の場合	*/
       (glbFlSaveMode == GCN_WARM_MODE))							
	{
		switch(glbFlDriveMode)	{
			case	GCN_DRIVE_START_MODE:						/* 真空引き開始時		*/
			case	GCN_VACUUM_PULL_MODE:						/* 真空引き工程			*/
			case	GCN_VACUUM_STOP_MODE:						/* 真空引き一時停止		*/
				Proc = 1;										/* 真空工程時にｴﾗｰ発生	*/
				break;
			case	GCN_GUS_INCLUSIOM_MODE:						/* ｶﾞｽ封入工程			*/
			case	GCN_GUS_STUBILITY_MODE:						/* ｶﾞｽ安定待ち工程		*/
				Proc = 2;										/* ｶﾞｽ工程時にｴﾗｰ発生	*/
				break;
			case	GCN_SEEL_MODE:								/* ｼｰﾙ工程				*/
			case	GCN_HEATER_BLOCK_MODE:						/*  ﾋｰﾀﾌﾞﾛｯｸ上昇待ち処理（０．７秒）*/
				Proc = 3;										/* ｼｰﾙ工程時にｴﾗｰ発生	*/
				break;
			case	GCN_SEEL_COOLING_MODE:								
				Proc = 4;										/* ｼｰﾙ冷却工程時にｴﾗｰ発生*/
				break;
			default:
				Proc = 0;										/* 工程運転以外			*/
		}
		
		ErrorKind = ((geefFlCourse+1) << 4) | Proc;				/* 上位4ﾋﾞｯﾄ：ｴﾗｰ発生時のｺｰｽ番号,下位4ﾋﾞｯﾄ:その時の工程		*/
	}
	for(i=8;i>=0;i--)											/* ｴﾗｰ履歴ﾃｰﾌﾞﾙを１つ古くする	*/
	{
		geefBfErrorHistory[i+1][0] = geefBfErrorHistory[i][0];	/* ｴﾗｰ要因移動			*/
		geefBfErrorHistory[i+1][1] = geefBfErrorHistory[i][1];	/* ｴﾗｰ発生時のｺｰｽNoと工程移動	*/
	}
	geefBfErrorHistory[0][0] = ErrorNo; 						/* 最新のｴﾗｰ発生内容保存				*/
	geefBfErrorHistory[0][1] = ErrorKind; 						/* 最新のｴﾗｰ発生内容保存				*/
	err = pdlWriteFlash();										/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀをﾌﾗｯｼｭへ書き込み				*/

	mcmSetBuzzerError();
	
	powerLedControl(2,5,3);		//エラー点滅
	
	if(glbFlError & GCN_ERROR_WATCHDOG_CPU_8)					/* ｳｫｯﾁﾄﾞｯｸﾀｲﾏｴﾗｰの場合はﾘﾀｰﾝ				*/
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
			if(glbFlPowerSwLongPalse)	//ResetSwで操作可能
			{
				WDTE = 0xFFU;  				/* 内部ﾘｾｯﾄをかけるために、WDTに*/
			}
		}
	
	}
	
}


/***********************************************************************************************************************
*    関数名    mainStanbyMode
*    引  数    
*    戻り値    
*    関数機能  
*    処理内容  
*    作成日    
*    作成者   
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
		
		mcmSetBuzzerEffective();									/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/	
	
		ledControl_Course(geefFlCourse,1,0);					/* LED			*/
		glbFlProc = GCN_PROC_VACUUM;
		ledControl_Proc(glbFlProc,1,0);										

		glbFlMainMode = GCN_WAIT_MODE;													/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* 真空工程が[秒]表示の場合						*/
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];								/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/
		}
		else																			/* 真空工程が[%]表示の場合						*/
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];							/* 選択されているｺｰｽ1の真空ﾚﾍﾞﾙ					*/

		}
		glbFlSecLevel = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?			/* 真空工程が[秒][%]指定をLED反映				*/
						GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;							

		ledControl_Sec(glbFlSecLevel,1,0);		
		mainDisplay7Seg(data);															/* 真空工程時間またはﾚﾍﾞﾙを7SEGに表示			*/

		mainFlDrawer = 0;	
	}
}
/***********************************************************************************************************************
*    関数名    mainStanbyCheck
*    引  数    
*    戻り値    
*    関数機能  
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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

		glbFlReqDot = 2;	//電源ついているのか？故障なのか分からないからSEGのドットLED表示	
	}
}


/***********************************************************************************************************************
*    関数名    mainStartVacuum
*    引  数    
*    戻り値    
*    関数機能  真空運転前処理
*    処理内容  真空運転に遷移。その前にバックアップデータ変更があればフラッシュメモリへ書き込み
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainStartVacuum(void)
{	
	if(mcmFlashWriteCheck())										/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更ﾁｪｯｸ				*/
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;
		glbFlMainMode = GCN_ERROR_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
		return;
	}		

	glbFlMainMode 		= GCN_DRIVE_MODE;						/* 運転ﾓｰﾄﾞへ遷移		*/
	glbFlDriveMode 		= GCN_DRIVE_START_MODE;					/* 運転ﾓｰﾄﾞ時の状態を運転開始に設定	*/
	glbFlProc 			= GCN_PROC_VACUUM;						/* 工程を「真空」に移動				*/
	ledControl_Proc(glbFlProc,1,0);											
	
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)	/* '秒'(LED8)指定の場合				*/
	{
		glbFlSecLevel       = GCN_VACUUM_SECONDS;
	}
	else														/* '%'(LED9)指定の場合				*/
	{
		glbFlSecLevel       = GCN_VACUUM_LEVEL;
	}
	
	ledControl_Sec(glbFlSecLevel,1,0);
	glbFlReqDot         = 0;									/* ﾋﾟﾘｵﾄﾞ表示無し					*/
}
/***********************************************************************************************************************
*    関数名    glbGetReadKey
*    引  数    
*    戻り値    ＳＷ種別
*    関数機能  ＳＷ入力処理
*    処理内容  ５ｍｓタイマ割り込みで検出した7つのＳＷのＡＣＴＩＶＥ検出通知を入力する
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

UBYTE	glbGetReadKey(void)
{
	UBYTE	key=0;

	DI();
	R_WDT_Restart();											/* ｳｫｯﾁﾄﾞｯｸﾀｲﾏﾘｾｯﾄ							*/
	if(glbFlKydt & 0x02) 										/* ｺｰｽ(SW1)の場合				*/
	{
		key = GCN_KEY_COURSE_SET;								/* ｺｰｽ(SW1)ON認識				*/
	}
	else if(glbFlKydt & 0x04)									/* 工程(SW2)の場合				*/
	{
		key = GCN_KEY_PROC_SET;									/* 工程(SW2)ON認識				*/
	}
	else if(glbFlKydt & 0x08)									/* ▲(SW3)の場合				*/
	{
		key = GCN_KEY_UP_SET;									/* ▲(SW3)ON認識				*/
		if(glbFlKydt & 0x10)									/* 更に▼(SW4)も押されている場合	*/
		{
			key = GCN_KEY_UPDOWN_SET;							/* ▲▼同時認識					*/
		}
	}
	else if(glbFlKydt & 0x10)									/* ▼(SW4)の場合				*/
	{
		key = GCN_KEY_DOWN_SET;									/* ▼(SW4)ON認識					*/
	}

	else if(glbFlKydt & 0x20)									/* START/STOP(SW5)の場合		*/
	{
		key = GCN_KEY_START_SET;								/* START/STOP(SW5)ON認識		*/
	}
	else if(glbFlKydt & 0x40)									/* 真空停止(SW6)の場合			*/
	{
		key = GCN_KEY_STOP_SET;									/* 真空停止(SW6)認識			*/
	}
	else if(glbFlKydt & 0x80)									/* PUSH（基盤裏側)の場合		*/
	{
		key = GCN_KEY_PUSH_SET;									/* PUSH（基盤裏側)ON認識		*/
	}
	glbFlKydt ^= glbFlKydt;										/* SW入力ﾋﾞｯﾄｸﾘｱ	*/
	EI();
	return(key);
}

/***********************************************************************************************************************
*    関数名    glbWaitKeyOff
*    引  数    SW　No.
*    戻り値    0:指定時間内に離された、1:指定時間押されている
*    関数機能  
*    処理内容  ＫＥＹが指定時間押されているか監視
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

UBYTE   glbWaitKeyOff(UBYTE KeyNo,UWORD KeyOnMax)
{
	UBYTE	keydata[7] = {P21_KEY_SW1_COURSE_OFF,				/* ｺｰｽ(SW1)		*/
						  P22_KEY_SW2_PROCESS_OFF,				/* 工程(SW2)	*/
						  P23_KEY_SW3_UP_OFF,					/* ▲(SW3)		*/
						  P24_KEY_SW4_DOWN_OFF,					/* ▼(SW4)		*/
						  P25_KEY_SW5_START_OFF,				/* START/STOP(SW5)	*/
						  P26_KEY_SW6_STOP_OFF,					/* 真空停止(SW6)	*/
						  P27_KEY_SW7_PUSH_OFF					/* PUSH*/
						};

	DI();
	glbCtKeyOffTimer = 0;										/* ｶｳﾝﾀﾘｾｯﾄ								*/
	EI();
	KeyNo--;													/* SW No. 補正								*/
	KeyOnMax /= GCN_INT_TIME;									/* 時間を5ms単位に変換						*/
	while(1)
	{
		if(glbCtKeyOffTimer >= KeyOnMax)						/* 指定時間KEYが押された場合				*/
		{
			return	1;
		}
		if(iodInKey() & (keydata[KeyNo]))						/* 指定時間内にKEYが離された場合　(Hの時にKEY OFF)	*/
		{
			return 0;
		}
	}
}

/***********************************************************************************************************************
*    関数名    glbWaitKeyOn
*    引  数    
*    戻り値    0:SWが押されている、1:SWが離された
*    関数機能  指定のＳＷが押されているか否かのﾁｪｯｸ
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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
		if(!(iodInKey() & (keydata[KeyNo])))	                /* SWが押されているか	*/
		{
			return 0;											/* SWが押されている場合	*/
		}
		else
		{	
			return 1;											/* SWが離された場合	*/
		}
	}
}

/***********************************************************************************************************************
*    関数名    mainDisplay7Seg
*    引  数    
*    戻り値    
*    関数機能  ７ＳＥＧに表示させる数字を２ケタにする
*    処理内容  ここでセットした変数の値を１ｍｓタイマ割り込みで読み込み、７ＳＥＧに表示する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void mainDisplay7Seg(UWORD data)
{
		
	glbDt7Seg2 = data/10;					/* ｺｰｽNo.に対しての選択された工程時間ﾘｰﾄﾞし、2桁目をｾｯﾄ	*/
	glbDt7Seg1 = data%10;					/* ｺｰｽNo.に対しての選択された工程時間ﾘｰﾄﾞし、1桁目をｾｯﾄ	*/
}


/***********************************************************************************************************************
*    関数名    mainDisplayWarning7Seg
*    引  数    
*    戻り値    
*    関数機能  真空警告エラー[Ｅ３]の７ＳＥＧ表示データ作成
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/


void mainDisplayWarning7Seg(UWORD data)
{

	glbDt7Seg2 = 'E' - 0x37;				/* 'E'表示	*/
	glbDt7Seg1 = 3;							/* '3'表示	*/
}
	
/***********************************************************************************************************************
*    関数名    glbGetDipSW()
*    引  数    
*    戻り値    
*    関数機能  ＤＩＰＳＷの読み込み
*    処理内容  ＬでＤＩＰＳＷ ＯＮ
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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

	glbFlDipSW = dipsw;											/* DIPSWの状態ｾｯﾄ				*/
	return;
}

/***********************************************************************************************************************
*    関数名    mainPortOutputInitial
*    引  数    
*    戻り値    
*    関数機能  出力ポート初期化
*    処理内容  出力ポートを非アクティブにする。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainPortOutputInitial(void)
{
	P1 = 0x00;										/* 7SEG 8bit All Off	*/
	P5  |= (P50_LEDCOM1_OFF | P51_LEDCOM2_OFF);		/* LEDCOM1,2,3,4 OFF	*/
	P3  |= (P30_LEDCOM3_OFF);
	P7  |= (P70_LEDCOM4_OFF);
	P7  &= (P72_POWER_LED_OFF);						/* 電源SW				*/
	P7  &= (P73_HEATER_START_OFF);					/* ﾋｰﾀOFF			*/
	P7  &= (P74_PUMP_START_OFF);					/* ﾎﾟﾝﾌﾟOFF			*/
	P7  &= (P75_SOFTRELEASE_START_OFF);				/* ｿﾌﾄ解放弁OFF		*/
	P2  &= (P20_VACUUM_ELECTROMAG_OFF);				/* 真空電磁弁OFF	*/
	P13  &= (P130_GUS_ELECTROMAG_OFF);				/* ｶﾞｽ電磁弁OFF		*/
	P0  &= (P01_SEEL_ELECTROMAG_OFF);				/* ｼｰﾙ電磁弁OFF		*/
	P4  &= (P41_VACUUM_RELEASE_OFF);				/* 真空解放弁OFF	*/
	
}


/***********************************************************************************************************************
*    関数名    mainGlobalClear
*    引  数    
*    戻り値    
*    関数機能  共通メモリクリア
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainGlobalClear(void)
{
	geefFlCourse = 0;										/* ｺｰｽ指定ﾌﾗｸﾞ（0:ｺｰｽ1,1:ｺｰｽ2,2:ｺｰｽ3,3:ｺｰｽ4)	*/
	glbFlReqDot=0;										/* 7SEG ﾋﾟﾘｵﾄﾞ表示要求ﾌﾗｸﾞ							*/
	glbFlLedCom=0;										/* LED COM1～COM4のｶｳﾝﾀ（1～4のどのCOMに出力するか	*/
	glbCtKeyOffTimer=0;
	glbFlKydt=0;										/* KEY入力ﾃﾞｰﾀ				*/
														/* bit0:ｺｰｽ変更/設定ｷｰ		*/
														/* bit1:▲ｷｰ				*/
														/* bit2:▼ｷｰ				*/
														/* bit3:ｽﾀｰﾄ/ｽﾄｯﾌﾟｷｰ		*/
	glbFlMainMode=0;									/* ﾒｲﾝﾓｰﾄﾞﾌﾗｸﾞ			*/
	glbDt7Seg2=GCN_7SEG_ALLOFF;							/* 7SEG 2桁目ﾃﾞｰﾀ (ALL OFF)		*/
	glbDt7Seg1=GCN_7SEG_ALLOFF;							/* 7SEG 1桁目ﾃﾞｰﾀ (ALL OFF)		*/
	glbFlDipSW=0;										/* Dip SW				*/
	glbFlSysytem7Seg=0;									/* ｼｽﾃﾑﾓｰﾄﾞ7SEG出力ﾃｽﾄﾌﾗｸﾞ	*/
	glbFlSystemMode=0;									/* ｼｽﾃﾑﾓｰﾄﾞ中ﾌﾗｸﾞ			*/
	glbFlVacuumElectromangReq=0;						/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求*/
	glbFlSysHeaterReq=0;								/* ｼｽﾃﾑﾓｰﾄﾞ　ﾋｰﾀ監視要求=2秒		*/

	glbFlVaccumeTips=0;
	glbFlWarmCansel=0;										/* 暖気運転中ﾌﾗｸﾞ			*/
	glbCtVacuumElectromang=0;							/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後の10秒ｶｳﾝﾀ	*/
	glbCtHeaterBlockTimer=0;							/* ﾋｰﾀﾌﾞﾛｯｸ上昇時間ｶｳﾝﾀ		*/
	glbCtWarmTimer=0;									/* 暖気運転時間				*/
	glbFlDriveMode=0;									/* 運転ﾓｰﾄﾞ時のﾓｰﾄﾞﾌﾗｸﾞ		*/
	geefFlVacuumeDisp=0;								/* 真空工程残を[時間 or %]の切換え	*/
	glbCtDriveTimer=0;									/* 真空引き時間ｶｳﾝﾀ			*/
	glbCtGusInclusionTimer=0;							/* ｶﾞｽ封入時間ｶｳﾝﾀ			*/
	glbCtVacuumEmergncyTimer=0;							/* 真空一時停止ｶｳﾝﾀ			*/
	glbCtGusStubilityTimer=0;							/* ｶﾞｽ安定待ち時間ｶｳﾝﾀ		*/
	glbCtSeelTimer=0;									/* ｼｰﾙ工程時間ｶｳﾝﾀ			*/
	glbCtSeelCoolingTimer=0;							/* ｼｰﾙ冷却工程時間ｶｳﾝﾀ		*/
	glbCtSoftReleaseTimer=0;							/* ｿﾌﾄ解放時間				*/
	glbCtSysHeaterOffTimer=0;							/* ｼｽﾃﾑﾓｰﾄﾞ ﾋｰﾀ2秒監視ﾀｲﾏｶｳﾝﾀ		*/
	glbCtFunTimer=0;
	glbCtVacuumeWarm=0;									/* 暖気運転ﾀｲﾏｰ				*/
	glbFlProc = GCN_PROC_VACUUM;									/* 工程LEDを「真空」表示にする	*/


	glbCtVacuumTimeout1=0;								/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ		*/
	glbCtVacuumTimeout2=0;								/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ		*/
	glbCtVacuumTimeout3=0;								/* 真空ﾀｲﾑｱｳﾄﾁｪｯｸｶｳﾝﾀ		*/
	glbCtIntTimer = 0;
	glbFlSaveMode = glbFlDriveMode;									/* ﾒｲﾝﾓｰﾄﾞの状態を退避			*/
	glbFlError = 0;										/* ｴﾗｰﾌﾗｸﾞ							*/
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

			
	glbFlContinue = 0;									/* 連続ﾓｰﾄﾞ	*/	
	
	glbFlMainSystem = 0;
	mainFlDrawer = 0;									/* 引出しSW状態ｾｰﾌﾞ			*/
	glbCtWaitTimer = 0;
	glbCtBuzzerOnTimer = 0;
	glbCtBuzzerOffTimer = 0;

	glbCtWaitTimerT5 = 0;
		
	glbFlLed11 = 0;
	glbFlWarmMode = 0;

}

/***********************************************************************************************************************
*    関数名    mcmLocalClear
*    引  数    
*    戻り値    
*    関数機能  ローカルメモリクリア
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmLocalClear(void)
{
	stiFlKydt = 0xff;								/* KEY ONがLのため、FFHとする			*/
	stiFlFlashing = 0;								/* ﾄｸﾞﾙ更新								*/
	stiCtFlashing = 0;								/* 500ms周期ｶｳﾝﾀ						*/
	stiFlFlashing2 = 0;								/* ﾄｸﾞﾙ更新								*/
	stiCtFlashing2 = 0;								/* 500ms周期ｶｳﾝﾀ						*/
	stiFlFlashing3 = 0;								/* ﾄｸﾞﾙ更新								*/
	stiCtFlashing3 = 0;								/* 500ms周期ｶｳﾝﾀ						*/
	stiFlFlashing4 = 0;								/* ﾄｸﾞﾙ更新								*/
	stiCtFlashing4 = 0;								/* 500ms周期ｶｳﾝﾀ						*/

}


/***********************************************************************************************************************
*    関数名    mcmFlashDataRead
*    引  数    
*    戻り値    
*    関数機能  フラッシュメモリからバックアップデータを読み込み
*    処理内容  フラッシュメモリからの先頭から２バイトが５ＡＨ及びＡ５Ｈでなければフラッシュメモリ未初期化と判断し、
*              デフォルトのバックデータをフラッシュメモリに書き込む。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	mcmFlashDataRead(void)
{
	volatile pfdl_status_t fdlResult;	
	volatile UBYTE	err=0;
	UBYTE	retry = 0;

	/*先頭/END 固定データ読込*/	
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
			err= 2;												/* ﾌﾗｯｼｭﾘｰﾄﾞｴﾗｰ						*/
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
			err= 2;												/* ﾌﾗｯｼｭﾘｰﾄﾞｴﾗｰ						*/
			goto TAG_END;
		}
		
		continue;
	}
	
	R_FDL_Close();
	
	/*先頭/END 固定データ確認　フラッシュ初期化未？又は破損？*/	
	if( ((geefFlFixData[0] != 0x5a) && (geefFlFixData[1] != 0xa5))	/* ﾌﾗｯｼｭ未初期化の場合				*/
		|| ((geefFlFixEndData[0] != 0x5a) && (geefFlFixEndData[1] != 0xa5)) )	/* ﾌﾗｯｼｭ未初期化の場合				*/
	{
		//読込データNGのため初期化
		err = mcmFlashDataInitial();								/* ﾌﾗｯｼｭﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀ初期化と書き込み*/
		if(err)	
		{
			//書込み失敗
			return(err);
		}
	}
	
	/*書き込みデータの読込照合*/
	err = pdlReadFlash();											/* ﾌﾗｯｼｭからﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀを読み込む	*/
	if(err == 0)													/* ｴﾗｰ無しの場合					*/
	{
		//正常データなら作業エリアへ登録
		pldCopyBackupData();										/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀを比較ﾃﾞｰﾀｴﾘｱへｺﾋﾟｰ	*/
	}

	TAG_END:
	return(err);
}

/***********************************************************************************************************************
*    関数名    mcmFlashDataInitial
*    引  数    
*    戻り値    
*    関数機能  フラッシュメモリイニシャル処理
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	mcmFlashDataInitial(void)
{
	UBYTE	err = 0,i = 0;																																																				

	mcmFlashInitialClear();							/* ﾊﾞｯｸｱｯﾌﾟ領域初期化						*/
     
	for(i=0;i<3;i++)
	{
		err = pdlWriteFlash();								/* ﾃﾞｰﾀをﾌﾗｯｼｭへ書き込み	*/
		if(err == 0)	
		{
			break;		
		}
	}
																									
   return(err);
}

/***********************************************************************************************************************
*    関数名    mcmFlashInitialClear
*    引  数    
*    戻り値    
*    関数機能  フラッシュメモリのバックアップデータ初期化
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmFlashInitialClear(void)
{
	UBYTE	i,j;																											

	geefFlFixData[0] = geefFlFixEndData[0] = 0x5a;								/* ﾌﾗｯｼｭ初期化ﾃﾞｰﾀｾｯﾄ		*/
	geefFlFixData[1] = geefFlFixEndData[1] = 0xa5;								/* ﾌﾗｯｼｭ初期化ﾃﾞｰﾀｾｯﾄ		*/
	geefFlVacuumType[0] = GCN_VACUUM_LEVEL;				/* ｺｰｽ１：真空引き工程を%で表示	*/		
	geefFlVacuumType[1] = GCN_VACUUM_LEVEL;				/* ｺｰｽ２：真空引き工程を%で表示	*/		
	geefFlVacuumType[2] = GCN_VACUUM_LEVEL;				/* ｺｰｽ３：真空引き工程を%で表示	*/	
	geefFlVacuumType[3] = GCN_VACUUM_LEVEL	;				/* ｺｰｽ４：真空引き工程を%で表示	*/	
	geefCtAnalogAdjust = 0;									/* ｱﾅﾛｸﾞ調整値				*/
	geefCtPumpRunning = 0;
	geefCtPack = 0;
	geefFlCourse = 0;	/* ｺｰｽNo.初期化			*/
	for(i=0;i<10;i++)										/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙｸﾘｱ		*/
	{
		for(j=0;j<2;++j)
		{
			geefBfErrorHistory[i][j] = 0xff;
		}
	}
	for(i=0;i<4;++i)										/* ﾃﾞﾌｫﾙﾄのｺｰｽﾃﾞｰﾀをｾｯﾄ		*/
	{
		for(j=0;j<8;++j)
		{
			geefBfCourseData[i][j] = DefaultCourseData[i][j];
		}
	}
	for(i=0;i<5;++i)										/* ﾃﾞﾌｫﾙﾄのｼｽﾃﾑﾃﾞｰﾀをｾｯﾄ		*/
	{
		for(j=0;j<3;++j)
		{

			geefBfSystemData[i][j] = DefaultSystemData[i][j];
		}
	}


}

/***********************************************************************************************************************
*    関数名    mcmDataSettingRangeCheck	
*    引  数    
*    戻り値    
*    関数機能  設定データが入力範囲内（有効データ）か確認する
*    処理内容 
*    作成日    2022年1月14日
*    作成者    
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

	for(i=0;i<4;++i)										/* ｺｰｽﾃﾞｰﾀを確認		*/
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
	
	
	for(i=0;i<5;++i)										/* ｼｽﾃﾑﾃﾞｰﾀを確認*/
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
*    関数名    mcmStartBuzzer
*    引  数    ON時間（ms単位)
*    戻り値    
*    関数機能  指定時間ブザーを鳴らす
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	mcmStartBuzzer(UWORD Timer)
{
	glbCtBuzzerOnTimer = Timer / GCN_INT_TIME;				/* ﾌﾞｻﾞｰON時間を5ms単位に変換			*/
	R_TAU0_Channel2_Start();								/* PWM ON								*/
}				


/***********************************************************************************************************************
*    関数名    mcmFlashWriteCheck
*    引  数    
*    戻り値    
*    関数機能  バックアップデータの更新チェック
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	mcmFlashWriteCheck(void)							/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更ﾁｪｯｸ				*/
{
	volatile UBYTE	err=0;
	volatile UBYTE i;

	if(!pldVerifyCheck())									/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘに変更ﾃﾞｰﾀ有り？		*/
	{
		return(err);	//変更無し
	}
	
	//書き込みﾘﾄﾗｲ追加
	for(i=0;i<3;i++)
	{
		err = pdlWriteFlash();								/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀをﾌﾗｯｼｭへ書き込み	*/
		if(err == 0)	
		{
			pldCopyBackupData();								/* 比較用ﾊﾞｯﾌｧへｺﾋﾟｰ				*/
			break;		
		}
	}
	
	return(err);	
}

/***********************************************************************************************************************
*    関数名    mainCheckPumpthermal
*    引  数    
*    戻り値    
*    関数機能  ポンプサーマルエラーチェック([E0])
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainCheckPumpthermal(void)						/* ﾎﾟﾝﾌﾟｻｰﾏﾙのｴﾗｰ監視		*/
{
	volatile UBYTE	in1,in2;

	in1 = P14;
	in2 = P14;
	if(in1 != in2)
	{
		in1 = P14;
	}
	if(!(in1 & P146_PUMP_THERMAL))							/* ｴﾗｰ有り？					*/
	{
		glbFlError |= GCN_ERROR_PUMP_THERMAL_0;
		glbFlMainMode = GCN_ERROR_MODE;						/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
		return;
	}
}

/***********************************************************************************************************************
*    関数名    mcmSetBuzzerEffective
*    引  数    
*    戻り値    
*    関数機能  ボタン有効
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmSetBuzzerEffective(void)						/* 有効ﾌﾞｻﾞｰを鳴らす			*/
{
	mcmStartBuzzer(100);								/* 100ms*/

}

/***********************************************************************************************************************
*    関数名    mcmSetBuzzerInvalid
*    引  数    
*    戻り値    
*    関数機能  ボタン無効ブザー
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmSetBuzzerInvalid(void)						/* 無効ﾌﾞｻﾞｰを鳴らす			*/
{
	mcmStartBuzzer(400);								/* 400ms*/

}

/***********************************************************************************************************************
*    関数名    mcmSetBuzzerEnd
*    引  数    
*    戻り値    
*    関数機能  真空運転工程/暖気運転終了処理
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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
*    関数名    mcmSetBuzzerError
*    引  数    
*    戻り値    
*    関数機能  エラー発生時のブザー警告
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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
*    関数名    mcmBuzzerOff
*    引  数    
*    戻り値    
*    関数機能  ブザー音繰り返し時のブザーＯＦＦ時間監視
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmBuzzerOff(UWORD	Timer)						
{
	while(glbCtBuzzerOnTimer != 0);						/* ﾌﾞｻﾞｰON中ならばOFFになるまで待つ		*/
	glbCtBuzzerOffTimer = Timer / GCN_INT_TIME;			/* ﾌﾞｻﾞｰOFF時間を5ms単位に変換			*/
	while(glbCtBuzzerOffTimer != 0);					/* ﾌﾞｻﾞｰOFF時間の監視					*/
}				


/***********************************************************************************************************************
*    関数名    mcmWaitTimer
*    引  数    ＷＡＩＴ時間(ms)
*    戻り値    
*    関数機能  指定時間のＷＡＩＴ
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmWaitTimer(UWORD	Timer)
{
	glbCtWaitTimer = Timer /  GCN_INT_TIME;				/* ｳｪｲﾄ時間を5ms変換					*/
	while(glbCtWaitTimer != 0)
	{
		NOP();
	};							/* 指定時間経過の監視					*/

}

/***********************************************************************************************************************
*    関数名    mcmCheckContinue
*    引  数    
*    戻り値    0:状態変化なし、1:「開」から「閉」の状態変化有り
*    関数機能  引出しSWが「開」から「閉」の状態ﾁｪｯｸ
*    処理内容  連続運転モード時、引出しSWが「開」から「閉」の変化があれば、真空運転工程に遷移する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	mcmCheckDrawerOpenToClose(void)
{
	UBYTE	sens;

	if(((sens = DrawerSwRead()) == 0))					/* 引出しSWが「閉」の場合			*/
	{
		if(mainFlDrawer == 1)							/* 前回の引出しSWが「開」の場合		*/
		{
			mainFlDrawer = sens;						/* 引出しSWの状態保存				*/
			return(1);									/* 引出しSWの状態が「開」⇒「閉」に変化したため、真空工程へ遷移	*/
		}
	}
	mainFlDrawer = sens;								/* 引出しSWの状態保存				*/
	return(0);
}


/***********************************************************************************************************************
*    関数名    mcmCheckVacuumSensorError
*    引  数    
*    戻り値    
*    関数機能  真空センサーエラーチェックを行う
*    処理内容  真空運転中とそれ以外でエラーの閾値が異なる。
*              また、運転終了後WAIT時間（Ｔ５)の間はｴﾗｰ検出しない。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mcmCheckVacuumSensorError(void)					/* 真空ｾﾝｻｰｴﾗｰの検出		*/
{
	long int	adc;


	if(glbCtWaitTimerE6Cansel  != 0)					/* 電源投入から運転終了後WAIT時間（T5)の間は[E6]ｴﾗｰを検出しない	*/
	{
		return;											
	}
	adc = glbCtRealAD;									/* 平均化されたAD値入力　			*/
	if(glbFlMainMode == GCN_DRIVE_MODE)					/* 真空運転中の場合					*/
	{
		if(adc >= 0x3c1 || adc <= 0x20)					/* 平均化後ADﾃﾞｰﾀが3C1[hex]以上、または0x20[hex]以下	*/
		{
			glbFlError |= GCN_ERROR_VACUUM_ADC_6;		/* 真空ｾﾝｻｰｴﾗｰ[E6]					*/
			glbFlMainMode = GCN_ERROR_MODE;				/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
		}
	}
	else												/* 真空運転中以外（待機中と暖気運転中）*/
	{
		if(adc >= 0x3c1 || adc <= 0x1ff)				/* 平均化後ADﾃﾞｰﾀが3C1[hex]以上、または1ff[hex]以下	*/
		{
			glbFlError |= GCN_ERROR_VACUUM_ADC_6;		/* 真空ｾﾝｻｰｴﾗｰ[E6]					*/
			glbFlMainMode = GCN_ERROR_MODE;				/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
		}

	}
}

