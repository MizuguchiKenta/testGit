#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"
#include "r_cg_timer.h"

UBYTE	stiFlKydt;
UBYTE	stiFlFlashing;								/* ﾄｸﾞﾙ更新								*/
UWORD	stiCtFlashing;								/* 500ms周期ｶｳﾝﾀ						*/
UBYTE	stiFlFlashing2;								/* ﾄｸﾞﾙ更新								*/
UWORD	stiCtFlashing2;								/* 500ms周期ｶｳﾝﾀ						*/
UBYTE	stiFlFlashing3;								/* ﾄｸﾞﾙ更新								*/
UWORD	stiCtFlashing3;								/* 500ms周期ｶｳﾝﾀ						*/
UBYTE	stiFlFlashing4;								/* ﾄｸﾞﾙ更新								*/
UWORD	stiCtFlashing4;								/* 500ms周期ｶｳﾝﾀ						*/
long int stiCtP124=0;
long int stiCtP124Check=0;
long int stiCtChildError=0;
UWORD	stiBfAdc[4];
UWORD	stiCtMinute;


#define	LCN_KEY_SW1_COURSE_SET		0x02			/* ｺｰｽｷｰ		*/
#define	LCN_KEY_SW2_PROC_SET		0x04			/* 工程ｷｰ		*/
#define	LCN_KEY_SW3_UP_SET			0x08			/* ▲ｷｰ			*/
#define	LCN_KEY_SW4_DOWN_SET		0x10			/* ▼ｷｰ			*/
#define	LCN_KEY_SW5_START_SET		0x20			/* ｽﾀｰﾄｷｰ		*/
#define	LCN_KEY_SW6_STOP_SET		0x40			/* ｽﾄｯﾌﾟｷｰ(隠し）*/
#define	LCN_KEY_SW7_PUSH_SET		0x80			/* PUSHｷｰ		*/

#define	LCN_LCD_FLASH_TIME			500				/* LED点滅周期（点灯500m､消灯500ms)	*/
#define	LCN_LCDCOM_NUM				4				/* LEDCOM制御信号の個数			)	*/
#define	LCN_LCD_FLASH_CNT			(LCN_LCD_FLASH_TIME/LCN_LCDCOM_NUM)

#define	LCN_LCD_FLASH_TIME2			1000				/* LED点滅周期（点灯500m､消灯500ms)	*/
#define	LCN_LCD_FLASH_CNT2			(LCN_LCD_FLASH_TIME/LCN_LCDCOM_NUM)

struct	tagLEDCOM	LedCom[LCN_LCDCOM_NUM] = {{&P5,	P50_LEDCOM1_ON,P50_LEDCOM1_OFF},			/* LEDCOM1ｱﾄﾞﾚｽ、ON,OFF		*/
							   				  {&P5,	P51_LEDCOM2_ON,P51_LEDCOM2_OFF},			/* LEDCOM2ｱﾄﾞﾚｽ、ON,OFF		*/
										      {&P7,  P70_LEDCOM4_ON,P70_LEDCOM4_OFF},			/* LEDCOM4ｱﾄﾞﾚｽ、ON,OFF		*/
										      {&P3,  P30_LEDCOM3_ON,P30_LEDCOM3_OFF},			/* LEDCOM3ｱﾄﾞﾚｽ、ON,OFF		*/
                                };	

