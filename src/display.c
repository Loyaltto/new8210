#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <directfb.h>

#include <posapi.h>
#include "string_convert.h"
#define font_file_xx  "/usr/share/fonts/wqy-microhei.ttf"
#define font_file_arabic  "/usr/share/fonts/arial.ttf"

#define font_file_kannada  "/home/user0/fonts/kannada.ttf"

#include "visualstring.h"
extern IDirectFB				*dfb;

void Display_Demo(void)
{
	int retval = 0;
	IDirectFB				*dfb 	= NULL;
	IDirectFBDisplayLayer	*layer	= NULL;
	IDirectFBWindow			*window	= NULL;
	IDirectFBEventBuffer 	*events	= NULL;
	IDirectFBSurface        *main_surface = NULL;
	IDirectFBFont		    *font_16 = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	int width = 0, height = 0;
	const char * utf8text = NULL;
	int text_len = 0;

	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	DFBFontDescription	    fdesc;
	const char * disp_text = "HELLO WORLD";
	int x, y;
	

	if (0 != DirectFBCreate(&dfb)){
		printf("DirectFBCreate Failed\r\n");
		retval = -1;
	}
	else if (0 != dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer)){
		printf("GetDisplayLayer Failed\r\n");
		retval = -1;			
	}else if (0 != layer->GetConfiguration(layer, &config)){
		printf("GetConfiguration Failed\r\n");
		retval = -1;
	}
	else {
		desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
		desc.posx	= 0;
		desc.posy	= sys_get_status_bar_height();
		desc.width  = config.width;
		desc.height = config.height - desc.posy;
		printf("Screen size %d X %d\r\n", desc.width, desc.height);
		desc.caps	= DWCAPS_ALPHACHANNEL;
	}

	if (0 == retval){
		if (0 !=  layer->CreateWindow(layer, &desc, &window)){
			printf("Create Window Failed\r\n");	
			retval = -1;
		}
		else if (0 !=  window->CreateEventBuffer(window, &events)){
			printf("GetConfiguration Failed\r\n");
			retval = -1;
		}else if (0 != window->GetSurface(window, &main_surface)){
			printf("GetSurface Failed\r\n");
			retval = -1;
		}else if (0 != main_surface->GetSize(main_surface, &width, &height)){
			printf("GetSize Failed\r\n");
			retval = -1;		
		}	
	}
	
	if (0 == retval){
		fdesc.flags  = DFDESC_HEIGHT;
		fdesc.height = 16;	
		dfb->CreateFont(dfb, font_file_xx, &fdesc, &font_16);
		if (NULL == font_16){
			printf("craete font16 failed.\r\n");
			retval = -1;
		}
		
		fdesc.height = 24;	
		dfb->CreateFont(dfb, font_file_xx, &fdesc, &font_24);
		if (NULL == font_24){
			printf("craete font24 failed.\r\n");
			retval = -1;
		}
		
		fdesc.height = 32;	
		dfb->CreateFont(dfb, font_file_xx, &fdesc, &font_32);
		if (NULL == font_32){
			printf("craete font32 failed.\r\n");
			retval = -1;
		}

	}

	if (0 == retval){
		
		window->SetOptions(window, DWOP_ALPHACHANNEL);
		window->SetOpacity(window, 0xFF);
		main_surface->Clear(main_surface, 0xFF, 0xff, 0xff, 0xff);
		
		utf8text = string_covert(disp_text, strlen(disp_text));
		
		//output text on the screen
		x = config.width/2;
		y = 25;
		text_len = string_len(utf8text);
		
		main_surface->SetFont(main_surface, font_16);	
		main_surface->SetColor(main_surface, 0xFF, 0x00, 0x00, 0xff);		
		main_surface->DrawString(main_surface, utf8text, text_len, x, y, DSTF_CENTER);

		font_16->GetHeight(font_16, &height);		
		y+= height + 1;
		
		main_surface->SetFont(main_surface, font_24);	
		main_surface->SetColor(main_surface, 0x00, 0xFF, 0x00, 0xff);		
		main_surface->DrawString(main_surface, utf8text, text_len, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);		
		y+= height + 1;

		
		main_surface->SetFont(main_surface, font_24);	
		main_surface->SetColor(main_surface, 0x00, 0x00, 0xFF, 0xff);		
		main_surface->DrawString(main_surface, utf8text, text_len, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);		
		y+= height + 1;

		
		
		main_surface->Flip(main_surface, NULL, 0);
	}

	if (0 == retval){
		DFBWindowEvent  windowEvent;
		
		while(1){			
			if (events->HasEvent(events) == DFB_OK){
				if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
					events->GetEvent(events, DFB_EVENT(&windowEvent));
					if (windowEvent.type == DWET_KEYUP){
						window->Close(window);
					}else if (windowEvent.type == DWET_CLOSE){
						window->Destroy(window);
						break;
					}					
				}
			} 
		}
	}
	
	if (NULL != events)
		events->Release(events);
	
	if (NULL != main_surface)
		main_surface->Release(main_surface);
	
	if (NULL != window)
		window->Release(window);
	
	if (NULL != layer)
		layer->Release(layer);

	
	
	
	if (NULL != font_16)
		font_16->Release(font_16);
	
	if (NULL != font_24)
		font_24->Release(font_24);
	
	if (NULL != font_32)
		font_32->Release(font_32);	
	
	if (NULL != dfb)
		dfb->Release(dfb);
	
	return ;
}


