#include "pfdl_user.h"																																
#include "geefFlash.h"																																
#include "glbRam.h"																																
	
#include "string.h"


UBYTE	pdlWriteFlash(void);
UBYTE	pdlReadFlash(void);
UBYTE	calcFlashCheckSum(void);
void	pldCopyBackupData(void);	
UBYTE	pldVerifyCheck(void);	
void	pldRestoreBackupData(void);




/*--------------------------------------------------------*/
/* ���t���b�V���������\���e�[�u��						  */
/*--------------------------------------------------------*/
DATA_FLASH_INFO dataFlashTbl[] = 																																
{																																
    {(__near UBYTE* )geefFlFixData			, sizeof(geefFlFixData)			},			/* �Œ��ް��@A5,5A*/
    {(__near UBYTE* )&geefFlCourse			, sizeof(geefFlCourse)			},			/* ���No.							*/
    {(__near UBYTE* )geefFlVacuumType		, sizeof(geefFlVacuumType)		},			/* �^��H���\���@'�b' or '%' �؊���	*/
	{(__near UBYTE* )&geefCtAnalogAdjust	, sizeof(geefCtAnalogAdjust)	},			/* ��۸ޒ����l						*/
	{(__near UBYTE* )geefBfErrorHistory  	, sizeof(geefBfErrorHistory) 	},			/* �װ���o����ð��فi10��)		*/
    {(__near UBYTE* )geefBfCourseData		, sizeof(geefBfCourseData)		},			/* ����ް�(4�����)					*/
    {(__near UBYTE* )geefBfSystemData		, sizeof(geefBfSystemData)		},			/* �����ް�							*/
    {(__near UBYTE* )&geefCtPumpRunning		, sizeof(geefCtPumpRunning)		},			/* ����߉ғ�����					*/
    {(__near UBYTE* )&geefCtPack			, sizeof(geefCtPack)		},				/* �p�b�N��				*/	
	{(__near UBYTE* )geefFlFixEndData		, sizeof(geefFlFixEndData)		}			/* �Œ��ް��@A5,5A*/		
																																
};																																

/*--------------------------------------------------------*/
/* ���o�b�N�A�b�v�̈�\���e�[�u��						  */
/*--------------------------------------------------------*/
DATA_FLASH_INFO dataFlashTbl_Base[] = 																																
{																																
    {(__near UBYTE* )geefFlFixData_Base			, sizeof(geefFlFixData_Base)		},			/* �Œ��ް��@A5,5A*/
    {(__near UBYTE* )&geefFlCourse_Base			, sizeof(geefFlCourse_Base)			},			/* ���No.							*/
    {(__near UBYTE* )geefFlVacuumType_Base		, sizeof(geefFlVacuumType_Base)		},			/* �^��H���\���@'�b' or '%' �؊���	*/
	{(__near UBYTE* )&geefCtAnalogAdjust_Base	, sizeof(geefCtAnalogAdjust_Base)	},			/* ��۸ޒ����l						*/
	{(__near UBYTE* )geefBfErrorHistory_Base  	, sizeof(geefBfErrorHistory_Base) 	},			/* �װ���o����ð��فi10��)		*/
    {(__near UBYTE* )geefBfCourseData_Base		, sizeof(geefBfCourseData_Base)		},			/* ����ް�(4�����)					*/
    {(__near UBYTE* )geefBfSystemData_Base		, sizeof(geefBfSystemData_Base)		},			/* �����ް�							*/
    {(__near UBYTE* )&geefCtPumpRunning_Base	, sizeof(geefCtPumpRunning_Base)	},			/* ����߉ғ�����					*/
 	{(__near UBYTE* )&geefCtPack_Base			, sizeof(geefCtPack_Base)		},						/* �p�b�N��				*/		
	{(__near UBYTE* )geefFlFixEndData_Base		, sizeof(geefFlFixEndData_Base)		},			/* �Œ��ް��@A5,5A*/	
																																
};																																
																																
