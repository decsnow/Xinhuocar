//星火杯小车,191015开始,modified at 20211104 by decsnow

//sss => Super Sonic Sensor
//irs => InfraRed Sensor

#include <Servo.h>
//#define debug
Servo sssServo;                                              //sssServo => D9
short int irL = 8, irM = 7, irR = 10, irLL = 17, irRR = 11;  //17 - A3
short int sssL = 15, sssF = 14, sssR = 16;                   //14 - A0  15 - A1  16 - A2
short int mL1 = 2, mL2 = 3, mR1 = 12, mR2 = 13;
short int mL = 5, mR = 6;
short int lS = 160, rS = 160;
bool irS[5];                             //IR Sensor
double sssS[7];                          //Super Sonic Sensor Scan
short int mode = 1;                      //1: 循迹 0: 魔术手 2:自动避障
short int modePinA = 18, modePinB = 19;  //18 - A4  19 - A5
short int lidar = 14; //激光雷达结果输入口
short int scansign = 11;//开始扫描输出


/**
 * 读取超声波传感器数据
 */
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

/**
 * 检测工作模式 
 */
int detectMode() {
    pinMode(modePinA, INPUT_PULLUP);
    pinMode(modePinB, INPUT_PULLUP);
    if (digitalRead(modePinA) && digitalRead(modePinB))  //A+ B+
        return 1;
    else if (digitalRead(modePinA) && !digitalRead(modePinB))  //A+ B-
        return 0;
    else
        return 2;
}

/**.
 * 读取三次超声波传感器数据并取平均值
 */
double sssDist(int pin) {
    double result = 0;
    result += sssRead(pin);
    result += sssRead(pin);
    result += sssRead(pin);

    return result / 3;
}

/**
 * 前置超声波传感器进行扇形扫描
 */
void sssScanAll() {
    sssServo.write(30);
    delay(100);
    sssS[0] = sssDist(sssF);

    sssServo.write(50);
    delay(100);
    sssS[1] = sssDist(sssF);

    sssServo.write(70);
    delay(100);
    sssS[2] = sssDist(sssF);

    sssServo.write(90);
    delay(100);
    sssS[3] = sssDist(sssF);

    sssServo.write(110);
    delay(100);
    sssS[4] = sssDist(sssF);

    sssServo.write(130);
    delay(100);
    sssS[5] = sssDist(sssF);

    sssServo.write(150);
    delay(100);
    sssS[6] = sssDist(sssF);

    //回正到正前方
    sssServo.write(90);
}

void sssScanFwd() {
    sssServo.write(90);
    delay(10);
    sssS[3] = sssDist(sssF);
}

/**
 * 前进
 */
void moveFwd() {
    digitalWrite(mL1, LOW);
    digitalWrite(mL2, HIGH);
    digitalWrite(mR1, LOW);
    digitalWrite(mR2, HIGH);
    analogWrite(mL, lS);
    analogWrite(mR, rS);
}

/**
 * 左转
 */
void moveLeft() {
    digitalWrite(mL1, HIGH);
    digitalWrite(mL2, LOW);
    digitalWrite(mR1, LOW);
    digitalWrite(mR2, HIGH);
    analogWrite(mL, 150);
    analogWrite(mR, 230);
}

/**
 * 右转
 */
void moveRight() {
    digitalWrite(mL1, LOW);
    digitalWrite(mL2, HIGH);
    digitalWrite(mR1, HIGH);
    digitalWrite(mR2, LOW);
    analogWrite(mL, 230);
    analogWrite(mR, 150);
}

/**
 * 停止
 */
void moveStop() {
    digitalWrite(mL1, HIGH);
    digitalWrite(mL2, HIGH);
    digitalWrite(mR1, HIGH);
    digitalWrite(mR2, HIGH);
    analogWrite(mL, 255);
    analogWrite(mR, 255);
}

/**
 * 后退
 */
void moveBack() {
    digitalWrite(mL1, HIGH);
    digitalWrite(mL2, LOW);
    digitalWrite(mR1, HIGH);
    digitalWrite(mR2, LOW);
    analogWrite(mL, lS);
    analogWrite(mR, rS);
}

void swingLeft() {
    digitalWrite(mL1, LOW);
    digitalWrite(mL2, HIGH);
    digitalWrite(mR1, LOW);
    digitalWrite(mR2, HIGH);
    analogWrite(mL, lS / 2);
    analogWrite(mR, rS);
}

void swingRight() {
    digitalWrite(mL1, LOW);
    digitalWrite(mL2, HIGH);
    digitalWrite(mR1, LOW);
    digitalWrite(mR2, HIGH);
    analogWrite(mL, lS);
    analogWrite(mR, rS / 2);
}

/**
 * 前置红外扫描
 */
void irsScan() {
    irS[0] = digitalRead(irLL);
    irS[1] = digitalRead(irL);
    irS[2] = digitalRead(irM);
    irS[3] = digitalRead(irR);
    irS[4] = digitalRead(irRR);
}

/**
 * 根据红外传感器的数据进行移动
 */
