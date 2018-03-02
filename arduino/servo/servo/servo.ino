#include <Servo.h>

Servo servo;

int val=1500;
void setup(){
  //パルス出力ピンの設定
  //（デジタル出力なのでどのピンでも可）
  servo.attach(3,1000,2000);
  Serial.begin(9600);
}

void loop(){
  if (Serial.available() > 0) {
    String num = Serial.readString();
    val = num.toInt();
    Serial.println(val);
  }

  servo.writeMicroseconds(val);

}
