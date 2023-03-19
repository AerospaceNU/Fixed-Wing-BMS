/***Libraries****/
#include <LTC68042.h>
#include <UserInterface.h>
#include <Linduino.h>
#include <LT_SPI.h>

/***Definitions***/
//Pin Definitions
#define CS_PIN 10 //This is likely unnessary as arduino likely autoassigns SS to 10. See https://www.pjrc.com/teensy/td_libs_SPI.html the pin definition for LTC6804_CS
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN 13

//Other Definitions
#define TOTAL_ICs 1 
#define BYTE_MASK B0000000011111111

/***BMS Information***/
//Register Arrays
uint8_t wr_config [TOTAL_ICs][6]; //See LTC6804_wrcfg brief
uint8_t rd_config [TOTAL_ICs][8]; //See LTC6804_rdcfg brief
uint16_t rd_cell_codes [TOTAL_ICs][12]; //See LTC6804_rdcv brief

//Configuration Register Info
uint8_t wr_gpiox[TOTAL_ICs]; // Bit 7: None Bit 6: None Bit 5: None Bit 4: GPIO5 Bit 3: GPIO4 Bit 2: GPIO3 Bit 1: GPIO2 Bit 0: GPIO1 
uint8_t rd_gpiox[TOTAL_ICs]; // Some of these might be irrelavent because some of these might change without us sending a new write command

uint8_t wr_refon[TOTAL_ICs]; //wr_ parameters are to be written with the next configuration
uint8_t rd_refon[TOTAL_ICs]; //rd_ parameters are the state on the LTC6804 as of the last call of LTC6804_rdcfg

uint8_t wr_swtrd[TOTAL_ICs]; 
uint8_t rd_swtrd[TOTAL_ICs];

uint8_t wr_adcopt[TOTAL_ICs];
uint8_t rd_adcopt[TOTAL_ICs];

uint16_t wr_vuv[TOTAL_ICs];
uint16_t rd_vuv[TOTAL_ICs];

uint16_t wr_vov[TOTAL_ICs];
uint16_t rd_vov[TOTAL_ICs];

uint8_t wr_dcc[TOTAL_ICs][12];
uint8_t rd_dcc[TOTAL_ICs][12];

uint8_t wr_dcto[TOTAL_ICs];
uint8_t rd_dcto[TOTAL_ICs];

//Cell Voltages
float cell_voltages[TOTAL_ICs][12];

// Auxilary Register Info
float aux_voltages[TOTAL_ICs][5];
float vref2[TOTAL_ICs];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600)
  while(! Serial); //Waits until Serial is available, we might not want this down the line. 

}

void loop() {
  // put your main code here, to run repeatedly:

}

/******** LTC6804 Communication Functions ********/

/**** Configuration Register Functions****/
uint16_t UVtoVUVConfigReg (float undervoltage) {
  // Converts the desired undervoltage level to the integer used to send to the config register, however,
  // the data cannot be sent directly to the configuration register because the config register stores VUV in 12bits
  // while an uint16_t is 16bits
  uint16_t VUV = int(undervoltage/(16*0.0001))-1;
  return VUV;

}

float VUVConfigRegtoUV (uint16_t VUV) {

  float undervoltage = (float(VUV)+1)/(16*0.0001);
  return undervoltage;
}

uint16_t OVtoVOVConfigReg (float overvoltage) {
  // Converts the desired overvoltage level to the integer used to send to the config register, however,
  // the data cannot be sent directly to the configuration register because the config register stores VOV in 12bits
  // while an uint16_t is 16bits
  uint16_t VOV = int(overvoltage/(16*0.0001));
  return VOV;

}

float VOVConfigRegtoOV (uint16_t VOV) {

  float overvoltage = (float(VOV))/(16*0.0001);
  return overvoltage;
}

void CFGGenerator() {
  // Generates configuration register array for sending to LTC6804
  for(uint16_t i = 0; i< TOTAL_ICs; i++) {
    wr_config[i][0] = (((((wr_gpiox[i]<<1)|wr_refon[i]) <<1)|wr_swtrd[i]) <<1)|wr_adcopt[i]; //CFGR0
    wr_config[i][1] = uint8_t(wr_vuv); //Just taking the first 8 bits //CFGR1
    wr_config[i][2] = (uint8_t(wr_vov&B1111)<<4)|(uint8_t(wr_vuv>>8); //CFGR2
    wr_config[i][3] = uint8_t(wr_vov>>8); //CFGR3
    uint16_t dcc_block;
    for (uint16_t j = 0; j<12; j++) {
      dcc_block = (dcc_block<<1)|wr_dcc[i][j];
    };

    wr_config[i][4] = uint8_t(dcc_block); //CFGR4
    wr_config[i][5] = (wr_dcto<<4)|uint8_t(wr_dcc>>8); //CFGR5
  }
}


/**** Cell Voltage Registers Functions ****/
void ReadCellVoltages(uint8_t reg = 0) { // See LTC6804_rdcv for info about reg
  wakeup_idle();
  LTC6804_adcv();
  delay(10);
  wakeup_idle();
  
  int8_t error = LTC6804_rdcv(reg, TOTAL_ICs, rd_cell_codes);

  if (error == -1) {
    Serial.println("PEC error was detected in cell voltage data.");
  }
  
  for (int j = 0; j < TOTAL_ICs;j++) {
    for (int i = 0; i < sizeof(rd_cell_codes); i++) {
      cell_voltages[j][i] = rd_cell_codes[j][i] * 0.0001;
    }
  }
}


/**** Auxilary Registers Functions ****/
void ReadAuxilaryVoltages(uint8_t reg = 0) { // See LTC6804_rdaux brief for info about reg
  wakeup_idle();
  LTC6804_adax();
  delay(10);
  wakeup_idle();
  uint16_t aux_codes[TOTAL_ICs][6];
  int8_t error = LTC6804_rdaux(reg, TOTAL_ICs, aux_codes);

  if (error == -1) {
    Serial.println("PEC error was detected in aux voltage data");
  }

  for (int i = 0; i<TOTAL_ICs;i++) {
    for (int j = 0; j < 6; j++) {
      aux_voltages[i][j] = aux_codes[i][j] * 0.0001;
    }
  }
}