UBYTE	stiDt7Seg[]	=		{	0x3f,					/* 数字:'0'	*/
								0x06,					/* 数字:'1'	*/
								0x5b,					/* 数字:'2'	*/
								0x4f,					/* 数字:'3'	*/
								0x66,					/* 数字:'4'	*/
								0x6d,					/* 数字:'5'	*/
								0x7d,					/* 数字:'6'	*/
								0x27,					/* 数字:'7'	*/
								0x7f,					/* 数字:'8'	*/
								0x6f,					/* 数字:'9'	*/
								0x77,					/* 英字:'A'	*/
								0x7c,					/* 英字:'b'	*/
								0x39,					/* 英字:'C'	*/
								0x5e,					/* 英字:'d'	*/
								0x79,					/* 英字:'E'	*/
								0x71,					/* 英字:'F'	*/
								0x40,					/* 記号:'-'	*/
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
void	stiCheckPump(void);											/* ﾎﾟﾝﾌﾟ稼働時間積算		*/
UBYTE	stiCheckPumpthermal(void);						/* ﾎﾟﾝﾌﾟｻｰﾏﾙのｴﾗｰ監視		*/

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
*    関数名    stiIntMain
*    引  数    
*    戻り値    
*    関数機能  １ｍｓタイマ割り込み処理
*    処理内容  １ｍｓ周期の処理と５ｍｓ周期の処理がある。
*			   【１ｍｓ周期】
*			   		・ＬＥＤ制御
*                   ・ＣＮ４信号入力監視（ツインチャンバー用)
*                   ・ＣＮ７信号出力制御（ツインチャンバー用)
*					・親機にて子機運転中信号監視によるポンプ制御
*              【５ｍｓ周期】
*              　　 ・ポンプ稼働時間積算処理	
*                   ・共通タイマー時間管理
*                   ・ＳＷ入力処理	
*                   ・真空センサーＡ／Ｄ値平均化処理
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void stiIntMain(void)
{
	R_WDT_Restart();											/* ｳｫｯﾁﾄﾞｯｸﾀｲﾏﾘｾｯﾄ							*/

	if(glbCtIntTimer >= 5)									/* 5ms経過？					*/
	{
		stiCheckPump();										/* ﾎﾟﾝﾌﾟ稼働時間積算			*/
		GlobalTimerReneal();								/* 共通ﾀｲﾏ更新					*/
		stiInKey();											/* KEY 入力監視					*/
		stiReadSamplingADC();								/* 真空ｾﾝｻｰA/D値の入力と平均化	*/
		
		stiPowerSwCheck();
		
		if(glbFlSystemMode  != 1)						/* ｼｽﾃﾑﾓｰﾄﾞ以外の場合							*/
		{
			stiPowerLedOut();
		}
		
		glbCtIntTimer=0;									/* 5msﾀｲﾏｰﾘｾｯﾄ					*/
	}
	glbCtIntTimer++;
	glbCtCN4Timer++;
	
	
	stiChkLEDCOM();											/* LED制御					*/


}


