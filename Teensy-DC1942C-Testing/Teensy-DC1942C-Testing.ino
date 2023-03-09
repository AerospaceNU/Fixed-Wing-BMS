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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600)
  while(! Serial); //Waits until Serial is available, we might not want this down the line. 

}

void loop() {
  // put your main code here, to run repeatedly:

}
