#include "nrf24.h"

//------------------------------------------------
#define NRF_SPI_PORT hspi1
extern SPI_HandleTypeDef NRF_SPI_PORT;
extern char nrf_str1[20];
extern uint8_t val;
extern union 
 {
    uint8_t nrf_buf1[20];
    uint16_t valT;
 } data;
//unsigned char NRFptx, TX_ADDRESS[TX_ADR_WIDTH] = {0xb3,0xb4,0x01};
//unsigned char RX_BUF[TX_PLOAD_WIDTH] = {0};
//------------------------------------------------
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
  micros *= (SystemCoreClock / 1000000) / 9;
  while (micros--) ;    // Wait till done
}
//--- Считываем 1 байт по указанному адресу --------------------------------------------
uint8_t NRF_RDreg(uint8_t addr)
{
  unsigned char dt=0;
  NRF_CSN_lo;
  HAL_SPI_TransmitReceive(&NRF_SPI_PORT,&addr,&dt,1,1000);    // отправим адрес в шину
  if (addr!=STATUS || addr!=FLUSH_RX || addr!=FLUSH_TX)       // если это НЕ адрес "STATUS","FLUSH_RX" или "FLUSH_TX" то ...
   {
    addr=0xFF;
    HAL_SPI_TransmitReceive(&NRF_SPI_PORT,&addr,&dt,1,1000);  // возварщаем его значение.
   }
  NRF_CSN_hi;
  return dt;
}
//--- Reads an array of bytes from the given start position in the MiRF registers. ---
void NRF_read_data(uint8_t addr, uint8_t *pBuf, uint8_t bytes)
{
  NRF_CSN_lo;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);      // отправим адрес в шину
  HAL_SPI_Receive(&hspi1,pBuf,bytes,1000);    // прием данных в буфер
  NRF_CSN_hi;
}
//--- Записываем 1 байт по указанному адресу ---------------------------------------------
void NRF_WRreg_byte(uint8_t addr, uint8_t dt)
{
  addr |= W_REGISTER;//включим бит записи в адрес
  NRF_CSN_lo;
  HAL_SPI_Transmit(&NRF_SPI_PORT,&addr,1,1000);     //отправим адрес в шину
  HAL_SPI_Transmit(&NRF_SPI_PORT,&dt,1,1000);       //отправим данные в шину
  NRF_CSN_hi;
}
//------------------------------------------------
void NRF_WRreg_data(unsigned char addr,unsigned char *pBuf,unsigned char bytes)
{
  addr |= W_REGISTER;//включим бит записи в адрес
  NRF_CSN_lo;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);    //отправим адрес в шину
  HAL_SPI_Transmit(&hspi1,pBuf,bytes,1000); //отправим данные из буфера
  NRF_CSN_hi;
}
//------------------------------------------------
//void NRF_Mode(unsigned char mode)
//{
//  unsigned char regval;
//  NRF_CE_lo;                            // Deactivate transreceiver
//  regval = NRF_RDreg(CONFIG);
//  if(mode==MODE_PRX)
//   {
//     NRFptx = 0;                        // Set to receiving mode
//     regval |= (1<<PRIM_RX);
//   }
//  else
//   {
//     NRFptx = 1;                        // Set to transmitter mode
//     regval &= (0<<PRIM_RX);
//   }
//  regval |= (1<<PWR_UP);
//  NRF_WRreg_byte(CONFIG,regval);
//  NRF_CE_hi;                            // Listening for pakets
//  DelayMicro(150); //Задержка минимум 130 мкс
//  // Flush buffers
//  NRF_RDreg(FLUSH_RX);
//  NRF_RDreg(FLUSH_TX);
//}
//-----------------------------------------------
uint8_t NRF_ini(void)
{
 uint8_t errs=0; 
  NRF_CE_lo;                            // Deactivate transreceiver
  NRF_WRreg_byte(CONFIG, CONFIGDATA);   // режим приёмника 0x0b
//  if(NRF_RDreg(CONFIG)!= CONFIGDATA) errs|=1;   // неисправен chip
  
  DelayMicro(5000);                     // Дадим модулю включиться, по даташиту около 1,5 мсек, а лучше 5

//  NRF_WRreg_byte(EN_RXADDR, 0x03);                          // Разрешает прием данных по каналам Pipe0; Pipe1 (задано по умолчанию).
//  NRF_WRreg_byte(SETUP_AW, 0x01);                           // длина поля адреса 3-байта
//  NRF_WRreg_byte(EN_AA, (1 << ENAA_P0));                    // включим автоматическую отправку подтверждения приёма данных на P0
  NRF_WRreg_byte(SETUP_RETR, (SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT));                           
  if(NRF_RDreg(SETUP_RETR)!= (SETUP_RETR_DELAY_250MKS | SETUP_RETR_UP_TO_2_RETRANSMIT)) errs++;
  NRF_WRreg_byte(RF_CH, 50);                                  // частота 2400+50 MHz
  if(NRF_RDreg(RF_CH)!= 50) errs|=2;
//  NRF_WRreg_byte(RF_SETUP, 0x00);                           // TX_PWR:-18dBm,     Datarate:1Mbps
//  NRF_WRreg_data(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);        // Адрес удалённой стороны
//  NRF_WRreg_data(RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);     // Адрес удалённой стороны
//  NRF_WRreg_data(RX_ADDR_P1, RX_ADDRESS, TX_ADR_WIDTH);     // Собственный адрес
  NRF_WRreg_byte(RX_PW_P0, TX_PLOAD_WIDTH);                   // 12 bytes in RX payload in data pipe 0
  if(NRF_RDreg(RX_PW_P0)!= TX_PLOAD_WIDTH) errs|=4;
  NRF_WRreg_byte(RX_PW_P1, TX_PLOAD_WIDTH);                   // 12 bytes in RX payload in data pipe 1
  if(NRF_RDreg(RX_PW_P1)!= TX_PLOAD_WIDTH) errs|=8;
  NRF_RDreg(FLUSH_RX);
  NRF_WRreg_byte(STATUS, 0x70);                               // Reset flags for IRQ

  NRF_CE_hi;                            // Listening for pakets
  LED_ON;
  return errs;
}
/*
//------------------------------------------------
void NRF_displ_config(void){
 //-------------------------1--------------------------------------------------
    SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
//    sprintf(nrf_str1,"ERRORS NRF %2u",val); SSD1306_GotoXY(0,0); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
//    NRF_displ_reg(CONFIG    , 0,10, &Font_7x10);
//    NRF_displ_reg(RF_CH     , 0,20, &Font_7x10);
//    NRF_displ_reg(RX_PW_P0  , 0,30, &Font_7x10);
//    NRF_displ_reg(RX_PW_P1  , 0,40, &Font_7x10);
//    NRF_displ_reg(SETUP_RETR, 0,50, &Font_7x10);
    SSD1306_UpdateScreen();

    HAL_Delay(5000);                   // 5 sec.
//--------------------------2---------------------------------------------------- 
    SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
    NRF_displ_reg(EN_AA     , 0, 5, &Font_7x10);
    NRF_displ_reg(EN_RXADDR , 0,20, &Font_7x10);
    NRF_displ_reg(SETUP_AW  , 0,35, &Font_7x10);
    NRF_displ_reg(SETUP_RETR, 0,50, &Font_7x10);
    SSD1306_UpdateScreen();
    HAL_Delay(5000);                   // 5 sec.
//-------------------------------------------------------------------------------    
    SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
    NRF_read_data(TX_ADDR, data.nrf_buf1, 5);
    sprintf(nrf_str1,"TX_ADDR:");
    SSD1306_GotoXY(0,5);
    SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
//  HAL_UART_Transmit(&huart1,(uint8_t*)nrf_str1,strlen(nrf_str1),0x1000);
    
    sprintf(nrf_str1,"%02X,%02X,%02X,%02X,%02X",data.nrf_buf1[0],data.nrf_buf1[1],data.nrf_buf1[2],data.nrf_buf1[3],data.nrf_buf1[4]);
    SSD1306_GotoXY(0,20);
    SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
//  HAL_UART_Transmit(&huart1,(uint8_t*)nrf_str1,strlen(nrf_str1),0x1000);
    NRF_read_data(RX_ADDR_P0, data.nrf_buf1, 5);
    sprintf(nrf_str1,"RX_ADDR_P0:");
    SSD1306_GotoXY(0,35);
    SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
//  HAL_UART_Transmit(&huart1,(uint8_t*)nrf_str1,strlen(nrf_str1),0x1000);
    
    sprintf(nrf_str1,"%02X,%02X,%02X,%02X,%02X",data.nrf_buf1[0],data.nrf_buf1[1],data.nrf_buf1[2],data.nrf_buf1[3],data.nrf_buf1[4]);
    SSD1306_GotoXY(0,50);
    SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen();
    HAL_Delay(5000);                   // 5 sec.

}

void NRF24L01_Receive(void)
{
  uint8_t status=0x01;
  uint16_t dt=0;
//  while((GPIO_PinState)IRQ == GPIO_PIN_SET) {}
  status = NRF_read_register(STATUS);
  sprintf(nrf_str1,"CONFIG: 0x%02X",dt_reg);
  SSD1306_GotoXY(0,5);
  SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
  //LED_TGL;
  DelayMicro(10);
  status = NRF_read_register(STATUS);
  if(status & 0x40)
  {
    NRF_read_data(RD_RX_PLOAD,RX_BUF,TX_PLOAD_WIDTH);
    dt = *(int16_t*)RX_BUF;
    
    dt = *(int16_t*)(RX_BUF+2);
    
    NRF_WRreg_byte(STATUS, 0x40);
  }
}
*/
