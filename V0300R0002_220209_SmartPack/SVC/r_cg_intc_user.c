/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_intc_user.c
* Version      : CodeGenerator for RL78/G13 V2.05.05.01 [25 Nov 2020]
* Device(s)    : R5F100GJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTC module.
* Creation Date: 22/01/25
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intc.h"
/* Start user code for include. Do not edit comment generated here */
#include	"iodefine.h"
#include	"common.h"
#include	"glbRam.h"
#include "r_cg_timer.h"
extern	void	R_TAU0_Channel0_Stop(void);														/* 1msﾀｲﾏ割り込み停止		*/
UBYTE	pdlWriteFlash(void);
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_intc6_interrupt(vect=INTP6)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_intc6_interrupt
* Description  : This function is INTP6 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc6_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	volatile long int i;
	UBYTE	in1,in2;

	R_TAU0_Channel2_Stop();						/* ﾌﾞｻﾞｰ停止					*/
	
#if 0 
	if(!glbFlSystemMode)	//システムモードは登録しない
	{
		pdlWriteFlash();
	}
#endif

	P7 |= 0x01;							/* LEDCOM4 OFF*/
	P3 |= 0x01;							/* LEDCOM3 OFF*/
	P5 |= 0x01;							/* LEDCOM1 OFF*/
	P5 |= 0x02;							/* LEDCOM2 OFF*/
	P1 = 0x00;							/* 7SEG/LED ALL OFF*/

	P7 &= P74_PUMP_START_OFF;									/* ﾎﾟﾝﾌﾟ稼働停止					*/ 
	P7 &= P72_POWER_LED_OFF;										/* 電源								*/ 
	P2 &= P20_VACUUM_ELECTROMAG_OFF;								/* 真空電磁弁OFF					*/
	P4 &= P41_VACUUM_RELEASE_OFF;									/* 真空解放弁OFF						*/ 
	P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ｼｰﾙ電磁弁OFF							*/
	P7 &= P74_PUMP_START_OFF;										/* ﾎﾟﾝﾌﾟ稼働停止						*/ 
	P7 &= P73_HEATER_START_OFF;										/* ﾋｰﾀOFF								*/
	P7 &= P75_SOFTRELEASE_START_OFF;								/* ｿﾌﾄ解放弁OFF							*/
	P13 &= P130_GUS_ELECTROMAG_OFF;									/* ｶﾞｽ電磁弁 OFF						*/
	P4 &= P41_VACUUM_RELEASE_OFF;									/* 真空解放弁OFF						*/ 
	P0 &= P01_SEEL_ELECTROMAG_OFF;									/* ｼｰﾙ電磁弁OFF							*/

	while(1)
	{
		in1 = P14;
		in2 = P14;
		if(in1 != in2)
		{
			in1 = P14;
		}
		if(in1 & 1)							/* PFAIL = 'H' 電源再投入？	*/
		{
			 WDTE = 0xFFU;  				/* 内部ﾘｾｯﾄをかけるために、WDTに*/
		}
	}
	
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
