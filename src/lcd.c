#include "lcd.h"
#include <directfb.h>
#include <posapi.h>
#include <stdarg.h>
#include <led.h>
#include "string_convert.h"
#define font_file_xx  "/usr/share/fonts/wqy-microhei.ttf"
IDirectFB				*dfb 	= NULL;
static IDirectFBDisplayLayer	*layer	= NULL;
static IDirectFBWindow			*window	= NULL;
static IDirectFBEventBuffer 	*events	= NULL;
IDirectFBSurface               *main_surface = NULL;
static IDirectFBFont		    *font_16 = NULL;
static unsigned int              back_color = COLOR_WITE;
static unsigned int              font_color = COLOR_BLACK;

int lcd_init(int * argc, char **argv[]){
	int retval = 0;
	
	int width = 0, height = 0;



	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	DFBFontDescription	    fdesc;
	

	printf("lcd_init() >>\r\n");
	if (0 != DirectFBInit(argc, argv)){
		printf("DirectFInit Failed\r\n");
		retval = -1;
	}else if (0 != DirectFBCreate(&dfb)){
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
	}

	if (0 == retval){
		window->SetOptions(window, DWOP_ALPHACHANNEL);
		window->SetOpacity(window, 0xFF);
		main_surface->SetFont(main_surface, font_16);
		lcd_set_font_color(COLOR_BLACK);
		lcd_clean();
	}
	printf("lcd_init() <<\r\n");
	return retval;
}

void lcd_uninit(void){
	DFBWindowEvent  windowEvent;
	//DFBResult ret;

	if (NULL != window){
		window->Close(window);
		while(1){			
			if (events->HasEvent(events) == DFB_OK){
				if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
					events->GetEvent(events, DFB_EVENT(&windowEvent));
					if (windowEvent.type == DWET_CLOSE){
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

	
	if (NULL != dfb){
		dfb->WaitForSync(dfb);
		dfb->Release(dfb);	
	}
}

void lcd_set_bk_color(unsigned int color){
	back_color = color;
}

void lcd_set_font_color(unsigned int color){
	font_color = color;	
}

static int current_y = 1;
void lcd_clean(void){	
	main_surface->Clear(main_surface, 
		0xFF & (back_color>> 16),
		0xFF & (back_color >> 8),
		0xFF & (back_color >> 0),
		0xFF & (back_color)>>24);
	current_y = 1;	
}

void lcd_printf(LCD_ALG alg, const char * pszFmt,...){
	int width, height;
	int font_height;
	IDirectFBSurface * sub_surface = NULL;
	IDirectFBSurface * store_surface = NULL;
	DFBRectangle rect;
	DFBSurfaceDescription	surfdesc;
	IDirectFBFont * font = NULL;
	
	char textbuf[4096];
	const char *utf8text;
	const char * pnewline = NULL;
	int str_length, temp_width;
	va_list arg;
	
	
	
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);	
	va_end (arg);
	

	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);
	
	main_surface->GetSize(main_surface, &width, &height);
	main_surface->SetColor(main_surface,
			0xFF & (font_color>> 16),
		0xFF & (font_color >> 8),
		0xFF & (font_color >> 0),
		0xFF & (font_color>>24));
	
	if (current_y + font_height > height){
		rect.x = 0;
		rect.y = current_y + font_height - height;
		rect.w = width;
		rect.h = current_y - rect.y;
		main_surface->GetSubSurface (main_surface, &rect, &sub_surface);
		
	  	surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		surfdesc.width = width;
		surfdesc.height= height;
	   dfb->CreateSurface(dfb, &surfdesc, &store_surface);
	   store_surface->SetBlittingFlags(store_surface, 	DSBLIT_BLEND_ALPHACHANNEL);
	   store_surface->Blit(store_surface, sub_surface, NULL, 0, 0);
	   lcd_clean();
	   main_surface->SetBlittingFlags(main_surface, 	DSBLIT_BLEND_ALPHACHANNEL);
	   rect.y = 0;
	   main_surface->Blit(main_surface, store_surface, &rect, 0, 0);
	 
	   store_surface->Release(store_surface);
	   sub_surface->Release(sub_surface);
	   current_y = rect.h;
	   
	}

	
	font->GetStringBreak(font, textbuf, strlen(textbuf), width -2, &temp_width, &str_length, &pnewline);
	
	utf8text = string_covert(textbuf, str_length);
	
	if (ALG_CENTER == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width/2, current_y, DSTF_TOPCENTER);
	else if (ALG_LEFT == alg)
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), 2, current_y, DSTF_TOPLEFT);
	else 
		main_surface->DrawString(main_surface, utf8text, string_len(utf8text), width, current_y, DSTF_TOPRIGHT);		
		
	

	current_y = current_y + font_height + 1;
	
	
	if (NULL != pnewline)
	{
		lcd_printf(alg, pnewline);
	}
}

