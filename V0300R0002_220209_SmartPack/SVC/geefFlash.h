#include	"common.h"
//#include	"glbRam.h"

extern	UBYTE		geefFlFixData[2];
extern	UBYTE		geefFlCourse;							/* ｺｰｽ選択ﾌﾗｸﾞ								*/
extern	UBYTE		geefFlVacuumType[4];						/* 0:真空引き時間（秒),1:真空引きﾚﾍﾞﾙ（%)	*/
extern	SWORD		geefCtAnalogAdjust;						/* ｱﾅﾛｸﾞ調整値(-99～99)		*/
extern	UBYTE		geefBfErrorHistory[10][2];
extern	UBYTE		geefBfCourseData[4][8];
extern	UBYTE		geefBfSystemData[5][3];
extern	unsigned long int geefCtPumpRunning;					/* ﾎﾟﾝﾌﾟ稼働時間*/

extern	unsigned long int geefCtPack;					/* パック数*/

extern	UBYTE		geefFlFixEndData[2];			

extern	UBYTE		geefFlCheckSum;							/* ﾌﾗｯｼｭﾃﾞｰﾀ部のﾁｪｯｸｻﾑﾃﾞｰﾀ	*/

extern	UBYTE		geefFlFixData_Base[2];					/* Flash固定ﾃﾞｰﾀ　A5H,5AH					*/
extern	UBYTE		geefFlCourse_Base;						/* ｺｰｽ選択ﾌﾗｸﾞ								*/
extern	UBYTE		geefFlVacuumType_Base[4];				/* 0:真空引き時間（秒),1:真空引きﾚﾍﾞﾙ（%)	*/
extern	SWORD		geefCtAnalogAdjust_Base;				/* ｱﾅﾛｸﾞ調整値(-99～99)						*/
extern	UBYTE		geefBfErrorHistory_Base[10][2];			/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ							*/
extern	UBYTE		geefBfCourseData_Base[4][8];				/* 4ｺｰｽ分のｺｰｽﾃﾞｰﾀ							*/
extern	UBYTE		geefBfSystemData_Base[5][3];
extern	unsigned long int geefCtPumpRunning_Base;				/* ﾎﾟﾝﾌﾟ稼働時間							*/

extern	unsigned long int geefCtPack_Base;					/* パック数*/

extern	UBYTE		geefFlFixEndData_Base[2];					/* Flash固定ﾃﾞｰﾀ　A5H,5AH					*/