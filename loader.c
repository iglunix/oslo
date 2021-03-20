#include <stdint.h>
#include <stddef.h>

// /*#pragma pack(push)
// #pragma pack(1)

// /*
//  * FAT BIOS Paramater Block
//  */

// typedef struct fs_fat_bpb_t {
// 	uint8_t magic[3];
// 	uint8_t oem_id[8];
// 	uint16_t bytes_per_sector;
// 	uint8_t sectors_per_cluster;
// 	uint16_t reserved_sector_count;
// 	uint8_t fat_count;
// 	uint16_t dir_count;
// 	uint16_t sectors_per_volume;
// 	uint8_t media_descriptor_type;
// 	uint16_t sectors_per_fat;
// 	uint16_t sectors_per_track;
// 	uint16_t head_count;
// 	uint32_t hidden_count;
// 	uint32_t large_sector_count;
// } fs_fat_bpb_t;

// /*
//  * FAT Extended Boot Record
//  */

// typedef union fs_fat_ebr_t {
// 	struct {
// 		uint8_t drive_number;
// 		uint8_t windows_nt_flags;
// 		uint8_t signature;
// 		uint32_t volume_id;
// 		uint8_t volume_label_string[11];
// 		uint8_t system_id_string[8];
// 		uint8_t boot_code[448];
// 		uint16_t bootable_partition_signature;
// 	} fat16;
// 	struct {
// 		uint32_t sectors_per_fat;
// 		uint16_t flags;
// 		struct {
// 			uint8_t major;
// 			uint8_t minor;
// 		} version;
// 		uint32_t root_cluster_number;
// 		uint16_t fsinfo_sector;
// 		uint16_t backup_boot_sector_number;
// 		uint8_t reserved[12];
// 		uint8_t drive_number;
// 		uint8_t windows_nt_flags;
// 		uint8_t signature;
// 		uint32_t volume_id;
// 		uint8_t volume_label_string[11];
// 		uint8_t system_id_string[8];
// 		uint8_t boot_code[420];
// 		uint16_t bootablle_partition_signature;
// 	} fat32;
// } fs_fat_ebr_t;

// typedef union fs_fat32_fsinfo {
// 	uint32_t lead_signature;
// 	uint8_t reserved[480];
// 	uint32_t signature;
// 	uint32_t last_know_cluster_count;
// 	uint32_t cluster_search_hint;
// 	uint8_t more_reserved[12];
// 	uint32_t trail_signature;
// } fs_fat32_fsinfo;

// typedef union fs_fat_t {
// 	struct {
// 		fs_fat_bpb_t bpb;
// 		fs_fat_ebr_t ebr;
// 	} header;
// 	/* Padding to make single sector */
// 	uint8_t data[512];
// } fs_fat_t;

// #pragma pack(pop)

// int puts(char const *str);
// void puthex(unsigned char c);
// int putchar(int c);

// void disk_err() {
// 	puts("Disk Error");
// //	asm("hlt");
// }

// void fs_fat_load(fs_fat_t *dest, uint8_t cylinder, uint8_t sector, uint8_t head, uint8_t drive) {
// 	asm(
// 		"pushw %%dx;"
// 		"movb $0x02,%%ah;"
// 		"movb $0x01, %%al;"
// 		"movb %0, %%ch;"
// 		"movb %1, %%cl;"
// 		"movb %2, %%dh;"
// 		"movb %3, %%dl;"
// 		"movw %4, %%bx;"
// 		"movw %5, %%es;"
// 		"int $0x13;"
// 		"jc disk_err;"
// 		"popw %%dx;"
// 		"cmpb %%al, %%dh;"
// 		"jne disk_err;"
// 		:
// 		: "r" (cylinder)
// 		, "r" (sector)
// 		, "r" (head)
// 		, "r" (drive)
// 		, "r" ((uint16_t) (((uint64_t) dest) >> 16))
// 		, "r" ((uint16_t) ((uint64_t) dest))
// 	);
// }*/


size_t VGA_WIDTH = 80;
size_t VGA_HEIGHT = 25;
uint16_t *vga_buf = (uint16_t *) 0xb8000;

typedef struct vga_t {
	size_t line;
	size_t col;
	uint16_t *buf;
} vga_t;

vga_t vga_init() {
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		for (size_t j = 0; j < VGA_HEIGHT; j++) {
			vga_buf[j * VGA_WIDTH + i] = 0;
		}
	}
	vga_t ret = {
		.line = 0,
		.col = 0,
		.buf = (uint16_t *)0xb8000,
	};
	return ret;
}

void vga_putc(vga_t *self, char c) {
	if (c == '\n') {
		self->line++;
		self->col = 0;
	} else {
		self->buf[self->col + self->line * VGA_WIDTH] = 0x0f00 | c;
		self->col++;
	}
}

void vga_puts(vga_t *self, char const *s) {
	for (;*s;s++) {
		vga_putc(self, *s);
	}
}

/*int puts(char const *str) {
	for (int i = 0; str[i]; i++) {
		char c = str[i];
		putchar(c);
	}
	putchar('\r');
	putchar('\n');
	return 0;
}

void putnibble(unsigned char c) {
	char map[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	putchar(map[c]);
}

void puthex(unsigned char c) {
	putnibble(c >> 4);
	putnibble((c & 0x0f));
}*/



void main() {
	vga_t vga = vga_init();
	vga_puts(&vga, "Welcome to OSLO!\nLoking for vmlinuz");
//	fs_fat_t fat;
//	fs_fat_load(&fat, 0, 1, 0, 0x80);
}

extern void _start() {
	main();
}
