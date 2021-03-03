typedef  unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#pragma pack(push)
#pragma pack(1)

/*
 * FAT BIOS Paramater Block
 */

typedef struct fs_fat_bpb_t {
	uint8_t magic[3];
	uint8_t oem_id[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t fat_count;
	uint16_t dir_count;
	uint16_t sectors_per_volume;
	uint8_t media_descriptor_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t head_count;
	uint32_t hidden_count;
	uint32_t large_sector_count;
} fs_fat_bpb_t;

/*
 * FAT Extended Boot Record
 */

typedef union fs_fat_ebr_t {
	struct {
		uint8_t drive_number;
		uint8_t windows_nt_flags;
		uint8_t signature;
		uint32_t volume_id;
		uint8_t volume_label_string[11];
		uint8_t system_id_string[8];
		uint8_t boot_code[448];
		uint16_t bootable_partition_signature;
	} fat16;
	struct {
		uint32_t sectors_per_fat;
		uint16_t flags;
		struct {
			uint8_t major;
			uint8_t minor;
		} version;
		uint32_t root_cluster_number;
		uint16_t fsinfo_sector;
		uint16_t backup_boot_sector_number;
		uint8_t reserved[12];
		uint8_t drive_number;
		uint8_t windows_nt_flags;
		uint8_t signature;
		uint32_t volume_id;
		uint8_t volume_label_string[11];
		uint8_t system_id_string[8];
		uint8_t boot_code[420];
		uint16_t bootablle_partition_signature;
	} fat32;
} fs_fat_ebr_t;

typedef union fs_fat32_fsinfo {
	uint32_t lead_signature;
	uint8_t reserved[480];
	uint32_t signature;
	uint32_t last_know_cluster_count;
	uint32_t cluster_search_hint;
	uint8_t more_reserved[12];
	uint32_t trail_signature;
} fs_fat32_fsinfo;


/* 
 * This probably isn't going to work as is. I probably should be reading
 * the fs information seperately in a single pass; data should be sized
 * to make `fs_fat_t` a single sector making `fs_fat_load` only need 1
 * in AL
 */

typedef struct fs_fat_t {
	fs_fat_bpb_t bpb;
	fs_fat_ebr_t ebr;
	uint8_t data[1]; /* variable length */
} fs_fat_t;

#pragma pack(pop)

int puts(char const *str);

void disk_err() {
	puts("Disk Error");
	asm("hlt");
}

void fs_fat_load(fs_fat_t *dest, uint8_t count, uint8_t cylinder, uint8_t sector, uint8_t head, uint8_t drive) {
	asm(
		"pushw %%dx;"
		"movb $0x02,%%ah;"
		"movb %0, %%al;"
		"movb %1, %%ch;"
		"movb %2, %%cl;"
		"movb %3, %%dh;"
		"movb %4, %%dl;"
		"movw %5, %%bx;"
		"movw %6, %%es;"
		"int $0x13;"
		"jc disk_err;"
		"popw %%dx;"
		"cmpb %%al, %%dh;"
		"jne disk_err;"
		:
		: "r" (count)
		, "r" (cylinder)
		, "r" (sector)
		, "r" (head)
		, "r" (drive)
		, "r" ((uint16_t) (((int) dest) >> 16))
		, "r" ((uint16_t) dest)
	);
}

int putchar(int c) {
	asm(
		"mov %0, %%al;"
		"mov $0x0E, %%ah;"
		"int $0x10;"
		:
		: "r" ((char) c)
	);
	return 0;
}

int puts(char const *str) {
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
}

#include <alloca.h>

extern void loader_main() {
	puts("OSLO Stage 2 Loaded!");
	for (char *i = (char *) 0x7c00; i < (char*) (0x7c00 + 512); i++) {
		puthex(*i);
		putchar(' ');
		if (((unsigned long) i) % 24 == 0) {
			putchar('\r');
			putchar('\n');
		}
	}
	fs_fat_t *fat = alloca(sizeof(uint8_t) * 1024 * 1024);
	fs_fat_load(fat, 0x1,  0, 1, 0, 0x80);
	puts("read");
}