/***********************************************************************************************************************
*    関数名    GlobalTimerReneal
*    引  数    
*    戻り値    
*    関数機能  各種、監視タイマの監視
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void GlobalTimerReneal(void)
{

	glbCtKeyOffTimer++;									/* KEY OFFｶｳﾝﾀ更新			*/
	if(glbCtDriveTimer != 0)							/* 真空工程残り時間有の場合	*/
	{
		glbCtDriveTimer--;								/* ﾀｲﾏ値更新					*/
	}

	if(glbCtVacuumEmergncyTimer != 0)					/* 真空一時停止時間残の場合			*/
	{
		glbCtVacuumEmergncyTimer--;						/* ﾀｲﾏ値更新					*/
	}

	if(glbCtGusInclusionTimer != 0)						/* ｶﾞｽ封入工程実施中？			*/
	{
		glbCtGusInclusionTimer--;						/* ﾀｲﾏ値更新					*/
	}

	if(glbCtGusStubilityTimer != 0)						/* ｶﾞｽ安定時間ｶｳﾝﾄ中？			*/
	{
		glbCtGusStubilityTimer--;						/* ﾀｲﾏ値更新					*/
	}

	if(glbCtSeelTimer != 0)								/* ｼｰﾙ工程実施中？			*/
	{
		glbCtSeelTimer--;								/* ﾀｲﾏ値更新					*/
	}

	if(glbCtSeelCoolingTimer != 0)						/* */
	{
		glbCtSeelCoolingTimer--;
	}
	
	if(glbCtSoftReleaseTimer != 0)						/* */
	{
		glbCtSoftReleaseTimer--;
	}
	
	
	if(glbCtWarmTimer != 0)								/* 暖気運転実施中？				*/
	{
		glbCtWarmTimer--;								/* ﾀｲﾏ値更新					*/
	}
	if(glbCtVacuumeWarm != 0)							/* 暖気運転ﾓｰﾄﾞにて真空電磁弁開中？(0.5秒）*/
	{	
		glbCtVacuumeWarm--;								/* ﾀｲﾏ値更新					*/
	}
	if(glbCtFunTimer != 0)								/* ﾌｧﾝ起動中ﾀｲﾏ更新中			*/
	{	
		glbCtFunTimer--;								/* ﾀｲﾏ値更新					*/
	}

	if(glbFlSysHeaterReq == 1)							/* ｼｽﾃﾑﾓｰﾄﾞﾋｰﾀ監視要求有?		*/
	{
		glbCtSysHeaterOffTimer--;						/* 2秒監視ﾀｲﾏｶｳﾝﾀ更新			*/
		
	}
	if(glbFlVacuumElectromangReq == 1)					/* 運転工程（真空）/暖気運転終了時のﾎﾟﾝﾌﾟ停止後10秒間、真空電磁弁ONの要求有り？*/
	{
		if(glbCtVacuumElectromang != 0)					/* ﾀｲﾏ=0?						*/
		{
			glbCtVacuumElectromang--;					/* 10秒ﾀｲﾏ更新					*/
		}
	}
	if(glbCtHeaterBlockTimer != 0)						/*  ﾋｰﾀﾌﾞﾛｯｸ上昇時間ｶｳﾝﾄ中？（0.7秒）	*/
	{
		glbCtHeaterBlockTimer--;						/* 0.7秒ﾀｲﾏ更新					*/
	}

	if(glbCtVacuumEmergncyTimer == 0)					/* 真空一時停止中でない場合のみ真空ﾚﾍﾞﾙ到達のﾀｲﾑｱｳﾄ時間を更新する			*/
	{
		if(glbCtVacuumTimeout1 != 0)						/* 真空度ﾀｲﾑｱｳﾄ監視中？			*/
		{
			glbCtVacuumTimeout1--;							/* ﾀｲﾏ更新						*/
		}
		if(glbCtVacuumTimeout2 != 0)						/* 真空度ﾀｲﾑｱｳﾄ監視中？			*/
		{
			glbCtVacuumTimeout2--;							/* ﾀｲﾏ更新						*/
		}
		if(glbCtVacuumTimeout3 != 0)						/* 真空警告監視中？			*/
		{
			glbCtVacuumTimeout3--;							/* ﾀｲﾏ更新						*/
		}
	}
	if(glbCtBuzzerOnTimer != 0)							/* ﾌﾞｻﾞｰ鳴らし中?				*/
	{
		glbCtBuzzerOnTimer--;								/* ﾌﾞｻﾞｰONﾀｲﾏ更新				*/
		if(glbCtBuzzerOnTimer == 0)						/* ﾌﾞｻﾞｰONﾀｲﾏ=0?				*/
		{
			R_TAU0_Channel2_Stop();						/* ﾌﾞｻﾞｰ停止					*/
		}
	}

	if(glbCtBuzzerOffTimer != 0)						/* ﾌﾞｻﾞｰOFF中?				*/
	{
		glbCtBuzzerOffTimer--;								/* ﾌﾞｻﾞｰOFFﾀｲﾏ更新				*/
	}

	if(glbCtWaitTimer != 0)								/* 指定時間のｳｪｲﾄ監視中?			*/
	{
		glbCtWaitTimer--;		
		
		
		
		/* ﾀｲﾏ更新							*/
	}
	
	if(glbCtWaitTimerT5 != 0)							/* T5:予約運転時WAIT時間監視中？*/
	{
		glbCtWaitTimerT5--;								/* ﾀｲﾏ更新						*/
	}
	

	if(glbCtWaitTimerE6Cansel != 0)						/* 電源投入からT5時間は[E6]ｴﾗｰ検出しない監視中?*/
	{
		glbCtWaitTimerE6Cansel--;						/* ﾀｲﾏ更新						*/
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

	/*起動中ぐるぐる*/
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
*    関数名    stiInKey
*    引  数    
*    戻り値    
*    関数機能  SW入力の監視
*    処理内容  【ｺｰｽ（SW1)】、【工程(SW2)】、【▲(SW3)】、【▼(SW4)】、【START/STOP(SW5)】、【真空停止(SW6)】
               【PUSH（SW7)】の7つのSW入力のACTIVE変化を監視
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void stiInKey(void)
{
	UWORD	postd = 0;												
	UBYTE	keydt;												

	keydt = iodInKey();				 				/* ｷｰﾃﾞｰﾀ入力（Lの時にKEY ON)					    */
	if( keydt != stiFlKydt)							/* 前回と入力値が異なる場合							*/
	{

		stiCtKey++;									/* SW ON 連続ｶｳﾝﾀ更新								*/
		if(stiCtKey <= 14)							/*70ms以上連続的にｷｰが押された場合、有効とする	*/
		{
			return;
		}
		stiCtKey = 0;								/* 連続SW ONｶｳﾝﾀｸﾘｱ					*/
		keydt = ~keydt;								/* SWﾃﾞｰﾀ反転						*/
		if((keydt & P21_KEY_SW1_COURSE_OFF))		/* ｺｰｽ（SW1）の場合					*/
		{											
			postd |= LCN_KEY_SW1_COURSE_SET;				
		}											
		else if((keydt & P22_KEY_SW2_PROCESS_OFF))	/* 工程(SW2)の場合					*/
		{											
			postd |= LCN_KEY_SW2_PROC_SET;				
		}											
		else if((keydt & P23_KEY_SW3_UP_OFF))		/* ▲（SW3)の場合					*/
		{											
			postd |= LCN_KEY_SW3_UP_SET;			/* ▲ｷｰｾｯﾄ							*/
			if((keydt & P24_KEY_SW4_DOWN_OFF))		/* 更に▼ｷｰも同時に押されている場合	*/
			{
				postd |= LCN_KEY_SW4_DOWN_SET;		/* ▲▼ｷｰ同時ｾｯﾄ					*/
			}
		}											
		else if((keydt & P24_KEY_SW4_DOWN_OFF))		/* ▼(SW4)の場合					*/
		{
			postd |= LCN_KEY_SW4_DOWN_SET;				
		}
		else if((keydt & P25_KEY_SW5_START_OFF ))	/* START/STOP(SW5)の場合			*/
		{
			postd |= LCN_KEY_SW5_START_SET;				
		}
		else if((keydt & P26_KEY_SW6_STOP_OFF))		/* 真空停止(SW6)の場合				*/
		{
			postd |= LCN_KEY_SW6_STOP_SET;				
		}
		else if((keydt & P27_KEY_SW7_PUSH_OFF))		/* PUSH（隠しSW7)の場合				*/
		{
			postd |= LCN_KEY_SW7_PUSH_SET;				
		}
		keydt = ~keydt;								/* SWﾃﾞｰﾀ反転		*/
		stiFlKydt = keydt;							/* ｷｰﾃﾞｰﾀｾｰﾌﾞ		*/
		if(stiFlKydt != 0xff)						/* どれかのSWが押されてる場合		*/
		{
			if(glbFlKydt == 0)						/* ﾒｲﾝ側で前回のSW入力完了済の場合	*/
			{
				glbFlKydt = postd;					/* ﾒｲﾝへ通知する	*/
			}
		}
	}
}


