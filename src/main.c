#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lcd.h"
#include "postslib_extern.h"


const char menu[][100] = {
		"Display",
		//"Display Arabic",
		"Display Arabic & Kannada",
		"Key Board",
		"Touch Screen",
		"Printer",
		"Contact Smartcard",
		"Contactless Smartcard",
		"Magnetic Strip Card",
		"Uart",
		"Modem",
		"GPRS",
		"Socket Server",
		"Socket Client",
		"SSL Server",
		"SSL Client",
		"Bar Scanner",
		"NS10 Signature",
		"Modem PPP",
};
extern void Display_Demo(void);
extern void Display_Demo_Arabic(void);
extern void Display_Demo_Kannada(void);
extern void key_board_demo(void);
extern void Printer_Demo(void);
extern void ContactSmartCard_Demo(void);
extern void ContactlessSmartCard_Demo(void);
extern void magneticstrip_demo(void);
extern void uart_demo(void);

extern void socket_server_demo(void);
extern void socket_client_demo(void);
extern void ssl_server_demo(void);
extern void ssl_client_demo(void);
extern void modem_demo(void);
extern int demo_for_modem_ppp(void);
extern void gprs_demo(void);
extern void touch_demo(void);
extern void bar_scanner_demo(void);

#define CL_BACK_GROUND_PIC_FILE "cl_bk_pic.jpg"

int main(int argc, char *argv[]){
	int retval = 0;
	int i, selected;

	lcd_init(&argc, &argv);
	lcd_set_bk_color(COLOR_BLACK);
	lcd_set_font_color(COLOR_YELLOW);

	i = 0;
	selected = 0;
	while (selected>=0){
		selected = lcd_menu("NEW8210 DEMO", menu, sizeof(menu)/100, selected);
		switch (selected){
			case 0:
				Display_Demo();
				break;
			case 1:
				Display_Demo_Arabic();
				Display_Demo_Kannada();
				break;
			case 2:
				key_board_demo();
				break;
			case 3:
				touch_demo();
				break;
			case 4:
				Printer_Demo();
				break;
			case 5:
				ContactSmartCard_Demo();
				break;
			case 6:
				ContactlessSmartCard_Demo();
				break;
			case 7:
				magneticstrip_demo();
				break;
			case 8:
				uart_demo();
				break;
			case 9:
				modem_demo();
				break;
			case 10:
				gprs_demo();
				break;
			case 11:
				socket_server_demo();
				break;
			case 12:
				socket_client_demo();
				break;
			case 13:
				ssl_server_demo();
				break;
			case 14:
				ssl_client_demo();
				break;
			case 15:
				bar_scanner_demo();
                break;
			case 16:
				PedSignature("abcd","default.bmp",IMAGE_BMP, 50000,"hello");
                break;
            case 17:
                demo_for_modem_ppp();
                break;
			default:
				break;
		}
	}
		
	return retval;
}
