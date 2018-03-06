#include <Servo.h>

Servo servo;

int val=1500;
void setup(){
  servo.attach(3,1000,2000);
  Serial.begin(9600);
}

void loop(){
  if (Serial.available() > 0) {
    String num = Serial.readString();
    val = num.toInt();
    Serial.println(val);
  }

  //0°:1500, 120°:1200, 240°:1850
  servo.writeMicroseconds(val);

}
