#include <posapi.h>
#include <stdio.h>
#include "lcd.h"
#include <bar_scanner.h>

void bar_scanner_demo(void){
#define device_name  "/dev/ttyS2"
		int ifd_scanner = -1;
		char	szBarCode[1024];
		int 	BarCodeLen = 0;	
		int 	readed = 0;
		//int update = 1;
		
		lcd_clean();
		lcd_printf(ALG_LEFT,   "open device...");
		lcd_printf(ALG_LEFT, "device name %s", device_name);
		lcd_flip();		
		
	
	
		lcd_flip();
		
		ifd_scanner = bar_open(device_name, O_RDONLY);
		if (ifd_scanner < 0){
			lcd_printf(ALG_LEFT, "Open failed.");
			lcd_flip();
			kb_getkey();
		}
		else {
			szBarCode[0] = 0x00;
			BarCodeLen = 0;
			lcd_printf(ALG_LEFT, "Please show bar code....");
			bar_scan(ifd_scanner, 1);	
			lcd_flip();
			while (1)
			{	
				if (kb_hit() && DIKS_ESCAPE == kb_getkey())
					break;
				else {
					BarCodeLen = 0;
					memset(szBarCode, 0, sizeof(szBarCode));
					while (1){
						readed = bar_read_timeout(ifd_scanner, szBarCode + BarCodeLen, sizeof(szBarCode), 100);
						if (readed > 0){
							BarCodeLen += readed;
							//bar_scan(ifd_scanner, 0);
						}else
							break;
					}
								
					if (BarCodeLen > 0) {
						lcd_printf(ALG_LEFT, "Barcode:%s", szBarCode);
						lcd_printf(ALG_LEFT, "Please show bar code....");
						lcd_flip();
						usleep(200000);
						bar_scan(ifd_scanner, 1);
						
					}
					else
						usleep(100000);
				}
			}
		}
		
		if (ifd_scanner >=0){
			bar_scan(ifd_scanner, 0);
			bar_close(ifd_scanner);
		}
		

}

