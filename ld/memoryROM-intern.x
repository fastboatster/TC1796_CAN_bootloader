/*
 * memoryInternalROM.x -- phyCORE-TC1796 internal flash memory configuration
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2020
 *
 */
/* __TC1796__ with Core TC1.3 */
__TRICORE_DERIVATE_MEMORY_MAP__ = 0x1796;
/* the external RAM description */
__EXT_CODE_RAM_BEGIN = 0x80000000;
__EXT_CODE_RAM_SIZE = 2M;
__EXT_DATA_RAM_BEGIN = 0xa1000000;
__EXT_DATA_RAM_SIZE = 2M;
__RAM_END = __EXT_DATA_RAM_BEGIN + __EXT_DATA_RAM_SIZE;
/* the internal ram description */
__INT_CODE_RAM_BEGIN = 0xd4000000;
__INT_CODE_RAM_SIZE = 48K;
__INT_DATA_RAM_BEGIN = 0xd0000000;
__INT_DATA_RAM_SIZE = 56K;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN = 0xf0060000;
__PCP_CODE_RAM_SIZE = 32K;
__PCP_DATA_RAM_BEGIN = 0xf0050000;
__PCP_DATA_RAM_SIZE = 16K;

/* External Bus Memory Configuration word */
__EBMCFG = 0x0000803D;

MEMORY
{
  ext_cram (rx!p): org = 0x80000000, len = 2M
  ext_dram (w!xp): org = 0xa1000000, len = 2M
  int_cram (rx!p): org = 0xd4000000, len = 48K
  int_dram (w!xp): org = 0xd0000000, len = 56K
  pcp_data (wp!x): org = 0xf0050000, len = 16K
  pcp_text (rxp):  org = 0xf0060000, len = 32K
}

/* the symbol __TRICORE_DERIVATE_NAME__ will be defined in the crt0.S and is
 * tested here to confirm that this memory map and the startup file will
 * fit together
*/
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), "Using wrong Memory Map. This Map is for TC1796");