/***********************************************************************************************************************
*    �֐���    calcFlashCheckSum
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �t���b�V���������̈�̃`�F�b�N�T���Z�o
*    �������e  
*    ���l	�@ �`�F�b�N�T���̈�͌v�Z�O�Ƃ���
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE	calcFlashCheckSum(void)																																
{																																
    int i, j;																																
    pfdl_u08 cs;																																
    																																
    cs = 0;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )	/* �ޯ����ߗ̈�������т��v�Z���邽�߂�ٰ��	*/																															
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )																																
        {																																
            cs = cs ^ dataFlashTbl[i].address[j];							/* XOR�������ьv�Z							*/																															
        }																																
    }																																
																																
    if( cs == 0 )															/* �����т�0�̏ꍇ				*/																															
    {																																
        cs = 0xAA;															/* 0xaa�ɕ␳					*/																															
    }																																
    ctTest = cs;																																
    return cs;																																
}		

/***********************************************************************************************************************
*    �֐���    pdlWriteFlash
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �t���b�V���������փo�b�N�A�b�v�f�[�^��������
*    �������e  
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	pdlWriteFlash(void)
{
    volatile pfdl_status_t pfdlsts;																																																					
	UBYTE err = 0;
	
	/* �������ޑO������т��Z�o	*/
    geefFlCheckSum = calcFlashCheckSum();																										
   
	/*�t���b�V���A�N�Z�X�J�n*/
	R_FDL_Open();
	
	/*�������݉\�H*/
	pfdlsts = R_FDL_BlankCheck(0,0x3FF);
	if(pfdlsts == PFDL_ERR_MARGIN )
	{
		pfdlsts = R_FDL_Erase( 0 );				/* �ׯ���ڰ�				*/   
		if( pfdlsts != PFDL_OK )																											
	    {
			err = 0x02; // Erase error	
			goto TAG_END;
		}
	}
	
	/*�Œ�擪�f�[�^*/
	pfdlsts = R_FDL_Write( PFDL_FIXDATA_ADDR , (__near pfdl_u08*)geefFlFixData, sizeof(geefFlFixData));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;						
	}
		
	/* ���*/
	pfdlsts = R_FDL_BlankCheck(PFDL_COURSE_TYPE_ADDR,sizeof(geefFlCourse));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_COURSE_TYPE_ADDR , (__near pfdl_u08*)&geefFlCourse, sizeof(geefFlCourse));				
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
		
	/* �^��H���\���@'�b' or '%' �؊���	*/
	pfdlsts = R_FDL_BlankCheck(PFDL_VACUUM_TYPE_ADDR,sizeof(geefFlVacuumType));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_VACUUM_TYPE_ADDR, (__near pfdl_u08*)geefFlVacuumType, sizeof(geefFlVacuumType));			
   	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	/*  ��۸ޒ����l						*/
	pfdlsts = R_FDL_BlankCheck(PFDL_ANALOG_ADJUST_ADDR,sizeof(geefCtAnalogAdjust));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_ANALOG_ADJUST_ADDR , (__near pfdl_u08*)&geefCtAnalogAdjust, sizeof(geefCtAnalogAdjust)); 
    if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	/* �װ���o����ð��فi10��)		*/
	pfdlsts = R_FDL_BlankCheck(PFDL_ERROR_HISTORY_ADDR,sizeof(geefBfErrorHistory));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_ERROR_HISTORY_ADDR , (__near pfdl_u08*)geefBfErrorHistory, sizeof(geefBfErrorHistory) );	
   	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;						
	}
	
	/* ����ް�(4�����)					*/
	pfdlsts = R_FDL_BlankCheck(PFDL_COURSE_DATA_ADDR,sizeof(geefBfCourseData));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_COURSE_DATA_ADDR , (__near pfdl_u08*)geefBfCourseData, sizeof(geefBfCourseData) );		
    if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;						
	}
	
	/* ����߉ғ�����					*/
	pfdlsts = R_FDL_BlankCheck(PFDL_PUMP_RINNING_ADR,sizeof(geefCtPumpRunning));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_PUMP_RINNING_ADR , (__near pfdl_u08*)&geefCtPumpRunning, sizeof(geefCtPumpRunning) ); 	
    if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	
	/* �����ް�							*/	
	pfdlsts = R_FDL_BlankCheck(PFDL_SYSTEM_DATA_ADDR,sizeof(geefBfSystemData));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_SYSTEM_DATA_ADDR , (__near pfdl_u08*)geefBfSystemData, sizeof(geefBfSystemData) );		
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	/* �p�b�N��					*/
	pfdlsts = R_FDL_BlankCheck(PFDL_PACK_COUNT_ADR,sizeof(geefCtPack));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_PACK_COUNT_ADR , (__near pfdl_u08*)&geefCtPack, sizeof(geefCtPack) ); 					
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	/* �Œ��ް��@A5,5A*/
	pfdlsts = R_FDL_BlankCheck(PFDL_FIXEND_ADDR,sizeof(geefFlFixEndData));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_FIXEND_ADDR , (__near pfdl_u08*)geefFlFixEndData, sizeof(geefFlFixEndData) );			
   	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	/* ������							*/		
	pfdlsts = R_FDL_BlankCheck(PFDL_CHECKSUM_ADDR,sizeof(geefFlCheckSum));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;	
	}
	pfdlsts = R_FDL_Write( PFDL_CHECKSUM_ADDR, (__near pfdl_u08*)&geefFlCheckSum, sizeof(geefFlCheckSum) );						
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;							
	}
	
	TAG_END:
   
   	R_FDL_Close();
   
   	return(err);
   
}

