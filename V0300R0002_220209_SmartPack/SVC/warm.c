#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"



void	mainWarmMode(void);
void	mainWarmStart(void);
void	mainWarmEnd(void);
void	mainWarmDrive(void);

void	(*WarmProcTbl[])(void);

extern	void	mainDisplay7Seg(UWORD);										/* 7SEG表示							*/
extern	UBYTE	glbGetReadKey(void);
extern	UBYTE	DrawerSwRead(void);
extern	void	mcmSetBuzzerEnd(void);										/* 終了ﾌﾞｻﾞｰを鳴らす			*/
extern	UBYTE	mcmCheckDrawerOpenToClose(void);
extern	UBYTE	mcmFlashWriteCheck(void);							/* ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘ変更ﾁｪｯｸ				*/
extern	void	mcmSetBuzzerInvalid(void);
extern	void	mcmSetBuzzerEffective(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

void	(*WarmProcTbl[])() =
{
	mainWarmDrive				/* 暖機運転監視						*/
};
/***********************************************************************************************************************
*    関数名    mainDriveMode
*    引  数    
*    戻り値    
*    関数機能  暖気運転モードメイン処理
*    処理内容  暖機運転と終了後のT5時間監視処理
*    注意　　　
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void mainWarmMode(void)
{
	(*WarmProcTbl[glbFlWarmMode])();								/* 現状態のﾓｰﾄﾞへ遷移		*/

}
/***********************************************************************************************************************
*    関数名    mainWarmDrive
*    引  数    
*    戻り値    
*    関数機能  暖気運転処理
*    処理内容  
*    注意　　　暖機運転中にＳＴＡＲＴ／ＳＴＯＰスイッチが押された場合は、暖機運転を中止する
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainWarmDrive(void)
{
	UBYTE	key;
	UWORD	data;

	glbFlSaveMode = glbFlDriveMode;										/* ﾒｲﾝﾓｰﾄﾞの状態を退避			*/
	data = ((glbCtWarmTimer * GCN_INT_TIME)/GCN_TIME_1S);				/* 5ms単位のｶｳﾝﾀを秒に変換			*/
	mainDisplay7Seg((UWORD)data);										/* 7SEG表示							*/

	if((key = glbGetReadKey()))											/* KEYの入力ﾁｪｯｸ					*/
	{		
		if(key == GCN_KEY_START_SET)									/* ｽﾀｰﾄ/ｽﾄｯﾌﾟｷｰが押された場合		*/
		{
			mcmSetBuzzerEffective();									/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			mainWarmEnd();												/* 暖気運転終了処理					*/
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
		else if(key == GCN_KEY_STOP_SET)								/* ｽﾄｯﾌﾟｷｰ（隠し）が押された場合		*/
		{
																		/* ﾌﾞｻﾞｰを鳴らさない					*/
		}
		else 
		{
			mcmSetBuzzerInvalid();										/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
		}
	}
	
	if(glbFlPowerSw)
	{	
		mcmSetBuzzerEffective();									/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
		mainWarmEnd();												/* 暖気運転終了処理					*/
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
				

	if(glbCtVacuumElectromang == 0)										/* 真空電磁弁ON経過？(0.5秒)		*/
	{
		P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* 真空電磁弁OFF					*/
	}

	if(glbCtWarmTimer == 0)												/* 暖気運転終了時間経過?			*/
	{
		mainWarmEnd();													/* 暖気運転終了処理					*/
		glbFlWarmMode = 1;												/* 暖機運転ﾓｰﾄﾞをT5時間監視へ		*/
	}

}

/***********************************************************************************************************************
*    関数名    mainWarmStart
*    引  数    
*    戻り値    
*    関数機能  暖気運転開始処理
*    処理内容  
*    注意　　　暖気運転再開の場合は、０．５秒間の真空電磁弁はONしないこと。。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainWarmStart(void)
{
	unsigned long int	data;
	volatile	int	i;
	
	DI();

	P7 |= P74_PUMP_START_ON;										/* ﾎﾟﾝﾌﾟ稼働開始					*/ 

	EI();
	
	ledControl_Proc(ALL_LED_SELECT,0,0);									

	glbFlReqDot         = 0;											/* ﾋﾟﾘｵﾄﾞ表示無し					*/
	glbFlWarmMode = 0;													/* 暖機運転ﾒｲﾝﾌﾗｸﾞ⇒暖機運転中へ	*/
	if(glbFlWarmCansel == 0)											/* 暖気運転途中ｷｬﾝｾﾙ⇒真空運転⇒再度、暖気運転の場合は、0.5秒間の真空電磁ONは行わない	*/
	{
		P2 |= P20_VACUUM_ELECTROMAG_ON;									/* 真空電磁弁ON						*/
		glbCtVacuumElectromang = GCN_TIMER_500MS;						/* 真空電磁弁を0.5秒開くためのﾀｲﾏｾｯﾄ*/
		glbFlVacuumElectromangReq = 1;									/* 真空電磁弁を0.5秒開くための要求ｾｯﾄ	*/
	}
	data = geefBfSystemData[GCN_SYSTEM_WARM][Default];
	mainDisplay7Seg((UWORD)data);										/* 7SEG表示							*/

	glbFlSecLevel   =  GCN_VACUUM_SECONDS;								/* 「秒」表示にする					*/
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbCtWarmTimer = (geefBfSystemData[GCN_SYSTEM_WARM][Default] *		/* 暖気運転時間ｾｯﾄ（秒）			*/
					 GCN_TIMER_1S);

}

/***********************************************************************************************************************
*    関数名    mainWarmEnd
*    引  数    
*    戻り値    
*    関数機能  暖気運転終了処理
*    処理内容  
*    注意　　　
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	mainWarmEnd(void)
{
	UWORD	data;

	DI();
	P7 &= P74_PUMP_START_OFF;											/* ﾎﾟﾝﾌﾟ稼働停止					*/ 
	EI();
	P2 |= P20_VACUUM_ELECTROMAG_ON;										/* 真空電磁弁ON						*/
	glbFlVacuumElectromangReq = 1;										/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求ｾｯﾄ	*/
	glbCtVacuumElectromang = GCN_TIMER_10S;								/* 暖気運転終了後、真空電磁弁を10秒間開く（ON）するためのﾀｲﾏｾｯﾄ	*/
// ↓20180630
//	glbFlFunReq = 1;													/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後,3分間ﾌｧﾝを駆動させるための要求ｾｯﾄ	*/
// ↑	20180630
	glbCtFunTimer = GCN_TIME_3MN;										/* 暖気運転終了後、ﾌｧﾝを3分間回すためのﾀｲﾏｾｯﾄ	*/
	glbFlWarmCansel = 0;
	mcmSetBuzzerEnd();													/* 終了ﾌﾞｻﾞｰ			*/
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)			/* '秒'表示の場合								*/
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];					/* 選択されているｺｰｽの真空時間ﾘｰﾄﾞ				*/
	}
	else																/* '%'表示の場合								*/
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];				/* 選択されているｺｰｽの真空時間ﾘｰﾄﾞ				*/

	}
	mainDisplay7Seg(data);												/* 7SEG表示					*/
	
	glbFlProc 		= GCN_PROC_VACUUM;						/* 工程を「真空」にする	*/
	ledControl_Proc(glbFlProc,1,0);											
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbFlMainMode = GCN_WAIT_MODE;					/* ﾒｲﾝﾓｰﾄﾞを待機ﾓｰﾄﾞへ		*/

}


