/*--------------------------------------------------------*/
/* ���t���b�V���������֊i�[����f�[�^�Q			   		  */
/*--------------------------------------------------------*/

#include	"common.h"
#include	"glbRam.h"
UBYTE		geefFlFixData[2];						/* Flash�Œ��ް��@A5H,5AH					*/
UBYTE		geefFlCourse;							/* ����I���׸�								*/
UBYTE		geefFlVacuumType[4];						/* 0:�^��������ԁi�b),1:�^��������فi%)	*/
SWORD		geefCtAnalogAdjust;						/* ��۸ޒ����l(-99�`99)						*/
UBYTE		geefBfErrorHistory[10][2];				/* �װ���o����ð���							*/
UBYTE		geefBfCourseData[4][8];					/* 4������̺���ް�							*/
UBYTE		geefFlCheckSum;							/* �ׯ���ް������������ް�	*/
unsigned long	int geefCtPumpRunning;				/* ����߉ғ�����							*/
UBYTE		geefBfSystemData[5][3];					/* �����ް�							*/
unsigned long	int geefCtPack;						/* �p�b�N��							*/
UBYTE		geefFlFixEndData[2];						/* Flash�Œ��ް��@A5H,5AH					*/ 


UBYTE		geefFlFixData_Base[2];					/* Flash�Œ��ް��@A5H,5AH					*/
UBYTE		geefFlCourse_Base;						/* ����I���׸�								*/
UBYTE		geefFlVacuumType_Base[4];					/* 0:�^��������ԁi�b),1:�^��������فi%)	*/
SWORD		geefCtAnalogAdjust_Base;				/* ��۸ޒ����l(-99�`99)						*/
UBYTE		geefBfErrorHistory_Base[10][2];			/* �װ���o����ð���							*/
UBYTE		geefBfCourseData_Base[4][8];				/* 4������̺���ް�							*/
unsigned long int geefCtPumpRunning_Base;				/* ����߉ғ�����							*/
UBYTE		geefBfSystemData_Base[5][3];					/* �����ް�							*/
unsigned long	int geefCtPack_Base;						/* �p�b�N��							*/
UBYTE		geefFlFixEndData_Base[2];					/* Flash�Œ��ް��@A5H,5AH	*/	
									
