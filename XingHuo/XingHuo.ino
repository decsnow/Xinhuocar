#include "function.h"
void setup() {
    sssServo.attach(9);
    sssServo.write(90);

    //设定各个引脚的输入输出
    pinMode(irL, INPUT);
    pinMode(irM, INPUT);
    pinMode(irR, INPUT);
    pinMode(irRR, INPUT);
    pinMode(irLL, INPUT);
    pinMode(lidar, INPUT);
    pinMode(mL1, OUTPUT);
    pinMode(mL2, OUTPUT);
    pinMode(mR1, OUTPUT);
    pinMode(mR2, OUTPUT);
    delay(15);  // For sssServo, 舵机准备需要时间
    mode = detectMode();
    irsScan();
    Serial.begin(9600); //串口波特率9600（手机端使用）
}

void loop() {
reve();
}
