#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"
#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"


/*============バージョン============*/
/* スマートパックﾘﾘｰｽ　220201		
・引き出し?スマートパック化
・自動アナログ調整後のバグ修正
・データフラッシュ破損？データ設定範囲外を追加
・データフラッシュの書き込みタイミング追加
　※電源切り時の書き込みは不可。基板入力電源の立下がり具合によって、書き込み失敗する恐れある
・データフラッシュの書き込み時に固定ENDコード追加
・各LED制御を関数にて行うように修正。
　※元は出力用変数を直接弄っていたが配列指定でも使っているため、範囲外になる恐れあった
 ※電圧検出時のリセットは使わないこと。ハードのリセット回路との関係の検証未のため

*/	
const	UBYTE	Version[] = {"Ver03.00"};

//const	UBYTE	Revision[] = {"Rev00.00"};
//const	UBYTE	Revision[] = {"Rev00.01"};	//出力検査時のファンOut不具合修正＆ﾘﾋﾞｼﾞｮﾝ追加
const	UBYTE	Revision[] = {"Rev00.02"};	//表示検査時の連続と予備LEDの不具合修正


/*=======================================*/


void	sysSoftVersionMode(void);					/* ｿﾌﾄﾊﾞｰｼﾞｮﾝﾓｰﾄﾞ処理				*/
void	sysRamClearMode(void);						/* RAMｸﾘｱ設定ﾓｰﾄﾞ					*/
void	sysAnalogAdjustMode(void);					/* ｱﾅﾛｸﾞ調整ﾓｰﾄﾞ					*/
void	sysPanelTestMode(void);						/* ﾊﾟﾈﾙﾃｽﾄﾓｰﾄﾞ						*/
void	sysInputTestMode(void);						/* 入力ﾃｽﾄﾓｰﾄﾞ						*/
void	sysOutputTestMode(void);					/* 出力ﾃｽﾄﾓｰﾄﾞ						*/
void	sysErrorLogMode(void);					/* ｴﾗｰﾛｸﾞﾓｰﾄﾞ						*/

void	mainSystemMode(void);
UBYTE	glbGetDipSW(void);
void	sys7SegLedAllOff(void);						/* 7SEG/LED ALL OFF			*/
void	sys7SegLedAllOn(void);						/* 7SEG/LED ALL ON			*/
void	sysLedAllOff(void);							/* LED ALL OFF			*/
void	sysLedAllOn(void);							/* LED ALL ON				*/
void	sysAnalogAdjustAuto(void);
void	sysRamClearMode(void);
void	sysErrorLogMode(void);					/* ｴﾗｰﾛｸﾞﾓｰﾄﾞ						*/
void	sysPumpTimeMode(void);

UWORD	R_ADC_Get_Result2(void);
void	sysGetVacuumLevel(UBYTE ,UBYTE *,UBYTE *);
void	sysGetPumpTime(UBYTE ,UBYTE *,UBYTE *);
void	sysFlashWrite(void);
void	(*sysProcTbl[])(void);
void	sysVacuumStart(void);						/* 真空引き開始				*/