void lcd_printf_ex(LCD_ALG alg, unsigned y, const char * pszFmt,...){
	
	char textbuf[4096];

	va_list arg;
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);
	current_y = y;	
	lcd_printf(alg, textbuf);
}

void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
	int screen_width, screen_height;
	main_surface->GetSize(main_surface, &screen_width, &screen_height);
	if ((x+1 < screen_width) && (y +1 < screen_height)){
		if (x + 1 + width  > (unsigned int)screen_width)
			width = screen_width - x -1;
		if (x + 1 + height  > (unsigned int)screen_height)
			height = screen_height - y -1;

		main_surface->SetColor(main_surface,
			0xFF & (font_color >> 24),
			0xFF & (font_color >> 16),
			0xFF & (font_color >> 8),
			0xFF & (font_color));
		main_surface->DrawRectangle(main_surface, x, y, width, height);
	}	
}

static IDirectFBWindow	*back_ground_window	= NULL;

void lcd_set_background_picture(const char * pszPictureFileName){
	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	IDirectFBImageProvider *provider;
	IDirectFBSurface		*imgsurf = NULL;
	IDirectFBSurface		*windowSurf = NULL;
	DFBSurfaceDescription	 dsc;
	DFBResult				 err;
	if (NULL == back_ground_window){
		if (0 != layer->GetConfiguration(layer, &config)){
			printf("GetConfiguration Failed\r\n");
		}
		else {
			desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
			desc.posx	= 0;
			desc.posy	= sys_get_status_bar_height();
			desc.width  = config.width;
			desc.height = config.height - desc.posy;
			desc.caps	= DWCAPS_ALPHACHANNEL;
			if (0 !=  layer->CreateWindow(layer, &desc, &back_ground_window)){
				printf("Create Window Failed\r\n");	
			}else{
				back_ground_window->SetOptions(back_ground_window, DWOP_ALPHACHANNEL);
				back_ground_window->SetOpacity(back_ground_window, 0xFF);
				back_ground_window->GetSurface(back_ground_window, &windowSurf);
			}
		}
		
	}

	if (NULL != back_ground_window){
		 err = dfb->CreateImageProvider(dfb, pszPictureFileName, &provider);
		 if (err != DFB_OK) {
			  printf( "Couldn't load image from file '%s': %s\n",
					  pszPictureFileName, DirectFBErrorString( err ));
		 }else{
			provider->GetSurfaceDescription(provider, &dsc);
		 	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
		 	dsc.pixelformat = DSPF_ARGB;
		 	if (dfb->CreateSurface(dfb, &dsc, &imgsurf) == DFB_OK)
			  provider->RenderTo(provider, imgsurf, NULL);

			windowSurf->StretchBlit (windowSurf,imgsurf, NULL, NULL);
			
			imgsurf->Release(imgsurf);
			imgsurf = NULL;
		 	provider->Release(provider);
			provider = NULL;
		 }
	}
	
	if (NULL != windowSurf){
		windowSurf->Release(windowSurf);
		windowSurf = NULL;
	}
}

void lcd_clear_background_picture(void){
	window->SetOpacity(window, 0xFF);
	window->RaiseToTop(window);
	lcd_flip();
	if (back_ground_window != NULL){
		back_ground_window->Release(back_ground_window);
		back_ground_window = NULL;
	}
}


IDirectFBSurface * __SCREEN_SAVED = NULL;
void lcd_save(void){
	DFBSurfaceDescription	surfdesc;

	if (NULL == __SCREEN_SAVED){
		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		main_surface->GetSize(main_surface, &surfdesc.width, &surfdesc.height);
		dfb->CreateSurface(dfb, &surfdesc, &__SCREEN_SAVED);
	}

	if (NULL != __SCREEN_SAVED)
		__SCREEN_SAVED->Blit(__SCREEN_SAVED, main_surface, NULL, 0, 0);
}
	
void lcd_restore(void){
	if (NULL != __SCREEN_SAVED)
		main_surface->Blit(main_surface, __SCREEN_SAVED, NULL, 0, 0);
}


