#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include	"geefFlash.h"




UBYTE	vacFlHeaterBlock;										/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞ		*/
UBYTE	glbFlSaveVacuumType;
SWORD	vacCtStartAdLevel;
UBYTE	vacFlEndAdLevel;
UBYTE	vacFlRestart;

uint8_t vacFlAbnormalEnd; 										//非正常運転終了ﾌﾗｸﾞ 

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
extern	void	mainDisplay7Seg(UWORD);							/* 7SEG表示							*/
extern	void	mainDisplayWarning7Seg(void);					/* 7SEG表示							*/
extern	UBYTE	glbGetReadKey(void);
extern	UBYTE   glbWaitKeyOn(UBYTE KeyNo);
extern	UWORD	R_ADC_Get_Result2(void);
extern	void	mcmSetBuzzerEnd(void);
extern	void	mcmSetBuzzerInvalid(void);						/* 無効ﾌﾞｻﾞｰ			*/
extern	void	mcmSetBuzzerEffective(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

/***********************************************************************************************************************
ｺｰｽﾃﾞｰﾀの初期値：RAMｸﾘｱ時、このﾃﾞｰﾀがﾌﾗｯｼｭに書き込まれる
***********************************************************************************************************************/

const	UBYTE	DefaultCourseData[4][8] = 		/* 4ｺｰｽ分			*/
								{		/* ﾃﾞﾌｫﾙﾄのｺｰｽﾃﾞｰﾀ　★★真空時間単位は1.0秒で、それ以外は0.1秒単位★★	*/
										/*                  ★★真空ﾚﾍﾞﾙに関しては、1%単位				★★	*/
														/* 【ｺｰｽ1】		*/
										{ 20,			/* 真空時間		*/
									  	  25,			/* ｼｰﾙ時間		*/
						                  40,			/* ｼｰﾙ冷却時間	*/
									   	   0,			/* ｶﾞｽ時間		*/
                                           0,			/* ｶﾞｽ安定時間	*/
                                           0,			/* ｶﾞｽﾚﾍﾞﾙ		*/
									       30,			/* ｿﾌﾄ解放時間	*/
										  80			/* 真空ﾚﾍﾞﾙ		*/
									     },
														/* 【ｺｰｽ2】		*/
										{ 35,			/* 真空時間		*/
									  	  25,			/* ｼｰﾙ時間		*/
						                  40,			/* ｼｰﾙ冷却時間	*/
									   	   0,			/* ｶﾞｽ時間		*/
                                           0,			/* ｶﾞｽ安定時間	*/
                                           0,			/* ｶﾞｽﾚﾍﾞﾙ		*/
									       30,			/* ｿﾌﾄ解放時間	*/
										  90			/* 真空ﾚﾍﾞﾙ		*/
									     },
														/* 【ｺｰｽ3】		*/
										{ 15,			/* 真空時間		*/
									  	  25,			/* ｼｰﾙ時間		*/
						                  40,			/* ｼｰﾙ冷却時間	*/
									   	   0,			/* ｶﾞｽ時間		*/
                                           0,			/* ｶﾞｽ安定時間	*/
                                           0,			/* ｶﾞｽﾚﾍﾞﾙ		*/
									       30,			/* ｿﾌﾄ解放時間	*/
										  70			/* 真空ﾚﾍﾞﾙ		*/
									     },
														/* 【ｺｰｽ4】		*/
										{ 10,			/* 真空時間		*/
									  	  25,			/* ｼｰﾙ時間		*/
						                  40,			/* ｼｰﾙ冷却時間	*/
									   	   0,			/* ｶﾞｽ時間		*/
                                           0,			/* ｶﾞｽ安定時間	*/
                                           0,			/* ｶﾞｽﾚﾍﾞﾙ		*/
									       30,			/* ｿﾌﾄ解放時間	*/
										  40			/* 真空ﾚﾍﾞﾙ		*/
									     }
								};


/***********************************************************************************************************************
ｼｽﾃﾑﾃﾞｰﾀの初期値：RAMｸﾘｱ時、このﾃﾞｰﾀがﾌﾗｯｼｭに書き込まれる
***********************************************************************************************************************/
const	UBYTE	DefaultSystemData[5][3] = {  {60,0,99},					/* T1:真空停止時間	（単位：1.0秒)*/
									    	 {99,0,99},					/* T2:暖気運転時間	（単位：1.0秒)*/
									 		 {90,0,99},					/* T3:真空ﾀｲﾑｱｳﾄ	（単位：1.0秒)*/
									 		 { 5,0,99},					/* T4:予約運転時WAIT時間（単位：0.1秒)*/
									 		 {20,0,99}					/* T5:運転時終了WAIT時間（単位：1秒)*/

								            };

struct		tagSYSTEMMAXMIN	geefBfCourseDataMaxMin[8] = {{  5,99},		/* 真空時間(MIN,MAX)(単位:1秒)		*/
													     {  0,40},		/* ｼｰﾙ時間(MIN,MAX)(単位:0.1秒)		*/
													     {  0,99},		/* ｼｰﾙ冷却時間(MIN,MAX)(単位:0.1秒)	*/
													     {  0,99},		/* ｶﾞｽ時間(MIN,MAX)(単位:0.1秒)		*/
													     {  0,99},		/* ｶﾞｽ安定時間(MIN,MAX)単位:0.1秒	*/
													     {  0,90},		/* ｶﾞｽﾚﾍﾞﾙ(MIN,MAX)(単位:%)		*/
													     {  0,90},		/* ｿﾌﾄ解放時間(MIN,MAX)単位:0.1秒	*/
													     { 40,99},		/* 真空ﾚﾍﾞﾙ（単位:1%)				*/
													 };


/***********************************************************************************************************************
真空運転処理テーブル
***********************************************************************************************************************/
void	(*DriveProcTbl[])() =
{
	DriveStartMode,				/* 運転開始時					*/
	VacuumPullMode,				/* 真空引き工程					*/
	GusInclusionMode,			/* ｶﾞｽ封入工程					*/
	GusStubilityMode,			/* ｶﾞｽ安定待ち工程				*/
	SeelMode,					/* ｼｰﾙ工程						*/
	SeelCoolingMode,			/* ｼｰﾙ冷却工程（ソフト開放工程）*/
	VacuumStopMode,				/* 真空引き一時停止ﾓｰﾄﾞ			*/
	SeelHeaterBlockWait,		/* ﾋｰﾀｰﾌﾞﾛｯｸ上昇待ち処理		*/
	DriveEndMode,				/* 工程終了						*/
};

/***********************************************************************************************************************
*    関数名    mainDriveMode
*    引  数    
*    戻り値    
*    関数機能  真空運転メイン処理
*    処理内容  真空工程⇒シール工程⇒冷却工程⇒真空運転終了までを本処理で行う。
*    注意　　　真空運転中に引出しスイッチが開いた場合は、ただちに「工程終了時動作」に移行すること。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void mainDriveMode(void)
{
	uint8_t driveEndFlag;

	driveEndFlag = 0;												//電源SWによる運転終了
	vacFlAbnormalEnd = 0;
	
	glbFlSaveMode = glbFlMainMode;
	
	/* 真空運転　工程終了動作中以外*/
	if(glbFlDriveMode != GCN_DRIVE_END_MODE)						/* 真空運転工程終了動作中は引出しSWの状態を検出しない	*/
	{
		if(glbFlPowerSw)	//電源SWによる終了ON？
		{
			driveEndFlag = 1;
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/	
		}
		
		if( (DrawerSwRead())|| driveEndFlag)						/* 運転中に引出しSWが「開」状態になった場合	*/
		{
			/* 真空運転を中止し、待機ﾓｰﾄﾞへ遷移する		*/
			P0 &= P01_SEEL_ELECTROMAG_OFF;							/* ｼｰﾙ電磁弁OFF							*/
			VacuumAllStop();										/* 真空運転出力信号をすべてOFF			*/
			VacuumEndControl();										/* 真空運転終了時の信号制御（真空電磁弁=10秒 ON,ﾌｧﾝ=3分間 ON*/
			
			vacFlAbnormalEnd = 1;
			
			glbFlDriveMode = GCN_DRIVE_END_MODE;					/* 真空運転工程終了動作へ遷移	*/
		}	
	}
	
	(*DriveProcTbl[glbFlDriveMode])();								/* 現状態のﾓｰﾄﾞへ遷移		*/
}