UBYTE	sysFlMode;
UBYTE	sysFlCourse;
extern	UBYTE   glbWaitKeyOff(UBYTE,UWORD);
extern	UBYTE	glbGetReadKey(void);
extern	void	mainDisplay7Seg(UWORD);			/* 7SEG表示							*/
extern	void	VacuumStart(void);						/* 真空引き開始				*/
extern	UBYTE   glbWaitKeyOn(UBYTE KeyNo);
extern	void	mainVacuumElectromangCheck(void);
//extern	void	mainFlashClear(void);
extern	UBYTE   glbWaitKeyOff(UBYTE,UWORD);
extern	void	mcmSetBuzzerInvalid(void);					/* 無効ﾌﾞｻﾞｰ			*/
extern	void	mcmSetBuzzerEffective(void);
extern	void	mainErrorMode(void);				/* ｴﾗｰ検出ﾓｰﾄﾞ				*/
extern	UBYTE	pdlWriteFlash(void);
extern	void	pldRestoreBackupData(void);
extern	UBYTE	DrawerSwRead(void);
extern	void	VacuumEndControl(void);
extern	UBYTE	mcmFlashDataInitial(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

void	(*sysProcTbl[])() =
{
	sysSoftVersionMode,					/* ｿﾌﾄﾊﾞｰｼﾞｮﾝﾓｰﾄﾞ処理				*/
	sysRamClearMode,					/* RAMｸﾘｱ設定ﾓｰﾄﾞ					*/
	sysAnalogAdjustMode,				/* ｱﾅﾛｸﾞ調整ﾓｰﾄﾞ					*/
	sysPanelTestMode,					/* ﾊﾟﾈﾙﾃｽﾄﾓｰﾄﾞ						*/
	sysInputTestMode,					/* 入力ﾃｽﾄﾓｰﾄﾞ						*/
	sysOutputTestMode,					/* 出力ﾃｽﾄﾓｰﾄﾞ						*/
	sysErrorLogMode,					/* ｴﾗｰﾛｸﾞﾓｰﾄﾞ						*/
	sysPumpTimeMode						/* ﾎﾟﾝﾌﾟ稼働時間表示ﾓｰﾄﾞ			*/

};

#define	LCN_SOFTWARE_VERSION		0						/* ｿﾌﾄｳｪｱﾊﾞｰｼﾞｮﾝ表示		*/
#define	LCN_RAM_CLEAR				1						/* RAMｸﾘｱ					*/
#define	LCN_ANALOG_ADJUST			2						/* ｱﾅﾛｸﾞ調整ﾓｰﾄﾞ			*/
#define	LCN_PANEL_TEST				3						/* ﾊﾟﾈﾙﾃｽﾄﾓｰﾄﾞ				*/
#define	LCN_INPUT_TEST				4						/* 入力ﾃｽﾄﾓｰﾄﾞ				*/
#define	LCN_OUTPUT_TEST				5						/* 出力ﾃｽﾄﾓｰﾄﾞ				*/
#define	LCN_ERROR_LOG				6						/* ｴﾗｰﾛｸﾞ表示ﾓｰﾄﾞ			*/
#define	LCN_PUMP_TIME				7						/* ﾎﾟﾝﾌﾟ稼働時間表示ﾓｰﾄﾞ	*/
#define	LCN_NON_SYSTEM_MODE			8						/* bit0が0の場合は通常ﾓｰﾄﾞへ	*/

UBYTE	DipSwToIndexTBL[16] = {
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_SOFTWARE_VERSION	,					/* ｿﾌﾄｳｪｱﾊﾞｰｼﾞｮﾝ表示		*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_INPUT_TEST		,					/* 入力ﾃｽﾄﾓｰﾄﾞ				*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_ANALOG_ADJUST		,					/* ｱﾅﾛｸﾞ調整ﾓｰﾄﾞ			*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_ERROR_LOG			,					/* ｴﾗｰﾛｸﾞ表示ﾓｰﾄﾞ			*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_RAM_CLEAR			,					/* RAMｸﾘｱ					*/
						      LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_OUTPUT_TEST		,					/* 出力ﾃｽﾄﾓｰﾄﾞ				*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
						      LCN_PANEL_TEST		,					/* ﾊﾟﾈﾙﾃｽﾄﾓｰﾄﾞ				*/
							  LCN_NON_SYSTEM_MODE	,					/* 通常ﾓｰﾄﾞへ				*/
							  LCN_PUMP_TIME								/* ﾎﾟﾝﾌﾟ稼働時間表示		*/
							};



/***********************************************************************************************************************
*    関数名    mainSystemMode
*    引  数    
*    戻り値    
*    関数機能  テストモードメイン処理
*    処理内容  DIPSWの設定により、各テスト項目に遷移する
*              該当するテスト項目が無いDIPSWの設定の場合は通常ﾓｰﾄﾞに遷移する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void mainSystemMode(void)
{
	UBYTE	key;
	
	sysFlMode = DipSwToIndexTBL[glbFlDipSW];				/* DipSW状態によるﾃｽﾄNo.を読み込む			*/
	if(sysFlMode == LCN_NON_SYSTEM_MODE)					/* 該当するﾃｽﾄﾓｰﾄﾞがない場合は、通常ﾓｰﾄﾞへ遷移	*/
	{
		if( glbFlDipSW & 0x08 )	//　ソフト開放有無
		{
			//ソフト開放無
			glbFlSoftOpen = 0;
		}
		else
		{
			//ソフト開放有
			glbFlSoftOpen = 1;
		}
		return;
	}
	
	glbFlSystemMode = 1;									/* ｼｽﾃﾑﾓｰﾄﾞ中ﾌﾗｸﾞ			*/
	sysFlCourse = geefFlCourse;								/* ｺｰｽNo.ｾｰﾌﾞ					*/
	sys7SegLedAllOff();										/* 7SEG/LED ALL OFF			*/
	glbDt7Seg2 = 'D' - 0x37;								/* 7SEGの2桁目に'd'を表示		*/
	glbDt7Seg1 = sysFlMode;									/* 7SEGの1桁目にﾃｽﾄ項目No.を表示		*/
	while(1)												/* PUSH(SW7)（基盤裏側）が押されるまでｳｪｲﾄ	*/
	{
		if(key = glbGetReadKey())							/* KEYの入力ﾁｪｯｸ			*/
		{
			if( (key == GCN_KEY_PUSH_SET)||(key == GCN_KEY_START_SET) )	//スタートSWでも良い
			{
				break;
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
	}
	mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
	while(1)
	{
		(*sysProcTbl[sysFlMode])();							/* 現状態のﾓｰﾄﾞへ遷移		*/
	}
	

}

/***********************************************************************************************************************
*    関数名    sysSoftVersionMode
*    引  数    
*    戻り値    
*    関数機能  ﾌｧｰﾑｳｪｱのﾊﾞｰｼﾞｮﾝを表示する
*    処理内容  ・パネルスイッチが押されていない場合バージョンの上位２桁及び[.]を表示
*			   ・ ▼のスイッチを押している間、バージョンの下位２桁を表示（▼スイッチを放すとバージョン上位表示に戻る）
*    備考　　　・他のスイッチ操作は全て無効とする。
*              ・主電源切断で終了。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void sysSoftVersionMode(void)
{
	UBYTE	key,data,sts;
	UWORD	MaxKeyOnTime;
	
	static UBYTE   turnFl = 0;


	while(1)
	{
		if(turnFl == 0)	//Ver表示
		{
			ledControl_Sec(ALL_LED_SELECT,0,0);	
			
			data = (Version[3] - '0')*10;
			data = data + (Version[4] - '0');
			glbFlReqDot = 2;											/* 1桁の位置にﾋﾟﾘｵﾄﾞ表示	*/
			mainDisplay7Seg(data);										/* ﾊﾞｰｼﾞｮﾝの上位2桁を表示	*/

			if((key = glbGetReadKey()))									/* KEYの入力ﾁｪｯｸ			*/
			{
				if(key == GCN_KEY_DOWN_SET)								/* ▼(SW4)入力？				*/
				{
					MaxKeyOnTime = GCN_TIME_100MS;						/* SW長押し通知時間	（0.1秒)			*/
					sts = 1;
					mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
					while(sts == 1)										/* ｷｰが離されるまでﾊﾞｰｼﾞｮﾝの下位2桁を表示	*/
					{
						data = (Version[6] - '0')*10;
						data = data + (Version[7] - '0');
						glbFlReqDot = 0;								/* ﾋﾟﾘｵﾄﾞ表示無し			*/
						mainDisplay7Seg(data);							/* ﾊﾞｰｼﾞｮﾝの下位2桁を表示	*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* SWが離されるのを待つ				*/
					}
				}
				else if(key == GCN_KEY_PUSH_SET)
				{
					turnFl = 1;
				}
				else
				{
					mcmSetBuzzerInvalid();								/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
				}
			}
		}
		else	//Rev表示
		{
			ledControl_Sec(ALL_LED_SELECT,1,0);	
			
			data = (Revision[3] - '0')*10;
			data = data + (Revision[4] - '0');
			glbFlReqDot = 2;											/* 1桁の位置にﾋﾟﾘｵﾄﾞ表示	*/
			mainDisplay7Seg(data);										/* ﾊﾞｰｼﾞｮﾝの上位2桁を表示	*/

			if((key = glbGetReadKey()))									/* KEYの入力ﾁｪｯｸ			*/
			{
				if(key == GCN_KEY_DOWN_SET)								/* ▼(SW4)入力？				*/
				{
					MaxKeyOnTime = GCN_TIME_100MS;						/* SW長押し通知時間	（0.1秒)			*/
					sts = 1;
					mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
					while(sts == 1)										/* ｷｰが離されるまでﾊﾞｰｼﾞｮﾝの下位2桁を表示	*/
					{
						data = (Revision[6] - '0')*10;
						data = data + (Revision[7] - '0');
						glbFlReqDot = 0;								/* ﾋﾟﾘｵﾄﾞ表示無し			*/
						mainDisplay7Seg(data);							/* ﾊﾞｰｼﾞｮﾝの下位2桁を表示	*/
						sts = glbWaitKeyOff(key,MaxKeyOnTime);			/* SWが離されるのを待つ				*/
					}
				}
				else if(key == GCN_KEY_PUSH_SET)
				{
					turnFl = 0;
				}
				else
				{
					mcmSetBuzzerInvalid();								/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
				}
			}
			
			
		}
	}
}


