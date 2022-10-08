#include <posapi.h>
#include <modem.h>
#include <modem_iface.h>
#include "lcd.h"
#include "ppp.h"
#include "netif.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <error.h>





#define MODEM_DEVICE_NAME "/dev/ttyS1"
#define TEL_NUMBER 		  "9,96169"
#define MSG               "HELLO WORLD"



extern uint32_t sysGetTimerCount(void);

void modem_demo(void){
	int retval = 0;
	int ifd = -1;
	int status = 0;
	int iRet;
	ModemDialParms_t  dailparam;
	dailparam.connect_mode=MODEM_COMM_ASYNC;
	dailparam.connect_speed=MODEM_CONNECT_56000BPS;
	dailparam.dial_timeo=30;
	dailparam.idle_timeo=10;



    lcd_clean();
	lcd_printf(ALG_CENTER, "Modem Demo");
	lcd_printf(ALG_LEFT, "Open device.....");
	lcd_flip();

	ifd = modem_open(MODEM_DEVICE_NAME, O_RDWR | O_NONBLOCK);
	if (ifd < 0){

		lcd_printf(ALG_LEFT, "Device open failed.");
		lcd_flip();
		kb_getkey();
		retval = -1;
	}else {

		lcd_printf(ALG_LEFT, "Wait modem ready.....");
		lcd_flip();

		while (1){
			iRet=modem_get_status(ifd, &status);

			lcd_printf(ALG_LEFT,"status=%d",status);
			lcd_printf(ALG_LEFT,"return=%d",iRet);
			lcd_flip();

			if ((status >> 31) == 0)
			{
				iRet = modem_set_dial_parms(ifd,&dailparam);
				iRet=modem_get_status(ifd, &status);
				lcd_printf(ALG_LEFT,"init status=%d",status);
				lcd_flip();

				break;

			}
			else
				usleep(10000);
		}
	}

	if (0 == retval)
	{
		lcd_printf(ALG_LEFT, "Dialing %s...", TEL_NUMBER);
		lcd_flip();
		kb_hit();
        usleep(1000000);
		retval = modem_dialing(ifd, TEL_NUMBER);
		if (0 != retval){
			lcd_printf(ALG_LEFT, "moem_dialing() failed.");
			lcd_flip();
			kb_getkey();
			retval = -1;

		}
		else
		{
			while (1)
			{
				modem_get_status(ifd, &status);

				if ((status >> 31) == 0)
					break;
				else
					usleep(10000);
			}

			status = status & 0x7FFFFFFF;
			if (MODEM_STATE_CONNECT != status){


				lcd_printf(ALG_LEFT, "dial failed.");
				lcd_flip();
				kb_getkey();
				retval = -1;
			}else {
			    lcd_printf(ALG_LEFT, "Connected");
				lcd_flip();
                 usleep(1000000);
				lcd_printf(ALG_LEFT, "Sendding data...");
				lcd_flip();
                 usleep(1000000);



				modem_write(ifd, MSG, strlen(MSG) + 1);
				modem_wait_until_sent(ifd);
				lcd_printf(ALG_LEFT, "success...");
				lcd_flip();
                 usleep(1000000);


			}
		}
		modem_hangup(ifd);
	}


	if (ifd >=0){
        lcd_printf(ALG_LEFT, "closed...");
        lcd_flip();
         usleep(1000000);


		modem_close(ifd);
		ifd = -1;
	}

}


int demo_for_modem_ppp(void)
{
	int  retval, sk;
	char host[]        = "210.22.14.210";
	short port		   = 9005;
	char tx[] = "Hello world";
	char rx[100];
    char modem_chat_file[] = {
    	"ABORT          'NO CARRIER'"		"\n"
    	"ABORT          'NO DIALTONE'"		"\n"
    	"ABORT          'ERROR'"			"\n"
    	"ABORT          'NO ANSWER'"		"\n"
    	"ABORT          'BUSY'"				"\n"
    	"TIMEOUT        '60'"				"\n"
    	"''             'ATZ'"				"\n"
    	"'OK'           'ATH0'"				"\n"
    	"'OK'           'ATDT9,96169'"			"\n"
    	"'CONNECT'      ''"					"\n"
    };
    lcd_clean();
    //lcd_printf(ALG_LEFT,  "PPP login %s...", telephone);
    //lcd_flip();
	/*
	 * Step 1: login
	 */
	lcd_printf(ALG_LEFT, "PPP logon...");
	lcd_flip();
    modem_power_control(1);
	retval = ppp_open(MODEM_DEVICE_NAME, modem_chat_file, "card", "card", 1, 30);

	if (! retval) {
        lcd_printf(ALG_LEFT,  "PPP check...");
        lcd_flip();
		/*
		 * Step 2: Check whether link connect.
		 */
        do {
                retval = ppp_check(MODEM_DEVICE_NAME);
                lcd_printf(ALG_LEFT,  "%5d ", retval);
                lcd_flip();
                usleep(400*1000);
                } while (retval == -1032);
		if (! retval) { /* ppp link connected */
			/*
			 * Now, we can use ppp link to communicate with the server
			 */
            lcd_printf(ALG_LEFT, "Socket...");
            lcd_flip();
			if ((sk = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
				struct sockaddr_in servaddr;

				memset(&servaddr, 0, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(port);
				inet_pton(AF_INET, host, &servaddr.sin_addr);

                lcd_printf(ALG_LEFT, "Connect %s...", host);
                lcd_flip();
				if (! (retval = connect(sk, (struct sockaddr *)&servaddr, sizeof(servaddr)))) {

/*
 * Notice: the code below is optional
 */
#if 0
{
					struct timeval rtimeo, stimeo;
					rtimeo.tv_sec = 0;
					rtimeo.tv_usec = 10 * 1000; /* receive timeout: 10 ms */
					NetSetsockopt(sk, SOL_SOCKET, SO_RCVTIMEO, &rtimeo, sizeof(rtimeo));
					stimeo.tv_sec = 0;
					stimeo.tv_usec = 10 * 1000; /* send timeout: 10 ms */
					NetSetsockopt(sk, SOL_SOCKET, SO_SNDTIMEO, &stimeo, sizeof(stimeo));
}
#endif

					/*
					 * Send data to server and receive data from server
					 */
					retval = send(sk, tx, sizeof(tx), 0);
                    lcd_printf(ALG_LEFT, "Tx %d...%d", sizeof(tx), retval);
                    lcd_flip();
					retval = recv(sk, rx, sizeof(rx), 0);
                    lcd_printf(ALG_LEFT, "Rx %d...%d", sizeof(rx), retval);
                    lcd_flip();
				}
                lcd_printf(ALG_LEFT, "Close socket...");
                lcd_flip();
				/*
				 * Close the socket if we did not use it
				 */
				close(sk);
			}
		}
        lcd_printf(ALG_LEFT, "PPP Logout...");
        lcd_flip();
		/*
		 * Last: we should logout to release the link
		 */
        ppp_close(MODEM_DEVICE_NAME);
	    modem_power_control(0);
	    usleep(500*1000); //500ms
	}
    lcd_printf(ALG_LEFT, "Press any key to conntinue");
    lcd_flip();
	kb_getkey();
	return 0;
}







//end file