/***********************************************************************************************************************
*    関数名    
*    引  数    
*    戻り値    
*    関数機能  運転開始動作前処理
*    処理内容  ・真空ポンプ⇒ON,ファン->ON,真空解放弁（閉)->ON
*              ・コースデータ内の真空時間セット（秒指定）
*              ・真空開始時、現在の真空レベル＋５％を保存し、これを真空運転終了時に使用する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
********************************************************************************************************************/

void DriveStartMode(void)
{
	SWORD	Level;

	if(vacFlRestart == 0)															/* 真空停止からの再開でない場合		*/
	{
																					/* 真空停止からの再開の場合は、最初の真空開始時の真空ﾚﾍﾞﾙを使用する	*/
		Level = GetVacuumLevel();													/* 真空開始時の真空ﾚﾍﾞﾙを算出（整数:単位%)		*/
		vacCtStartAdLevel = Level + 5;												/* 真空運転終了時の終了判定ﾚﾍﾞﾙ		*/
	}																				/* 運転開始前の+5%(AD値は0x1f程度)を終了真空圧ﾚﾍﾞﾙとする	*/
	vacFlRestart = 0;
	VacuumStart();																	/* 真空運転開始						*/
	glbFlDriveMode = GCN_VACUUM_PULL_MODE;											/* 真空引き工程へ遷移				*/

	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)						/* 真空引き工程を秒で表示			*/
	{
		glbCtDriveTimer = (geefBfCourseData[geefFlCourse][Vacuum])* GCN_TIMER_1S;		/* 秒を5ms単位に変換				*/
	}
	glbCtVacuumTimeout1 = geefBfSystemData[GCN_SYSTEM_VACUUM_TIMEOUT][Default]			/* 真空ﾀｲﾑﾀｲﾑｱｳﾄ時間ﾘｰﾄﾞ(T3)		*/
						 * GCN_TIMER_1S;
		
	glbCtVacuumTimeout2 = GCN_TIMER_10S;												/* 10.0秒経過しても真空ﾚﾍﾞﾙが20%に達しないことをﾁｪｯｸする時間	*/
	glbCtVacuumTimeout3 = GCN_TIMER_30S;											/* 30秒経過後、真空ﾚﾍﾞﾙが90%ﾁｪｯｸ	*/
	glbFlVacuumWarning = 0;															/* 真空警告ﾌﾗｸﾞｾｯﾄ				*/

}


