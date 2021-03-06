#include	"iodefine.h"
#include	"glbRam.h"
#include	"common.h"
#include 	"geefFlash.h"



void	mainWarmMode(void);
void	mainWarmStart(void);
void	mainWarmEnd(void);
void	mainWarmDrive(void);

void	(*WarmProcTbl[])(void);

extern	void	mainDisplay7Seg(UWORD);										/* 7SEG\¦							*/
extern	UBYTE	glbGetReadKey(void);
extern	UBYTE	DrawerSwRead(void);
extern	void	mcmSetBuzzerEnd(void);										/* I¹ÌÞ»Þ°ðÂç·			*/
extern	UBYTE	mcmCheckDrawerOpenToClose(void);
extern	UBYTE	mcmFlashWriteCheck(void);							/* ÊÞ¯¸±¯ÌßÒÓØÏXÁª¯¸				*/
extern	void	mcmSetBuzzerInvalid(void);
extern	void	mcmSetBuzzerEffective(void);

extern	void ledControl_Proc(UBYTE proc,UBYTE ctl,UBYTE mode);
extern	void ledControl_Course(UBYTE course,UBYTE ctl,UBYTE mode);
extern	void ledControl_Sec(UBYTE toggle,UBYTE ctl,UBYTE mode);

void	(*WarmProcTbl[])() =
{
	mainWarmDrive				/* g@^]Ä						*/
};
/***********************************************************************************************************************
*    Ö¼    mainDriveMode
*    ø      
*    ßèl    
*    Ö@\  gC^][hC
*    àe  g@^]ÆI¹ãÌT5ÔÄ
*    Ó@@@
*    ì¬ú    2018N210ú
*    ì¬Ò    \ª@Pv
***********************************************************************************************************************/
void mainWarmMode(void)
{
	(*WarmProcTbl[glbFlWarmMode])();								/* »óÔÌÓ°ÄÞÖJÚ		*/

}
/***********************************************************************************************************************
*    Ö¼    mainWarmDrive
*    ø      
*    ßèl    
*    Ö@\  gC^]
*    àe  
*    Ó@@@g@^]Érs`qs^rsnoXCb`ª³ê½êÍAg@^]ð~·é
*    ì¬ú    2018N210ú
*    ì¬Ò    \ª@Pv
***********************************************************************************************************************/
void	mainWarmDrive(void)
{
	UBYTE	key;
	UWORD	data;

	glbFlSaveMode = glbFlDriveMode;										/* Ò²ÝÓ°ÄÞÌóÔðÞð			*/
	data = ((glbCtWarmTimer * GCN_INT_TIME)/GCN_TIME_1S);				/* 5msPÊÌ¶³ÝÀðbÉÏ·			*/
	mainDisplay7Seg((UWORD)data);										/* 7SEG\¦							*/

	if((key = glbGetReadKey()))											/* KEYÌüÍÁª¯¸					*/
	{		
		if(key == GCN_KEY_START_SET)									/* ½À°Ä/½Ä¯Ìß·°ª³ê½ê		*/
		{
			mcmSetBuzzerEffective();									/* ÎÞÀÝLøÌÞ»Þ°					*/
			mainWarmEnd();												/* gC^]I¹					*/
			if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
			{
				data = geefBfCourseData[geefFlCourse][Vacuum];			/* Ið³êÄ¢éº°½1Ì^óÔØ°ÄÞ				*/
			}
			else
			{
				data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* Ið³êÄ¢éº°½1Ì^óÔØ°ÄÞ				*/
			}
			mainDisplay7Seg(data);										/* 7SEG\¦					*/
		}
		else if(key == GCN_KEY_STOP_SET)								/* ½Ä¯Ìß·°iBµjª³ê½ê		*/
		{
																		/* ÌÞ»Þ°ðÂç³È¢					*/
		}
		else 
		{
			mcmSetBuzzerInvalid();										/* ÎÞÀÝ³øÌÞ»Þ°			*/
		}
	}
	
	if(glbFlPowerSw)
	{	
		mcmSetBuzzerEffective();									/* ÎÞÀÝLøÌÞ»Þ°					*/
		mainWarmEnd();												/* gC^]I¹					*/
		if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)
		{
			data = geefBfCourseData[geefFlCourse][Vacuum];			/* Ið³êÄ¢éº°½1Ì^óÔØ°ÄÞ				*/
		}
		else
		{
			data = geefBfCourseData[geefFlCourse][VacuumLevel];		/* Ið³êÄ¢éº°½1Ì^óÔØ°ÄÞ				*/
		}
		mainDisplay7Seg(data);										/* 7SEG\¦					*/	
	}
				

	if(glbCtVacuumElectromang == 0)										/* ^ód¥ÙONoßH(0.5b)		*/
	{
		P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* ^ód¥ÙOFF					*/
	}

	if(glbCtWarmTimer == 0)												/* gC^]I¹Ôoß?			*/
	{
		mainWarmEnd();													/* gC^]I¹					*/
		glbFlWarmMode = 1;												/* g@^]Ó°ÄÞðT5ÔÄÖ		*/
	}

}