int move_IR() {
    // 1 - line down; 若下方有线则为 1
    // 面对车尾, 从左到右红外传感器编号为 0~5
    if (irS[1] == 0 && irS[2] == 1 && irS[3] == 0) {  // 仅中间 (即正下方) 有线
        moveFwd();
        return 0;
    } else if (irS[1] == 1 && irS[3] == 0) {  // 左侧有线, 右侧无
        moveLeft();
    } else if (irS[1] == 0 && irS[3] == 1) {  // 右侧有线, 左侧无
        moveRight();
    } else if (irS[0] + irS[1] < irS[2] + irS[3] + irS[4]) {  // 右侧比左侧多, 且不在正下方, 说明可能出界
        moveBack();                                           // 回退
        delay(5);
        moveStop();
        delay(50);
        irS[2] = irS[1] = irS[3] = 0;
        moveRight();
        irS[2] = 0;
        while (!irS[2]) {  // 一直向右转直到正下方有线结束
            moveRight();
            delay(25);
            irsScan();
            if (irS[2])
                break;
        }
        return 0;
    } else if (irS[3] + irS[4] < irS[0] + irS[1] + irS[2]) {  // 类似上边
        moveBack();
        delay(5);
        moveStop();
        delay(50);
        irsScan();
        moveLeft();
        irS[2] = 0;
        while (!irS[2]) {
            moveLeft();
            delay(25);
            irsScan();
            if (irS[2])
                break;
        }
        return 0;
    } else if (irS[0] == 1 && irS[1] == 1 && irS[2] == 1 && irS[3] == 1 && irS[4] == 1) {  // 下方横线, 说明到终点了
        moveBack();
        delay(20);
        moveStop();
        for (;;) {
            delay(1);
        }
        return 0;
    }
    return 0;
}

/**
 * 魔术手, 即保持与手的距离固定, 并跟随移动
 */
void magicHand() {
    double distF, distCurr;
    while ((distF = sssDist(sssF) >= 25)) {
        ;
    }
    distF = sssDist(sssF);  // 获取手的距离
    // DEBUG
    // distF = 30;
    distCurr = sssDist(sssF);
    while (1) {
        distCurr = sssDist(sssF);
        // 当手的距离离得过近的时候, 由于超声波传感器的特性, 获取到的距离可能是无穷大
        // 所以此时停止 (不管是真的离得太远还是太近)
        if (distCurr >= 80 || abs(distCurr - distF) <= 6) {
            moveStop();
            continue;
        }
        if (distCurr - distF > 5)
            moveFwd();
        if (distF - distCurr > 1)
            moveBack();
    }
}

/**
 * 忘了是干啥的了...没用到
 */
void followSide(bool side) {
    int sPin;
    double distSide;
    double distCurr;
    if (side) {
        sPin = sssL;
        distSide = sssDist(sPin);
        while (irS[2] == 0) {
            distCurr = sssRead(sPin);
            if (distCurr - distSide > 5) {
                moveLeft();
                delay(10);
            }
            if (distSide - distCurr > 1) {
                moveRight();
                delay(10);
            }
            moveFwd();
            delay(100);
            irsScan();
        }
    } else {
        sPin = sssR;
        distSide = sssDist(sPin);
        while (irS[2] == 0) {
            distCurr = sssRead(sPin);
            if (distCurr - distSide > 5) {
                moveRight();
                delay(10);
            }
            if (distSide - distCurr > 1) {
                moveLeft();
                delay(10);
            }
            moveFwd();
            delay(100);
            irsScan();
        }
    }
}

/**
 * 自动移动+避障
 * 就是走到快撞墙再转向的那种
 */
void freeRide() {
S:
    double distFront = sssRead(sssF);
    bool dir = 1;  //0 => 左  1 => 右

    if (distFront <= 40) {
        moveStop();
        delay(100);
        sssScanAll();
        if (sssS[3] > 40)
            goto A;
        if (sssS[3] <= 10) {
            moveBack();
            delay(800);
            goto S;
        }
        if (sssS[0] + sssS[1] + sssS[2] < sssS[4] + sssS[5] + sssS[6])
            dir = 1;
        else
            dir = 0;
        if (dir) {
            moveRight();
            delay(900);
        } else {
            moveLeft();
            delay(900);
        }
        moveFwd();
        delay(100);
    }
A:;
}

/**
 * 激光雷达急停
 * 激光雷达检测到目标后小车停止
 */
void LidarStop() {
  if (digitalRead(lidar)){
    moveStop();
    }
  } 

void firescan(){
  digitalWrite(scansign,LOW);
  delay(500);
  digitalWrite(scansign,HIGH);
  delay(500);
  digitalWrite(scansign,LOW);
  }

/**
 * 蓝牙操控模块
 */
int receive;
 
void reve(void)
{
          receive=Serial.parseInt();
       if(receive==7)     {moveFwd()         ;delay(2500);moveStop();}//前进
  else if(receive==6)     {moveBack()        ;delay(2500);moveStop();}//后退
  else if(receive==1)     {moveStop()        ;delay(2500);moveStop();}//停车
  else if(receive==3)     {swingLeft()       ;delay(1500);moveStop();}//左前
  else if(receive==2)     {swingRight()      ;delay(1500);moveStop();}//右前
  else if(receive==5)     {moveStop()        ;delay(2500);moveStop();}//左后(null)
  else if(receive==4)     {moveStop()        ;delay(2500);moveStop();}//右后(null)
  else if(receive==8)     {moveLeft()        ;delay(1000);moveStop();}//原地左
  else if(receive==9)     {moveLeft()        ;delay(1000);moveStop();}//原地右
  else if(receive==10)    {firescan()        ;delay(1000);moveStop();}
}