/***********************************************************************************************************************
*    関数名    sysAnalogAdjustMode
*    引  数    
*    戻り値    
*    関数機能  アナログ調整モード
*    処理内容  圧力センサー／ハードウエア／チャンバー内容量において、微妙な個体差が発生する。
*			   その為、１００％の場合の基準値に加算する為の補正値（アナログ調整値）を求める。
*              アナログ調整値は,－９９～＋９９である。
*              ＜手動調整方法＞
*					［▲］［▼］で表示されている調整値を±１ずつＵＰ／ＤＯＷＮする。（範囲：－９９～＋９９）
*                    ‐の場合は、秒のLEDを点灯させ、「工程」スイッチを押すと、変更後の値を記憶する。
*              ＜自動調整方法＞
*					 sysAnalogAdjustAuto関数参照。
*    備考　　　
*             
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysAnalogAdjustMode(void)
{
	UBYTE	key,Flag=0,sts;
	UBYTE	data;
	SWORD	sdata;
	UWORD	MaxKeyOnTime;

	glbFlAdjust = 1;

	data = geefCtAnalogAdjust;								/* ｱﾅﾛｸﾞ調整値ﾘｰﾄﾞ		*/
	if(data & 0x80	)										/* 負の場合				*/
	{
		data = ~data+1;										/* 調整値の2の補数		*/
		Flag = 1;											/* 負を表すためのLEDの秒を点灯	*/
	}
	glbFlReqDot = 0;										/* ﾋﾟﾘｵﾄﾞ無						*/
	mainDisplay7Seg(data);									/* 調整値を7SEG表示					*/
	glbFlSecLevel = (Flag)?GCN_VACUUM_SECONDS:				/* 負の場合は、'秒'を点灯			*/
						   GCN_VACUUM_LEDALLOFF;
	ledControl_Sec(glbFlSecLevel,1,0);
			
						   
	sdata = (SWORD)((Flag)?~data+1:data);					/* 負の場合なら、再度、値を負にする	*/
	while(1)
	{
		if((key = glbGetReadKey()))							/* SWの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)入力？				*/
			{
				if(DrawerSwRead())							/* 引出しSWが「開」状態の場合	*/
				{
					mcmSetBuzzerInvalid();					/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
					return;
				}
				else
				{
					mcmSetBuzzerEffective();				/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				}
				sysAnalogAdjustAuto();						/* 自動調整ﾓｰﾄﾞ				*/
			}
			else if((key == GCN_KEY_UP_SET) ||				/* ▲(SW3)が押された場合		*/
				    (key == GCN_KEY_DOWN_SET))
			{
				sts = 0;
				MaxKeyOnTime = GCN_TIME_1S;					/* ｷｰ長押し通知時間	（1秒)				*/
				do
				{
					if(key ==  GCN_KEY_UP_SET)				/* ▲(SW3)						*/
					{
						sdata++;							/* 調整値+1					*/
					}
					else									/* ▼(SW4)						*/
					{
						sdata--;							/* 調整値-1					*/
					}
					if(sdata >= 0)
					{
						ledControl_Sec(ALL_LED_SELECT,0,0);
					}
					if(sdata > 99)							/* 上限ﾁｪｯｸ						*/
					{
						sdata = 99;
						mcmSetBuzzerInvalid();				/* 無効ﾌﾞｻﾞｰを鳴らす			*/
					}
					else
					{
						if(sts == 0)
						{
							mcmSetBuzzerEffective();		/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
						}
					}
					if(sdata < 0)
					{
						glbFlSecLevel = GCN_VACUUM_SECONDS;	/* 秒 ｾｯﾄ				*/
						ledControl_Sec(glbFlSecLevel,1,0);	
					}
					if(sdata < -99)							/* 下限ﾁｪｯｸ						*/
					{
						sdata = -99;
						mcmSetBuzzerInvalid();				/* 無効ﾌﾞｻﾞｰを鳴らす			*/
					}
					else
					{
						if(sts == 0)
						{
							mcmSetBuzzerEffective();		/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
						}
					}

					if(glbFlSecLevel == GCN_VACUUM_SECONDS)	/* 負の場合?			*/
					{
						data = (UBYTE)(~sdata + 1);
					}
					else
					{
						data = (UBYTE)(sdata);
					}
					mainDisplay7Seg((UBYTE)data);			/* 調整値を7SEG表示					*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);	/* SWが離されるのを待つ				*/
					MaxKeyOnTime = GCN_TIME_100MS;			/* ｷｰ長押し通知時間	（0.1秒)			*/
				}while(sts == 1);
				DI();
				glbFlKydt = 0;								/* ﾘﾋﾟｰﾄ中に入力した他ｷｰは捨てる		*/
				EI();
			}
			else if(key == GCN_KEY_PROC_SET)				/* 工程(SW2)が押された場合		*/
			{
				geefFlCourse = sysFlCourse;
				geefCtAnalogAdjust = sdata;					/* 調整値をﾊﾞｯｸｱｯﾌﾟﾒﾓﾘへｾｯﾄ	*/
				sysFlashWrite();
				
				ledControl_Course(ALL_LED_SELECT,0,0);							/* 7SEG/LED ALL OFF			*/
				mcmSetBuzzerEffective();					/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
	}
}

