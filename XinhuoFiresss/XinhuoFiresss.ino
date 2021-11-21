#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>

Servo sssServo; 
Servo gunServo;
int pos = 0;
#define sss 14 //A0
#define sign 17
#define scansign 18
#define chargePin 15
#define firePin 16
#define sound 13

double sssRead(int sssPin) {
    float distance = 0;
    pinMode(sssPin, OUTPUT);
    digitalWrite(sssPin, LOW);
    delayMicroseconds(2);
    digitalWrite(sssPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sssPin, LOW);
    pinMode(sssPin, INPUT);
    distance = pulseIn(sssPin, HIGH) / 58.00;
    delay(40);
    if (distance >= 80)  // 受限于这个传感器的精度与原理, 太远的距离没有实际意义
        distance = 80;
    return distance;
}

double sssDist(int pin) {
    double result = 0;
    result += sssRead(pin);
    result += sssRead(pin);
    result += sssRead(pin);

    return result / 3;
}


void beginscan() {
  while(digitalRead(scansign)==LOW){
    
    }
}

void mimo() {
  for(int i=0;i<=1;i++){
  for(int i=600;i<=1000;i=i+(5)){
tone(sound,i);
delay(8);
noTone(sound);
  }
  }
  }


void setup() {
Serial.begin(9600);  
sssServo.attach(11);
gunServo.attach(10);
gunServo.write(90);
sssServo.write(90);
pinMode(sss, OUTPUT);
pinMode(sign, OUTPUT);
pinMode(scansign, INPUT);
pinMode(chargePin, OUTPUT);
pinMode(firePin, OUTPUT);
pinMode(sound,OUTPUT);
}

void loop() {

beginscan();

AGAIN:  
  for (pos = 0; pos <= 180; pos +=40) { // 0°到180°
    // in steps of 1 degree
    sssServo.write(pos);              // 舵机角度写入
                      // 等待转动到指定角度
    double dist=sssDist(sss);
    Serial.print(dist);
    Serial.print("\n");
    if(dist<=50){
      gunServo.write(pos);
      goto FIRE;
      }
    }
  
  
  for (pos = 180; pos >= 0; pos -=40) { // 从180°到0°
    sssServo.write(pos);              // 舵机角度写入
    
    double dist=sssDist(sss);
    Serial.print(dist);
    Serial.print("\n");
    if(dist<=50){
      gunServo.write(pos);
      goto FIRE;
      }
}
goto AGAIN;


FIRE:
mimo();
noTone(sound);
tone(sound,1000);
digitalWrite(chargePin,HIGH);
delay(10000);
digitalWrite(chargePin,LOW);
delay(500);
noTone(sound);
digitalWrite(firePin,HIGH);
delay(500);
digitalWrite(firePin,LOW);
delay(500);
gunServo.write(90);
sssServo.write(90);
}
