#include	"common.h"
//#include	"glbRam.h"

extern	UBYTE		geefFlFixData[2];
extern	UBYTE		geefFlCourse;							/* ����I���׸�								*/
extern	UBYTE		geefFlVacuumType[4];						/* 0:�^��������ԁi�b),1:�^��������فi%)	*/
extern	SWORD		geefCtAnalogAdjust;						/* ��۸ޒ����l(-99�`99)		*/
extern	UBYTE		geefBfErrorHistory[10][2];
extern	UBYTE		geefBfCourseData[4][8];
extern	UBYTE		geefBfSystemData[5][3];
extern	unsigned long int geefCtPumpRunning;					/* ����߉ғ�����*/

extern	unsigned long int geefCtPack;					/* �p�b�N��*/

extern	UBYTE		geefFlFixEndData[2];			

extern	UBYTE		geefFlCheckSum;							/* �ׯ���ް������������ް�	*/

extern	UBYTE		geefFlFixData_Base[2];					/* Flash�Œ��ް��@A5H,5AH					*/
extern	UBYTE		geefFlCourse_Base;						/* ����I���׸�								*/
extern	UBYTE		geefFlVacuumType_Base[4];				/* 0:�^��������ԁi�b),1:�^��������فi%)	*/
extern	SWORD		geefCtAnalogAdjust_Base;				/* ��۸ޒ����l(-99�`99)						*/
extern	UBYTE		geefBfErrorHistory_Base[10][2];			/* �װ���o����ð���							*/
extern	UBYTE		geefBfCourseData_Base[4][8];				/* 4������̺���ް�							*/
extern	UBYTE		geefBfSystemData_Base[5][3];
extern	unsigned long int geefCtPumpRunning_Base;				/* ����߉ғ�����							*/

extern	unsigned long int geefCtPack_Base;					/* �p�b�N��*/

extern	UBYTE		geefFlFixEndData_Base[2];					/* Flash�Œ��ް��@A5H,5AH					*/