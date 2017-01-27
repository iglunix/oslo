#ifndef _EFI_SIMPLE_TEXT_OUT_H
#define _EFI_SIMPLE_TEXT_OUT_H

typedef struct {
	int32_t    max_mode;
	int32_t    mode;
	int32_t    attr;
	int32_t    cursor_col;
	int32_t    cursor_row;
	efi_bool_t cursor_visible;
} efi_simple_text_out_mode;

typedef struct _efi_simple_text_out_protocol efi_simple_text_out_protocol_t;
struct _efi_simple_text_out_protocol {
	efi_status_t (efi_func *reset)          (efi_simple_text_out_protocol_t* self, efi_bool_t ext_verf);
	efi_status_t (efi_func *output_string)  (efi_simple_text_out_protocol_t* self, efi_char16_t* str);
	efi_status_t (efi_func *test_string)    (efi_simple_text_out_protocol_t* self, efi_char16_t* str);
	efi_status_t (efi_func *query_mode)     (efi_simple_text_out_protocol_t* self, uintn_t mode_num, uintn_t* cols, uintn_t* rows);
	efi_status_t (efi_func *set_mode)       (efi_simple_text_out_protocol_t* self, uintn_t mode_num);
	efi_status_t (efi_func *set_attr)       (efi_simple_text_out_protocol_t* self, uintn_t attr);
	efi_status_t (efi_func *clear_screen)   (efi_simple_text_out_protocol_t* self);
	efi_status_t (efi_func *set_cursor_pos) (efi_simple_text_out_protocol_t* self, uintn_t col, uintn_t row);
	efi_status_t (efi_func *enable_cursor)  (efi_simple_text_out_protocol_t* self, efi_bool_t visible);
	efi_simple_text_out_mode *mode;
};

// EFI boxdraw constants
#define BOXDRAW_HORIZONTAL                  0x2500
#define BOXDRAW_VERTICAL                    0x2502
#define BOXDRAW_DOWN_RIGHT                  0x250c
#define BOXDRAW_DOWN_LEFT                   0x2510
#define BOXDRAW_UP_RIGHT                    0x2514
#define BOXDRAW_UP_LEFT                     0x2518
#define BOXDRAW_VERTICAL_RIGHT              0x251c
#define BOXDRAW_VERTICAL_LEFT               0x2524
#define BOXDRAW_DOWN_HORIZONTAL             0x252c
#define BOXDRAW_UP_HORIZONTAL               0x2534
#define BOXDRAW_VERTICAL_HORIZONTAL         0x253c
#define BOXDRAW_DOUBLE_HORIZONTAL           0x2550
#define BOXDRAW_DOUBLE_VERTICAL             0x2551
#define BOXDRAW_DOWN_RIGHT_DOUBLE           0x2552
#define BOXDRAW_DOWN_DOUBLE_RIGHT           0x2553
#define BOXDRAW_DOUBLE_DOWN_RIGHT           0x2554
#define BOXDRAW_DOWN_LEFT_DOUBLE            0x2555
#define BOXDRAW_DOWN_DOUBLE_LEFT            0x2556
#define BOXDRAW_DOUBLE_DOWN_LEFT            0x2557
#define BOXDRAW_UP_RIGHT_DOUBLE             0x2558
#define BOXDRAW_UP_DOUBLE_RIGHT             0x2559
#define BOXDRAW_DOUBLE_UP_RIGHT             0x255a
#define BOXDRAW_UP_LEFT_DOUBLE              0x255b
#define BOXDRAW_UP_DOUBLE_LEFT              0x255c
#define BOXDRAW_DOUBLE_UP_LEFT              0x255d
#define BOXDRAW_VERTICAL_RIGHT_DOUBLE       0x255e
#define BOXDRAW_VERTICAL_DOUBLE_RIGHT       0x255f
#define BOXDRAW_DOUBLE_VERTICAL_RIGHT       0x2560
#define BOXDRAW_VERTICAL_LEFT_DOUBLE        0x2561
#define BOXDRAW_VERTICAL_DOUBLE_LEFT        0x2562
#define BOXDRAW_DOUBLE_VERTICAL_LEFT        0x2563
#define BOXDRAW_DOWN_HORIZONTAL_DOUBLE      0x2564
#define BOXDRAW_DOWN_DOUBLE_HORIZONTAL      0x2565
#define BOXDRAW_DOUBLE_DOWN_HORIZONTAL      0x2566
#define BOXDRAW_UP_HORIZONTAL_DOUBLE        0x2567
#define BOXDRAW_UP_DOUBLE_HORIZONTAL        0x2568
#define BOXDRAW_DOUBLE_UP_HORIZONTAL        0x2569
#define BOXDRAW_VERTICAL_HORIZONTAL_DOUBLE  0x256a
#define BOXDRAW_VERTICAL_DOUBLE_HORIZONTAL  0x256b
#define BOXDRAW_DOUBLE_VERTICAL_HORIZONTAL  0x256c

/* EFI block element constants */
#define BLOCKELEMENT_FULL_BLOCK   0x2588
#define BLOCKELEMENT_LIGHT_SHADE  0x2591

/* EFI geometric shape constants */
#define GEOMETRICSHAPE_UP_TRIANGLE    0x25b2
#define GEOMETRICSHAPE_RIGHT_TRIANGLE 0x25ba
#define GEOMETRICSHAPE_DOWN_TRIANGLE  0x25bc
#define GEOMETRICSHAPE_LEFT_TRIANGLE  0x25c4

/* EFI arrow shape constants */
#define ARROW_LEFT  0x2190
#define ARROW_UP    0x2191
#define ARROW_RIGHT 0x2192
#define ARROW_DOWN  0x2193

// EFI text colors
#define EFI_BLACK                 0x00
#define EFI_BLUE                  0x01
#define EFI_GREEN                 0x02
#define EFI_CYAN                  0x03
#define EFI_RED                   0x04
#define EFI_MAGENTA               0x05
#define EFI_BROWN                 0x06
#define EFI_LIGHTGRAY             0x07
#define EFI_BRIGHT                0x08
#define EFI_DARKGRAY              0x08
#define EFI_LIGHTBLUE             0x09
#define EFI_LIGHTGREEN            0x0a
#define EFI_LIGHTCYAN             0x0b
#define EFI_LIGHTRED              0x0c
#define EFI_LIGHTMAGENTA          0x0d
#define EFI_YELLOW                0x0e
#define EFI_WHITE                 0x0f

/* EFI background colors */
#define EFI_BACKGROUND_BLACK      0x00
#define EFI_BACKGROUND_BLUE       0x10
#define EFI_BACKGROUND_GREEN      0x20
#define EFI_BACKGROUND_CYAN       0x30
#define EFI_BACKGROUND_RED        0x40
#define EFI_BACKGROUND_MAGENTA    0x50
#define EFI_BACKGROUND_BROWN      0x60
#define EFI_BACKGROUND_LIGHTGRAY  0x70

/* Macro for creating EFI text attributes from colors */
#define EFI_TEXT_ATTR(fg, bg) ((fg) | ((bg) << 4))

#endif