/***********************************************************************************************************************
*    関数名    VacuumPullMode
*    引  数    
*    戻り値    
*    関数機能  真空引き工程
*    処理内容  ・真空引残り時間または度合を7SEGに1秒単位で表示
*              ・本工程中に「START/STOP」が押された場合、真空電磁弁を閉じて、真空停止時間［T1］秒間真空引きを停止する。
*                真空電磁弁ＯＦＦ、ポンプＯＦＦ
*			   ・真空停止時間（［T1］秒）が経過した場合、または、再度操作パネルの「START/STOP」スイッチ
*                を押された場合、真空電磁弁を開いて、真空工程を再開する。
*              ・再開の場合の時間はコースデータの時間から再度再開。
*              ・真空引き時間が経過したら、真空電磁弁ＯＦＦ。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void VacuumPullMode(void)
{
	UBYTE	sts,key,VacuumEnd=0;
	UWORD	MaxKeyOnTime,VacuumLebelMax;
	SWORD	Level;
	UWORD	data;

	Level = GetVacuumLevel();												/* 真空ﾚﾍﾞﾙを算出（整数:単位%)	*/

	glbFlProc 		= GCN_PROC_VACUUM;										/* 工程を真空へ					*/
	ledControl_Proc(glbFlProc,1,0);											
	glbFlReqDot 	= (glbFlProc > GCN_PROC_VACUUM)?1:0;					/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_LEVEL)					/* 真空工程表示が「%」の場合			*/
	{
		if(glbCtVacuumTimeout1 == 0)										/* 90秒経過しても、設定真空度まで到達しない場合、次の工程に進まない状態を避けるため、*/
																			/* 真空ﾀｲﾑｱｳﾄｴﾗｰとしていたが、これをｴﾗｰにせず次工程（ｼｰﾙ）へ遷移するようにした。	 */
		{
			VacuumEnd = 1;													/* 真空引き終了ﾌﾗｸﾞｾｯﾄ				*/
		}
	}
	if(glbCtVacuumTimeout2 == 0)											/* 真空開始から5秒経過？		*/
	{
		if(Level < 20)														/* 5秒経過しても真空ﾚﾍﾞﾙが20%に達していない場合					*/
		{
			glbFlError |= GCN_ERROR_VACUUM_LEVEL_2;							/* 真空ｴﾗｰ[E2]					*/
			glbFlMainMode = GCN_ERROR_MODE;									/* ﾒｲﾝﾓｰﾄﾞをｴﾗｰ検出ﾓｰﾄﾞへ		*/
			return;
		}
	}
	

	if(glbCtVacuumTimeout3 == 0)											/* 真空開始から30秒経過？		*/
	{
		if(Level < 70)														/* 30秒経過しても真空ﾚﾍﾞﾙが90%に達していない場合					*/
		{
			glbFlVacuumWarning = 1;											/* 真空警告ﾌﾗｸﾞｾｯﾄ				*/
		}
	}


	if(glbFlVacuumWarning == 1)												/* 真空警告中？					*/
	{
		mainDisplayWarning7Seg();											/* 真空警告中7SEG表示([E3])		*/

	}
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)				/* 真空工程表示が秒の場合			*/
	{
		if(glbFlVacuumWarning == 0)											/* 真空警告中？					*/
		{
			data = ((glbCtDriveTimer * GCN_INT_TIME)/GCN_TIME_1S);			/* 5ms単位のｶｳﾝﾀを秒に変換			*/
			mainDisplay7Seg((UWORD)data);									/* 7SEG表示							*/
		}
		if(glbCtDriveTimer == 0)											/* 真空引き工程終了	?				*/
		{
			VacuumEnd = 1;													/* 真空引き終了ﾌﾗｸﾞｾｯﾄ				*/
		}																
	}	
	else 																	/* 真空工程表示が%の場合			*/
	{
		VacuumLebelMax = (geefBfCourseData[geefFlCourse][VacuumLevel]);		/* ｺｰｽ毎の真空到達ﾚﾍﾞﾙをﾘｰﾄﾞ	*/
		if(glbFlVacuumWarning == 0)											/* 真空警告中？					*/
		{
			mainDisplay7Seg(Level);											/* 真空ﾚﾍﾞﾙ(%)7SEG表示			*/
		}
		if(Level >= VacuumLebelMax )										/* ｺｰｽﾃﾞｰﾀに設定された真空ﾚﾍﾞﾙに達した場合*/
		{
			VacuumEnd = 1;													/* 真空引き工程終了ｾｯﾄ			*/
		}
	}
	if(VacuumEnd == 1)														/* 真空引き工程終了の場合			*/
	{
		P2 &= P20_VACUUM_ELECTROMAG_OFF;									/* 真空電磁弁OFF					*/
																			/* ■ｶﾞｽ封入工程開始				*/
		glbCtGusInclusionTimer = (geefBfCourseData[geefFlCourse][Gus] *		/* ｶﾞｽ封入時間（単位：0.1秒）		*/
							      GCN_TIMER_100MS);
		glbFlDriveMode = GCN_GUS_INCLUSIOM_MODE	;							/* 運転ﾓｰﾄﾞをｶﾞｽ封入工程へ遷移	*/
		glbFlVacuumWarning  = 0;											/* 真空警告(E3）ｸﾘｱ				*/
		glbFlSecLevel  = GCN_VACUUM_SECONDS;								/* 「秒」表示					*/
		ledControl_Sec(glbFlSecLevel,1,0);									
		return;

	}
	if((key = glbGetReadKey()))												/* KEYの入力ﾁｪｯｸ					*/
	{
		if(key == GCN_KEY_START_SET)										/* ｽﾀｰﾄが押された場合(真空ｼﾞｬﾝﾌﾟ機能)	*/
		{
			Level = GetVacuumLevel();										/* 真空ﾚﾍﾞﾙを算出				*/
			if(Level < 40)													/* 真空ﾚﾍﾞﾙが40%に達していない場合は真空ｼﾞｬﾝﾌﾟ機能無効とする	*/
			{
				mcmSetBuzzerInvalid();										/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
				return;
			}	
			mcmSetBuzzerEffective();										/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* 真空電磁弁OFF					*/
																			/* ■ｶﾞｽ封入工程開始				*/
			glbCtGusInclusionTimer = (geefBfCourseData[geefFlCourse][Gus] *	/* ｶﾞｽ封入時間（単位：0.1秒）		*/
                                      GCN_TIMER_100MS);
			glbFlVacuumWarning  = 0;										/* 真空警告(E3）ｸﾘｱ				*/
			glbFlSecLevel  = GCN_VACUUM_SECONDS;							/* 「秒」表示					*/
			ledControl_Sec(glbFlSecLevel,1,0);								
					
			glbFlDriveMode = GCN_GUS_INCLUSIOM_MODE	;						/* 運転ﾓｰﾄﾞをｶﾞｽ封入工程へ遷移	*/
		}	
		else if(key == GCN_KEY_STOP_SET)												/* ｽﾄｯﾌﾟｷｰ（隠し）が押された場合		*/
		{	

			MaxKeyOnTime = GCN_TIME_1S;													/* ｷｰ長押し通知時間	（1秒)				*/
			sts = glbWaitKeyOff(key,MaxKeyOnTime);										/* KEYが1秒以上押されたかﾁｪｯｸ			*/
			if(sts == 1)																/* KEYが1秒以上おされた場合				*/
			{
				mcmSetBuzzerEffective();												/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				VacuumEmergencyStop();													/* 真空工程一時停止					*/

				glbCtVacuumEmergncyTimer = (geefBfSystemData[GCN_SYSTEM_VACUUM][Default]) 	/* T1:真空一時停止時間ｾｯﾄ（秒）			*/
										     * GCN_TIMER_1S;
				glbFlSecLevel = GCN_VACUUM_SECONDS;										/* 「秒」表示にする				*/
				ledControl_Sec(glbFlSecLevel,1,0);
				
				glbFlDriveMode = GCN_VACUUM_STOP_MODE;									/* 運転ﾓｰﾄを一時停止へ遷移			*/
		
			}
		}
		else
		{
			mcmSetBuzzerInvalid();														/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
		}
	}

}

