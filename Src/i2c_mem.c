#include "main.h"
#include "ssd1306.h"
#include <string.h>

extern const char wmsg[];
extern char rmsg[], len, writing0[], writing1[];
extern uint16_t devAddr; // HAL expects address to be shifted one bit to the left
uint16_t memAddr = 0x0000;
uint16_t ret_stat= 0;

void dspl_error(uint8_t status)
{
  SSD1306_GotoXY(0,40);
  switch(status)
   {
     case HAL_OK:
         SSD1306_Puts("Ok!", &Font_11x18, SSD1306_COLOR_WHITE); break;
     case HAL_ERROR:
         SSD1306_Puts("Error", &Font_11x18, SSD1306_COLOR_WHITE); break;
     case HAL_BUSY:
         SSD1306_Puts("Busy", &Font_11x18, SSD1306_COLOR_WHITE); break;
     case HAL_TIMEOUT:
         SSD1306_Puts("TimeOut", &Font_11x18, SSD1306_COLOR_WHITE); break;
   }
  SSD1306_UpdateScreen();
  HAL_Delay(2000);
}

void mem_display(uint16_t wait, uint16_t begin, char* str0, char* str1){
  SSD1306_Fill(SSD1306_COLOR_BLACK); //clear oled
  if (begin == 0){
  SSD1306_GotoXY(0,begin+0);
  SSD1306_Puts(str0, &Font_11x18, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(0,begin+20);
  SSD1306_Puts(str1, &Font_11x18, SSD1306_COLOR_WHITE);
  }
  else {
  SSD1306_GotoXY(0,begin+0);
  SSD1306_Puts(str0, &Font_7x10, SSD1306_COLOR_WHITE);
  SSD1306_GotoXY(0,begin+10);
  SSD1306_Puts(str1, &Font_7x10, SSD1306_COLOR_WHITE);
  }
  SSD1306_UpdateScreen();
  HAL_Delay(wait);
}


void mem_init() {
  char i;
//  ret_stat = HAL_I2C_IsDeviceReady(&EEPROM_I2C_PORT, devAddr, 1, HAL_MAX_DELAY);    // #1
//  if(ret_stat) {dspl_error(ret_stat); return;}
/*  
  sprintf(writing0, "Start");//"Starting the test - writing to the memory...\r\n"
  sprintf(writing1, "writing");
  mem_display(1000, (char*)writing0, (char*)writing1);
    
	// Hint: try to comment this line!
  ret_stat = HAL_I2C_Mem_Write(&EEPROM_I2C_PORT, devAddr, memAddr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)wmsg, sizeof(wmsg), HAL_MAX_DELAY);
  if(ret_stat) dspl_error(ret_stat);
*/  
  sprintf(writing0, "Waiting");//"OK, now waiting until device is ready...\r\n"
  sprintf(writing1, "is ready");
  mem_display(1000, 0, (char*)writing0, (char*)writing1);
  
	for(i=0;i<100;i++) { // wait...
		ret_stat = HAL_I2C_IsDeviceReady(&EEPROM_I2C_PORT, devAddr, 1, HAL_MAX_DELAY);//#2
		if(ret_stat == HAL_OK) break;
	}
  if (i>99) {
    sprintf(writing0, "Device");//"Done, now comparing...\r\n"
    sprintf(writing1, "notReady");
    mem_display(2000, 0, (char*)writing0, (char*)writing1);
    return;
  }

  sprintf(writing0, "Start");//"Device is ready, now reading...\r\n"
  sprintf(writing1, "reading");
  mem_display(1000, 0, (char*)writing0, (char*)writing1);

  ret_stat = HAL_I2C_Mem_Read(&EEPROM_I2C_PORT, devAddr, memAddr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)rmsg, len, HAL_MAX_DELAY);
  if(ret_stat) dspl_error(ret_stat);
  
  sprintf(writing0, "Now");//"Done, now comparing...\r\n"
  sprintf(writing1, "comparing");
  mem_display(1000, 0, (char*)writing0, (char*)writing1);

  ret_stat = memcmp(rmsg, wmsg, len);
  if(memcmp(rmsg, wmsg, len) == 0) {
	sprintf(writing0, "Test");//"Done, now comparing...\r\n"
    sprintf(writing1, "passed!");
    mem_display(2000, 0, (char*)writing0, (char*)writing1);
  }
  else {
	sprintf(writing0, "Test");//"Done, now comparing...\r\n"
    sprintf(writing1, "failed");
    mem_display(2000, 0, (char*)writing0, (char*)writing1);
//		char tmp_buff[64];
//		snprintf(tmp_buff, sizeof(tmp_buff), "rmsg = '%s'\r\n", rmsg);
//		HAL_UART_Transmit(&huart2, (uint8_t*)tmp_buff, strlen(tmp_buff), HAL_MAX_DELAY);
	}
  mem_display(2000, 30, (char*)wmsg, (char*)rmsg);
}