/***********************************************************************************************************************
*    関数名    sysAnalogAdjustMode
*    引  数    
*    戻り値    
*    関数機能  自動調整モード
*    処理内容  ①［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチを押すと、真空引きを開始する。（［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチを押した時点で
*			   　 自動調整と判断する）
*             			ポンプ		ＯＮ	稼働
*                       ファン		ＯＮ
*                       真空開放弁	ＯＮ	閉じる
*                       真空電磁弁	ＯＮ	開く
*			   ②［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチを押すと、真空引きを停止する。
*						真空電磁弁	ＯＦＦ	閉じる
* 			   ③［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチを押すと、真空引きを再開する。
*                  		真空電磁弁	ＯＮ	開く
*			   ④真空引き停止中に、［▼］スイッチを押されたら、押されている間だけ真空開放を行う。
*                       真空開放弁	ＯＦＦ	開く
*                       真空電磁弁	ＯＦＦ	閉じる
*                ［▼］スイッチを放したら、③真空引き停止中の状態に戻す。
*						真空開放弁	ＯＮ	閉じる
*                       真空電磁弁	ＯＦＦ	閉じる
*			　 ⑤　②～④を操作して、引出し内の真空計の値が「１．２ｋＰａ」になる様に調整する。
*    備考　　　
*             
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	sysAnalogAdjustAuto(void)
{
	UBYTE	key,sts,togle=0,data;
	SWORD	adData,MaxKeyOnTime ;

	sysVacuumStart();											/* 真空引き開始				*/
	while(1)
	{
		if(DrawerSwRead())							/* 引出しSWが「開」状態の場合	*/
		{
			DI();
			P7 &= P74_PUMP_START_OFF;					/* ﾎﾟﾝﾌﾟOFF 					*/
			EI();
			P4 &= P41_VACUUM_RELEASE_OFF;				/* 真空解放弁OFF（開く）		*/ 
			VacuumEndControl();							/* 真空運転終了時の信号制御（真空電磁弁=10秒 ON,ﾌｧﾝ=3分間 ON)*/
			
			break;
		}
		
		mainVacuumElectromangCheck();
		if((key = glbGetReadKey()))							/* SWの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)入力？				*/
			{
				mcmSetBuzzerEffective();					/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				DI();
				P7 |= P74_PUMP_START_ON;					/* ﾎﾟﾝﾌﾟON (稼働)			*/
				EI();
				P4 |= P41_VACUUM_RELEASE_ON;				/* 真空解放弁ON（閉じる）	*/ 
				togle++;									/* ﾄｸﾞﾙ更新					*/
				if(togle & 1)								/*							*/
				{
					P2 &= P20_VACUUM_ELECTROMAG_OFF;		/* 真空電磁弁OFF（真空引き停止)	*/
				}
				else
				{
					P2 |= P20_VACUUM_ELECTROMAG_ON;			/* 真空電磁弁ON	（真空引き再開）*/
				}
			}
			else if(key == GCN_KEY_PROC_SET)				/* 工程(SW2)押された場合			*/
			{
				adData = R_ADC_Get_Result2();				/* 圧力ｾﾝｻｰのA/D値入力		*/
				adData = GCN_BASE_AD - adData;				/* AD値補正(基準A/D(110 - A/D値)		*/
				if(adData > 99 || adData < -99)				/* 範囲ﾁｪｯｸ								*/
				{
					mcmSetBuzzerInvalid();					/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/

				}
				else
				{
				
					if(adData < 0)							/* 負の場合					*/
					{
						glbFlSecLevel = GCN_VACUUM_SECONDS;	/* 秒 ｾｯﾄ				*/
						ledControl_Sec(glbFlSecLevel,1,0);	
							
						data = (UBYTE)(~adData + 1);
					}
					else
					{
						ledControl_Sec(glbFlSecLevel,1,0);	
						data = (UBYTE)(adData);
					}
					geefFlCourse = sysFlCourse;				/* ｺｰｽ復元							*/			
					geefCtAnalogAdjust = data;				/* 調整値をﾊﾞｯｸｱｯﾌﾟﾒﾓﾘへｾｯﾄ	*/
					sysFlashWrite();
					
					mainDisplay7Seg((UBYTE)data);			/* 調整値を7SEG表示					*/
					ledControl_Course(ALL_LED_SELECT,0,0);							/* 7SEG/LED ALL OFF			*/ 
					mcmSetBuzzerEffective();				/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				}
			}
			else if(key == GCN_KEY_DOWN_SET)				/* ▼(SW4)							*/
			{
															/* 真空引き中の場合	、▼ｷｰが押されている間、真空解放を行う	*/
				if(togle & 1)								/* 真空引き中の場合？				*/
				{
					if(!(P7 & P74_PUMP_START_ON))			/* ﾎﾟﾝﾌﾟ停止中？ 					*/
					{
						mcmSetBuzzerInvalid();				/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
						continue;
					}
					mcmSetBuzzerEffective();				/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
					P2 &= P20_VACUUM_ELECTROMAG_OFF;		/* 真空電磁弁OFF（真空引き停止)	*/
					sts = 1;
					MaxKeyOnTime = GCN_TIME_100MS;			/* ｷｰ長押し通知時間	（0.1秒)			*/
					while(sts == 1)
					{
						sts = glbWaitKeyOff(key,MaxKeyOnTime);	/* KEYが離されるのを待つ				*/
						P4 &= P41_VACUUM_RELEASE_OFF;		/* 真空解放弁OFF（開く）		*/ 
					}
					P4 |= P41_VACUUM_RELEASE_ON;			/* 真空解放弁ON（閉じる）		*/ 
				}
				else										/* 真空引き中でない場合			*/
				{
					mcmSetBuzzerInvalid();					/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
				}
			}
			else if(key == GCN_KEY_UP_SET)					/* ▲(SW3)						*/
			{
				if(!(P7 & P74_PUMP_START_ON))				/* ﾎﾟﾝﾌﾟ停止中？ 					*/
				{
					mcmSetBuzzerInvalid();					/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
					continue;
				}
				mcmSetBuzzerEffective();					/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				DI();
				P7 &= P74_PUMP_START_OFF;					/* ﾎﾟﾝﾌﾟOFF 					*/
				EI();
				P4 &= P41_VACUUM_RELEASE_OFF;				/* 真空解放弁OFF（開く）		*/ 
				VacuumEndControl();							/* 真空運転終了時の信号制御（真空電磁弁=10秒 ON,ﾌｧﾝ=3分間 ON)*/
				
				break;
			}
			else
			{
				mcmSetBuzzerInvalid();						/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
	}
}


#define	LCN_7SEG_ALL_OFF	0x00
#define	LCN_7SEG_ALL_ON		0xff

#define	LCN_LED_ALL_OFF		8
#define	LCN_LED_ALL_ON		9