/***********************************************************************************************************************
*    関数名    GusInclusionMode
*    引  数    
*    戻り値    
*    関数機能  　ガス封入工程処理
*    処理内容  ・［コースデータ］に設定された時間（秒数）が経過するまでガス封入を行う。
*              ・ガス電磁弁を開け（ＯＮ）ガスを封入する。
*				 ガス電磁弁ＯＮ
*              ・設定時間が経過したらガス電磁弁を閉じ、次の工程（ガス安定工程）へ移行する。
*              ・但し、［コースデータ］の設定時間（ガス時間）が０．０秒の場合、ガス封入工程は実行せず、次の工程
*　　　　　　　（シール工程）へ移行する
*    注意　　　２０１８年８月２８日現在、コースデータ内のこの時間は０．０秒となっている。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	GusInclusionMode(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEYの入力ﾁｪｯｸ					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* ｽﾄｯﾌﾟｷｰ（隠し）以外の場合		*/
		{
			mcmSetBuzzerInvalid();										/* 無効ﾌﾞｻﾞｰ			*/
		}
	}
	if(geefBfCourseData[geefFlCourse][Gus] == 0)							/* ｶﾞｽ封入設定時間が0秒の場合		*/
	{
		vacFlHeaterBlock = 1;											/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞｾｯﾄ	*/
		glbFlDriveMode = GCN_SEEL_MODE;									/* ｶﾞｽ封入工程は実行せず、次の工程（ｼｰﾙ工程）へ遷移する	*/
		return;
	}
	P13 |= P130_GUS_ELECTROMAG_ON;										/* ｶﾞｽ電磁弁 ON						*/
	if(glbCtGusInclusionTimer == 0)									 	/* ｶﾞｽ封入時間が0秒になった場合		*/
	{
		P13 &= P130_GUS_ELECTROMAG_OFF;									/* ｶﾞｽ電磁弁 OFF					*/
		glbCtGusStubilityTimer = (geefBfCourseData[geefFlCourse][GusStability] *	/* ｶﾞｽ安定待ち時間（単位：0.1秒）ﾘｰﾄﾞ		*/
								  GCN_TIMER_100MS);						/* 0.1s単位を5ms単位に変換					*/
		glbFlDriveMode = GCN_GUS_STUBILITY_MODE	;						/* 運転ﾓｰﾄﾞをｶﾞｽ安定待ち工程へ遷移	*/
	}
}

/***********************************************************************************************************************
*    関数名    GusStubilityMode
*    引  数    
*    戻り値    
*    関数機能  　ガス安定待ち工程
*    処理内容  ・[コースデータ］に設定されたガス安定待ち時間の間ウエイトする。
*    注意　　　２０１８年８月２８日現在、コースデータ内のこの時間は０．０秒となっている。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	GusStubilityMode(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEYの入力ﾁｪｯｸ					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* ｽﾄｯﾌﾟｷｰ（隠し）以外の場合		*/
		{
			mcmSetBuzzerInvalid();										/* 無効ﾌﾞｻﾞｰ			*/
		}
	}

	if(glbCtGusStubilityTimer == 0)										/* ｶﾞｽ安定待ち時間に達した場合	*/
	{
		glbFlDriveMode = GCN_SEEL_MODE	;								/* 運転ﾓｰﾄﾞをｼｰﾙ工程へ遷移	*/
		vacFlHeaterBlock = 1;											/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞｾｯﾄ	*/
	}
}

