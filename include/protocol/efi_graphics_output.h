/*
 * EFI graphics output protocol
 */

#ifndef EFI_GRAPHICS_OUTPUT_H
#define EFI_GRAPHICS_OUTPUT_H

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
  { 0x9042a9de, 0x23dc, 0x4a38, 0x96, 0xfb, 0x7a, 0xde, \
  	0xd0, 0x80, 0x51, 0x6a }

typedef enum {
	pixel_red_green_blue_reserved_8_bit_per_color,
	pixel_blue_green_red_reserved_8_bit_per_color,
	pixel_bit_mask,
	pixel_blt_only,
	pixel_format_max,
} efi_graphics_pixel_format;

typedef struct {
	efi_u32	red_mask;
	efi_u32	green_mask;
	efi_u32	blue_mask;
	efi_u32	reserved_mask;
} efi_pixel_bitmask;

typedef struct {
	efi_u32				version;
	efi_u32				horizontal_resolution;
	efi_u32				vertical_resolution;
	efi_graphics_pixel_format	pixel_format;
	efi_pixel_bitmask		pixel_information;
	efi_u32				pixels_per_scan_line;
} efi_graphics_output_mode_information;

typedef struct {
	efi_u8	blue;
	efi_u8	green;
	efi_u8	red;
	efi_u8	reserved;
} efi_graphics_output_blt_pixel;

typedef enum {
	efi_blt_video_fill,
	efi_blt_video_to_blt_buffer,
	efi_blt_buffer_to_video,
	efi_blt_video_to_video,
	efi_graphic_ouput_blt_operation_max,
} efi_graphics_output_blt_operation;

typedef struct {
	efi_u32					max_mode;
	efi_u32					mode;
	efi_graphics_output_mode_information	*info;
	efi_size				size_of_info;
	efi_physical_address			frame_buffer_base;
	efi_size				frame_buffer_size;
} efi_graphics_output_protocol_mode;

typedef struct efi_graphics_output_protocol efi_graphics_output_protocol;
struct efi_graphics_output_protocol {
	efi_status (efiapi *query_mode)(
		efi_graphics_output_protocol *self,
		efi_u32 mode_number,
		efi_size *size_of_info,
		efi_graphics_output_mode_information **info);
	efi_status (efiapi *set_mode)(
		efi_graphics_output_protocol *self,
		efi_u32 mode_number);
	efi_status (efiapi *blt)(
		efi_graphics_output_protocol *self,
		efi_graphics_output_blt_pixel *blt_buffer,
		efi_graphics_output_blt_operation blt_operation,
		efi_size source_x,
		efi_size source_y,
		efi_size destination_x,
		efi_size desitnation_y,
		efi_size width,
		efi_size height,
		efi_size delta);
	efi_graphics_output_protocol_mode	*mode;
};

#endif
