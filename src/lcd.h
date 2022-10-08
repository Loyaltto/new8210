#ifndef lcd_h_
#define lcd_h_
#include <directfb.h>
extern IDirectFB				*dfb;
#define RGBA(R, G, B, A) ((((unsigned int)(unsigned char)R)<< 16) | \
						  (((unsigned int)(unsigned char)G)<< 8) | \
						  (((unsigned int)(unsigned char)B)<< 0)  | \
						  (((unsigned int)(unsigned char)A)<< 24))

#define COLOR_RED   RGBA(0xFF, 0x00, 0x00, 0xFF)
#define COLOR_GREEN RGBA(0x00, 0xFF, 0x00, 0xFF)
#define COLOR_BLUE  RGBA(0x00, 0x00, 0xFF, 0xFF)
#define COLOR_WITE  RGBA(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_BLACK RGBA(0x00, 0x00, 0x00, 0xFF)
#define COLOR_YELLOW RGBA(0xFF, 0xFF, 0x00, 0xFF)


typedef enum {
	ALG_CENTER,
		ALG_LEFT,
		ALG_RIGHT
}LCD_ALG;


int  lcd_init(int * argc, char **argv[]);
void lcd_uninit(void);
void lcd_set_bk_color(unsigned int color);
void lcd_set_font_color(unsigned int color);
void lcd_clean(void);
void lcd_printf(LCD_ALG alg, const char * pszFmt,...);
void lcd_printf_ex(LCD_ALG alg, unsigned int y, const char * pszFmt,...);
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void lcd_set_background_picture(const char * pszPictureFileName);
void lcd_clear_background_picture(void);
void lcd_show_picture(const char * pszPictureFileName);
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color);
void lcd_save(void);
void lcd_restore(void);
void lcd_flip(void);
int lcd_menu(const char * pszTitle, const char menu[][100], unsigned int count, int select);
int  kb_hit(void);
int  kb_getkey(void);
#endif
