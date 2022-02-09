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
/* ■フラッシュメモリ構成テーブル						  */
/*--------------------------------------------------------*/
DATA_FLASH_INFO dataFlashTbl[] = 																																
{																																
    {(__near UBYTE* )geefFlFixData			, sizeof(geefFlFixData)			},			/* 固定ﾃﾞｰﾀ　A5,5A*/
    {(__near UBYTE* )&geefFlCourse			, sizeof(geefFlCourse)			},			/* ｺｰｽNo.							*/
    {(__near UBYTE* )geefFlVacuumType		, sizeof(geefFlVacuumType)		},			/* 真空工程表示　'秒' or '%' 切換え	*/
	{(__near UBYTE* )&geefCtAnalogAdjust	, sizeof(geefCtAnalogAdjust)	},			/* ｱﾅﾛｸﾞ調整値						*/
	{(__near UBYTE* )geefBfErrorHistory  	, sizeof(geefBfErrorHistory) 	},			/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ（10回分)		*/
    {(__near UBYTE* )geefBfCourseData		, sizeof(geefBfCourseData)		},			/* ｺｰｽﾃﾞｰﾀ(4ｺｰｽ分)					*/
    {(__near UBYTE* )geefBfSystemData		, sizeof(geefBfSystemData)		},			/* ｼｽﾃﾑﾃﾞｰﾀ							*/
    {(__near UBYTE* )&geefCtPumpRunning		, sizeof(geefCtPumpRunning)		},			/* ﾎﾟﾝﾌﾟ稼働時間					*/
    {(__near UBYTE* )&geefCtPack			, sizeof(geefCtPack)		},				/* パック数				*/	
	{(__near UBYTE* )geefFlFixEndData		, sizeof(geefFlFixEndData)		}			/* 固定ﾃﾞｰﾀ　A5,5A*/		
																																
};																																

/*--------------------------------------------------------*/
/* ■バックアップ領域構成テーブル						  */
/*--------------------------------------------------------*/
DATA_FLASH_INFO dataFlashTbl_Base[] = 																																
{																																
    {(__near UBYTE* )geefFlFixData_Base			, sizeof(geefFlFixData_Base)		},			/* 固定ﾃﾞｰﾀ　A5,5A*/
    {(__near UBYTE* )&geefFlCourse_Base			, sizeof(geefFlCourse_Base)			},			/* ｺｰｽNo.							*/
    {(__near UBYTE* )geefFlVacuumType_Base		, sizeof(geefFlVacuumType_Base)		},			/* 真空工程表示　'秒' or '%' 切換え	*/
	{(__near UBYTE* )&geefCtAnalogAdjust_Base	, sizeof(geefCtAnalogAdjust_Base)	},			/* ｱﾅﾛｸﾞ調整値						*/
	{(__near UBYTE* )geefBfErrorHistory_Base  	, sizeof(geefBfErrorHistory_Base) 	},			/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ（10回分)		*/
    {(__near UBYTE* )geefBfCourseData_Base		, sizeof(geefBfCourseData_Base)		},			/* ｺｰｽﾃﾞｰﾀ(4ｺｰｽ分)					*/
    {(__near UBYTE* )geefBfSystemData_Base		, sizeof(geefBfSystemData_Base)		},			/* ｼｽﾃﾑﾃﾞｰﾀ							*/
    {(__near UBYTE* )&geefCtPumpRunning_Base	, sizeof(geefCtPumpRunning_Base)	},			/* ﾎﾟﾝﾌﾟ稼働時間					*/
 	{(__near UBYTE* )&geefCtPack_Base			, sizeof(geefCtPack_Base)		},						/* パック数				*/		
	{(__near UBYTE* )geefFlFixEndData_Base		, sizeof(geefFlFixEndData_Base)		},			/* 固定ﾃﾞｰﾀ　A5,5A*/	
																																
};																																
																																
/***********************************************************************************************************************
*    関数名    calcFlashCheckSum
*    引  数    
*    戻り値    
*    関数機能  フラッシュメモリ領域のチェックサム算出
*    処理内容  
*    備考	　 チェックサム領域は計算外とする
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

UBYTE	calcFlashCheckSum(void)																																
{																																
    int i, j;																																
    pfdl_u08 cs;																																
    																																
    cs = 0;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )	/* ﾊﾞｯｸｱｯﾌﾟ領域のﾁｪｯｸｻﾑを計算するためのﾙｰﾌﾟ	*/																															
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )																																
        {																																
            cs = cs ^ dataFlashTbl[i].address[j];							/* XORでﾁｪｯｸｻﾑ計算							*/																															
        }																																
    }																																
																																
    if( cs == 0 )															/* ﾁｪｯｸｻﾑが0の場合				*/																															
    {																																
        cs = 0xAA;															/* 0xaaに補正					*/																															
    }																																
    ctTest = cs;																																
    return cs;																																
}		

