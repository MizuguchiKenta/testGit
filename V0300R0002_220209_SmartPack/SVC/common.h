#include "r_cg_macrodriver.h"

#define	UBYTE	unsigned char
#define	SBYTE			 char
#define	UWORD	unsigned int
#define	SWORD			 int

#define	GCN_ERROR_PUMP_THERMAL_0	0x01				/* ポンプサーマルトリップの検出	*/
#define	GCN_ERROR_MASTER_5			0x02				/* 親機ｴﾗｰ						*/
#define	GCN_ERROR_VACUUM_LEVEL_2	0x04				/* 真空エラー					*/
														/* 真空引き開始から２０秒経過後、真空レベルが１０％に達していない場合	*/

#define	GCN_ERROR_VACUUM_WARNING_3	0x08				/* 真空警告						*/
														/* 真空引き開始から３０秒経過後、真空レベルが９０％に達していない場合	*/

#define	GCN_ERROR_VACUUM_ADC_6		0x10				/* 真空センサーエラー			*/
														/* １０ビットＡＤデータの値が１０ｈを下回った場合						*/

#define	GCN_ERROR_R78_CPU_7			0x20				/* ＣＰＵエラー					*/
														/* 例外割込み他															*/
#define	GCN_ERROR_WATCHDOG_CPU_8	0x40				/* ＷＤＯＧエラー					*/
														/* ＷＤＯＧタイムアップ													*/
#define	GCN_ERROR_FLASH_DATA_9		0x80				/* データエラー						*/
														/* 保存していたデータが破損していた										*/


#define	GCN_WAIT_MODE	0		/* 待機ﾓｰﾄﾞ			*/
#define	GCN_COURSE_MODE	1		/* ｺｰｽ設定ﾓｰﾄﾞ		*/
#define	GCN_WARM_MODE	2		/* 暖気運転ﾓｰﾄﾞ		*/
#define	GCN_DRIVE_MODE	3		/* 運転ﾓｰﾄﾞ			*/
#define	GCN_ERROR_MODE	4		/* ｴﾗｰ検出ﾓｰﾄﾞ		*/
#define	GCN_STANBY_MODE	5		/* 空きﾓｰﾄﾞ			*/
#define	GCN_SYSTEM_MODE	6		/* ｼｽﾃﾑﾃﾞｰﾀ設定ﾓｰﾄﾞ	*/

#define	GCN_PROC_VACUUM	0		/* 真空工程			*/
#define	GCN_PROC_SEEL	1		// ｼｰﾙ工程
#define	GCN_PROC_COOL	2		// 冷却工程

#define	GCN_KEY_COURSE_SET		1			/* ｺｰｽｷｰ		*/
#define	GCN_KEY_PROC_SET		2			/* 工程ｷｰ		*/
#define	GCN_KEY_UP_SET			3			/* ▲ｷｰ			*/
#define	GCN_KEY_DOWN_SET		4			/* ▼ｷｰ			*/
#define	GCN_KEY_START_SET		5			/* ｽﾀｰﾄｷｰ		*/
#define	GCN_KEY_STOP_SET		6			/* ｽﾄｯﾌﾟｷｰ(隠し）*/
#define	GCN_KEY_PUSH_SET		7			/* PUSHｷｰ		*/
#define	GCN_KEY_UPDOWN_SET		8			/* ▲▼同時ｷｰ	*/

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
#define	GCN_TIME_1MN				12000                                       /* (GCN_TIME_1S * 60 * 1)/GCN_INT_TIME;		 1分*/
#define	GCN_TIME_3MN				36000                                       /* (GCN_TIME_1S * 60 * 3)/GCN_INT_TIME;		 3分*/
#define	GCN_TIME_9999HR				9999 * 60                                   /* 9999時間を分に変換した値						*/

#define	GCN_TIMER_100MS				GCN_TIME_100MS/GCN_INT_TIME					/* 100msを5ms単位に換算			*/
#define	GCN_TIMER_500MS				GCN_TIME_500MS/GCN_INT_TIME				/* 500ms						*/
#define	GCN_TIMER_700MS				GCN_TIME_700MS/GCN_INT_TIME					/* 700msを5ms単位に換算			*/
#define	GCN_TIMER_1S				(GCN_TIME_1S/GCN_INT_TIME)					/* 1000msを5ms単位に換算		*/
#define	GCN_TIMER_2S				(2*(GCN_TIME_1S/GCN_INT_TIME))				/* 2000msを5ms単位に換算		*/
#define	GCN_TIMER_5S				(GCN_TIME_5S/GCN_INT_TIME))					/* 5000msを5ms単位に換算		*/
#define	GCN_TIMER_4_9S				(GCN_TIME_4_9S/GCN_INT_TIME)				/* 4900msを5ms単位に換算		*/
#define	GCN_TIMER_10S				(GCN_TIME_10S/GCN_INT_TIME)					/* 10000msを5ms単位に換算		*/
#define	GCN_TIMER_20S				(GCN_TIME_20S/GCN_INT_TIME)					/* 20000msを5ms単位に換算		*/
#define	GCN_TIMER_30S				(GCN_TIME_30S/GCN_INT_TIME)					/* 20000msを5ms単位に換算		*/