/***********************************************************************************************************************
*    関数名    SeelMode
*    引  数    
*    戻り値    
*    関数機能  　シール工程
*    処理内容  ・シール電磁弁を開く（ＯＮ）ことで、ヒーターブロックを上昇させ、シール電流を通電し、袋口を密閉する。
*                但し、シール時間が０秒の場合は次の工程（シール冷却工程）へ移行する。
*              【STEP1】シール電磁弁を開き（ＯＮ）ヒーターブロックを上昇させる。
*                       ■シール電磁弁 ＯＮ	開く
*              【STEP2】ヒーターブロック上昇待ち時間（０．７秒）が経過するまで待つ。
*			   【STEP3】ヒーターブロック上昇待ち時間が経過したら、ヒーター電流を通電（ＯＮ）し、
*                       ［コースデータ］のシール時間に従ってシールを実行する。
*                       ■ヒーター信号 ＯＮ 通電
*			   【STEP4】シール時間経過後ヒーターをＯＦＦする。
*						■ヒーター信号	ＯＦＦ	通電停止
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void 	SeelMode(void)
{	
	UWORD	data;

	volatile	UBYTE	key;

	if((key = glbGetReadKey()))													/* KEYの入力ﾁｪｯｸ					*/
	{
		if(key != GCN_KEY_STOP_SET)												/* ｽﾄｯﾌﾟｷｰ（隠し）以外の場合		*/
		{
			mcmSetBuzzerInvalid();												/* 無効ﾌﾞｻﾞｰ			*/
		}
	}

	glbFlProc 		= GCN_PROC_SEEL;											/* 工程をｼｰﾙへ				*/
	ledControl_Proc(glbFlProc,1,0);											
		
	glbFlSecLevel   = GCN_VACUUM_SECONDS;
	ledControl_Sec(glbFlSecLevel,1,0);
			
	if(geefBfCourseData[geefFlCourse][Seel] == 0)								/* ｼｰﾙ設定時間が0秒の場合		*/
	{
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;						/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
		DI();
		P7 &= P73_HEATER_START_OFF;												/* ﾋｰﾀOFF						*/
		EI();
		glbCtSeelCoolingTimer = (geefBfCourseData[geefFlCourse][SeelCooling] *	/* ｼｰﾙ冷却工程時間（単位：0.1秒）		*/
							      GCN_TIMER_100MS);
		glbCtSoftReleaseTimer = (geefBfCourseData[geefFlCourse][SoftRelease] *	/* ｿﾌﾄ解放時間（単位：0.1秒）		*/
							      GCN_TIMER_100MS);
		glbFlDriveMode = GCN_SEEL_COOLING_MODE;									/* ｼｰﾙ工程は実行せず、次の工程（ｼｰﾙ冷却工程）へ遷移する	*/
		vacFlHeaterBlock = 0;													/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞﾘｾｯﾄ	*/
		return;
	}
	P0 |= P01_SEEL_ELECTROMAG_ON;												/* ｼｰﾙ電磁弁 ON						*/
	if(vacFlHeaterBlock == 1)													/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞｾｯﾄ	*/
	{
		data = geefBfCourseData[geefFlCourse][Seel];
		mainDisplay7Seg((UWORD)data);											/* 7SEG表示							*/
		glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;						/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
		glbFlDriveMode = GCN_HEATER_BLOCK_MODE;									/* ﾋｰﾀﾌﾞﾛｯｸ上昇待ち時間waitへ遷移（0.7秒）*/
		
		glbCtHeaterBlockTimer = GCN_TIMER_500MS*3;								/* 1500ms(1.5秒）5msｶｳﾝﾄに変換した値		*/
		return;
	}
	data = ((glbCtSeelTimer * GCN_INT_TIME)/GCN_TIME_100MS);					/* 5ms単位のｶｳﾝﾀを0.1秒単位に変換			*/
	mainDisplay7Seg((UWORD)data);												/* 7SEG表示							*/
	glbFlReqDot = (glbFlProc > GCN_PROC_VACUUM)?1:0;							/* 「真空」以外はﾋﾟﾘｵﾄﾞ表示				*/
	if(glbCtSeelTimer == 0)														/* ｼｰﾙ工程時間終了?					*/
	{
		DI();
		P7 &= P73_HEATER_START_OFF;												/* ﾋｰﾀOFF						*/
		EI();
		glbCtSeelCoolingTimer = (geefBfCourseData[geefFlCourse][SeelCooling] *	/* ｼｰﾙ冷却工程時間（単位：0.1秒）		*/
							      GCN_TIMER_100MS);
								  
		if(glbFlSoftOpen)
		{
			glbCtSoftReleaseTimer = (geefBfCourseData[geefFlCourse][SoftRelease] *	/* ｿﾌﾄ解放時間（単位：0.1秒）		*/
								      GCN_TIMER_100MS);
			if(glbCtSoftReleaseTimer)
			{
			  P7 |= P75_SOFTRELEASE_START_ON;									/* ｿﾌﾄ解放弁ON							*/
			}
		}
		else
		{
			glbCtSoftReleaseTimer = 0;
		}

		glbFlDriveMode = GCN_SEEL_COOLING_MODE;									/* ｼｰﾙ冷却工程へ遷移する	*/
	}
	
}

