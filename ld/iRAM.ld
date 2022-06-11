
/*
 * Name: iRAM.ld 
 * 
 * Generated Linker Description File
 * Copyright (C) 2010 HighTec EDV-Systeme GmbH. 
 * (!Do not edit outsite of the protection areas!)
 *
 * Description: 
 * internal RAM configuration
 */

/*
 * Define Entrypoint of Executable
 */
ENTRY(_start)

/*
 * Global
 */
/*Data Memory*/ 
__SRAM_BEGIN = 0xC0000000;
__SRAM_SIZE = 64K;
/*Stand-by Data Memory*/ 
__SBRAM_BEGIN = 0xC03FC000;
__SBRAM_SIZE = 16K;
/*Program Flash Memory (PFLASH)*/ 
__PMU_PFLASH_BEGIN = 0x80000000;
__PMU_PFLASH_SIZE = 2M;
/*Data Flash Memory (DFLASH)*/ 
__PMU_DFLASH_BEGIN = 0xAFE00000;
__PMU_DFLASH_SIZE = 128K;
/*Boot ROM (BROM)*/ 
__BROM_BEGIN = 0xAFFFC000;
__BROM_SIZE = 8K;
/*Test ROM (TROM)*/ 
__TROM_BEGIN = 0xAFFFE000;
__TROM_SIZE = 8K;
/*Scratch-Pad RAM (SPRAM)*/ 
__PMI_SPRAM_BEGIN = 0xD4000000;
__PMI_SPRAM_SIZE = 48K;
/*Local Data RAM (LDRAM)*/ 
__DMI_LDRAM_BEGIN = 0xD0000000;
__DMI_LDRAM_SIZE = 56K;
/*Dual-Port RAM (DPRAM)*/ 
__DMI_DPRAM_BEGIN = 0xD000E000;
__DMI_DPRAM_SIZE = 8K;
/*PCP Code Memory (CMEM)*/ 
__PCP_CMEM_BEGIN = 0xF0060000;
__PCP_CMEM_SIZE = 32K;
/*PCP Data Memory (PRAM)*/ 
__PCP_PRAM_BEGIN = 0xF0050000;
__PCP_PRAM_SIZE = 16K;
/*External Code Memory*/ 
__EXT_FLASH_BEGIN = 0x84000000;
__EXT_FLASH_SIZE = 4M;
/*External Data Memory*/ 
__EXT_RAM_BEGIN = 0xa1000000;
__EXT_RAM_SIZE = 1M;


__USTACK_SIZE = DEFINED (__USTACK_SIZE) ? __USTACK_SIZE : 64K;  /* Section for ustack*/
__ISTACK_SIZE = DEFINED (__ISTACK_SIZE) ? __ISTACK_SIZE : 256;  /* Section for istack*/ 
__HEAP_SIZE = DEFINED (__HEAP_SIZE) ? __HEAP_SIZE : 4K;  /* Section for heap*/ 
__CSA_SIZE = DEFINED (__CSA_SIZE) ? __CSA_SIZE : 16K;  /* Section for CSA*/ 

/**
 *	User defined global region
 */
/*PROTECTED REGION ID(Protection:iRAM-Global) ENABLED START*/
/*Protection-Area for your own LDF-Code*/
/*PROTECTED REGION END*/

/*
 * internal RAM configuration
 */
MEMORY
{
	SRAM (w!xp):	org = 0xC0000000, len = 64K  /*Data Memory*/ 
	SBRAM (w!xp):	org = 0xC03FC000, len = 16K  /*Stand-by Data Memory*/ 
	PMU_PFLASH (rx!p):	org = 0x80000000, len = 2M  /*Program Flash Memory (PFLASH)*/ 
	PMU_DFLASH (r!xp):	org = 0xAFE00000, len = 128K  /*Data Flash Memory (DFLASH)*/ 
	BROM (rx!p):	org = 0xAFFFC000, len = 8K  /*Boot ROM (BROM)*/ 
	TROM (rx!p):	org = 0xAFFFE000, len = 8K  /*Test ROM (TROM)*/ 
	PMI_SPRAM (wx!p):	org = 0xD4000000, len = 48K  /*Scratch-Pad RAM (SPRAM)*/ 
	DMI_LDRAM (w!xp):	org = 0xD0000000, len = 56K  /*Local Data RAM (LDRAM)*/ 
	DMI_DPRAM (w!xp):	org = 0xD000E000, len = 8K  /*Dual-Port RAM (DPRAM)*/ 
	PCP_CMEM (rxp):	org = 0xF0060000, len = 32K  /*PCP Code Memory (CMEM)*/ 
	PCP_PRAM (wp!x):	org = 0xF0050000, len = 16K  /*PCP Data Memory (PRAM)*/ 
	EXT_FLASH (rx):	org = 0x84000000, len = 4M  /*External Code Memory*/ 
	EXT_RAM (w!x):	org = 0xa1000000, len = 1M  /*External Data Memory*/ 
	
}			

