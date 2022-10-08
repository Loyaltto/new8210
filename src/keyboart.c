
#include "lcd.h"
void key_board_demo(void){
	int cancel_times = 0;
	int key = 0, key_pre = 0;;
	lcd_clean();
	lcd_printf(ALG_CENTER, "PLEASE PRESS KEY");
	lcd_printf(ALG_CENTER, "DOUBLE PRESS CANCEL TO EXIT");
	while (cancel_times < 2){
		
		lcd_flip();
		key = kb_getkey();
		lcd_clean();
		lcd_printf(ALG_CENTER, "PLEASE PRESS KEY");
		lcd_printf(ALG_CENTER, "DOUBLE PRESS CANCEL TO EXIT");
		lcd_printf_ex(ALG_CENTER, 100, "KEY CODE = %04X", key);
		if (key == 0x1B){
			if (key_pre == key)
				cancel_times++;
			else {
				key_pre = key;
				cancel_times = 1;	
			}
		}else {
			cancel_times = 0;
			key_pre = key;
		}			
	}	
	
}

