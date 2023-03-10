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


/***BMS Register Information***/
//Register Arrays
uint8_t wr_config [TOTAL_ICs][6]; //See LTC6804_wrcfg brief
uint8_t rd_config [TOTAL_ICs][8]; //See LTC6804_rdcfg brief

//Configuration Register Info
bool wr_refon[TOTAL_ICs]; //wr_ parameters are to be written with the next configuration
bool rd_refon[TOTAL_ICs]; //rd_ parameters are the state on the LTC6804 as of the last call of LTC6804_rdcfg

bool wr_swtrd[TOTAL_ICs]; 
bool rd_swtrd[TOTAL_ICs];

bool wr_adcopt[TOTAL_ICs];
bool rd_adcopt[TOTAL_ICs];

bool wr_dcc[TOTAL_ICs][12];
bool rd_dcc[TOTAL_ICs][12];

int wr_dcto[TOTAL_ICs];
int rd_dcto[TOTAL_ICs];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600)
  while(! Serial); //Waits until Serial is available, we might not want this down the line. 

}

void loop() {
  // put your main code here, to run repeatedly:

}

int UVtoVUVConfigReg (float undervoltage) {
  // Converts the desired undervoltage level to the integer used to send to the config register, however,
  // the data cannot be sent directly to the configuration register because the config register stores VUV in 12bits
  // while an int is 16bits
  int VUV = int(undervoltage/(16*0.0001))-1;
  return VUV;

}

float VUVConfigRegtoUV (int VUV) {

  float undervoltage = (float(VUV)+1)/(16*0.0001);
  return undervoltage;
}

int OVtoVOVConfigReg (float overvoltage) {
  // Converts the desired overvoltage level to the integer used to send to the config register, however,
  // the data cannot be sent directly to the configuration register because the config register stores VOV in 12bits
  // while an int is 16bits
  int VOV = int(overvoltage/(16*0.0001))-1;
  return VOV;

}

float VOVConfigRegtoOV (int VOV) {

  float overvoltage = (float(VOV)+1)/(16*0.0001);
  return overvoltage;
}

