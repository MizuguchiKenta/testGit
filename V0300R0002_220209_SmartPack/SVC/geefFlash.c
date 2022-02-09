/*--------------------------------------------------------*/
/* ■フラッシュメモリへ格納するデータ群			   		  */
/*--------------------------------------------------------*/

#include	"common.h"
#include	"glbRam.h"
UBYTE		geefFlFixData[2];						/* Flash固定ﾃﾞｰﾀ　A5H,5AH					*/
UBYTE		geefFlCourse;							/* ｺｰｽ選択ﾌﾗｸﾞ								*/
UBYTE		geefFlVacuumType[4];						/* 0:真空引き時間（秒),1:真空引きﾚﾍﾞﾙ（%)	*/
SWORD		geefCtAnalogAdjust;						/* ｱﾅﾛｸﾞ調整値(-99～99)						*/
UBYTE		geefBfErrorHistory[10][2];				/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ							*/
UBYTE		geefBfCourseData[4][8];					/* 4ｺｰｽ分のｺｰｽﾃﾞｰﾀ							*/
UBYTE		geefFlCheckSum;							/* ﾌﾗｯｼｭﾃﾞｰﾀ部のﾁｪｯｸｻﾑﾃﾞｰﾀ	*/
unsigned long	int geefCtPumpRunning;				/* ﾎﾟﾝﾌﾟ稼働時間							*/
UBYTE		geefBfSystemData[5][3];					/* ｼｽﾃﾑﾃﾞｰﾀ							*/
unsigned long	int geefCtPack;						/* パック数							*/
UBYTE		geefFlFixEndData[2];						/* Flash固定ﾃﾞｰﾀ　A5H,5AH					*/ 


UBYTE		geefFlFixData_Base[2];					/* Flash固定ﾃﾞｰﾀ　A5H,5AH					*/
UBYTE		geefFlCourse_Base;						/* ｺｰｽ選択ﾌﾗｸﾞ								*/
UBYTE		geefFlVacuumType_Base[4];					/* 0:真空引き時間（秒),1:真空引きﾚﾍﾞﾙ（%)	*/
SWORD		geefCtAnalogAdjust_Base;				/* ｱﾅﾛｸﾞ調整値(-99～99)						*/
UBYTE		geefBfErrorHistory_Base[10][2];			/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ							*/
UBYTE		geefBfCourseData_Base[4][8];				/* 4ｺｰｽ分のｺｰｽﾃﾞｰﾀ							*/
unsigned long int geefCtPumpRunning_Base;				/* ﾎﾟﾝﾌﾟ稼働時間							*/
UBYTE		geefBfSystemData_Base[5][3];					/* ｼｽﾃﾑﾃﾞｰﾀ							*/
unsigned long	int geefCtPack_Base;						/* パック数							*/
UBYTE		geefFlFixEndData_Base[2];					/* Flash固定ﾃﾞｰﾀ　A5H,5AH	*/	
									
