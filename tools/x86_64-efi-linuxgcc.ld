OUTPUT_FORMAT(pei-x86-64)
SECTIONS
{
	. = SIZEOF_HEADERS;
	. = ALIGN(__section_alignment__);
	.text __image_base__ + ( __section_alignment__ < 0x1000 ? . : __section_alignment__ ) :
	{
		*(.text)
	}
	.data BLOCK(__section_alignment__) :
	{
		*(.data)
	}
	.rdata BLOCK(__section_alignment__) :
	{
		/* linuxgcc calls it rodata for some reason */
		*(.rodata)
	}
	.bss BLOCK(__section_alignment__) :
	{
		*(.bss)
		*(COMMON)
	}
	.reloc BLOCK(__section_alignment__) :
	{
		*(.reloc)
	}
	/DISCARD/ :
	{
		*(*)
	}
}