#define	GCN_DRIVE_START_MODE		0							/* 運転開始時					*/
#define	GCN_VACUUM_PULL_MODE		1							/* 真空引き工程					*/
#define	GCN_GUS_INCLUSIOM_MODE		2							/* ｶﾞｽ封入工程					*/
#define	GCN_GUS_STUBILITY_MODE		3							/* ｶﾞｽ安定待ち工程				*/
#define	GCN_SEEL_MODE				4							/* ｼｰﾙ工程						*/
#define	GCN_SEEL_COOLING_MODE		5							/* ｼｰﾙ冷却工程（ソフト開放工程）*/
#define	GCN_VACUUM_STOP_MODE		6							/* 真空引き一時停止ﾓｰﾄﾞ			*/
#define	GCN_HEATER_BLOCK_MODE		7							/* ﾋｰﾀﾌﾞﾛｯｸ上昇待ち処理（０．７秒）*/
#define	GCN_DRIVE_END_MODE			8							/* 工程終了						*/

#define	P20_VACUUM_ELECTROMAG_ON	0x01						/* 真空電磁弁ON(H->Act	)	bit0*/
#define	P20_VACUUM_ELECTROMAG_OFF	~P20_VACUUM_ELECTROMAG_ON	/* 真空電磁弁OFF(L->非Act)	bit0*/
#define	P130_GUS_ELECTROMAG_ON		0x01						/* ｶﾞｽ電磁弁ON(H->Act	)	bit1*/
#define	P130_GUS_ELECTROMAG_OFF		~P130_GUS_ELECTROMAG_ON		/* ｶﾞｽ電磁弁OFF(L->非Act)	bit1*/
#define	P01_SEEL_ELECTROMAG_ON		0x02						/* ｼｰﾙ電磁弁ON(H->Act	)	bit2*/
#define	P01_SEEL_ELECTROMAG_OFF		~P01_SEEL_ELECTROMAG_ON		/* ｼｰﾙ電磁弁OFF(L->非Act)	bit2*/
#define	P41_VACUUM_RELEASE_ON		0x02						/* 真空解放弁ON （H->Act)	bit3*/
#define	P41_VACUUM_RELEASE_OFF		~P41_VACUUM_RELEASE_ON		/* 真空解放弁OFF（L->非Act) bit3*/

#define	P72_POWER_LED_ON			0x04						/* 電源照光SW	(H->Act)	bit2*/
#define	P72_POWER_LED_OFF			~P72_POWER_LED_ON			/* 電源照光SW	(L->NonAct)	bit2*/
#define	P73_HEATER_START_ON			0x08						/* ﾋｰﾀｽﾀｰﾄ	(H->Act)	bit3*/
#define	P73_HEATER_START_OFF		~P73_HEATER_START_ON		/* ﾋｰﾀｽﾄｯﾌﾟ	(L->NonAct)	bit3*/
#define	P74_PUMP_START_ON			0x10						/* ﾎﾞﾝﾌﾟ稼働（H->Act)	bit4*/
#define	P74_PUMP_START_OFF			~P74_PUMP_START_ON			/* ﾎﾞﾝﾌﾟ停止（L->NonAct)bit4*/
#define	P75_SOFTRELEASE_START_ON	0x20						/* ｿﾌﾄ解放（H->Act)		bit5*/
#define	P75_SOFTRELEASE_START_OFF	~P75_SOFTRELEASE_START_ON	/* ｿﾌﾄ非解放（L->NonAct)bit5*/

#define	P50_LEDCOM1_OFF				0x01						/* LEDCOM1 OFF (H->非Act)	bit0	*/
#define	P50_LEDCOM1_ON				~P50_LEDCOM1_OFF			/* LEDCOM1 ON  (L->Act)		bit0	*/
#define	P51_LEDCOM2_OFF				0x02						/* LEDCOM2 OFF (H->非Act)	bit1	*/
#define	P51_LEDCOM2_ON				~P51_LEDCOM2_OFF			/* LEDCOM2 ON  (L->Act)		bit1	*/
#define	P30_LEDCOM3_OFF				0x01						/* LEDCOM3 OFF (H->非Act)	bit0	*/
#define	P30_LEDCOM3_ON				~P30_LEDCOM3_OFF			/* LEDCOM3 ON  (L->Act)		bit0	*/
#define	P70_LEDCOM4_OFF				0x01						/* LEDCOM4 OFF (H->非Act)	bit0	*/
#define	P70_LEDCOM4_ON				~P70_LEDCOM4_OFF			/* LEDCOM4 ON  (L->Act)		bit0	*/

#define	P137_DRAWER_SW				0x80						/* 引出しSW　（L->閉、H->開)*/
#define	P146_PUMP_THERMAL			0x40						/* ﾎﾟﾝﾌﾟｻｰﾏﾙ（L→異常過熱	)*/

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

#define	GCN_BASE_AD					110								/* 基準AD値							*/
#define	GCN_NORMAL_LEVEL			724								/* 標準大気圧の時のAD値				*/

#define ALL_LED_SELECT				0xFF