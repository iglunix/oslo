SECTIONS
{
	. = 0x7c00;
	.text : { *(.text) }
	/DISCARD/ : { *(.note.gnu.property) }
}