/***********************************************************************************************************************
*    関数名    sysPanelTestMode
*    引  数    
*    戻り値    
*    関数機能  パネルテストモード
*    処理内容  パネル上の表示部（７セグ、ＬＥＤ）、スイッチ部等の動作テストを行う。
*				操作１：［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチを押すと、「全てのＬＥＤ」及び「７セグ全セグメント表示」を点灯。
*					     再度押すと「全てのＬＥＤ」及び「７セグ全セグメント」を消灯。
*                       ［ｽﾀｰﾄ/ｼﾞｬﾝﾌﾟ］スイッチ押すたびに全点灯⇔全消灯を繰り返す。
*    			操作２：［コース］ＳＷを押すごとに、各「コースＬＥＤ」を点灯移動する。
*               操作３：［工程］ＳＷを押すごとに、各「工程ＬＥＤ」点灯移動する。
*               操作４：［ｽﾄｯﾌﾟ］ＳＷ（隠しスイッチ）を押すごとに、「秒」「％」のＬＥＤが点灯移動する。
*               操作５：［▲］スイッチを押すごとに、７セグＬＥＤの「セグメント」単位で個別に点灯移動する。
*
*    備考　　　・他のスイッチ操作は全て無効とする。
*              ・主電源切断で終了。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysPanelTestMode(void)
{
	SWORD	ctcourse = -1;
	SWORD	ctproc   = -1;
	SWORD	cttips   = -1;
	SWORD	ct7seg 	 = -1;
	UBYTE	key,togle=0;
	UBYTE	operation = 1;
	UBYTE	tbl7seg2[16] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UBYTE	tbl7seg1[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


	glbFlSysytem7Seg = 1;											/* ﾊﾟﾈﾙﾃｽﾄ7SEG用表示ﾓｰﾄﾞｾｯﾄ	*/
	sys7SegLedAllOff();												/* 7SEG/LED ALL OFF			*/
	while(1)
	{
		if((key = glbGetReadKey()))									/* KEYの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_START_SET)							/* ｽﾀｰﾄｷｰ入力？				*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				togle++;											/* 表示切換えﾌﾗｸﾞ更新		*/
				togle &= 1;											/* 表示切換えﾌﾗｸﾞﾄｸﾞﾙ		*/
				if(togle)
				{
					sys7SegLedAllOn();								/* 7SEG/LED ALL ON			*/
				}
				else
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
				}

				operation = 1;
				ctcourse = -1;
				ctproc   = -1;
				cttips   = -1;
				ct7seg 	 = -1;
			}
			else if(key == GCN_KEY_COURSE_SET)						/* ｺｰｽｷｰ入力？				*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				if(operation == 1)									/* 操作1から選択された場合	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				ctcourse++;											/* ｺｰｽ更新					*/
				ctcourse &= 0x03;									/* ｺｰｽ上限ﾁｪｯｸ				*/
				geefFlCourse =  ctcourse;							/* ｺｰｽNo.ｾｯﾄ				*/	
				ledControl_Course(geefFlCourse,1,0);					/* 7SEG/LED ALL OFF			*/
			}
			else if(key == GCN_KEY_PROC_SET)						/* 工程ｷｰｰ入力？			*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				if(operation == 1)									/* 操作1から選択された場合	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				ctproc++;											/* 工程更新					*/
				ctproc = (ctproc > GCN_PROC_COOL)?					/* 工程上限ﾁｪｯｸ				*/
						   GCN_PROC_VACUUM:ctproc;
				
				ledControl_Proc(ctproc,1,0);						
			}
			else if(key == GCN_KEY_STOP_SET)						/* ｽﾄｯﾌﾟｷｰ（隠しｷｰ）入力？	*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				if(operation == 1)									/* 操作1から選択された場合	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
				}
				cttips++;											/* 秒 or % 切換え更新	*/
				cttips &= 1;										
				glbFlSecLevel  = cttips;							/* 秒 or % ｾｯﾄ				*/
				ledControl_Sec(glbFlSecLevel,1,0);					
			}	
			else if(key == GCN_KEY_UP_SET)							/* ▲ｷｰ入力？				*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				if(operation == 1)									/* 操作1から選択された場合	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
					ct7seg 	 = -1;
				}
				ct7seg++;											/* ﾁｪｯｸ1ｾｸﾞﾒﾝﾄ更新			*/
				ct7seg &= 15;										/* 上限ﾁｪｯｸ					*/
				glbDt7Seg2 = tbl7seg2[ct7seg];						/* 2桁目の7SEGの1箇所ON		*/
				glbDt7Seg1 = tbl7seg1[ct7seg];						/* 1桁目の7SEGの1箇所ON		*/
			}
			else if(key == GCN_KEY_DOWN_SET)						/* ▼ｷｰ入力？				*/
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				if(operation == 1)									/* 操作1から選択された場合	*/
				{
					sys7SegLedAllOff();								/* 7SEG/LED ALL OFF			*/
					operation = 0;
					ct7seg 	 = -1;
				}
				ct7seg--;											/* ﾁｪｯｸ1ｾｸﾞﾒﾝﾄ更新			*/
				if(ct7seg < 0)										/* ﾏｲﾅｽに移行？				*/
				{
					ct7seg = 15;
				}
				glbDt7Seg2 = tbl7seg2[ct7seg];						/* 2桁目の7SEGの1箇所ON		*/
				glbDt7Seg1 = tbl7seg1[ct7seg];						/* 1桁目の7SEGの1箇所ON		*/
			}
			else
			{
				mcmSetBuzzerInvalid();								/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
	}
}

/***********************************************************************************************************************
*    関数名    sysInputTestMode
*    引  数    
*    戻り値    
*    関数機能  入力テストモード
*    処理内容  ・「７セグ」に圧力センサーの値（ＨＥＸ）を表示する。
*			   ・  表示範囲：０００ｈ～３ＦＦｈ
*    備考　　　・他のスイッチ操作は全て無効とする。
*              ・主電源切断で終了。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void 	sysInputTestMode(void)
{
	UWORD	sts,key,MaxKeyOnTime ;
	UBYTE	seg1,seg2;

	while(1)
	{

		if((key = glbGetReadKey()))								/* SWの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_DOWN_SET)							/* ▼(SW4)の場合				*/
			{
				sts = 1;
				MaxKeyOnTime = GCN_TIME_1S;						/* SW長押し通知時間	（1秒)				*/
				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				while(sts == 1)									/* SWが押されている間はADの下位1ﾊﾞｲﾄを表示	*/
				{
					sysGetVacuumLevel(0,&seg2,&seg1);			/* 下位1ﾊﾞｲﾄのAD値を2桁で入力				*/
																/* 例）0x3B7⇒seg1=0x07,seg2=0x0b			*/
 					glbDt7Seg2 = seg2;							/* 2桁目をｾｯﾄ								*/
 					glbDt7Seg1 = seg1;							/* 1桁目をｾｯﾄ								*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);		/* SWが離されるのを待つ					*/
					MaxKeyOnTime = GCN_TIME_100MS;				/* SW長押し通知時間	（0.1秒)				*/
				}
			}
			else
			{
				mcmSetBuzzerInvalid();							/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
		else
		{
			sysGetVacuumLevel(1,&seg2,&seg1);					/* 上位1ﾊﾞｲﾄのAD値を2桁で入力				*/
																/* 例）0x3B7⇒seg1=0x03,seg2=0x00			*/
			glbDt7Seg2 = seg2;									/* 2桁目をｾｯﾄ								*/
			glbDt7Seg1 = seg1;									/* 1桁目をｾｯﾄ								*/
		}
	}
}


/***********************************************************************************************************************
*    関数名    sysGetVacuumLevel
*    引  数    
*    戻り値    
*    関数機能  真空圧力センサーのAD値を入力
*    処理内容  AD値を入力し、１６進数の文字列に変換
*    備考　　　
*              
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysGetVacuumLevel(UBYTE culum,UBYTE *pSeg2,UBYTE *pSeg1)
{
	UWORD	iadData;
	UBYTE	data,answer[4],j,k,i;
	UBYTE	henkan[16] = "0123456789ABCDEF";

	for(i=0;i<4;++i)
	{
		answer[i] = '0';
	}
	iadData = R_ADC_Get_Result2();						/* 圧力ｾﾝｻｰのA/D値入力		*/
