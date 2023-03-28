#include <stdio.h>
#include "fonts.h"
#include "ssd1306.h"
#include "nrf24.h"

extern char nrf_str1[];
void NRF_displ_config(void);

void home_screen(void){
  int8_t temp;
  temp = SSD1306_Init();      // Return OK = 1
  
  if(temp){
    SSD1306_GotoXY(10,2);
    SSD1306_Puts("www.graviton.com.ua", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(15,15);
    SSD1306_Puts("Khloponin", &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(25,35);
    SSD1306_Puts("NRF24", &Font_16x26, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen(); //display
    HAL_Delay(1000);
    SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
    SSD1306_GotoXY(5,15);
    SSD1306_Puts("nRF24 init", &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen(); //display
    temp = NRF_ini();                   // Return OK = 0
    if(temp){                           // если присутствуют ошибки тестовая индикация состояний регистров
      SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
      NRF_displ_config();
      sprintf(nrf_str1,"error: 0x%02X",temp);
      SSD1306_GotoXY(10,50); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
      SSD1306_UpdateScreen();
      HAL_Delay(10000);
    }
    else {
      SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
      SSD1306_GotoXY(15,15);
      SSD1306_Puts("nRF24 Ok!", &Font_11x18, SSD1306_COLOR_WHITE);
      SSD1306_UpdateScreen(); //display
      HAL_Delay(1000);
    }
  }
}

void NRF_displ_config(void){
  uint8_t temp;
  temp = NRF_RDreg(CONFIG);
  SSD1306_GotoXY(0,0);
  sprintf(nrf_str1,"R:%02X = 0x%02X=%3u",CONFIG,temp,temp);
  SSD1306_Puts(nrf_str1,&Font_7x10, SSD1306_COLOR_WHITE);
//  SSD1306_UpdateScreen(); //display
  temp = NRF_RDreg(RF_CH);
  SSD1306_GotoXY(0,10);
  sprintf(nrf_str1,"R:%02X = 0x%02X=%3u",RF_CH,temp,temp);
  SSD1306_Puts(nrf_str1,&Font_7x10, SSD1306_COLOR_WHITE);
//  SSD1306_UpdateScreen(); //display
  temp = NRF_RDreg(RX_PW_P0);
  SSD1306_GotoXY(0,20);
  sprintf(nrf_str1,"R:%02X = 0x%02X=%3u",RX_PW_P0,temp,temp);
  SSD1306_Puts(nrf_str1,&Font_7x10, SSD1306_COLOR_WHITE);
//  SSD1306_UpdateScreen(); //display
  temp = NRF_RDreg(RX_PW_P1);
  SSD1306_GotoXY(0,30);
  sprintf(nrf_str1,"R:%02X = 0x%02X=%3u",RX_PW_P1,temp,temp);
  SSD1306_Puts(nrf_str1,&Font_7x10, SSD1306_COLOR_WHITE);
//  SSD1306_UpdateScreen(); //display
  temp = NRF_RDreg(SETUP_RETR);
  SSD1306_GotoXY(0,40);
  sprintf(nrf_str1,"R:%02X = 0x%02X=%3u",SETUP_RETR,temp,temp);
  SSD1306_Puts(nrf_str1,&Font_7x10, SSD1306_COLOR_WHITE); 
//  SSD1306_UpdateScreen(); //display
//  HAL_Delay(1000);
}
