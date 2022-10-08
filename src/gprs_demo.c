#include <posapi.h>
#include <wnet.h>
#include <ppp.h>
#include <sys/time.h>

#include "lcd.h"

char gprs_chat_file[] = {
	"ABORT          'NO CARRIER'"		"\n"
	"ABORT          'NO DIALTONE'"		"\n"
	"ABORT          'ERROR'"			"\n"
	"ABORT          'NO ANSWER'"		"\n"
	"ABORT          'BUSY'"				"\n"
	"ABORT          'Invalid Login'"	"\n"
	"ABORT          'Login incorrect'"	"\n"
	"TIMEOUT        '60'"				"\n"
	"''             'ATZ'"				"\n"
	"'OK'           'AT+CGDCONT=1,\"IP\",\"CMNET\"'"	"\n"
	"'OK'           'ATDT*99***1#'"			"\n"
	"'CONNECT'      ''"					"\n"
};


void gprs_demo(void){
	int retval = 0;
	
	struct timeval start,cur, diff, end;
	struct timezone tz;

	
	

	lcd_clean();	
	lcd_printf(ALG_CENTER, "GPRS Demo");
	lcd_printf(ALG_LEFT, "Power On device.....");
	lcd_flip();
	
	retval = wnet_power_on();
	if (0 != retval){
		lcd_printf(ALG_LEFT, "Power On Failed");
		lcd_flip();	
		kb_getkey();
	}else {
		lcd_printf(ALG_LEFT, "Init wnet.....");
		lcd_flip();	
		retval = wnet_init("/var/mux0");
		if (0 != retval){
			lcd_printf(ALG_LEFT, "Init wnet failed %d", retval);
			lcd_flip();
			kb_getkey();
		}else {
			gettimeofday(&start, &tz);
			diff.tv_sec = 60;
			diff.tv_usec = 0;
			timeradd(&start, &diff, &end);
			lcd_printf(ALG_LEFT, "set attached...");
			lcd_flip();
			
			while (1){
				retval = wnet_set_attached(1);				
				if (0 != retval){
					gettimeofday(&cur, &tz);
				
					if (timercmp(&cur, &end, < ))
						usleep(10000);
					else {
						lcd_printf(ALG_LEFT, "wnet_set_attached() failed %d", retval);
						lcd_flip();
						kb_getkey();
						break;
					}
				}else
					break;
			}

			if (0 == retval){
				lcd_printf(ALG_LEFT, "PPP logon...");
				lcd_flip();
				retval = ppp_open("/var/mux1", gprs_chat_file, "wap", "wap", PPP_ALG_PAP, 30);
				if (0 != retval){
					lcd_printf(ALG_LEFT, "PPP open failed %d", retval);
					lcd_flip();
					kb_getkey();
				}else {
					gettimeofday(&start, &tz);
					diff.tv_sec = 60;
					diff.tv_usec = 0;
					timeradd(&start, &diff, &end);
					lcd_printf(ALG_LEFT, "PPP check...");
					lcd_flip();
					while (1){
						retval = ppp_check("/var/mux1");
						if (0 == retval){
							lcd_printf(ALG_LEFT, "PPP logon Ok");
							lcd_flip();
							kb_getkey();
							break;
						}
						else{
							gettimeofday(&cur, &tz);				
							if (timercmp(&cur, &end, < ))
								usleep(10000);
							else {
								lcd_printf(ALG_LEFT, "PPP logon failed %d", retval);
								lcd_flip();
								kb_getkey();
								break;
							}
						}
					}
					
					ppp_close("/var/mux1");	
				}			
			}
		}				
		
		wnet_power_down();
	}
}