SECTIONS
{
	/*Code-Sections*/
	
	/*
	 * Startup code for TriCore
	 */
	.startup_code  :
	{
		PROVIDE(__startup_code_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .startup_code.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.startup_code) /*Startup code for TriCore*/ 
		*(.startup_code*)
		
		/*PROTECTED REGION ID(Protection: iRAM .startup_code) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__startup_code_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Code section
	 */
	.text  :
	{
		PROVIDE(__text_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .text.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.text) /*Code section*/ 
		*(.text*)
		*(.gnu.linkonce.t.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .text) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__text_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Code executed before calling main
	 */
	.init  :
	{
		PROVIDE(__init_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .init.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		KEEP(*(.init)) /*Code executed before calling main*/ 
		KEEP(*(.init*))
		
		/*PROTECTED REGION ID(Protection: iRAM .init) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__init_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Code executed before exiting program
	 */
	.fini  :
	{
		PROVIDE(__fini_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .fini.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		KEEP(*(.fini)) /*Code executed before exiting program*/ 
		KEEP(*(.fini*))
		
		/*PROTECTED REGION ID(Protection: iRAM .fini) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__fini_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Section for trap table
	 */
	.traptab  :
	{
		PROVIDE(__traptab_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .traptab.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.traptab) /*Section for trap table*/ 
		*(.traptab*)
		
		/*PROTECTED REGION ID(Protection: iRAM .traptab) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__traptab_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Section for interrupt table
	 */
	.inttab  :
	{
		PROVIDE(__inttab_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .inttab.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.inttab) /*Section for interrupt table*/ 
		*(.inttab*)
		
		/*PROTECTED REGION ID(Protection: iRAM .inttab) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__inttab_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Exception handling frame for C++ exceptions
	 */
	.eh_frame  :
	{
		PROVIDE(__eh_frame_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .eh_frame.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.eh_frame) /*Exception handling frame for C++ exceptions*/ 
		*(.eh_frame*)
		
		/*PROTECTED REGION ID(Protection: iRAM .eh_frame) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__eh_frame_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	



	/*Absolute Data-Sections*/
	
	/*
	 * Initialised data addressed as absolute
	 */
	.zdata  :
	{
		PROVIDE(__zdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .zdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.zdata) /*Initialised data addressed as absolute*/ 
		*(.zdata*)
		*(.zdata.rodata) /*.zdata.rodata*/ 
		*(.zdata.rodata*)
		*(.zrodata) /*.zrodata*/ 
		*(.zrodata*)
		*(.gnu.linkonce.z.*)
		*(.gnu.linkonce.zr.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .zdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__zdata_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM AT > PMI_SPRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Not initialised data addressed as absolute
	 */
	.zbss (NOLOAD) :
	{
		PROVIDE(__zbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .zbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.zbss) /*Not Initialised data addressed as absolute*/ 
		*(.zbss*)
		*(.gnu.linkonce.zb.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .zbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__zbss_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ 
	
	/*
	 * Not initialised bit data
	 */
	.bbss (NOLOAD) :
	{
		PROVIDE(__bbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .bbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bbss) /*Not initialised bit data*/ 
		*(.bbss*)
		
		/*PROTECTED REGION ID(Protection: iRAM .bbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__bbss_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ 
	
	/*
	 * Bit variables
	 */
	.bdata  :
	{
		PROVIDE(__bdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .bdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bdata) /*Bit variables*/ 
		*(.bdata*)
		
		/*PROTECTED REGION ID(Protection: iRAM .bdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/

		PROVIDE(__bdata_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM AT > PMI_SPRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	


	/*Small Data-Sections*/
	
	/*
	 * Storage of write-protected data addressed as small
	 */
	.sdata2  :
	{
		PROVIDE(__sdata2_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .sdata2.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sdata.rodata) /*Storage of write-protected data addressed as small*/ 
		*(.sdata.rodata*)
		*(.gnu.linkonce.sr.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .sdata2) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sdata2_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Section stores initialised data which is addressable by small data area pointer (%a0)
	 */
	.sdata  :
	{
		PROVIDE(__sdata_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .sdata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sdata) /*Section stores initialised data which is addressable by small data area pointer (%a0)*/ 
		*(.sdata*)
		*(.gnu.linkonce.s.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .sdata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sdata_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM AT > PMI_SPRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)
	 */
	.sbss (NOLOAD) :
	{
		PROVIDE(__sbss_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .sbss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.sbss) /*Not initialised data in section ’.sbss’, addressable by small data area pointer (%a0)*/ 
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .sbss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__sbss_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ 
	


	/*Normal Data-Sections*/
	
	/*
	 * Storage of write-protected data
	 */
	.rodata  :
	{
		PROVIDE(__rodata_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .rodata.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.rodata) /*Storage of write-protected data*/ 
		*(.rodata*)
		*(.gnu.linkonce.r.*)
		*(.jcr.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .rodata) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__rodata_end = .);
		. = ALIGN(8);
		
	} > PMI_SPRAM /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Initialised data
	 */
	.data  :
	{
		PROVIDE(__data_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .data.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.data) /*Initialised data*/ 
		*(.data*)
		*(.gnu.linkonce.d.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .data) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__data_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM AT > PMI_SPRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ /* PMI_SPRAM: Scratch-Pad RAM (SPRAM) */ 
	
	/*
	 * Not Initialised data
	 */
	.bss (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		
		/*PROTECTED REGION ID(Protection: iRAM .bss.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		*(.bss) /*Not Initialised data*/ 
		*(.bss*)
		*(.gnu.linkonce.b.*)
		
		/*PROTECTED REGION ID(Protection: iRAM .bss) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		
		PROVIDE(__bss_end = .);
		. = ALIGN(8);
		
	} > DMI_LDRAM /* DMI_LDRAM: Local Data RAM (LDRAM) */ 
	
	

	 
	 
	/*
	 * Section for constructors
	 */
	.ctors          :
	{
		__CTOR_LIST__ = . ;

		/*PROTECTED REGION ID(Protection: iRAMconstructor.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		LONG(0) ;
		/*PROTECTED REGION ID(Protection: iRAMconstructor) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		__CTOR_END__ = . ;
		 . = ALIGN(8);
	}  > PMI_SPRAM


	/*
	 * Section for destructors
	 */
	.dtors          :
	{
		__DTOR_LIST__ = . ;
		/*PROTECTED REGION ID(Protection: iRAM destructor.begin) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		LONG(0) ;
		/*PROTECTED REGION ID(Protection: iRAM destructor) ENABLED START*/
			/*Protection-Area for your own LDF-Code*/
		/*PROTECTED REGION END*/
		__DTOR_END__ = . ;
		. = ALIGN(8);
	} > PMI_SPRAM
	
	
	/*
	 * Section for clear table
	 */
	.clear_sec :
	{
		. = ALIGN(8);
		PROVIDE(__clear_table = .) ;
			LONG(0 + ADDR(.bss));	LONG(SIZEOF(.bss));
			LONG(0 + ADDR(.sbss));	LONG(SIZEOF(.sbss));
			LONG(0 + ADDR(.zbss));	LONG(SIZEOF(.zbss));
			LONG(0 + ADDR(.bbss));	LONG(SIZEOF(.bbss));
			/*PROTECTED REGION ID(Protection: iRAM clear section) ENABLED START*/
				/*Protection-Area for your own LDF-Code*/
			/*PROTECTED REGION END*/
			LONG(-1);                 LONG(-1);
	} > PMI_SPRAM
	 
	/*
	* Section for copy table
	*/	
	.copy_sec :
	{
		. = ALIGN(8);
		PROVIDE(__copy_table = .) ;
			LONG(LOADADDR(.data));	LONG(0 + ADDR(.data));	LONG(SIZEOF(.data));
			LONG(LOADADDR(.sdata));	LONG(0 + ADDR(.sdata));	LONG(SIZEOF(.sdata));
			LONG(LOADADDR(.zdata));	LONG(0 + ADDR(.zdata));	LONG(SIZEOF(.zdata));
			LONG(LOADADDR(.bdata));	LONG(0 + ADDR(.bdata));	LONG(SIZEOF(.bdata));
			/*PROTECTED REGION ID(Protection: iRAM copy section) ENABLED START*/
				/*Protection-Area for your own LDF-Code*/
			/*PROTECTED REGION END*/
			LONG(-1);                 LONG(-1);                  LONG(-1);
	} > PMI_SPRAM 
	 
	
	/*
	 * Section for ustack
	 */	
	.ustack :
	{
		. = ALIGN(8);
		 __USTACK_BEGIN = . ;
		. += __USTACK_SIZE ;
		. = ALIGN(8);
		 __USTACK = . ;
		 __USTACK_END = . ;
	} > SRAM /* was set to DMI_LDRAM before */
	
	/*
	 * Section for istack
	 */	
	.istack :
	{
		. = ALIGN(8);
		 __ISTACK_BEGIN = . ;
		. += __ISTACK_SIZE ;
		. = ALIGN(8);
		 __ISTACK = . ;
		 __ISTACK_END = . ;
	} > DMI_LDRAM
	
	/*
	 * Section for heap
	 */	
	.heap :
	{
		. = ALIGN(8);
		 __HEAP_BEGIN = . ;
		 __HEAP = . ;
		. += __HEAP_SIZE ;
		. = ALIGN(8);
		 __HEAP_END = . ;
	} > DMI_LDRAM
	
	/*
	 * Section for CSA
	 */	
	.csa :
	{
		. = ALIGN(64);
		 __CSA_BEGIN = . ;
		 __CSA = . ;
		. += __CSA_SIZE ;
		. = ALIGN(64);
		 __CSA_END = . ;
	} > DMI_LDRAM
	
	
	
	
	/*PROTECTED REGION ID(Protection:iRAM-User-Sections) ENABLED START*/
		/*Protection-Area for your own LDF-Code*/
	/*PROTECTED REGION END*/
	
}

