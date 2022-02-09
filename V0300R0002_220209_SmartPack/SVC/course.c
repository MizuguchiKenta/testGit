#include "iodefine.h"
#include "glbRam.h"
#include "geefFlash.h"

#include "common.h"

#if 0
extern	void	mainDisplay7Seg(UBYTE);							/* 7SEG�\��							*/
extern	UBYTE	glbGetReadKey(void);
#endif
/***********************************************************************************************************************
*    �֐���    mainCourseMode
*    ��  ��    
*    �߂�l    
*    �֐��@�\  ����ύX/�ݒ�Ӱ�ޏ���
*    �������e  �w�肳�ꂽ�����3�̍H�����Ԃł���y�^��z�A�y��فz�A�y��p�z�̎��Ԃ��������ŕύX����
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
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

	glbFlProc = GCN_PROC_VACUUM;								/* �H��LED���u�^��v�ɂ���				*/
	pt = &tmpCourseData[geefFlCourse][Vacuum];					/* �I������Ă��麰��̐擪���ڽذ��						*/
	mainDisplay7Seg(*pt);										/* 7SEG�\��					*/

	if(!(key = glbGetReadKey()))								/* KEY�̓�������			*/
	{
		return;													/* ���͂��Ȃ��������		*/
	}
	if(key == GCN_KEY_COURSE_SET)								/* ����I��/�ݒ跰�̏ꍇ		*/
	{
		sts = glbWaitKeyOff(key);									/* KEY���������̂�҂�	*/
		if(sts == 0)											/* 1�b�ȓ���KEY�������ꂽ	*/
		{
			glbFlProc++;										/* �H�����X�V����			*/
			if(glbFlProc > GCN_PROC_COOL)
			{
				glbFlProc = GCN_PROC_VACUUM;						/* �H�����u�^��v�Ɉړ�		*/
			}
			mainDisplay7Seg(*pt);								/* 7SEG�\��					*/
		}
		else
		{
			/* �����ŕύX��������ׯ���ɕ`������	*/
			glbFlMainMode = GCN_WAIT_MODE	;						/* �ҋ@Ӱ�ނ֑J��			*/

		}
	}
	else if(key == GCN_KEY_UP_SET)								/* �����̏ꍇ											*/
	{
		sts = glbWaitKeyOff(key);								/* KEY���������̂�҂�	*/
		Time = *(pt + glbFlProc);		;						/* ���No.�ɑ΂��Ă̑I�����ꂽ�H�����Ԃ�ذ��				*/
		Time++;													/* ���ԍX�V�i+)											*/
		Max = geefBfCourseDataMaxMin[glbFlProc].Max;				/* �I������Ă���H���l��MAX�lذ��						*/
		if(Time > Max)											/* Max�𒴂����ꍇ										*/
		{
			Time = Max;											/* Max�ɕ␳											*/
		}
		*(pt + glbFlProc) = Time;								/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�								*/
	
	}
	else if(key == GCN_KEY_DOWN_SET)								/* �����̏ꍇ											*/
	{
		sts = glbWaitKeyOff(key);								/* KEY���������̂�҂�	*/
		Time = *(pt + glbFlProc);								/* ���No.�ɑ΂��Ă̑I�����ꂽ�H�����Ԃ�ذ��				*/
		Time--;													/* ���ԍX�V�i+)											*/
		Min = geefBfCourseDataMaxMin[glbFlProc].Min;				/* �I������Ă���H���l��MIN�lذ��						*/
		if(Time < Min)											/* Min����������ꍇ									*/
		{
			Time = Min;
		}
		*(pt + glbFlProc) = Time;								/* �ύX���Ԃ�TMP�ޯ̧�ɕۑ�								*/
	}
#endif
}


