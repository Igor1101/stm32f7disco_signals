/*
 * main.c
 *
 *  Created on: Feb 16, 2022
 *      Author: igor
 */
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dcmi.h"
#include "dma2d.h"
#include "eth.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "quadspi.h"
#include "rtc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spdifrx.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"
/*
    Name: exp1.c

    Function: Read sine tabled data into an input buffer
              multiply it by a gain coeficient and write to output buffer.
*/

#include <stdlib.h>
volatile uint32_t RGB565_480x272[65280] = { 0x00000000 };
const uint16_t green_color = 0x07E0;
const uint16_t black_color = 0x0000;

int exp_data(void);
void task_app0(void const * argument)
{
	void app_main(void);
  /* USER CODE BEGIN task_app0 */
  /* Infinite loop */
	app_main();
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END task_app0 */
}
/**
 * fill background
 */
void lcd_background(uint16_t color)
{
	uint32_t n = hltdc.LayerCfg[0].ImageHeight * hltdc.LayerCfg[0].ImageWidth;
		for (int i = 0; i < n; i++) {
			*(__IO uint16_t*) (hltdc.LayerCfg[0].FBStartAdress + (i * 2)) =
					(uint16_t) color;
		}
}
void lcd_col(uint16_t offset, uint16_t color, uint32_t vert_start, uint32_t vert_end)
{
	uint32_t h = hltdc.LayerCfg[0].ImageHeight;
	uint32_t w = hltdc.LayerCfg[0].ImageWidth;

		for (int i = vert_start; i < vert_end; i++) {
			*(__IO uint16_t*) (hltdc.LayerCfg[0].FBStartAdress + (2*(offset+i * w))) =
					(uint16_t) color;
		}
}

void lcd_diag_item(uint16_t offset, int item, uint16_t color)
{
	uint32_t h = hltdc.LayerCfg[0].ImageHeight;
		uint32_t w = hltdc.LayerCfg[0].ImageWidth;
	int item_sz = abs(item);
	uint32_t vert_start, vert_end;
	if(item<0) {
		vert_start = h / 2;
		vert_end = h / 2 + item_sz;
	} else {
		vert_end = h / 2;
		vert_start = h / 2 - item_sz;
		if(vert_start<0)
			vert_start = 0;
	}
	lcd_col(offset, color, vert_start, vert_end);
}

void lcd_diagram(int*arr, size_t sz)
{

	uint32_t h = hltdc.LayerCfg[0].ImageHeight;
	uint32_t w = hltdc.LayerCfg[0].ImageWidth;
	lcd_background(black_color);
	// find max in arr
	int i, max, min;
	for(max = arr[0], min = arr[0], i=0; i<sz; i++) {
		if(arr[i] > max)
			max = arr[i];
		if(arr[i] < min)
			min  = arr[i];
	}
	int nmax = (abs(min) > abs(max))?abs(min):abs(max);
	for(int i=0; i<sz; i++) {
		int rec_item = (int)h * arr[i] / nmax /2;
		lcd_diag_item(i*10, rec_item, green_color);
	}
}
int    Ai[8];
int    Xi[8];
int    result1,result2;
void app_main(void) {

	// memmap video mem
	HAL_LTDC_SetAddress(&hltdc, (uint32_t) &RGB565_480x272, 0);
	lcd_background(black_color);
	__disable_irq();
	// expA
	extern int _sum(int *);			/* Assembly routine */
	int x[2]={0x1234,0x4321};		/* define x1[ ] as global array */
	int s;
	s = _sum (x);
	// expB
	extern void _expb_1(void);
	extern void _expb_2(void);
	extern int _expb_3(int *, int *);
	extern int _expb_4(int *, int *);

	_expb_1();
	_expb_2();
	result1= _expb_3(Ai, Xi);
	result2 = _expb_4(Ai, Xi);
	//exp_data();
	__enable_irq();
}

#define BUF_SIZE    40
const int sineTable[BUF_SIZE]=
    {0x0000,0x000f,0x001e,0x002d,0x003a,0x0046,0x0050,0x0059,
     0x005f,0x0062,0x0063,0x0062,0x005f,0x0059,0x0050,0x0046,
     0x003a,0x002d,0x001e,0x000f,0x0000,0xfff1,0xffe2,0xffd3,
     0xffc6,0xffba,0xffb0,0xffa7,0xffa1,0xff9e,0xff9d,0xff9e,
     0xffa1,0xffa7,0xffb0,0xffba,0xffc6,0xffd3,0xffe2,0xfff1};
int in_buffer[BUF_SIZE];
int out_buffer[BUF_SIZE];
int Gain, Noise,Ac;
static int arr[5] = {-1, 3,-4,8, -7};
int exp_data(void)
{

    int i,j;

    lcd_diagram(arr, sizeof arr / sizeof(int));
    Gain = 0x20;
    Noise = 580;
    Ac=0;
    while (1)
    {
    	Ac++;
    	/* <- set profile point on this line */
        for (i = BUF_SIZE-1; i>= 0; i--)
        {
            j = BUF_SIZE-1-i;
            out_buffer[j] = 0;
            in_buffer[j] = 0;
        }
        for (i = BUF_SIZE-1; i>= 0; i--)
        {
            j = BUF_SIZE-1-i;
            in_buffer[i] = (short int)sineTable[i];/* set breakpoint */
            in_buffer[i] = 0 - in_buffer[i];
            out_buffer[j] = Gain*in_buffer[i] + (rand() / (float)RAND_MAX * Noise)+Ac;
        }

            lcd_diagram(out_buffer, sizeof(out_buffer)/ sizeof(int));

    }   /* <- set probe and profile point on this line */

}