/***********************************************************************************************************************
*    関数名    stiChkLEDCOM
*    引  数    
*    戻り値    
*    関数機能  LEDCOM１～4の制御
*    処理内容  LEDCOM1:7SEG_2桁目、LEDCOM2:7SEG_1桁目、LEDCOM3:工程LED [真空][ｼｰﾙ][冷却]
*              LEDCOM4:ｺｰｽLED [ｺｰｽ1][ｺｰｽ2][ｺｰｽ3][ｺｰｽ4]
			   ﾒｲﾝﾙｰﾁﾝで設定したLED情報を定期的にLEDCOMxﾎﾟｰﾄに出力
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void stiChkLEDCOM(void)
{
	UBYTE	i,data=0;
	volatile UBYTE	*pIO;


	P1 = 0x00;										/* 出力ﾃﾞｰﾀALL OFF			*/
	for(i=0;i<4;i++)								/* LEDCOM1～4 ALL OFF		*/
	{
		pIO   = LedCom[i].pIO;						/* 指定のLCDCOMxｱﾄﾞﾚｽﾘｰﾄﾞ	*/
		*pIO  |= LedCom[i].OFF;						/* LEDCOMx <- OFF			*/
	}
	pIO  = LedCom[glbFlLedCom].pIO;					/* 指定のLCDCOMxｱﾄﾞﾚｽﾘｰﾄﾞ	*/
	*pIO &= LedCom[glbFlLedCom].ON;					/* 上記のﾎﾟｰﾄへLEDCOM ON (Active Low)	*/

	if(glbFlLedCom == 0)							/* LEDCOM1:7Segの2桁目表示要求		*/
	{
		if(glbFlSysytem7Seg  == 1)					/* ｼｽﾃﾑﾓｰﾄﾞの場合							*/
		{
			P1 = glbDt7Seg2;						/* 1SEGのみの出力とする				*/
		}
		else
		{
			if(!stiGuruGuruFlag)
			{
				data  = (glbFlReqDot == 1)?0x80:0x00;	/* ﾋﾟﾘｵﾄﾞ表示要求ﾁｪｯｸ		*/
				data  |= stiDt7Seg[glbDt7Seg2];			/* 表示数字とﾋﾟﾘｵﾄﾞ合体		*/
			}
			else
			{
				data = Data_GuruGuru7Seg[1];
				
			}
			
			P1    = data;			 				/* 表示数字を7SEG出力ﾃﾞｰﾀに変換してﾎﾟｰﾄ出力	*/

		}
	}
	else if(glbFlLedCom == 1)							/* LEDCOM2:7Segの1桁目表示要求		*/
	{
		if(glbFlSysytem7Seg  == 1)						/* ｼｽﾃﾑﾓｰﾄﾞの場合							*/
		{
			P1 = glbDt7Seg1;							/* 1SEGのみの出力とする				*/
		}
		else
		{
			if(!stiGuruGuruFlag)
			{
				data  = (glbFlReqDot == 2)?0x80:0x00;	/* ﾋﾟﾘｵﾄﾞ表示要求ﾁｪｯｸ		*/
				data  |= stiDt7Seg[glbDt7Seg1];			/* 表示数字とﾋﾟﾘｵﾄﾞ合体		*/
			}
			else
			{
				data = Data_GuruGuru7Seg[0];
			}
			
			P1 = data;			 					/* 表示数字を7SEG出力ﾃﾞｰﾀに変換してﾎﾟｰﾄ出力	*/
		}
	}
	else if(glbFlLedCom == 2)							/* LEDCOM3:工程LEDの表示要求	*/
	{
		if(glbFlSysytem7Seg  == 1)						/* ｼｽﾃﾑﾓｰﾄﾞの場合							*/
		{
			if((stiCheckPumpthermal()))					/* ﾎﾟﾝﾌﾟｻｰﾏﾙ異常の場合			*/
			{
				stiCtFlashing2++;						/* 500msｶｳﾝﾀ更新						*/
				if(stiCtFlashing2 > LCN_LCD_FLASH_CNT)	/* 500ms経過							*/
				{
					stiCtFlashing2 = 0;					/* 500ms周期ｶｳﾝﾀ						*/
					stiFlFlashing2++;					/* ﾄｸﾞﾙ更新								*/
					stiFlFlashing2 &= 1;				/* 点滅切替ﾄｸﾞﾙﾌﾗｸﾞ						*/
				}
				if(stiFlFlashing2)						/* 点灯ﾀｲﾐﾝｸﾞの場合						*/
				{
					P1 |= stiLedOut_Com2;			/* 指定の工程LED点灯				*/
				}
				else									/* 消灯ﾀｲﾐﾝｸﾞの場合					*/
				{
					P1  &= ~0x07;	/* 3つの工程LED消灯					*/
				}
			}
			else
			{
				P1 |= stiLedOut_Com2;			/* 指定の工程LED点灯				*/
			}
		}
		else
		{
			if(glbFlMainMode == GCN_WARM_MODE)			/* 暖機運転中は「真空」LEDを点滅させる	*/
			{
				stiCtFlashing2++;						/* 500msｶｳﾝﾀ更新						*/
				if(stiCtFlashing2 > LCN_LCD_FLASH_CNT)	/* 500ms経過							*/
				{
					stiCtFlashing2 = 0;					/* 500ms周期ｶｳﾝﾀ						*/
					stiFlFlashing2++;					/* ﾄｸﾞﾙ更新								*/
					stiFlFlashing2 &= 1;				/* 点滅切替ﾄｸﾞﾙﾌﾗｸﾞ						*/
				}
				if(stiFlFlashing2)
				{
					P1 |= stiLedOut_Com2;			/* 指定の工程LED点灯				*/
				}
				else
				{
					P1  = (stiLedOut_Com2 & 0xfe);
				}
			}
			else
			{
				P1 = stiLedOut_Com2;				/* 指定の工程LED表示			*/
			}
		}
	
	}
	else if(glbFlLedCom == 3)							/* LEDCOM4:ｺｰｽLEDと「秒」or 「%」表示要求	*/
	{
		/* courseLed */
		if(glbFlAdjust == 1)							/* ﾃｽﾄﾓｰﾄﾞのｱﾅﾛｸﾞ調整ﾓｰﾄﾞの場合				*/
		{
			P1  = stiDtBit[8];							/* LED1～LED4,LED8,LED9消灯					*/
		}
		else
		{
			if(DrawerSwRead())							/* 引出しSWが「開」の場合は、点滅	*/
			{
				stiCtFlashing++;						/* 500msｶｳﾝﾀ更新						*/
				if(stiCtFlashing > LCN_LCD_FLASH_CNT)	/* 500ms経過							*/
				{
					stiCtFlashing = 0;					/* 500ms周期ｶｳﾝﾀ						*/
					stiFlFlashing++;					/* ﾄｸﾞﾙ更新								*/
					stiFlFlashing &= 1;					/* 点滅切替ﾄｸﾞﾙﾌﾗｸﾞ						*/
				}
				if(stiFlFlashing)						/* 点灯ﾀｲﾐﾝｸﾞの場合						*/
				{
					P1  |= stiLedOut_Com3;		/* 指定のｺｰｽLED点灯						*/
				}
				else									/* 消灯ﾀｲﾐﾝｸﾞの場合						*/
				{
					P1  &= ~stiLedOut_Com3;
				}
			}
			else
			{
				P1  |= stiLedOut_Com3;		/* 指定のｺｰｽLED点灯						*/
			}
		}

		/* Sec/% */
		P1 |= stiLedOut_Com3_Sec;	
		
		/* spear */
		if(glbFlLed11 == 1)								/* LED11点灯要求?				*/
		{
			P1 |= 0x80;									/* 「秒 & %」点灯	*/
		}
		
		/* cycleModeLed */
		if(glbFlSystemMode  == 0)						/* ｼｽﾃﾑﾓｰﾄﾞ以外の場合							*/
		{
			if( (glbFlContinue == 1)&&(!glbFlStanby) )						/* 連続運転ﾓｰﾄﾞ?			*/
			{
				P1 |= 0x40;								/* LED10点灯（連続ﾓｰﾄﾞ ON) 	*/
			}
			else										
			{
				P1 &= ~0x40;							/* LED10消灯（連続ﾓｰﾄﾞ OFF) 	*/
			}
		}
		else
		{
			 if(glbFlContinue == 1)
			 {
				 P1 |= 0x40;								/* LED10点灯（連続ﾓｰﾄﾞ ON) 	*/	 
			 }
		}

	}
	


	glbFlLedCom++;										/* LEDCOMx出力ｶｳﾝﾀ更新		*/
	glbFlLedCom &= 0x03;								/* 上限ﾁｪｯｸ					*/
}


