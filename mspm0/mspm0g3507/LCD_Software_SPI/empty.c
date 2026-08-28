/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "lcd.h"
#include "lcd_init.h"
int main(void)
{
    SYSCFG_DL_init();
		LCD_Init();//屏幕初始化
    LCD_Fill(0,0,LCD_W,LCD_H,BLACK);//清全屏为黑色
		float t = 0;
    while (1) {
			LCD_ShowString(0,16*2,(uint8_t *)"LCD_W:",WHITE,BLACK,16,0);
			LCD_ShowIntNum(48,16*2,LCD_W,3,WHITE,BLACK,16);
			LCD_ShowString(80,16*2,(uint8_t *)"LCD_H:",WHITE,BLACK,16,0);
			LCD_ShowIntNum(128,16*2,LCD_H,3,WHITE,BLACK,16);

			LCD_ShowString(0,16*3,(uint8_t *)"Nun:",WHITE,BLACK,16,0);
			LCD_ShowFloatNum1(8*4,16*3,t,4,WHITE,BLACK,16);
			t+=0.11;
			delay_ms(1000);
    }
}