//	iadData = 0x3ff - iadData;							/* AD値を反転させる			*/
	i=0;
	while(iadData > 0) {								/* 10進数を16進文字列に変換	*/
		j = iadData % 16;
   		iadData = iadData / 16;
   		answer[i] = henkan[j];
   		i++;
	}
														/* 例）AD値：0x3b7の場合		*/
														/* answer[0] = '7',answer[1] = 'B',answer[2] = '3',answer[3] = '0'	*/
	k = culum * 2;										/* k=0 answer[0]とanswer[1] / k=2 nswer[2]とanswer[3] */

	if(answer[k] >= '0' && answer[k] <= '9')				
	{
		data = answer[k] - '0';
	}
	else
	{
		data = answer[k] - 0x37;
	}
	*pSeg1 = data;										/* 7SEGの1桁目に表示		*/
	if(answer[k+1] >= '0' && answer[k+1] <= '9')
	{
		data = answer[k+1] - '0';
	}
	else
	{
		data = answer[k+1] - 0x37;
	}
	*pSeg2 = data;										/* 7SEGの2桁目に表示		*/

}



/***********************************************************************************************************************
*    関数名    sysOutputTestMode
*    引  数    
*    戻り値    
*    関数機能  出力テストモード
*    処理内容  ・パネル上の各スイッチを操作して各出力機器のテストを行う。
*			   ・［▲］［▼］スイッチを押して、出力するユニットを選択
*              ・［スタート／ストップ］を押すたびに、表示されている番号対応の機器をＯＮ／ＯＦＦする。
*              ・[コース］スイッチを押されたら、全ての出力をＯＦＦする。
*    備考　　　・他のスイッチ操作は全て無効とする。
*              ・主電源切断で終了。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
#define	LCN_SYS_OUTPUT_NUM		8

struct	tagOUTPUTTEST	sysOutputTest[LCN_SYS_OUTPUT_NUM] = {
											{&P7,P74_PUMP_START_ON          ,P74_PUMP_START_OFF			},			/* ﾎﾟﾝﾌﾟ出力I/O,ONﾃﾞｰﾀ	*/
											{&P2,P20_VACUUM_ELECTROMAG_ON   ,P20_VACUUM_ELECTROMAG_OFF	},			/* 真空電磁弁I/O,ONﾃﾞｰﾀ	*/
											{&P13,P130_GUS_ELECTROMAG_ON    ,P130_GUS_ELECTROMAG_OFF     },			/* ｶﾞｽ電磁弁I/O,ONﾃﾞｰﾀ	*/
											{&P0,P01_SEEL_ELECTROMAG_ON		,P01_SEEL_ELECTROMAG_OFF	},			/* ｼｰﾙ電磁弁I/O,ONﾃﾞｰﾀ	*/
											{&P4,P41_VACUUM_RELEASE_ON		,P41_VACUUM_RELEASE_OFF		},			/* 真空解放弁I/O,ONﾃﾞｰﾀ	*/
											{&P7,P75_SOFTRELEASE_START_ON	,P75_SOFTRELEASE_START_OFF	},			/* ｿﾌﾄ解放弁I/O,ONﾃﾞｰﾀ	*/
											{&P7,P73_HEATER_START_ON        ,P73_HEATER_START_OFF		},			/* ﾋｰﾀ,ONﾃﾞｰﾀ			*/
											{&P7,P72_POWER_LED_ON          ,P72_POWER_LED_OFF 			}			/* ﾌｧﾝ,ONﾃﾞｰﾀ			*/
										   };
void	sysOutputTestMode(void)
{
	SWORD	OutputNo=0;
	UBYTE	key,i;
	volatile	UBYTE	*pIO;

	while(1)
	{
		if(glbFlSysHeaterReq == 1)								/* ﾋｰﾀ監視要求有り？		*/
		{
			if(glbCtSysHeaterOffTimer == 0)						/* 2秒経過？				*/
			{
				DI();
				*pIO &= sysOutputTest[6].BitOFF;				/* ﾋｰﾀOFF					*/
				EI();
			}
		}
		mainDisplay7Seg(OutputNo);								/* 出力機器No.を表示		*/
		if((key = glbGetReadKey()))								/* KEYの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_UP_SET)							/* ▲(SW3)入力？				*/
			{
				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				OutputNo++;										/* 出力No.更新				*/
				if(OutputNo >= LCN_SYS_OUTPUT_NUM)
				{
					OutputNo = 0;
				}
			}
			else if(key == GCN_KEY_DOWN_SET)					/*▼(SW4)入力？				*/
			{
				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				OutputNo--;										/* 出力No.更新				*/
				if(OutputNo < 0)
				{
					OutputNo = LCN_SYS_OUTPUT_NUM - 1;
				}
			}
			else if(key == GCN_KEY_START_SET)					/* START/STOP(SW5)入力？				*/
			{
				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				pIO = sysOutputTest[OutputNo].pIO;				/* 出力機器ﾎﾟｰﾄｱﾄﾞﾚｽﾘｰﾄﾞ	*/
				DI();
				if(*pIO & sysOutputTest[OutputNo].BitON)		/* 出力機器ﾎﾟｰﾄの指定ﾋﾞｯﾄがHの状態の場合	*/
				{
					DI();
					*pIO &= sysOutputTest[OutputNo].BitOFF;		/* 指定I/OのﾋﾞｯﾄをLにする	*/
					EI();
				}
				else
				{
					DI();


					*pIO |= sysOutputTest[OutputNo].BitON;		/* 指定I/OのﾋﾞｯﾄをLにする	*/
					EI();
					if(OutputNo == 6)							/* ﾋｰﾀ出力信号の場合		*/
					{
						glbFlSysHeaterReq = 1;					/* ﾋｰﾀ監視要求ｾｯﾄ			*/
						glbCtSysHeaterOffTimer = GCN_TIMER_2S;	/* 2秒監視ﾀｲﾏｾｯﾄ			*/
					}
				}
				EI();
			}
			else if(key == GCN_KEY_COURSE_SET)					/*ｺｰｽ(SW1)ｰ入力？				*/
			{
				mcmSetBuzzerInvalid();							/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
//				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				for(i=0;i<LCN_SYS_OUTPUT_NUM;i++)
				{
					pIO = sysOutputTest[i].pIO;					/* 出力機器ﾎﾟｰﾄｱﾄﾞﾚｽﾘｰﾄﾞ	*/
					DI();
					*pIO &= sysOutputTest[i].BitOFF;			/* OFF出力					*/
					EI();
				}
				glbFlSysHeaterReq = 0;							/* ﾋｰﾀ監視要求ﾘｾｯﾄ			*/
			}
			else
			{
				mcmSetBuzzerInvalid();														/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}

		}
	}		
}
/***********************************************************************************************************************
*    関数名    sysRamClearMode
*    引  数    
*    戻り値    
*    関数機能  RAMｸﾘｱ（ﾊﾞｯｸｱｯﾌﾟﾒﾓﾘの初期化）
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysRamClearMode(void)
{
	mcmFlashDataInitial();						/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀの初期化			*/
	glbDt7Seg2 = 'E' - 0x37;					/* 2桁目の7SEGに'E'を表示		*/
	glbDt7Seg1 = 'D' - 0x37;					/* 1桁目の7SEGに'd'を表示		*/
	sysLedAllOff();								/* すべてのLED　OFF				*/
	
	while(1);
}