/***********************************************************************************************************************
*    関数名    pdlWriteFlash
*    引  数    
*    戻り値    
*    関数機能  フラッシュメモリへバックアップデータ書き込み
*    処理内容  
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	pdlWriteFlash(void)
{
    volatile pfdl_status_t pfdlsts;																																																					
	UBYTE err = 0;
	
	/* 書き込む前にﾁｪｯｻﾑを算出	*/
    geefFlCheckSum = calcFlashCheckSum();																										
   
	/*フラッシュアクセス開始*/
	R_FDL_Open();
	
	/*書き込み可能？*/
	pfdlsts = R_FDL_BlankCheck(0,0x3FF);
	if(pfdlsts == PFDL_ERR_MARGIN )
	{
		pfdlsts = R_FDL_Erase( 0 );				/* ﾌﾗｯｼｭｲﾚｰｽ				*/   
		if( pfdlsts != PFDL_OK )																											
	    {
			err = 0x02; // Erase error	
			goto TAG_END;
		}
	}
	
	/*固定先頭データ*/
	pfdlsts = R_FDL_Write( PFDL_FIXDATA_ADDR , (__near pfdl_u08*)geefFlFixData, sizeof(geefFlFixData));
	if( pfdlsts != PFDL_OK )
	{
		err = 0x01; // write error	
		goto TAG_END;						
	}
		
	/* ｺｰｽ*/
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
		
	/* 真空工程表示　'秒' or '%' 切換え	*/
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
	
	/*  ｱﾅﾛｸﾞ調整値						*/
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
	
	/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ（10回分)		*/
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
	
	/* ｺｰｽﾃﾞｰﾀ(4ｺｰｽ分)					*/
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
	
	/* ﾎﾟﾝﾌﾟ稼働時間					*/
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
	
	
	/* ｼｽﾃﾑﾃﾞｰﾀ							*/	
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
	
	/* パック数					*/
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
	
	/* 固定ﾃﾞｰﾀ　A5,5A*/
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
	
	/* ﾁｪｯｸｻﾑ							*/		
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
*    関数名    pdlReadFlash
*    引  数    
*    戻り値    
*    関数機能  バックアップデータをフラッシュメモリから読み込む
*    処理内容  読み込んだバックアップデータのチェックサムとバックアップされているチェックサムが同じであること確認。
*              差異があった場合はバックアップデータの破損と判断する。
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/
UBYTE	pdlReadFlash(void)
{
	volatile pfdl_status_t fdlResult;	
	volatile UBYTE err = 0;
	UBYTE checkSum;
	
	
	/* フラッシュ読み込み開始 */	
	R_FDL_Open();		
	
	/* 固定ﾃﾞｰﾀ　A5,5A*/	
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
	
	/* ｺｰｽ						*/	
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
	
	/* 真空工程表示　'秒' or '%' 切換え	*/
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
   	/*  ｱﾅﾛｸﾞ調整値						*/
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

   	/* ｴﾗｰ検出履歴ﾃｰﾌﾞﾙ（10回分)		*/
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
	
   	/* ｺｰｽﾃﾞｰﾀ(4ｺｰｽ分)					*/
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
											
   	/* ﾎﾟﾝﾌﾟ稼働時間					*/
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
	
	/* ｼｽﾃﾑﾃﾞｰﾀ							*/
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
		
	/* パック数					*/	
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
	
	/* 固定ENDコード					*/	
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
	
	/* ﾁｪｯｸｻﾑ							*/		
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
	/* フラッシュ読み込み終了 */	
	R_FDL_Close();
    
    /* チェックサム確認 */		
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
*    関数名    pldVerifyCheck
*    引  数    
*    戻り値    ０：差異なし、１：差異あり
*    関数機能  バックアップデータの比較
*    処理内容  最新のバックアップデータと直前のバックアップデータを比較
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

UBYTE	pldVerifyCheck(void)	
{
    int i, j;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )			/* ﾊﾞｯｸｱｯﾌﾟ領域の比較をするためのﾙｰﾌﾟ	*/																														
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )									/* 項目単位のﾙｰﾌﾟ						*/																											
        {																																
            if(dataFlashTbl_Base[i].address[j] != dataFlashTbl[i].address[j])		/* 異なる場合							*/
			{
				return(1);															/* 差異有り								*/
			}																																
        }																																
    }																																
    return 0;																																
}

/***********************************************************************************************************************
*    関数名    pldCopyBackupData
*    引  数    
*    戻り値    
*    関数機能  バックアップデータを最新に更新する
*    処理内容
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
***********************************************************************************************************************/

void	pldCopyBackupData(void)	
{
    int i, j;																																
    																																
    for ( i = 0; i < sizeof(dataFlashTbl)/sizeof(DATA_FLASH_INFO); i++ )			/* ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀ更新をするためのﾙｰﾌﾟ	*/	
    {																																
        for(j = 0; j < dataFlashTbl[i].size ; j++ )									/* 項目単位のﾙｰﾌﾟ						*/																														
        {																																
            dataFlashTbl_Base[i].address[j] = dataFlashTbl[i].address[j];			/* ｺﾋﾟｰ									*/
        }																																
    }																																
}


/***********************************************************************************************************************
*    関数名    pldRestoreBackupData
*    引  数    
*    戻り値    
*    関数機能  ﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀの比較
*    処理内容  最新のﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀと直前のﾊﾞｯｸｱｯﾌﾟﾃﾞｰﾀの比較
*    作成日    2018年2月10日
*    作成者    曾根　輝夫
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
																										