/***********************************************************************************************************************
*    関数名    iodInKey
*    引  数    
*    戻り値    
*    関数機能  ７個のキーSW入力
*    処理内容  信号ﾚﾍﾞﾙがLでKEY　ONとなる。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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
	keydt1 |= 0x01;									/* ﾋﾞｯ0は未使用のためKEY入力無し（1)とする	*/
	return(keydt1);
}

/***********************************************************************************************************************
*    関数名    stiReadSamplingADC
*    引  数    
*    戻り値    
*    関数機能  真空度AD値の平均化
*    処理内容  5ms毎にAD値を入力し、最小値と最大値を取り除き、残り２つのAD値の平均値を算出
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	stiReadSamplingADC(void)
{
	UWORD	adc,min1,min2,mid1,mid2,max1,max2,adave,i;
	
	adc = R_ADC_Get_Result2();								/* AD値入力　ANI18					*/


	for (i = 0; i < 3; i++){								/* AD値ﾛｰﾙｱｯﾌﾟ				*/
		stiBfAdc[i] = stiBfAdc[i + 1];						
	}															
	stiBfAdc[3] = adc;										/* 最新AD値ｾｯﾄ				*/
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
*    関数名    stiInitialSamplingADC
*    引  数    
*    戻り値    
*    関数機能  真空AD値の初期化
*    処理内容  現在のAD値を入力して４個のバッファに展開
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	stiInitialSamplingADC(void)
{
	UWORD	i,adc;

	adc = R_ADC_Get_Result2();									/* AD値入力　ANI18					*/
	for(i=0;i<4;++i)
	{
		stiBfAdc[i] = adc;										/* 最新AD値ｾｯﾄ				*/
	}
	glbCtRealAD = adc;
}