/***********************************************************************************************************************
*    関数名    sysErrorLogMode
*    引  数    
*    戻り値    
*    関数機能  エラー履歴表示モード
*    処理内容 １０回分のエラー履歴を表示する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysErrorLogMode(void)					
{
	UBYTE	key,ErrorCourse,ErrorProcess,ErrorCode,flContent=0 ;
	SWORD	i = 1;

	mainDisplay7Seg(i);												/* 7SEG表示					*/
	while(1)
	{
		while(!(key = glbGetReadKey()))								/* SWの入力ﾁｪｯｸ			*/
		{
																	/* 入力がなければﾘﾀｰﾝ		*/
		}
		if(key == GCN_KEY_UP_SET)									/* ▲(SW3)の場合				*/
		{
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			i++;													/* ｴﾗｰ履歴No.更新(+)		*/
			if(i > 10)												/* 上限ﾁｪｯｸ					*/
			{
				i = 1;
			}
			flContent = 0;											/* START/STOP(SW5)が押されたことを認識するﾌﾗｸﾞﾘｾｯﾄ	*/
			mainDisplay7Seg(i);										/* 7SEG表示					*/
			sysLedAllOff();											/* LED ALL OFF	*/
		}
		else if(key == GCN_KEY_DOWN_SET)							/* ▼(SW4)の場合				*/
		{
			mcmSetBuzzerEffective();								/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			i--;													/* ｴﾗｰ履歴No.更新(-)		*/
			if(i < 1)												/* 下限ﾁｪｯｸ					*/
			{
				i = 10;
			}
			flContent = 0;											/* START/STOP(SW5)が押されたことを認識するﾌﾗｸﾞﾘｾｯﾄ	*/
			mainDisplay7Seg(i);										/* 7SEG表示					*/
			sysLedAllOff();											/* LED ALL OFF	*/
		}
		else if(key == GCN_KEY_START_SET)							/* START/STOP(SW5)					*/
		{
			if(flContent == 1)										/* START/STOP(SW5)が続けて押された場合		*/
			{
				mcmSetBuzzerInvalid();								/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
			else
			{
				mcmSetBuzzerEffective();							/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
			}
			ErrorCourse		= geefBfErrorHistory[i-1][1] >> 4;		/* ｴﾗｰ発生時のｺｰｽNo.ﾘｰﾄﾞ	*/
			ErrorProcess    = geefBfErrorHistory[i-1][1] & 0x0f;	/* ｴﾗｰ発生時の工程ﾘｰﾄﾞ		*/
 			if(ErrorCourse == 0)									/* ｺｰｽ運転中以外のｴﾗ-の場合	*/
			{
				ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL OFF			*/
			}
			else
			{
				geefFlCourse = ErrorCourse - 1;						/* ｺｰｽに対応したLED点灯		*/
				ledControl_Course(geefFlCourse,1,0);					/* 7SEG/LED ALL			*/
			}
			if(ErrorProcess == 0)									/* 工程運転以外の場合		*/
			{
				ledControl_Proc(ALL_LED_SELECT,0,0);									
			}
			else
			{
				ErrorProcess = (ErrorProcess > 1)?(ErrorProcess - 2):(ErrorProcess - 1);	/* ｺｰｽに対応したLED点灯		*/				   
			   ledControl_Proc(ErrorProcess,1,0);								
			}
			ErrorCode = geefBfErrorHistory[i-1][0];					/* 選択されたｴﾗｰ履歴No.のｴﾗｰｺｰﾄﾞﾘｰﾄﾞ	*/
			if(ErrorCode == 0xff)									/* ｴﾗｰ未発生の場合			*/
			{
				
				ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
				ledControl_Proc(ALL_LED_SELECT,0,0);											
				glbDt7Seg2 = 										/* 7SEGの2桁目に'-'表示		*/
				glbDt7Seg1 = GCN_7SEG_HAIPHONG;						/* 7SEGの2桁目に'-'表示		*/
				ledControl_Sec(ALL_LED_SELECT,0,0);		
			}
			else
			{
				glbDt7Seg2 = 'E' - 0x37;							/* 7SEGの2桁目に'E'表示		*/
				glbDt7Seg1 = ErrorCode;								/* 7SEGの1桁目にｴﾗｰｺｰﾄﾞ表示	*/
				ledControl_Sec(ALL_LED_SELECT,0,0);	
			}
			flContent = 1;											/* START/STOP(SW5)が押されたことを認識するﾌﾗｸﾞｾｯﾄ	*/
		}
		else
		{
			mcmSetBuzzerInvalid();									/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
		}
	}

}

/***********************************************************************************************************************
*    関数名    sysPumpTimeMode
*    引  数    
*    戻り値    
*    関数機能  「７セグ」表示器に「ポンプ積算値（０～９９９９時間）」を表示する。
*    処理内容  パネルスイッチが押されていない場合「ポンプ積算値」の上位２桁を表示
*			   ▼のスイッチを押している間、「ポンプ積算値」の下位２桁を表示
*             （▼スイッチを放すと「ポンプ積算値」の上位表示)
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	sysPumpTimeMode(void)
{
	UWORD	sts,key,MaxKeyOnTime ;
	UBYTE	seg1,seg2;



	while(1)
	{

		if((key = glbGetReadKey()))								/* SWの入力ﾁｪｯｸ			*/
		{
			if(key == GCN_KEY_DOWN_SET)							/* ▼（SW4)の場合				*/
			{
				mcmSetBuzzerEffective();						/* ﾎﾞﾀﾝ有効ﾌﾞｻﾞｰ					*/
				sts = 1;
				MaxKeyOnTime = GCN_TIME_1S;						/* SW長押し通知時間	（1秒)				*/
				while(sts == 1)									/* KEYが押されている間はADの下位1ﾊﾞｲﾄを表示	*/
				{
					sysGetPumpTime(0,&seg2,&seg1);				/* 下位1ﾊﾞｲﾄのﾎﾟﾝﾌﾟ稼働時間を2桁で入力		*/
																/* 例）1234⇒seg1=0x04,seg2=0x03					*/
 					glbDt7Seg2 = seg2;							/* 2桁目をｾｯﾄ								*/
 					glbDt7Seg1 = seg1;							/* 1桁目をｾｯﾄ								*/
					sts = glbWaitKeyOff(key,MaxKeyOnTime);		/* KEYが離されるのを待つ					*/
					MaxKeyOnTime = GCN_TIME_100MS;				/* ｷｰ長押し通知時間	（0.1秒)				*/
				}
			}
			else
			{
				mcmSetBuzzerInvalid();							/* ﾎﾞﾀﾝ無効ﾌﾞｻﾞｰ			*/
			}
		}
		else
		{
			sysGetPumpTime(1,&seg2,&seg1);						/* 上位1ﾊﾞｲﾄのAD値を2桁で入力				*/
																/* 例）1234⇒seg1=0x02,seg2=0x01			*/
			glbDt7Seg2 = seg2;									/* 2桁目をｾｯﾄ								*/
			glbDt7Seg1 = seg1;									/* 1桁目をｾｯﾄ								*/
		}
	}
}

