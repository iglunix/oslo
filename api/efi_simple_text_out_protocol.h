#ifndef __EFI_SIMPLE_TEXT_OUT_PROTOCOL_H
#define __EFI_SIMPLE_TEXT_OUT_PROTOCOL_H

typedef struct {
int32_t max_mode;
int32_t mode;
int32_t attr;
int32_t cursor_col;
int32_t cursor_row;
efi_bool_t cursor_visible;
} efi_simple_text_out_mode;

typedef struct _efi_simple_text_out_protocol efi_simple_text_out_protocol_t;
struct _efi_simple_text_out_protocol {
efi_status_t (efi_func *reset)(efi_simple_text_out_protocol_t* self, efi_bool_t ext_verf);
efi_status_t (efi_func *output_string)(efi_simple_text_out_protocol_t* self, efi_char16_t* str);
efi_status_t (efi_func *test_string)(efi_simple_text_out_protocol_t* self, efi_char16_t* str);
efi_status_t (efi_func *query_mode)(efi_simple_text_out_protocol_t* self, uintn_t mode_num, uintn_t* cols, uintn_t* rows);
efi_status_t (efi_func *set_mode)(efi_simple_text_out_protocol_t* self, uintn_t mode_num);
efi_status_t (efi_func *set_attr)(efi_simple_text_out_protocol_t* self, uintn_t attr);
efi_status_t (efi_func *clear_screen)(efi_simple_text_out_protocol_t* self);
efi_status_t (efi_func *set_cursor_pos)(efi_simple_text_out_protocol_t* self, uintn_t col, uintn_t row);
efi_status_t (efi_func *enable_cursor)(efi_simple_text_out_protocol_t* self, efi_bool_t visible);
efi_simple_text_out_mode *mode;
};

// Boxdraw values
#define boxdraw_horizontal 0x2500
#define boxdraw_vertical 0x2502
#define boxdraw_down_right 0x250c
#define boxdraw_down_left 0x2510
#define boxdraw_up_right 0x2514
#define boxdraw_up_left 0x2518
#define boxdraw_vertical_right 0x251c
#define boxdraw_vertical_left 0x2524
#define boxdraw_down_horizontal 0x252c
#define boxdraw_up_horizontal 0x2534
#define boxdraw_vertical_horizontal 0x253c
#define boxdraw_double_horizontal 0x2550
#define boxdraw_double_vertical 0x2551
#define boxdraw_down_right_double 0x2552
#define boxdraw_down_double_right 0x2553
#define boxdraw_double_down_right 0x2554
#define boxdraw_down_left_double 0x2555
#define boxdraw_down_double_left 0x2556
#define boxdraw_double_down_left 0x2557
#define boxdraw_up_right_double 0x2558
#define boxdraw_up_double_right 0x2559
#define boxdraw_double_up_right 0x255a
#define boxdraw_up_left_double 0x255b
#define boxdraw_up_double_left 0x255c
#define boxdraw_double_up_left 0x255d
#define boxdraw_vertical_right_double 0x255e
#define boxdraw_vertical_double_right 0x255f
#define boxdraw_double_vertical_right 0x2560
#define boxdraw_vertical_left_double 0x2561
#define boxdraw_vertical_double_left 0x2562
#define boxdraw_double_vertical_left 0x2563
#define boxdraw_down_horizontal_double 0x2564
#define boxdraw_down_double_horizontal 0x2565
#define boxdraw_double_down_horizontal 0x2566
#define boxdraw_up_horizontal_double 0x2567
#define boxdraw_up_double_horizontal 0x2568
#define boxdraw_double_up_horizontal 0x2569
#define boxdraw_vertical_horizontal_double 0x256a
#define boxdraw_vertical_double_horizontal 0x256b
#define boxdraw_double_vertical_horizontal 0x256c

#define blockelement_full_block 0x2588
#define blockelement_light_shade 0x2591

#define geometricshape_up_triangle 0x25b2
#define geometricshape_right_triangle 0x25ba
#define geometricshape_down_triangle 0x25bc
#define geometricshape_left_triangle 0x25c4

#define arrow_left  0x2190
#define arrow_up    0x2191
#define arrow_right 0x2192
#define arrow_down  0x2193

// EFI text attribs
#define efi_black 0x00
#define efi_blue 0x01
#define efi_green 0x02
#define efi_cyan 0x03
#define efi_red 0x04
#define efi_magenta 0x05
#define efi_brown 0x06
#define efi_lightgray 0x07
#define efi_bright 0x08
#define efi_darkgray 0x08
#define efi_lightblue 0x09
#define efi_lightgreen 0x0a
#define efi_lightcyan 0x0b
#define efi_lightred 0x0c
#define efi_lightmagenta 0x0d
#define efi_yellow 0x0e
#define efi_white 0x0f

#define efi_background_black 0x00
#define efi_background_blue 0x10
#define efi_background_green 0x20
#define efi_background_cyan 0x30
#define efi_background_red 0x40
#define efi_background_magenta 0x50
#define efi_background_brown 0x60
#define efi_background_lightgray 0x70

#define efi_text_attr(foreground,background) ((foreground) | ((background) << 4))

#endif
