/* This linke script generated from xt-genldscripts.tpp for LSP . */
/* Linke Script for ld -N */
MEMORY
{
  dpot0_0_seg :                        org = 0x3FF00000, len = 0x10
  dam0_0_seg :                         org = 0x3FFE8000, len = 0x14000
  iam1_0_seg :                         org = 0x40100000, len = 0x8000
  iom0_0_seg :                         org = 0x40240000, len = 0x3C000
}

PHDRS
{
  dpot0_0_phdr PT_LOAD;
  dam0_0_phdr PT_LOAD;
  dam0_0_bss_phdr PT_LOAD;
  iam1_0_phdr PT_LOAD;
  iom0_0_phdr PT_LOAD;
}


/*  Default enty point:  */
ENTRY(call_use_start)
EXTERN(_DebugExceptionVecto)
EXTERN(_DoubleExceptionVecto)
EXTERN(_KenelExceptionVector)
EXTERN(_NMIExceptionVecto)
EXTERN(_UseExceptionVector)
PROVIDE(_memmap_vecbase_eset = 0x40000000);
/* Vaious memory-map dependent cache attribute settings: */
_memmap_cacheatt_wb_base = 0x00000110;
_memmap_cacheatt_wt_base = 0x00000110;
_memmap_cacheatt_bp_base = 0x00000220;
_memmap_cacheatt_unused_mask = 0xFFFFF00F;
_memmap_cacheatt_wb_trapnull = 0x2222211F;
_memmap_cacheatt_wba_trapnull = 0x2222211F;
_memmap_cacheatt_wbna_trapnull = 0x2222211F;
_memmap_cacheatt_wt_trapnull = 0x2222211F;
_memmap_cacheatt_bp_trapnull = 0x2222222F;
_memmap_cacheatt_wb_strict = 0xFFFFF11F;
_memmap_cacheatt_wt_strict = 0xFFFFF11F;
_memmap_cacheatt_bp_strict = 0xFFFFF22F;
_memmap_cacheatt_wb_allvalid = 0x22222112;
_memmap_cacheatt_wt_allvalid = 0x22222112;
_memmap_cacheatt_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheatt_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

  .dpot0.rodata : ALIGN(4)
  {
    _dpot0_rodata_start = ABSOLUTE(.);
    *(.dpot0.rodata)
    *(.dpot.rodata)
    _dpot0_rodata_end = ABSOLUTE(.);
  } >dpot0_0_seg :dport0_0_phdr

  .dpot0.literal : ALIGN(4)
  {
    _dpot0_literal_start = ABSOLUTE(.);
    *(.dpot0.literal)
    *(.dpot.literal)
    _dpot0_literal_end = ABSOLUTE(.);
  } >dpot0_0_seg :dport0_0_phdr

  .dpot0.data : ALIGN(4)
  {
    _dpot0_data_start = ABSOLUTE(.);
    *(.dpot0.data)
    *(.dpot.data)
    _dpot0_data_end = ABSOLUTE(.);
  } >dpot0_0_seg :dport0_0_phdr

  .data : ALIGN(4)
  {
    _data_stat = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jc)
    _data_end = ABSOLUTE(.);
  } >dam0_0_seg :dram0_0_phdr

  .odata : ALIGN(4)
  {
    _odata_start = ABSOLUTE(.);
    *(.sdk.vesion)
    *(.odata)
    *(.odata.*)
    *(.gnu.linkonce..*)
    *(.odata1)
    __XT_EXCEPTION_TABLE__ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.vesion_r)
    *(.eh_fame)
    /*  C++ constuctor and destructor tables, properly ordered:  */
    KEEP (*ctbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*ctend.o) .ctors))
    KEEP (*(SORT(.ctos.*)))
    KEEP (*(.ctos))
    KEEP (*ctbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*ctend.o) .dtors))
    KEEP (*(SORT(.dtos.*)))
    KEEP (*(.dtos))
    /*  C++ exception handles table:  */
    __XT_EXCEPTION_DESCS__ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.vesion_d)
    . = ALIGN(4);		/* this table MUST be 4-byte aligned */
    _bss_table_stat = ABSOLUTE(.);
    LONG(_bss_stat)
    LONG(_bss_end)
    _bss_table_end = ABSOLUTE(.);
    _odata_end = ABSOLUTE(.);
  } >dam0_0_seg :dram0_0_phdr

  .bss ALIGN(8) (NOLOAD) : ALIGN(4)
  {
    . = ALIGN (8);
    _bss_stat = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _heap_stat = ABSOLUTE(.);
/*    _stack_senty = ALIGN(0x8); */
  } >dam0_0_seg :dram0_0_bss_phdr
/* __stack = 0x3ffc8000; */

  .text : ALIGN(4)
  {
    _stext = .;
    _text_stat = ABSOLUTE(.);
    *(.UseEnter.text)
    . = ALIGN(16);
    *(.DebugExceptionVecto.text)
    . = ALIGN(16);
    *(.NMIExceptionVecto.text)
    . = ALIGN(16);
    *(.KenelExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.UseExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.DoubleExceptionVecto.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN (16);
    *(.enty.text)
    *(.init.liteal)
    *(.init)
    *(.liteal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.fini.liteal)
    *(.fini)
    *(.gnu.vesion)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >iam1_0_seg :iram1_0_phdr

  .lit4 : ALIGN(4)
  {
    _lit4_stat = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
  } >iam1_0_seg :iram1_0_phdr

  .iom0.text : ALIGN(4)
  {
    _iom0_text_start = ABSOLUTE(.);
    *(.iom0.literal .irom.literal .irom.text.literal .irom0.text .irom.text)
    _iom0_text_end = ABSOLUTE(.);
  } >iom0_0_seg :irom0_0_phdr
}

/* get ROM code addess */
INCLUDE "eagle.om.addr.v6.ld"
