#ifndef __PFDL_USER_H_INCLUDED																				
#define __PFDL_USER_H_INCLUDED																				
																				
																				
#include "r_cg_pfdl.h"																				
#include "geefFlash.h"																				
																				
																				
#define PFDL_FIXDATA_ADDR     	(0)	
#define PFDL_COURSE_TYPE_ADDR   (PFDL_FIXDATA_ADDR 	 		+ sizeof(geefFlFixData))																				
#define PFDL_VACUUM_TYPE_ADDR   (PFDL_COURSE_TYPE_ADDR 	 	+ sizeof(geefFlCourse))																				
#define PFDL_ANALOG_ADJUST_ADDR (PFDL_VACUUM_TYPE_ADDR	    + sizeof(geefFlVacuumType))																				
#define PFDL_ERROR_HISTORY_ADDR (PFDL_ANALOG_ADJUST_ADDR    + sizeof(geefCtAnalogAdjust))																				
#define PFDL_COURSE_DATA_ADDR 	(PFDL_ERROR_HISTORY_ADDR    + sizeof(geefBfErrorHistory))		
#define	PFDL_PUMP_RINNING_ADR	(PFDL_COURSE_DATA_ADDR      + sizeof(geefBfCourseData)) 
#define	PFDL_SYSTEM_DATA_ADDR   (PFDL_PUMP_RINNING_ADR      + sizeof(geefCtPumpRunning))
#define PFDL_PACK_COUNT_ADR      (PFDL_SYSTEM_DATA_ADDR      + sizeof(geefBfSystemData))
#define PFDL_FIXEND_ADDR      	(PFDL_PACK_COUNT_ADR      + sizeof(geefCtPack))
#define PFDL_CHECKSUM_ADDR      (PFDL_FIXEND_ADDR      + sizeof(geefFlFixEndData))

																																																								
																				
																																					
extern pfdl_u08 calcFlashCheckSum(void);																				
																				
typedef struct																				
{																				
    __near pfdl_u08* address;																				
    pfdl_u16 size;																				
} DATA_FLASH_INFO;																				
																				
#endif			

																