/***********************************************************************************************************************
*    関数名    SeelHeaterBlockWait
*    引  数    
*    戻り値    
*    関数機能  ヒーターブロック上昇待ち処理（０．７秒）
*    処理内容  ・ヒーターブロック上昇待ち時間（０．７秒）が経過するまで待つ。
*              ・ヒーターブロック上昇待ち時間が経過したら、ヒーター電流を通電（ＯＮ）し、
*                コースデータのシール時間に従ってシールを実行する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	SeelHeaterBlockWait(void)
{
	volatile	UBYTE	key;

	if((key = glbGetReadKey()))											/* KEYの入力ﾁｪｯｸ					*/
	{
		if(key != GCN_KEY_STOP_SET)										/* ｽﾄｯﾌﾟｷｰ（隠し）以外の場合		*/
		{
			mcmSetBuzzerInvalid();										/* 無効ﾌﾞｻﾞｰ			*/
		}
	}

	if(glbCtHeaterBlockTimer == 0)										/* 0.7秒経過？					*/
	{
		DI();
		P7 &= P74_PUMP_START_OFF;										/* ポンプ停止					*/
		P7 |= P73_HEATER_START_ON;										/* ﾋｰﾀON						*/
		EI();
		glbFlDriveMode = GCN_SEEL_MODE	;								/* 運転ﾓｰﾄﾞを再度ｼｰﾙ工程へ遷移	*/
		glbCtSeelTimer = (geefBfCourseData[geefFlCourse][Seel] *			/* ｼｰﾙ工程時間（単位：0.1秒）		*/
						  GCN_TIMER_100MS);
		vacFlHeaterBlock = 0;											/* ﾋｰﾀﾌﾞﾛｯｸ上昇要求ﾌﾗｸﾞﾘｾｯﾄ		*/
	}

}
/***********************************************************************************************************************
*    関数名    SeelCoolingMode
*    引  数    
*    戻り値    
*    関数機能  シール冷却工程（ソフト開放）
*    処理内容  ・［コースデータ］のシール冷却時間設定に従ってシールを冷却する。
*              ・ソフト開放時間が設定されている場合、ソフト開放を行う。
*              ・ソフト開放時間が設定されていない場合［コースデータ］のシール冷却時間の間、ウエイトし
*                時間経過後、工程終了動作へ移行する。
*                 ■シール電磁弁->ON （開く）
*              ・［コースデータ］のソフト開放時間が設定されている場合、［コースデータ］のシール冷却時間
*                 設定時間ウエイトと同時にソフト開放弁を開く（ＯＮ）
*                 ■ソフト解放弁->ON （開く）
*              ・ソフト開放時間が経過したら、ソフト開放弁を閉じる。
*                 ■ソフト解放弁->OFF （閉じる）
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void 	SeelCoolingMode(void)
{
	UWORD	data;

	glbFlProc = GCN_PROC_COOL;											/* 工程を冷却へ				*/
	ledControl_Proc(glbFlProc,1,0);		
		
	glbFlSecLevel   = GCN_VACUUM_SECONDS;
	ledControl_Sec(glbFlSecLevel,1,0);
		
	if(geefBfCourseData[geefFlCourse][SeelCooling]  == 0) 					/* ｼｰﾙ冷却工程時間が0秒の場合	*/
	{
		if(geefBfCourseData[geefFlCourse][SoftRelease] == 0)				/* ｿﾌﾄ解放工程時間が0秒の場合	*/
		{
			P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ｼｰﾙ電磁弁OFF							*/
			VacuumAllStop();												/* 真空運転出力信号をすべてOFF			*/
			VacuumEndControl();												/* 真空運転終了時の信号制御（真空電磁弁=10秒 ON,ﾌｧﾝ=3分間 ON*/
			glbFlDriveMode = GCN_DRIVE_END_MODE;							/* 真空運転工程終了動作へ遷移	*/
			return;
		}
	}


	if(glbCtSoftReleaseTimer == 0)										/* ｿﾌﾄ解放弁ON指定時間経過？			*/
	{
		P7 &= P75_SOFTRELEASE_START_OFF;								/* ｿﾌﾄ解放弁OFF							*/
	}

	data = ((glbCtSeelCoolingTimer * GCN_INT_TIME)/GCN_TIME_100MS);		/* 5ms単位のｶｳﾝﾀを0.1秒単位に変換		*/
	mainDisplay7Seg((UWORD)data);										/* 7SEG表示								*/
	if((glbCtSeelCoolingTimer == 0)&&(glbCtSoftReleaseTimer == 0))										/* ｼｰﾙ冷却指定時間経過？				*/
	{
		P7 &= P75_SOFTRELEASE_START_OFF;								/* ｿﾌﾄ解放弁OFF							*/
		
		P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ｼｰﾙ電磁弁OFF							*/
		VacuumAllStop();												/* 真空運転出力信号をすべてOFF			*/
		VacuumEndControl();												/* 真空運転終了時の信号制御（真空電磁弁=10秒 ON,ﾌｧﾝ=3分間 ON*/
		glbFlDriveMode = GCN_DRIVE_END_MODE;							/* 真空運転工程終了動作へ遷移	*/

	}
}



/***********************************************************************************************************************
*    関数名    DriveEndMode
*    引  数    
*    戻り値    
*    関数機能  　工程終了時動作
*    処理内容  ・チャンバー内を大気開放し、真空運転を終了する。
*               シール電磁弁	ＯＦＦ	閉じる
*				真空開放弁	    ＯＦＦ	開く
*               ポンプ	        ＯＦＦ	停止(*1)
*               ファン	        ＯＮ	ポンプ停止後３分間起動
*               真空電磁弁	    ＯＮ	ポンプ停止後１０秒間開く
*               ガス電磁弁	    ＯＦＦ	閉じる
*               ソフト開放弁	ＯＦＦ	閉じる
*               ヒーター信号	ＯＦＦ	通電停止
*              ・ポンプ停止時、配管内を大気開放する為、１０秒間真空弁を開く。真空弁を開いている途中で、
*                次の運転が始まった場合、その時点で次の運転を開始する。(DriveStartModeへ）
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	DriveEndMode(void)
{
	SWORD	Level;
	UBYTE	key;


	if((key = glbGetReadKey()))							/* SWの入力ﾁｪｯｸ					*/
	{
		if(key != GCN_KEY_STOP_SET)						/* 真空停止(SW6)以外の場合		*/
		{
			mcmSetBuzzerInvalid();						/* 無効ﾌﾞｻﾞｰ			*/
		}
	}
	
	if(glbCtWaitTimerT5 == 0)							/* 運転終了後のWAIT時間経過？	*/
	{
		vacFlAbnormalEnd = 1;
		if(vacFlEndAdLevel == 0)						/* T5時間内に真空度が開始時のﾏｲﾅｽ5%に達してない場合	*/
		{
			VacuumSetWaitMode(vacFlAbnormalEnd);		/* 待機ﾓｰﾄﾞへ遷移するため、7SEG及びLEDを真空工程の初期値に戻す*/
														/* ﾌﾞｻﾞｰを鳴らさずに待機ﾓｰﾄﾞへ遷移					*/
														/* ﾏｲﾅｽ5%に達した場合はﾌﾞｻﾞｰを鳴らして待機ﾓｰﾄﾞへ遷移	*/
		}
		glbFlMainMode = GCN_WAIT_MODE;					/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/
		return;
	}

	Level = GetVacuumLevel();							/* 真空ﾚﾍﾞﾙを算出										*/
	if(Level > vacCtStartAdLevel)						/* 真空ﾚﾍﾞﾙが真空開始時の(真空ﾚﾍﾞﾙ + 5%)以下になるまで真空工程を終了しない		*/
	{
		return;											/* 達するまで、このﾓｰﾄﾞに留まる													*/
	}
	
	if(!DrawerSwRead())	//フタが開いたら終了
	{
		return;	
	}
	
	vacFlEndAdLevel = 1;								/* 真空ﾚﾍﾞﾙが真空開始時の(真空ﾚﾍﾞﾙ + 5%)以下に達した							*/
	
	mcmSetBuzzerEnd();									/* 有効ﾌﾞｻﾞｰ						*/
	
	glbFlMainMode = GCN_WAIT_MODE;						/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/
	

	VacuumSetWaitMode(vacFlAbnormalEnd);				/* 待機ﾓｰﾄﾞへ遷移するため、7SEG及びLEDを真空工程の初期値に戻す*/
	
	glbFlPackCountDisplay_Start = 1;
	
	return;

}
/***********************************************************************************************************************
*    関数名    VacuumStart
*    引  数    
*    戻り値    
*    関数機能  真空運転終了から待機ﾓｰﾄﾞ遷移時の７ＳＥＧ／ＬＥＤ初期表示
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	VacuumSetWaitMode(uint8_t endType)
{
	UWORD	data;
	
	/*工程選択初期化*/
	glbFlReqDot = 0;											/* 「真空」工程のため、ﾋﾟﾘｵﾄﾞ表示なし					*/
	glbFlProc 		= GCN_PROC_VACUUM;							/* 工程を「真空」表示にする	*/
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
		
	/*正常にパック完了*/
	if(endType == 0)
	{
		geefCtPack++;				/* パック数	*/
		if(geefCtPack > 999)		/* 999回を超えた場合			*/
		{
			geefCtPack = 999	;	/* 999回を上限とする			*/
		}
	}
	else
	{	
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];			/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/
		}
		else
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* 選択されているｺｰｽ1の真空時間ﾘｰﾄﾞ				*/

		}
		mainDisplay7Seg(data);										/* 7SEG表示					*/
	}
	ledControl_Proc(glbFlProc,1,0);			

}
/***********************************************************************************************************************
*    関数名    VacuumStart
*    引  数    
*    戻り値    
*    関数機能  真空運転の冷却工程終了後の終了時処理
*    処理内容  
*    注意　　　Ｖｅｒ０２．００よりファンの使用は止め、ツインチャンバー時の親機・子機間の制御信号として使用。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	VacuumEndControl(void)
{

	P2 |= P20_VACUUM_ELECTROMAG_ON;						/* 真空電磁弁ON						*/
	glbFlVacuumElectromangReq = 1;						/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求ｾｯﾄ	*/
	glbCtVacuumElectromang = GCN_TIMER_10S;				/* 10秒ﾀｲﾏｾｯﾄ(5ms単位に変換した値）					*/

	glbCtFunTimer = GCN_TIME_3MN;						/* 暖気運転終了後、ﾌｧﾝを3分間回すためのﾀｲﾏｾｯﾄ	*/
	
	glbCtWaitTimerT5 = geefBfSystemData[GCN_SYSTEM_DRIVE_AFTER_WAIT]
                       [Default] * GCN_TIMER_1S;		/* 運転終了後WAIT時間(T5)	*/
					   
	vacFlEndAdLevel = 0;								/* 冷却工程終了後の真空度が開始時の真空度ﾏｲﾅｽ5%に達したた否かのﾌﾗｸﾞｸﾘｱ	*/

}
																
