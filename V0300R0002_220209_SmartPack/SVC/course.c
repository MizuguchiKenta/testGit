#include "iodefine.h"
#include "glbRam.h"
#include "geefFlash.h"

#include "common.h"

#if 0
extern	void	mainDisplay7Seg(UBYTE);							/* 7SEG表示							*/
extern	UBYTE	glbGetReadKey(void);
#endif
/***********************************************************************************************************************
*    関数名    mainCourseMode
*    引  数    
*    戻り値    
*    関数機能  ｺｰｽ変更/設定ﾓｰﾄﾞ処理
*    処理内容  指定されたｺｰｽの3つの工程時間である【真空】、【ｼｰﾙ】、【冷却】の時間を▲▼ｷｰで変更する
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
#if 0
extern	UBYTE	glbFlMainMode;
extern	UBYTE   glbWaitKeyOff(UBYTE);
#endif

void	mainCourseMode(void)
{
#if 0
	UBYTE	key,sts;
	SWORD	Max,Min;
	UWORD	Time,*pt;

	glbFlProc = GCN_PROC_VACUUM;								/* 工程LEDを「真空」にする				*/
	pt = &tmpCourseData[geefFlCourse][Vacuum];					/* 選択されているｺｰｽの先頭ｱﾄﾞﾚｽﾘｰﾄﾞ						*/
	mainDisplay7Seg(*pt);										/* 7SEG表示					*/

	if(!(key = glbGetReadKey()))								/* KEYの入力ﾁｪｯｸ			*/
	{
		return;													/* 入力がなければﾘﾀｰﾝ		*/
	}
	if(key == GCN_KEY_COURSE_SET)								/* ｺｰｽ選択/設定ｷｰの場合		*/
	{
		sts = glbWaitKeyOff(key);									/* KEYが離されるのを待つ	*/
		if(sts == 0)											/* 1秒以内にKEYが離された	*/
		{
			glbFlProc++;										/* 工程を更新する			*/
			if(glbFlProc > GCN_PROC_COOL)
			{
				glbFlProc = GCN_PROC_VACUUM;						/* 工程を「真空」に移動		*/
			}
			mainDisplay7Seg(*pt);								/* 7SEG表示					*/
		}
		else
		{
			/* ここで変更があればﾌﾗｯｼｭに描き込む	*/
			glbFlMainMode = GCN_WAIT_MODE	;						/* 待機ﾓｰﾄﾞへ遷移			*/

		}
	}
	else if(key == GCN_KEY_UP_SET)								/* ▲ｷｰの場合											*/
	{
		sts = glbWaitKeyOff(key);								/* KEYが離されるのを待つ	*/
		Time = *(pt + glbFlProc);		;						/* ｺｰｽNo.に対しての選択された工程時間をﾘｰﾄﾞ				*/
		Time++;													/* 時間更新（+)											*/
		Max = geefBfCourseDataMaxMin[glbFlProc].Max;				/* 選択されている工程値のMAX値ﾘｰﾄﾞ						*/
		if(Time > Max)											/* Maxを超えた場合										*/
		{
			Time = Max;											/* Maxに補正											*/
		}
		*(pt + glbFlProc) = Time;								/* 変更時間をTMPﾊﾞｯﾌｧに保存								*/
	
	}
	else if(key == GCN_KEY_DOWN_SET)								/* ▼ｷｰの場合											*/
	{
		sts = glbWaitKeyOff(key);								/* KEYが離されるのを待つ	*/
		Time = *(pt + glbFlProc);								/* ｺｰｽNo.に対しての選択された工程時間をﾘｰﾄﾞ				*/
		Time--;													/* 時間更新（+)											*/
		Min = geefBfCourseDataMaxMin[glbFlProc].Min;				/* 選択されている工程値のMIN値ﾘｰﾄﾞ						*/
		if(Time < Min)											/* Minを下回った場合									*/
		{
			Time = Min;
		}
		*(pt + glbFlProc) = Time;								/* 変更時間をTMPﾊﾞｯﾌｧに保存								*/
	}
#endif
}


