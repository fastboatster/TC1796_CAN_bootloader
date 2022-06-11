/**************************************************************************
** 																		  *
**  INFINEON TriCore Bootstrap Loader, Version 1.0                        *
**																		  *
**  Id: reg176x.h 2008-09-12 16:20:32Z winterstein                        *
**                                                                        *
**  DESCRIPTION :                                                         *
**      TriCore register definition                                       *
**      						                                          *
**		                                                                  *
**  							                                          *
**************************************************************************/


#ifndef REG1766_H
#define REG1766_H



/*---- Includes --------------------------------------------------------------*/


/*---- Defines ---------------------------------------------------------------*/

/*
 * System Control Unit (SCU)
 */

/* Watchdog Timer Control Register 0 */
#define WDT_CON0 (*((unsigned int volatile *) 0xF0000020))

/* Watchdog Timer Control Register 1 */
#define WDT_CON1 (*((unsigned int volatile *) 0xF0000024))

/* PLL Clock Control Register */
#define PLL_CLC  (*((unsigned int volatile *) 0xF0000040))


/*
 * System Timer (STM)
 */

/* STM Clock Control Register */
#define STM_CLC   (*((unsigned int volatile *) 0xF0000200))

/* STM Timer Register 0 - Bits 31..0 */
#define STM_TIM0  (*((unsigned int volatile *) 0xF0000210))

/* STM Timer Capture Register - Bits 55..32 */
#define STM_CAP   (*((unsigned int volatile *) 0xF000022C))

/* Compare Register 1 */
#define STM_CMP0  (*((unsigned int volatile *) 0xF0000230))

/* Compare Match Control Register */
#define STM_CMCON (*((unsigned int volatile *) 0xF0000238))

/* STM Interrupt Control Register */
#define STM_ICR   (*((unsigned int volatile *) 0xF000023C))

/* STM Service Request Control Register 0 */
#define STM_SRC0  (*((unsigned int volatile *) 0xF00002FC))


/*
 * GPIO Ports and Peripheral I/O (GPIO)
 */

/* Port 1 Output Modification Register */
#define P1_OMR (*((unsigned int volatile *) 0xF0000D04))

/* Port 1 Input/Output Control Registers 0 */
#define P1_IOCR0  (*((unsigned int volatile *) 0xF0000D10))

/* Port 1 Input/Output Control Register 4 */
#define P1_IOCR4 (*((unsigned int volatile *) 0xF0000D14))

/* Port 1 Input/Output Control Register 8 */
#define P1_IOCR8 (*((unsigned int volatile *) 0xF0000D18))

/* Port 1 Input/Output Control Register 12 */
#define P1_IOCR12 (*((unsigned int volatile *) 0xF0000D1C))

/* Port 1 Pad Driver Mode Register */
#define P1_PDR (*((unsigned int volatile *) 0xF0000D40))

/* Port 3 Output Modification Register */
#define P3_OMR (*((unsigned int volatile *) 0xF0000F04))

/* Port 3 Input/Output Control Registers 0 */
#define P3_IOCR0 (*((unsigned int volatile *) 0xF0000F10))

/* Port 3 Input/Output Control Registers 3 */
#define P3_IOCR4 (*((unsigned int volatile *) 0xF0000F14))

/* Port 3 Input/Output Control Registers 8 */
#define P3_IOCR8 (*((unsigned int volatile *) 0xF0000F18))

/* Port 3 Input/Output Control Registers 12 */
#define P3_IOCR12 (*((unsigned int volatile *) 0xF0000F1C))

/* Port 3 Pad Driver Mode Register */
#define P3_PDR (*((unsigned int volatile *) 0xF0000F40))

/* Port 5 Input/Output Control Registers 0 */
#define P5_IOCR0 (*((unsigned int volatile *) 0xF0001110))

/* Port 5 Input/Output Control Register 4  */
#define P5_IOCR4 (*( (unsigned int volatile *) 0xF0001114))

/* Port 5 Input/Output Control Register 8  */
#define P5_IOCR8 (*( (unsigned int volatile *) 0xF0001118))

/* Port 5 Input/Output Control Register 12  */
#define P5_IOCR12 (*( (unsigned int volatile *) 0xF000111C))

/*
 * Asynchronous/Synchronous Serial Interface (ASC)
 */

/* ASC Clock Control Register */
#define ASC0_CLC (*((unsigned int volatile *) 0xF0000A00))

/* ASC Control Register */
#define ASC0_CON (*((unsigned int volatile *) 0xF0000A10))

/* ASC Baudrate Timer Reload Register */
#define ASC0_BG  (*((unsigned int volatile *) 0xF0000A14))

/* ASC Fractional Divider Register */
#define ASC0_FDV (*((unsigned int volatile *) 0xF0000A18))

/* ASC Transmit Buffer Register */
#define ASC0_TBUF (*((unsigned int volatile *) 0xF0000A20))

/* ASC Transmit Buffer Register */
#define ASC0_RBUF (*((unsigned int volatile *) 0xF0000A24))

/* ASC Transmit Interrupt Service Request Control Register */
#define ASC0_TSRC (*((unsigned int volatile *) 0xF0000AF0))

/* ASC Receive Interrupt Service Request Control Register */
#define ASC0_RSRC (*((unsigned int volatile *) 0xF0000AF4))



#define CAN_MODATAL0 (*((unsigned int volatile *) 0xF0004610))	/* Message Object 0 Data Register Low  */
#define CAN_MODATAL1 (*((unsigned int volatile *) 0xF0004630))	/* Message Object 1 Data Register Low  */

#define CAN_MOFCR0   (*((unsigned int volatile *) 0xF0004600))	/* Message Object 0 Function Control Register  */
#define CAN_MOFCR1   (*((unsigned int volatile *) 0xF0004620))	/* Message Object 1 Function Control Register  */

#define CAN_MOCTR0	(*((unsigned int volatile *) 0xF000461C))	/* Message Object 0 Control Register  */
#define CAN_MOCTR1	(*((unsigned int volatile *) 0xF000463C))	/* Message Object 1 Control Register  */

#define CAN_MODATAH0 (*((unsigned int volatile *) 0xF0004614))	/* Message Object 0 Data Register High  */
#define CAN_MODATAH1 (*((unsigned int volatile *) 0xF0004634))	/* Message Object 1 Data Register High  */


#define CAN_MODATAL0_BB (*((unsigned int volatile *) 0xF0004410))	/* Message Object 0 Data Register Low  */
#define CAN_MODATAL1_BB (*((unsigned int volatile *) 0xF0004430))	/* Message Object 1 Data Register Low  */

#define CAN_MOFCR0_BB   (*((unsigned int volatile *) 0xF0004400))	/* Message Object 0 Function Control Register  */
#define CAN_MOFCR1_BB   (*((unsigned int volatile *) 0xF0004420))	/* Message Object 1 Function Control Register  */

#define CAN_MOCTR0_BB	(*((unsigned int volatile *) 0xF000441C))	/* Message Object 0 Control Register  */
#define CAN_MOCTR1_BB	(*((unsigned int volatile *) 0xF000443C))	/* Message Object 1 Control Register  */

#define CAN_MODATAH0_BB (*((unsigned int volatile *) 0xF0004414))	/* Message Object 0 Data Register High  */
#define CAN_MODATAH1_BB (*((unsigned int volatile *) 0xF0004434))	/* Message Object 1 Data Register High  */


/*
 * Value for accessing the Core Registers using MTCR(...) and MFCR(...)
 */

#define ICR  0xFE2C                             /* Interrupt Control Register */






#endif /* REG1766_H */
