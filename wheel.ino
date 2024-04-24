#include <Arduino.h>
#define TrimUpPin 36
#define TrimDownPin 39
#define GainUpPin 34
#define GainDownPin 35
#define EStopPin 32
#define EncoderPin 33
#define ThrottlePin 25
#define LED1 26
#define LED2 27
#define LED3 14
#define LED4 12
#define LED5 13
#define LED6 4

#include <CAN.h>

float encoderVal = 0;
float throttleVal = 0;
uint16_t i = 0;

void setup() {
  Serial.begin(9600);

  while(!Serial){}

  Serial.println("CAN mcu");

  pinMode(GainDownPin, INPUT);
  pinMode(EStopPin, INPUT);
  pinMode(EncoderPin, INPUT);
  pinMode(ThrottlePin, INPUT);
  pinMode(LED1, OUTPUT);

  if(!CAN.begin(500E3)){
    Serial.println("Starting CAN fail");
    while(1);
  }
}

void loop() {
  encoderVal = readEncoder();
  Serial.print(encoderVal);
  encodertoCAN(encoderVal);

  throttleVal = readThrottle();
  throttletoCAN(throttleVal);

  buttonstoCAN();
}

float readEncoder(){
  return (float)analogRead(EncoderPin)*360./4095.; //Scale => [0,360]
}

float readThrottle(){
  return (float)analogRead(ThrottlePin)*360./4095.; //Scale => [0,360]
}

void encodertoCAN(float val){
  CAN.beginPacket(1999);
  char data[sizeof(val)];               //Create char array
  memcpy(data, &val, sizeof(val));       //Store bytes of val to array
  for(int j = 0; j < sizeof(val); j++){  //Write bytes one by one to CAN
    CAN.write(data[j]);
  }
  CAN.endPacket();
}

void throttletoCAN(float val){
  CAN.beginPacket(1998);
  char data[sizeof(val)];                //Create char array
  memcpy(data, &val, sizeof(val));       //Store bytes of val to array
  for(int j = 0; j < sizeof(val); j++){  //Write bytes one by one to CAN
    CAN.write(data[j]);
  }
  CAN.endPacket();
}

/*----------------------------------
IDs
2000
-----------------------------------*/
void buttonstoCAN(){
  CAN.beginPacket(2000);
  CAN.write(analogRead(TrimUpPin));
  CAN.write(analogRead(TrimDownPin));
  CAN.write(analogRead(GainUpPin));
  CAN.write(analogRead(GainDownPin));
  CAN.write(analogRead(EStopPin));
  CAN.endPacket();
}
