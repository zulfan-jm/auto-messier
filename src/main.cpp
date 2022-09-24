#include <Arduino.h>
#include <PID_v1.h>
#include <ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Header.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Empty.h>

//motor driver pin
const int EL_right = 3;
const int ZF_right = 4;
const int VR_right = 5;
const int EL_left = 7;
const int ZF_left = 8;
const int VR_left = 9;

//tachometer or signal pin
const int encoder_right = 2; 
const int encoder_left = 6;

//assign pin
const int MOTOR_PINS[6] = {EL_right, ZF_right, VR_right, EL_left, ZF_left, VR_left};
const int ENCODER_PINS[2] = {encoder_right, encoder_left};

int countLeft = 0;
int countRight = 0;

void left_wheel_tick() {
  countLeft++;
}

void right_wheel_tick() {
  countRight++;
}

int getLeftTick() {
  if (digitalRead(ZF_left)==1) {
    return countLeft;
  } else {
    return -countLeft;
  } 
}

int getRightTick() {
  if (digitalRead(ZF_right)==1) {
    return countRight;
  } else {
    return -countRight;
  } 
}

int last_millis; //Number of tasks

void setup() {
  pinMode(EL_left, OUTPUT);
  pinMode(EL_right, OUTPUT);
  pinMode(VR_left, OUTPUT);
  pinMode(VR_right, OUTPUT);
  pinMode(ZF_left, OUTPUT);
  pinMode(ZF_right, OUTPUT);

  pinMode(encoder_left, INPUT_PULLUP);
  pinMode(encoder_right, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoder_left), left_wheel_tick, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder_right), right_wheel_tick, RISING);

  Serial.begin(9600);

  digitalWrite(EL_left, HIGH);
  digitalWrite(EL_right, HIGH);
  digitalWrite(VR_left, HIGH);
  digitalWrite(VR_right, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  bool ZF_value;
  getLeftTick();
  getRightTick();

  if(Serial.available()){
    ZF_value = Serial.read();
    Serial.print("ZF input: ");
    Serial.println(ZF_value);
    digitalWrite(ZF_left, ZF_value);
    digitalWrite(ZF_right, ZF_value);
    Serial.print("ZF left: ");
    Serial.println(digitalRead(ZF_left));
    Serial.print("ZF right: ");
    Serial.println(digitalRead(ZF_right));
  }

  if(millis() - last_millis > 100){
    last_millis = millis();
    // Serial.println(countLeft);
    // Serial.println(countRight);
  }
}