/***********************************************************************************************************************
*    Ö¼    mainWarmStart
*    ø      
*    ßèl    
*    Ö@\  gC^]Jn
*    àe  
*    Ó@@@gC^]ÄJÌêÍAODTbÔÌ^ód¥ÙÍONµÈ¢±ÆBB
*    ì¬ú    2018N210ú
*    ì¬Ò    \ª@Pv
***********************************************************************************************************************/
void	mainWarmStart(void)
{
	unsigned long int	data;
	volatile	int	i;
	
	DI();

	P7 |= P74_PUMP_START_ON;										/* ÎßÝÌßÒ­Jn					*/ 

	EI();
	
	ledControl_Proc(ALL_LED_SELECT,0,0);									

	glbFlReqDot         = 0;											/* ËßØµÄÞ\¦³µ					*/
	glbFlWarmMode = 0;													/* g@^]Ò²ÝÌ×¸ÞËg@^]Ö	*/
	if(glbFlWarmCansel == 0)											/* gC^]r·¬Ý¾ÙË^ó^]ËÄxAgC^]ÌêÍA0.5bÔÌ^ód¥ONÍsíÈ¢	*/
	{
		P2 |= P20_VACUUM_ELECTROMAG_ON;									/* ^ód¥ÙON						*/
		glbCtVacuumElectromang = GCN_TIMER_500MS;						/* ^ód¥Ùð0.5bJ­½ßÌÀ²Ï¾¯Ä*/
		glbFlVacuumElectromangReq = 1;									/* ^ód¥Ùð0.5bJ­½ßÌv¾¯Ä	*/
	}
	data = geefBfSystemData[GCN_SYSTEM_WARM][Default];
	mainDisplay7Seg((UWORD)data);										/* 7SEG\¦							*/

	glbFlSecLevel   =  GCN_VACUUM_SECONDS;								/* ubv\¦É·é					*/
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbCtWarmTimer = (geefBfSystemData[GCN_SYSTEM_WARM][Default] *		/* gC^]Ô¾¯Äibj			*/
					 GCN_TIMER_1S);

}

/***********************************************************************************************************************
*    Ö¼    mainWarmEnd
*    ø      
*    ßèl    
*    Ö@\  gC^]I¹
*    àe  
*    Ó@@@
*    ì¬ú    2018N210ú
*    ì¬Ò    \ª@Pv
***********************************************************************************************************************/
void	mainWarmEnd(void)
{
	UWORD	data;

	DI();
	P7 &= P74_PUMP_START_OFF;											/* ÎßÝÌßÒ­â~					*/ 
	EI();
	P2 |= P20_VACUUM_ELECTROMAG_ON;										/* ^ód¥ÙON						*/
	glbFlVacuumElectromangReq = 1;										/* ^]Höi^ój/gC^]I¹ÌÎßÝÌßâ~ã10bÔA^ód¥ÙONÌv¾¯Ä	*/
	glbCtVacuumElectromang = GCN_TIMER_10S;								/* gC^]I¹ãA^ód¥Ùð10bÔJ­iONj·é½ßÌÀ²Ï¾¯Ä	*/
// «20180630
//	glbFlFunReq = 1;													/* ^]Höi^ój/gC^]I¹ÌÎßÝÌßâ~ã,3ªÔÌ§Ýðì®³¹é½ßÌv¾¯Ä	*/
// ª	20180630
	glbCtFunTimer = GCN_TIME_3MN;										/* gC^]I¹ãAÌ§Ýð3ªÔñ·½ßÌÀ²Ï¾¯Ä	*/
	glbFlWarmCansel = 0;
	mcmSetBuzzerEnd();													/* I¹ÌÞ»Þ°			*/
	glbFlSecLevel   = (geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)?
			      	  GCN_VACUUM_SECONDS:GCN_VACUUM_LEVEL;
	if(geefFlVacuumType[geefFlCourse] == GCN_VACUUM_SECONDS)			/* 'b'\¦Ìê								*/
	{
		data = geefBfCourseData[geefFlCourse][Vacuum];					/* Ið³êÄ¢éº°½Ì^óÔØ°ÄÞ				*/
	}
	else																/* '%'\¦Ìê								*/
	{
		data = geefBfCourseData[geefFlCourse][VacuumLevel];				/* Ið³êÄ¢éº°½Ì^óÔØ°ÄÞ				*/

	}
	mainDisplay7Seg(data);												/* 7SEG\¦					*/
	
	glbFlProc 		= GCN_PROC_VACUUM;						/* Höðu^óvÉ·é	*/
	ledControl_Proc(glbFlProc,1,0);											
	ledControl_Sec(glbFlSecLevel,1,0);
	
	glbFlMainMode = GCN_WAIT_MODE;					/* Ò²ÝÓ°ÄÞðÒ@Ó°ÄÞÖ		*/

}