/***********************************************************************************************************************
*    関数名    VacuumStopMode
*    引  数    
*    戻り値    
*    関数機能  　真空停止機能
*    処理内容  ・真空引き工程中に操作パネルの「真空停止」スイッチ（隠しスイッチ）を１秒長押しされた場合、真空電磁弁を閉じて、
*                真空停止時間［T1］秒間真空引きを停止する。
*                	真空電磁弁	ＯＦＦ	閉じる
*		            ポンプ	    ＯＦＦ	停止
*              ・真空停止時間（［T1］秒）が経過した場合、または、再度操作パネルの「ｽﾀｰﾄ/ｽﾄｯﾌﾟ」スイッチ
*                を押された場合、真空電磁弁を開いて、真空工程を再開する。
*                   真空電磁弁	ＯＮ	開く
*                   ポンプ	    ＯＮ	稼働
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void VacuumStopMode(void)
{
	UBYTE	restart=0,key;
	UWORD	data;


	VacuumEmergencyStop();											/* 真空工程一時停止					*/
	if(glbFlSecLevel  == GCN_VACUUM_SECONDS )						/* 「秒」表示の場合					*/
	{
		data = ((glbCtVacuumEmergncyTimer * GCN_INT_TIME)/GCN_TIME_1S);	/* 5ms単位のｶｳﾝﾀを秒に変換			*/
	}
	else
	{
		data = GetVacuumLevel();									/* 真空ﾚﾍﾞﾙを算出										*/
	}
	mainDisplay7Seg((UWORD)data);									/* 7SEG表示							*/
	if(glbCtVacuumEmergncyTimer == 0)								/* 真空一時停止時間が経過したか？	*/
	{
		restart = 1;												/* 真空引き工程の再開				*/
	}
	if((key = glbGetReadKey()))										/* SWの入力ﾁｪｯｸ					*/
	{
		if(key == GCN_KEY_START_SET)								/* START/STOP(SW5)が押された場合	*/
		{
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			restart = 1;											/* 真空引き工程の再開				*/
		}
		else if(key == GCN_KEY_UP_SET)								/* ▲(SW3)の場合						*/
		{
			glbFlSecLevel = GCN_VACUUM_SECONDS;						/* 「秒」表示にする				*/
			ledControl_Sec(glbFlSecLevel,1,0);
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/

		}
		else if(key == GCN_KEY_DOWN_SET)							/* ▼(SW4)の場合						*/
		{
			glbFlSecLevel = GCN_VACUUM_LEVEL;						/* 「%」表示にする				*/
			ledControl_Sec(glbFlSecLevel,1,0);
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
		}
		else if(key == GCN_KEY_STOP_SET)							/* 真空停止(SW6)が押された場合	*/
		{
		}
		else
		{
			mcmSetBuzzerInvalid();									/* 無効ﾌﾞｻﾞｰ			*/
		}
	}
	if(restart == 1)												/* 真空引き工程の再開?				*/
	{
		DI();
		glbCtVacuumEmergncyTimer = 0;								/* 真空停止時間ﾘｾｯﾄ					*/
		glbCtVacuumTimeout1 = 0;									/* 真空ﾀｲﾑﾀｲﾑｱｳﾄ時間ｶｳﾝﾀｸﾘｱ			*/
		
		glbCtVacuumTimeout2 = 0;									/* 5秒経過しても真空ﾚﾍﾞﾙが20%に達しないことをﾁｪｯｸするｶｳﾝﾀｸﾘｱ	*/
		glbCtVacuumTimeout3 = 0;									/* 30秒経過後、真空ﾚﾍﾞﾙが90%ﾁｪｯｸするｶｳﾝﾀｸﾘｱ	*/

		EI();
		glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
	                           GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;		/* 「秒」or 「%」を真空工程前の表示に戻す			*/
		ledControl_Sec(glbFlSecLevel,1,0);
									
		vacFlRestart = 1;
		glbFlDriveMode = GCN_DRIVE_START_MODE;						/* 真空引き工程へ遷移				*/
																	/* 真空引き工程の初期から再開（途中からではない）	*/
	}
}