void lcd_show_picture(const char * pszPictureFileName){
	IDirectFBImageProvider *provider;
	IDirectFBSurface		*imgsurf = NULL;

	DFBSurfaceDescription	 dsc;
	DFBResult				 err;
	
		

	 err = dfb->CreateImageProvider(dfb, pszPictureFileName, &provider);
	 if (err != DFB_OK) {
		  printf( "Couldn't load image from file '%s': %s\n",
				  pszPictureFileName, DirectFBErrorString( err ));
	 }else{
		provider->GetSurfaceDescription(provider, &dsc);
	 	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	 	dsc.pixelformat = DSPF_ARGB;
	 	if (dfb->CreateSurface(dfb, &dsc, &imgsurf) == DFB_OK)
		  provider->RenderTo(provider, imgsurf, NULL);

		main_surface->StretchBlit (main_surface,imgsurf, NULL, NULL);
		main_surface->Flip(main_surface, NULL, 0);
		imgsurf->Release(imgsurf);
		imgsurf = NULL;
	 	provider->Release(provider);
		provider = NULL;
	 }

}

void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color){
	main_surface->SetColor(main_surface,
			0xFF & (color >> 24),
			0xFF & (color >> 16),
			0xFF & (color >> 8),
			0xFF & (font_color));
	y = y - sys_get_status_bar_height();
	main_surface->DrawLine(main_surface, x, y, x+1, y+1);
	
}
	


void lcd_flip(void){
	main_surface->Flip(main_surface, NULL, 0);
}


int lcd_menu(const char * pszTitle, const char menu[][100], unsigned int count, int select){
	int retval = -1;

	int key;
	unsigned int i;
	int bLoop;
	IDirectFBFont * font = NULL;
	unsigned int oldcolor = 0;


	unsigned int     max_lines = 0;
	unsigned int     istart = 0;
	
	int screen_width, screen_height, font_height;
	
	main_surface->GetSize(main_surface, &screen_width, &screen_height);
	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);

	if (select < 0)
		select = 0;

	if (count <= 0)
		retval = -1;
	else{
		led_set_brightness(LED_ARROW_DOWN,LED_BRIGHTNESS_MAX);	
		led_set_brightness(LED_ARROW_UP,LED_BRIGHTNESS_MAX);
			
		bLoop = 1;
		while (1 == bLoop)
		{
			lcd_clean();
			lcd_printf(ALG_CENTER, "************************");
			lcd_printf(ALG_CENTER, pszTitle);
			lcd_printf(ALG_CENTER, "************************");

			max_lines = (screen_height - current_y - 1) / font_height;
			
			
			istart = (select / max_lines) * max_lines; 
			for (i = 0;  i < max_lines; i++)
			{
				if (istart + i < count)
				{
					if (istart + i == select)
					{
						oldcolor = font_color;
						lcd_set_font_color(COLOR_GREEN);
						lcd_draw_rectangle(1, current_y -1, screen_width -1, font_height +1);
						lcd_printf(ALG_LEFT, menu[istart + i]);						
						lcd_set_font_color(oldcolor);
					}
					else
						lcd_printf(ALG_LEFT, menu[istart + i]);
					

				}
				else
				{
					break;
				}
			}
			lcd_flip();	
	LOOP:
			if (kb_hit())
			{
				key = kb_getkey();
			//	printf("key:%02x\r\n", key);
				switch((unsigned int)key)
				{
				case 0xF002:
					select --;
					if (select < 0)
					{
						select = count -1;
					}
					break;
				case 0xF003:
					select ++;
					if (select >= count)
					{
						select = 0;
					}
					break;
				case 0x1B:
					select = -1;
					bLoop = 0;
					break;
				case 0x0D:
					bLoop = 0;
					break;
				default:
					goto LOOP;
				}
			}
			else
			{
				goto LOOP;
			}
			
		}
		
		led_set_brightness(LED_ARROW_DOWN,LED_BRIGHTNESS_MIN);	
		led_set_brightness(LED_ARROW_UP,LED_BRIGHTNESS_MIN);
	}
	retval = select;
	lcd_clean();
	return retval;
}




int kb_hit(void)
{
	DFBWindowEvent  windowEvent;
	DFBResult ret;
	
	while(1){
		if (events->HasEvent(events) == DFB_OK){
			if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
				if (/*windowEvent.type == DWET_KEYDOWN || */ windowEvent.type == DWET_KEYUP)
					return windowEvent.type;
				
				ret = events->GetEvent(events, DFB_EVENT(&windowEvent));
				if(ret){
					DirectFBError("IDirectFBEventBuffer::GetEvent() failed", ret); 
				}
			}
		} else {
			break;
		}
	}

	return 0;
}

 int kb_getkey(void)
{
	DFBWindowEvent windowEvent;
	DFBResult ret;
	while (1){
		events->WaitForEvent(events);
		if ((ret = events->GetEvent(events, DFB_EVENT(&windowEvent))) == DFB_OK) {
			if (windowEvent.type == DWET_KEYUP)
					return windowEvent.key_symbol;
		} else {
			break;
		}
	}

	return 0;
}