/***********************************************************************************************************************
*    �֐���    pdlReadFlash
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �o�b�N�A�b�v�f�[�^���t���b�V������������ǂݍ���
*    �������e  �ǂݍ��񂾃o�b�N�A�b�v�f�[�^�̃`�F�b�N�T���ƃo�b�N�A�b�v����Ă���`�F�b�N�T���������ł��邱�Ɗm�F�B
*              ���ق��������ꍇ�̓o�b�N�A�b�v�f�[�^�̔j���Ɣ��f����B
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
UBYTE	pdlReadFlash(void)
{
	volatile pfdl_status_t fdlResult;	
	volatile UBYTE err = 0;
	UBYTE checkSum;
	
	
	/* �t���b�V���ǂݍ��݊J�n */	
	R_FDL_Open();		
	
	/* �Œ��ް��@A5,5A*/	
	fdlResult = R_FDL_IVerify( PFDL_FIXDATA_ADDR, sizeof(geefFlFixData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error	
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read( PFDL_FIXDATA_ADDR, (__near pfdl_u08*)geefFlFixData, sizeof(geefFlFixData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
	/* ���						*/	
	fdlResult = R_FDL_IVerify( PFDL_COURSE_TYPE_ADDR, sizeof(geefFlCourse));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read( PFDL_COURSE_TYPE_ADDR, (__near pfdl_u08*)&geefFlCourse, sizeof(geefFlCourse));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
	/* �^��H���\���@'�b' or '%' �؊���	*/
	fdlResult = R_FDL_IVerify( PFDL_VACUUM_TYPE_ADDR, sizeof(geefFlVacuumType));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read( PFDL_VACUUM_TYPE_ADDR, (__near pfdl_u08*)geefFlVacuumType, sizeof(geefFlVacuumType));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
   	/*  ��۸ޒ����l						*/
	fdlResult = R_FDL_IVerify( PFDL_ANALOG_ADJUST_ADDR, sizeof(geefCtAnalogAdjust));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}	
	fdlResult = R_FDL_Read(  PFDL_ANALOG_ADJUST_ADDR, (__near pfdl_u08*)&geefCtAnalogAdjust, sizeof(geefCtAnalogAdjust));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}

   	/* �װ���o����ð��فi10��)		*/
	fdlResult = R_FDL_IVerify( PFDL_ERROR_HISTORY_ADDR, sizeof(geefBfErrorHistory));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read(  PFDL_ERROR_HISTORY_ADDR, (__near pfdl_u08*)geefBfErrorHistory, sizeof(geefBfErrorHistory));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
   	/* ����ް�(4�����)					*/
	fdlResult =  R_FDL_IVerify( PFDL_COURSE_DATA_ADDR, sizeof(geefBfCourseData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read(  PFDL_COURSE_DATA_ADDR, (__near pfdl_u08*)geefBfCourseData, sizeof(geefBfCourseData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}			
											
   	/* ����߉ғ�����					*/
	fdlResult = R_FDL_IVerify( PFDL_PUMP_RINNING_ADR, sizeof(geefCtPumpRunning));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}		
	fdlResult = R_FDL_Read(  PFDL_PUMP_RINNING_ADR, (__near pfdl_u08*)&geefCtPumpRunning, sizeof(geefCtPumpRunning));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
	/* �����ް�							*/
	fdlResult = R_FDL_IVerify( PFDL_SYSTEM_DATA_ADDR, sizeof(geefBfSystemData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}		
	fdlResult = R_FDL_Read( PFDL_SYSTEM_DATA_ADDR , (__near pfdl_u08*)geefBfSystemData, sizeof(geefBfSystemData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
		
	/* �p�b�N��					*/	
	fdlResult = R_FDL_IVerify( PFDL_PACK_COUNT_ADR, sizeof(geefCtPack)); 
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}				
	fdlResult = R_FDL_Read( PFDL_PACK_COUNT_ADR , (__near pfdl_u08*)&geefCtPack, sizeof(geefCtPack));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
	/* �Œ�END�R�[�h					*/	
	fdlResult = R_FDL_IVerify( PFDL_FIXEND_ADDR, sizeof(geefFlFixEndData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;
	}
	fdlResult = R_FDL_Read( PFDL_FIXEND_ADDR , (__near pfdl_u08*)geefFlFixEndData, sizeof(geefFlFixEndData));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}
	
	/* ������							*/		
	fdlResult = R_FDL_IVerify( PFDL_CHECKSUM_ADDR, sizeof(geefFlCheckSum));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;	
	}
	fdlResult = R_FDL_Read( PFDL_CHECKSUM_ADDR, (__near pfdl_u08*)&geefFlCheckSum, sizeof(geefFlCheckSum));
	if(fdlResult != PFDL_OK )
	{
		err = 0x04; // read error
		goto TAG_END;			
	}

	
	TAG_END:
	/* �t���b�V���ǂݍ��ݏI�� */	
	R_FDL_Close();
    
    /* �`�F�b�N�T���m�F */		
	if(fdlResult == PFDL_OK )
	{
		checkSum = calcFlashCheckSum();
		if( geefFlCheckSum != checkSum )																											
		{																											
		   err = 0x08;// data error																											
		}
	}

	return(err);
}


/***********************************************************************************************************************
*    �֐���    pldVerifyCheck
*    ��  ��    
*    �߂�l    �O�F���قȂ��A�P�F���ق���
*    �֐��@�\  �o�b�N�A�b�v�f�[�^�̔�r
*    �������e  �ŐV�̃o�b�N�A�b�v�f�[�^�ƒ��O�̃o�b�N�A�b�v�f�[�^���r
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

UBYTE	pldVerifyCheck(void)	
{
    int i, j;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )			/* �ޯ����ߗ̈�̔�r�����邽�߂�ٰ��	*/																														
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )									/* ���ڒP�ʂ�ٰ��						*/																											
        {																																
            if(dataFlashTbl_Base[i].address[j] != dataFlashTbl[i].address[j])		/* �قȂ�ꍇ							*/
			{
				return(1);															/* ���ٗL��								*/
			}																																
        }																																
    }																																
    return 0;																																
}

/***********************************************************************************************************************
*    �֐���    pldCopyBackupData
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �o�b�N�A�b�v�f�[�^���ŐV�ɍX�V����
*    �������e
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/

void	pldCopyBackupData(void)	
{
    int i, j;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )			/* �ޯ������ް��X�V�����邽�߂�ٰ��	*/	
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )									/* ���ڒP�ʂ�ٰ��						*/																														
        {																																
            dataFlashTbl_Base[i].address[j] = dataFlashTbl[i].address[j];			/* ��߰									*/
        }																																
    }																																
}


/***********************************************************************************************************************
*    �֐���    pldRestoreBackupData
*    ��  ��    
*    �߂�l    
*    �֐��@�\  �ޯ������ް��̔�r
*    �������e  �ŐV���ޯ������ް��ƒ��O���ޯ������ް��̔�r
*    �쐬��    2018�N2��10��
*    �쐬��    �\���@�P�v
***********************************************************************************************************************/
void	pldRestoreBackupData(void)	
{
    int i, j;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )																																
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )																																
        {																																
            dataFlashTbl[i].address[j] = dataFlashTbl_Base[i].address[j];
        }																																
    }																																


}
																										