/***********************************************************************************************************************
*    関数名    VacuumStart
*    引  数    
*    戻り値    
*    関数機能  真空引き開始時の制御信号出力
*    処理内容  
*    注意　　　ツインチャンバー仕様時の子機指定の場合はポンプ稼働はしない。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void VacuumStart(void)
{
	DI();

	P7 |= P74_PUMP_START_ON;									/* ﾎﾟﾝﾌﾟ稼働開始					*/ 

	EI();
	P4 |= P41_VACUUM_RELEASE_ON;									/* 真空解放弁ON						*/ 
	P2 |= P20_VACUUM_ELECTROMAG_ON;									/* 真空電磁弁ON						*/
	glbFlVacuumElectromangReq = 0;									/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求ﾘｾｯﾄ	*/

}

/***********************************************************************************************************************
*    関数名    VacuumAllStop
*    引  数    
*    戻り値    
*    関数機能  真空運転（真空工程～シール冷却）に関わるすべての制御出力信号をOFFする
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void VacuumAllStop(void)
{
	if(glbFlWarmCansel == 0)										/* 運転前に「暖気運転」が起動されていた場合、ポンプは停止せずポンプ運転を継続する。	*/
																	/* よって、暖気運転の途中で真空工程に入った場合はﾎﾟﾝﾌﾟを停止させない（OFFしない)　	*/
	{
		DI();
		P7 &= P74_PUMP_START_OFF;									/* ﾎﾟﾝﾌﾟ稼働停止					*/ 
		EI();
	}
	
	P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* 真空電磁弁OFF					*/
	P4 &= P41_VACUUM_RELEASE_OFF;									/* 真空解放弁OFF						*/ 
	P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ｼｰﾙ電磁弁OFF							*/
	DI();
	P7 &= P74_PUMP_START_OFF;										/* ﾎﾟﾝﾌﾟ稼働停止						*/ 
	P7 &= P73_HEATER_START_OFF;										/* ﾋｰﾀOFF								*/
	P7 &= P75_SOFTRELEASE_START_OFF;								/* ｿﾌﾄ解放弁OFF							*/
	EI();
	P13 &= P130_GUS_ELECTROMAG_OFF;									/* ｶﾞｽ電磁弁 OFF						*/

}

/***********************************************************************************************************************
*    関数名    VacuumEmergencyStop
*    引  数    
*    戻り値    
*    関数機能  真空引き一時停止時のOFFする出力信号
*    処理内容  真空電磁弁	ＯＦＦ	閉じる
*			   ポンプ	    ＯＦＦ	停止
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void VacuumEmergencyStop(void)
{
	DI();
	P7 &= P74_PUMP_START_OFF;										/* ﾎﾟﾝﾌﾟ稼働停止					*/ 
	EI();
	P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* 真空電磁弁OFF						*/
}

/***********************************************************************************************************************
*    関数名    DrawerSwRead
*    引  数    
*    戻り値    0:閉、1:開
*    関数機能  引出しSWの開閉検出
*    処理内容  真空電磁弁	ＯＦＦ	閉じる
*			   ポンプ	    ＯＦＦ	停止
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

UBYTE	DrawerSwRead(void)
{
	UBYTE	data1,data2,sts;

	data1 = P13;											/* ﾄﾚｰ「閉」状態			*/
	data2 = P13;
	if(data1 != data2)
	{
		data1 =  P13;
	}
	if(!(data1 & P137_DRAWER_SW))							/* ﾄﾚｰ「閉」状態?			*/
	{
		sts = 0;											/* 「閉」*/
	}
	else
	{
		sts = 1;											/* 「開」*/
	}
	return(sts);

}

/***********************************************************************************************************************
*    関数名    GetVacuumLevel
*    引  数    
*    戻り値    真空ﾚﾍﾞﾙ（小数第一位まで）または真空ｴﾗｰ（-1)
*    関数機能  真空ﾚﾍﾞﾙ算出
*    処理内容  真空ﾚﾍﾞﾙを0%から99%まで算出。
*    備考：　　真空引きが始まると、ADCより読み込んだAD値は小さくなる
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

SWORD	GetVacuumLevel(void)
{
	long int	adc;
	long int	y;

	adc = glbCtRealAD;												/* 平均化されたAD値入力　			*/
	y =(GCN_NORMAL_LEVEL - (adc +geefCtAnalogAdjust)) *1000 / 		/* （標準大気圧の時のAD値－（AD値+補正値））÷（標準大気圧のAD値－基準値）*/
       (GCN_NORMAL_LEVEL - GCN_BASE_AD	);							
	y = (y + 5)/10;													/* 四捨五入して、小数点以下は切り捨てて整数にする	*/

	
	if(y <= 0)														/* 0%以下の場合						*/
	{
		y = 0;														/* 0.0%に補正						*/
	}
	if(y >= 99)														/* 99%以上の場合					*/
	{
		y = 99;														/* 99％に補正						*/
	}
	return((SWORD)y);
}