/***********************************************************************************************************************
*    関数名    stiCheckPump
*    引  数    
*    戻り値    
*    関数機能  ﾎﾟﾝﾌﾟ稼働時間の積算
*    処理内容  1分単位でﾎﾟﾝﾌﾟ稼働時間を積算
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
void	stiCheckPump(void)													
{

	if(P7 & P74_PUMP_START_ON)												/* ﾎﾟﾝﾌﾟ稼働中				*/ 
	{
		stiCtMinute++;														/* 分ｶｳﾝﾀ更新				*/
		if(stiCtMinute > GCN_TIME_1MN)										/* 1分経過？				*/
		{
			geefCtPumpRunning++;											/* ﾎﾟﾝﾌﾟ稼働時間更新（1分単位)	*/
			if(geefCtPumpRunning >(unsigned long int) GCN_TIME_9999HR)		/* 9999時間を超えた場合			*/
			{
				geefCtPumpRunning = (unsigned long int) GCN_TIME_9999HR	;	/* 9999時間を上限とする			*/
			}
			stiCtMinute = 0;												/* 1分ﾀｲﾏｰﾘｾｯﾄ					*/
		}
	}
}


/***********************************************************************************************************************
*    関数名    stiCheckPumpthermal
*    引  数    
*    戻り値    
*    関数機能  ﾎﾟﾝﾌﾟの状態検出
*    処理内容  0:正常,1:異常
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	stiCheckPumpthermal(void)						/* ﾎﾟﾝﾌﾟｻｰﾏﾙのｴﾗｰ監視		*/
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
* 電源SW入力処理		5ms更新
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
* 電源SW入力長押し処理		5ms更新
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
* 電源LED出力制御
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
* 電源LED出力制御
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
*　コースLed制御
*   [course]0:1ｺｰｽ,1:2ｺｰｽ,2:3ｺｰｽ,3:4ｺｰｽ,0xFF:全て
*	[ctl]0:OFF,1:ON
*	[mode]0:連動,1:個別制御
*===========================================*/
void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode)
{	
	if(ctl == 0)	//消灯
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
	else			//点灯
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
*　工程Led制御
*   [proc]0:1ｺｰｽ,1:2ｺｰｽ,2:3ｺｰｽ,3:4ｺｰｽ,0xFF:全て
*	[ctl]0:OFF,1:ON
*	[mode]0:連動,1:個別制御
*===========================================*/
void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode)
{	

	if(ctl == 0)	//消灯
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
	else			//点灯
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
*　秒/％Led制御
*   [toggle]0:秒,1:％
*	[ctl]0:OFF,1:ON
*	[mode]0:連動,1:個別制御
*===========================================*/
void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode)
{	
	if(ctl == 0)	//消灯
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
	else			//点灯
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