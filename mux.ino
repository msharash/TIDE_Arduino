#include <CAN.h>
#include <Arduino.h>

/*
volatile float BP1_P, BP2_P, BP3_P, BP4_P, BP5_P, BP6_P;
volatile int16_t BP1_T, BP2_T, BP3_T, BP4_T, BP5_T, BP6_T;
volatile float BP_P_Supply;
volatile int16_t BP_Flow;
volatile float Exch_P, Suct_P, Disch_P;
volatile int16_t Exch_T, Master_T;
volatile float IN1_P, IN2_P, IN_P_Supply;
volatile int16_t IN_T;
volatile int16_t IN_Flow;
volatile float M_P, M_P_Supply;
volatile int16_t M_T, M_Flow;
*/
// floats: 15
//ints: 10
// undef: 3 

const int NumberofSensors 28 
int i = 0;
static int result = 0;
static float fresult = 0;

void setup() {
  // put your setup code here, to run once:
  if (!CAN.begin(250E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  Serial.begin(115200);
  pinMode(A15,OUTPUT);
  pinMode(A14,OUTPUT);
  pinMode(A13,OUTPUT);
  pinMode(A12,OUTPUT);
  pinMode(A11,OUTPUT);
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  setMUXToReadSensor(i);
  delay(10);
  result = analogRead(A0);
  
  sendCANMessage(i, readSensorwithVal(i,result));
  
  
  i++;
  if(i >= NumberofSensors){
    i = 0;
  }
}

void setMUXToReadSensor(int i) {
  digitalWrite(A11, i & 1);
  digitalWrite(A12, i & (1<<1));
  digitalWrite(A13, i & (1<<2));
  digitalWrite(A14, i & (1<<3));
  digitalWrite(A15, i & (1<<4));
}

void* readSensorwithVal(int i, int val) {
  if (i < 15) { 
    fresult = ((float)val * 5 * 2002.9/1023)-14.5; //Scale => [-14.5, 10000]
    return &fresult;
  }else if(i < 25){ // isInt
    val = (val * 5 * 60/1023) - 50;         //Scale => [-50, 200]
    return &val;
  }else{
    //scale for flow here
    return &val;
  }
}

/*-----------------------------------------------------
400000000 - 400000014 is Pressure Sensors
400000000:  BP_1
400000001:  BP_2
400000002:  BP_3
400000003:  BP_4
400000004:  BP_5
400000005:  BP_6
400000006:  BP_Supply
400000007:  Exch
400000008:  Suct
400000009:  Disch
400000010:  Inverter_1
400000011:  Inverter_2
400000012:  Inverter_Supply
400000013:  Motor
400000014:  Motor_Supply

400000015 - 400000024 is Temp Sensor
400000015: BP_1
400000016: BP_2
400000017: BP_3
400000018: BP_4
400000019: BP_5
400000020: BP_6
400000021: Exch
400000022: Master
400000023: Inverter
400000024: Motor

400000025 - 400000027 is Flow
400000025: BP
400000026: Inverter
400000027: Motor
-------------------------------------------------------*/
void sendCANMessage(int i, void* muxOut) {
  CAN.beginExtendedPacket(400000000+i);
  
  if(i < 15){ //isFloat
    float val = *(float *)muxOut;          //Dereference

    char data[sizeof(val)];                //Create char array
    memcpy(data, &val, sizeof(val));       //Store bytes of val to array
    for(int j = 0; j < sizeof(val); j++){  //Write bytes one by one to CAN
      CAN.write(data[j]);
    }
  }else if(i < 25){ //isInt
    int val = *(int *)muxOut;              //Dereference

    char data[sizeof(val)];                //Create char array
    memcpy(data, &val, sizeof(val));       //Store bytes of val to array
    for(int j = 0; j < sizeof(val); j++){  //Write bytes one by one to CAN
      CAN.write(data[j]);
    }
  }else{ //isInt??
    int val = *(int *)muxOut;              //Dereference

    char data[sizeof(val)];                //Create char array
    memcpy(data, &val, sizeof(val));       //Store bytes of val to array
    for(int j = 0; j < sizeof(val); j++){  //Write bytes one by one to CAN
      CAN.write(data[j]);
    }
  }

  CAN.endPacket();
}