/***********************************************************************************************************************
*    関数名    sysGetPumpTime
*    引  数    
*    戻り値    
*    関数機能  ポンプ稼働時間を１０進文字列に変換
*    処理内容 
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	sysGetPumpTime(UBYTE culum,UBYTE *pSeg2,UBYTE *pSeg1)
{
	UBYTE	data;
	UWORD	iadData;
	UBYTE	answer[4],j,k,i;
	UBYTE	henkan[10] = "0123456789";


	for(i=0;i<4;++i)
	{
		answer[i] = '0';
	}
	iadData = geefCtPumpRunning / 60;								/* ﾎﾟﾝﾌﾟ稼働時間（geefCtPumpRunning)は分単位で 	*/
																	/* 格納されているため、これを時間（H)に変換する	*/
	i=0;
	while(iadData > 0) {											/* 10進数を10進文字列に変換	*/
		j = iadData % 10;
   		iadData = iadData / 10;
   		answer[i] = henkan[j];
   		i++;
	}
																	/* 例）時間：1234時間の場合		*/
																	/* answer[0] = '4',answer[1] = '3',answer[2] = '2',answer[3] = '1'	*/
	k = culum * 2;													/* k=0 answer[0]とanswer[1] / k=2 nswer[2]とanswer[3] */

	data = answer[k] - '0';
	*pSeg1 = data;													/* 7SEGの1桁目に表示		*/
	data = answer[k+1] - '0';
	*pSeg2 = data;													/* 7SEGの2桁目に表示		*/

}

/***********************************************************************************************************************
*    関数名    sys7SegLedAllOff
*    引  数    
*    戻り値    
*    関数機能  すべての７ＳＥＧ、ＬＥＤ　ＯＦＦ
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sys7SegLedAllOff(void)
{
	glbDt7Seg2 = 0x00;			/* 2桁目の7SEGのALL OFF		*/
	glbDt7Seg1 = 0x00;			/* 1桁目の7SEGのALL OFF		*/
	
	ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Sec(ALL_LED_SELECT,0,0);		
	
	glbFlLed11 = 0;
	glbFlContinue = 0;									/* 連続ﾓｰﾄﾞ	*/	
}

/***********************************************************************************************************************
*    関数名    sysLedAllOff
*    引  数    
*    戻り値    
*    関数機能  すべての７ＳＥＧ、ＬＥＤ　ＯＦＦ
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysLedAllOff(void)
{
	
	ledControl_Course(ALL_LED_SELECT,0,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,0,0);											
	ledControl_Sec(ALL_LED_SELECT,0,0);		
	
	glbFlLed11 = 0;
	glbFlContinue = 0;									/* 連続ﾓｰﾄﾞ	*/	
}

/***********************************************************************************************************************
*    関数名    sys7SegLedAllOn
*    引  数    
*    戻り値    
*    関数機能  すべての７ＳＥＧ、ＬＥＤ　ＯＮ
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sys7SegLedAllOn(void)				/* 7SEG/LED ALL ON			*/
{
	glbDt7Seg2 = 0xff;			/* 2桁目の7SEGのALL ON		*/
	glbDt7Seg1 = 0xff;			/* 1桁目の7SEGのALL ON		*/
	ledControl_Course(ALL_LED_SELECT,1,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,1,0);									
	ledControl_Sec(ALL_LED_SELECT,1,0);	
	
	glbFlLed11 = 1;
	glbFlContinue = 1;									/* 連続ﾓｰﾄﾞ	*/	
	
}

/***********************************************************************************************************************
*    関数名    sysLedAllOn
*    引  数    
*    戻り値    
*    関数機能  すべてのＬＥＤ　ＯＮ
*    処理内容 
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysLedAllOn(void)				/* 7SEG/LED ALL ON			*/
{
	
	ledControl_Course(ALL_LED_SELECT,1,0);					/* 7SEG/LED ALL			*/
	ledControl_Proc(ALL_LED_SELECT,1,0);											
	ledControl_Sec(ALL_LED_SELECT,1,0);
	
	glbFlLed11 = 1;
	glbFlContinue = 1;									/* 連続ﾓｰﾄﾞ	*/	
}

/***********************************************************************************************************************
*    関数名    R_ADC_Get_Result2
*    引  数    
*    戻り値    
*    関数機能  ＡＤ値入力
*    処理内容  ANI18から読み込む
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UWORD	R_ADC_Get_Result2(void)
{
    return((UWORD)(ADCR >> 6U));
}

/***********************************************************************************************************************
*    関数名    sysFlashWrite
*    引  数    
*    戻り値    
*    関数機能  設定ﾃﾞｰﾀをﾌﾗｯｼｭへ書き込み
*    処理内容  対象はアナログ補正値、ポンプ稼働時間
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	sysFlashWrite(void)
{
	UBYTE	err;

	err = pdlWriteFlash();						/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀをﾌﾗｯｼｭへ書き込み				*/
	if(err != 0)								/* ｴﾗｰ？							*/
	{
		glbFlError |= GCN_ERROR_FLASH_DATA_9;	/* ﾌﾗｯｼｭﾘｰﾄﾞｴﾗｰ						*/
		mainErrorMode();						/* ｴﾗｰﾓｰﾄﾞへ						*/
	}
	
}


/***********************************************************************************************************************
*    関数名    sysVacuumStar
*    引  数    
*    戻り値    
*    関数機能  　真空引き開始時の制御信号出力
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void sysVacuumStart(void)
{
	DI();

	P7 |= P74_PUMP_START_ON;									/* ﾎﾟﾝﾌﾟ稼働開始					*/ 

	EI();
	P4 |= P41_VACUUM_RELEASE_ON;									/* 真空解放弁ON						*/ 
	P2 |= P20_VACUUM_ELECTROMAG_ON;									/* 真空電磁弁ON						*/
	glbFlVacuumElectromangReq = 0;									/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求ﾘｾｯﾄ	*/

}