void Display_Demo_Arabic(void)
{
	int retval = 0;
	IDirectFB				*dfb 	= NULL;
	IDirectFBDisplayLayer	*layer	= NULL;
	IDirectFBWindow			*window	= NULL;
	IDirectFBEventBuffer 	*events	= NULL;
	IDirectFBSurface        *main_surface = NULL;
	IDirectFBFont		    *font_16 = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	int width = 0, height = 0;

	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	DFBFontDescription	    fdesc;

	int x, y;
	VisualString vs;
	char outputStringUtf8[255];
	//#define ARIABI_MSG "مرحبا"
    #define ARIABI_MSG "مجموعة الحكير"


	bidi_init(&vs);
	vs.inputString = ARIABI_MSG;
	vs.inputLength = strlen(ARIABI_MSG);
	bidi_visualize_line(&vs, outputStringUtf8,sizeof(outputStringUtf8));

	if (0 != DirectFBCreate(&dfb)){
		printf("DirectFBCreate Failed\r\n");
		retval = -1;
	}
	else if (0 != dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer)){
		printf("GetDisplayLayer Failed\r\n");
		retval = -1;
	}else if (0 != layer->GetConfiguration(layer, &config)){
		printf("GetConfiguration Failed\r\n");
		retval = -1;
	}
	else {
		desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
		desc.posx	= 0;
		desc.posy	= sys_get_status_bar_height();
		desc.width  = config.width;
		desc.height = config.height - desc.posy;
		printf("Screen size %d X %d\r\n", desc.width, desc.height);
		desc.caps	= DWCAPS_ALPHACHANNEL;
	}

	if (0 == retval){
		if (0 !=  layer->CreateWindow(layer, &desc, &window)){
			printf("Create Window Failed\r\n");
			retval = -1;
		}
		else if (0 !=  window->CreateEventBuffer(window, &events)){
			printf("GetConfiguration Failed\r\n");
			retval = -1;
		}else if (0 != window->GetSurface(window, &main_surface)){
			printf("GetSurface Failed\r\n");
			retval = -1;
		}else if (0 != main_surface->GetSize(main_surface, &width, &height)){
			printf("GetSize Failed\r\n");
			retval = -1;
		}
	}

	if (0 == retval){
		fdesc.flags  = DFDESC_HEIGHT;
		fdesc.height = 16;
		dfb->CreateFont(dfb, font_file_arabic, &fdesc, &font_16);
		if (NULL == font_16){
			printf("craete font16 failed.\r\n");
			retval = -1;
		}

		fdesc.height = 24;
		dfb->CreateFont(dfb, font_file_arabic, &fdesc, &font_24);
		if (NULL == font_24){
			printf("craete font24 failed.\r\n");
			retval = -1;
		}

		fdesc.height = 32;
		dfb->CreateFont(dfb, font_file_arabic, &fdesc, &font_32);
		if (NULL == font_32){
			printf("craete font32 failed.\r\n");
			retval = -1;
		}

	}

	if (0 == retval){

		window->SetOptions(window, DWOP_ALPHACHANNEL);
		window->SetOpacity(window, 0xFF);
		main_surface->Clear(main_surface, 0xFF, 0xff, 0xff, 0xff);



		//output text on the screen
		x = config.width/2;
		y = 25;


		main_surface->SetFont(main_surface, font_16);
		main_surface->SetColor(main_surface, 0xFF, 0x00, 0x00, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;

		main_surface->SetFont(main_surface, font_24);
		main_surface->SetColor(main_surface, 0x00, 0xFF, 0x00, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;


		main_surface->SetFont(main_surface, font_24);
		main_surface->SetColor(main_surface, 0x00, 0x00, 0xFF, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;



		main_surface->Flip(main_surface, NULL, 0);
	}

	if (0 == retval){
		DFBWindowEvent  windowEvent;

		while(1){
			if (events->HasEvent(events) == DFB_OK){
				if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
					events->GetEvent(events, DFB_EVENT(&windowEvent));
					if (windowEvent.type == DWET_KEYUP){
						window->Close(window);
					}else if (windowEvent.type == DWET_CLOSE){
						window->Destroy(window);
						break;
					}
				}
			}
		}
	}

	if (NULL != events)
		events->Release(events);

	if (NULL != main_surface)
		main_surface->Release(main_surface);

	if (NULL != window)
		window->Release(window);

	if (NULL != layer)
		layer->Release(layer);




	if (NULL != font_16)
		font_16->Release(font_16);

	if (NULL != font_24)
		font_24->Release(font_24);

	if (NULL != font_32)
		font_32->Release(font_32);

	if (NULL != dfb)
		dfb->Release(dfb);

	return ;
}

void Display_Demo_Kannada(void)
{
	int retval = 0;
	IDirectFB				*dfb 	= NULL;
	IDirectFBDisplayLayer	*layer	= NULL;
	IDirectFBWindow			*window	= NULL;
	IDirectFBEventBuffer 	*events	= NULL;
	IDirectFBSurface        *main_surface = NULL;
	IDirectFBFont		    *font_16 = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	int width = 0, height = 0;

	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	DFBFontDescription	    fdesc;

	int x, y;
	VisualString vs;
	char outputStringUtf8[255];
	//#define ARIABI_MSG "مرحبا"
    //#define ARIABI_MSG "مجموعة الحكير"
    #define KANNADA_MSG "ಹಲೋ, ನನ್ನ ಸ್ನೇಹಿತ"

	bidi_init(&vs);
	vs.inputString = KANNADA_MSG;
	vs.inputLength = strlen(KANNADA_MSG);
	bidi_visualize_line(&vs, outputStringUtf8,sizeof(outputStringUtf8));

	if (0 != DirectFBCreate(&dfb)){
		printf("DirectFBCreate Failed\r\n");
		retval = -1;
	}
	else if (0 != dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer)){
		printf("GetDisplayLayer Failed\r\n");
		retval = -1;
	}else if (0 != layer->GetConfiguration(layer, &config)){
		printf("GetConfiguration Failed\r\n");
		retval = -1;
	}
	else {
		desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
		desc.posx	= 0;
		desc.posy	= sys_get_status_bar_height();
		desc.width  = config.width;
		desc.height = config.height - desc.posy;
		printf("Screen size %d X %d\r\n", desc.width, desc.height);
		desc.caps	= DWCAPS_ALPHACHANNEL;
	}

	if (0 == retval){
		if (0 !=  layer->CreateWindow(layer, &desc, &window)){
			printf("Create Window Failed\r\n");
			retval = -1;
		}
		else if (0 !=  window->CreateEventBuffer(window, &events)){
			printf("GetConfiguration Failed\r\n");
			retval = -1;
		}else if (0 != window->GetSurface(window, &main_surface)){
			printf("GetSurface Failed\r\n");
			retval = -1;
		}else if (0 != main_surface->GetSize(main_surface, &width, &height)){
			printf("GetSize Failed\r\n");
			retval = -1;
		}
	}

	if (0 == retval){
		fdesc.flags  = DFDESC_HEIGHT;
		fdesc.height = 16;
		dfb->CreateFont(dfb, font_file_kannada, &fdesc, &font_16);
		if (NULL == font_16){
			printf("craete font16 failed.\r\n");
			retval = -1;
		}

		fdesc.height = 24;
		dfb->CreateFont(dfb, font_file_kannada, &fdesc, &font_24);
		if (NULL == font_24){
			printf("craete font24 failed.\r\n");
			retval = -1;
		}

		fdesc.height = 32;
		dfb->CreateFont(dfb, font_file_kannada, &fdesc, &font_32);
		if (NULL == font_32){
			printf("craete font32 failed.\r\n");
			retval = -1;
		}

	}

	if (0 == retval){

		window->SetOptions(window, DWOP_ALPHACHANNEL);
		window->SetOpacity(window, 0xFF);
		main_surface->Clear(main_surface, 0xFF, 0xff, 0xff, 0xff);



		//output text on the screen
		x = config.width/2;
		y = 25;


		main_surface->SetFont(main_surface, font_16);
		main_surface->SetColor(main_surface, 0xFF, 0x00, 0x00, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;

		main_surface->SetFont(main_surface, font_24);
		main_surface->SetColor(main_surface, 0x00, 0xFF, 0x00, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;


		main_surface->SetFont(main_surface, font_32);
		main_surface->SetColor(main_surface, 0x00, 0x00, 0xFF, 0xff);
		main_surface->DrawString(main_surface, outputStringUtf8, -1, x, y, DSTF_CENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;



		main_surface->Flip(main_surface, NULL, 0);
	}

	if (0 == retval){
		DFBWindowEvent  windowEvent;

		while(1){
			if (events->HasEvent(events) == DFB_OK){
				if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
					events->GetEvent(events, DFB_EVENT(&windowEvent));
					if (windowEvent.type == DWET_KEYUP){
						window->Close(window);
					}else if (windowEvent.type == DWET_CLOSE){
						window->Destroy(window);
						break;
					}
				}
			}
		}
	}

	if (NULL != events)
		events->Release(events);

	if (NULL != main_surface)
		main_surface->Release(main_surface);

	if (NULL != window)
		window->Release(window);

	if (NULL != layer)
		layer->Release(layer);




	if (NULL != font_16)
		font_16->Release(font_16);

	if (NULL != font_24)
		font_24->Release(font_24);

	if (NULL != font_32)
		font_32->Release(font_32);

	if (NULL != dfb)
		dfb->Release(dfb);

	return ;
}




