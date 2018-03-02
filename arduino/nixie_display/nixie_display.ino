#include <TimerOne.h>
//#include <FlexiTimer2.h>

volatile int nixie_num[8] = {
  2,
  10,
  7,
  1,
  7,
  2,
  8,
  1
};

volatile bool right_dot[8] = {
  0,
  1,
  0,
  0,
  0,
  0,
  0,
  0
};

volatile bool left_dot[8] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

volatile int pattern_t[8][4] = {
  {0,0,0,0},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,1,1},
  {0,1,0,1},
  {0,1,0,0},
  {0,1,1,0},
  {0,1,1,1}
};

volatile int pattern[11][4] = {
  {0,0,0,1}, // 1
  {0,0,0,0}, // 0
  {1,0,0,1}, // 9  
  {1,0,0,0}, // 8
  {0,1,1,1}, // 7
  {0,1,1,0}, // 6
  {0,1,0,1}, // 5
  {0,1,0,0}, // 4
  {0,0,1,1}, // 3
  {0,0,1,0}, // 2
  {1,1,1,1}  // x
};

int calc_num(char d){
      switch(d){
      case 'a':
        return 0;
        break;

      case 'b':
        return 1;
        break;

      case 'c':
        return 2;
        break;

      case 'd':
        return 3;
        break;

      case 'e':
        return 4;
        break;

      case 'f':
        return 5;
        break;

      case 'g':
        return 6;
        break;

      case 'h':
        return 7;
        break;

      case 'i':
        return 8;
        break;

      case 'j':
        return 9;
        break;

      case 'k':
        return 10;
        break;
    }
}

int nixie_index = 0;

void display(){
//  for(int nixie_index = 0; nixie_index < 8; ++nixie_index){
    ++nixie_index;

//    Serial.print(nixie_index);

    if(nixie_index == 8){
      nixie_index = 0;
    }
  
    digitalWrite(6,pattern_t[nixie_index][3]);
    digitalWrite(7,pattern_t[nixie_index][2]);
    digitalWrite(8,pattern_t[nixie_index][1]);
    digitalWrite(9,pattern_t[nixie_index][0]);

    digitalWrite(10,pattern[nixie_num[nixie_index]][3]);
    digitalWrite(11,pattern[nixie_num[nixie_index]][2]);
    digitalWrite(12,pattern[nixie_num[nixie_index]][1]);
    digitalWrite(13,pattern[nixie_num[nixie_index]][0]);

    digitalWrite(5,!left_dot[nixie_index]);
    digitalWrite(4,!right_dot[nixie_index]);

}

void end_display(){

    digitalWrite(9,1);
    digitalWrite(5,1);
    digitalWrite(4,1);
//  }
}

volatile int timer_count = 0;
int count = 0;

void flash(){
  timer_count++;
  switch(timer_count){
    case 2:
      display();
      break;
    case 5:
      end_display();
      break;
    case 10:
      timer_count = 0;
      break;
  }

  
}

void setup(){
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);

//  FlexiTimer2::set(200, 1.0 / 1000000, flash);
//  FlexiTimer2::start();
  Timer1.initialize(200);
  Timer1.attachInterrupt(flash);

  Serial.begin(9600);
}

void loop(){
  int kazu = 0;
  int index = 0;
  int num = 0;
  int dot = 0;
  
  if(Serial.available() > 0)
    
//    kazu = calc_num(Serial.read());

//    Serial.println(kazu);
    
//    for(int cc = 0; cc < kazu; ++cc){
      index = calc_num(Serial.read());
      num = calc_num(Serial.read());
      dot = calc_num(Serial.read());

      nixie_num[index] = num;

      switch(dot){
        case 0:
          right_dot[index] = 0;
          left_dot[index] = 0;
          break;
        case 1:
          right_dot[index] = 0;
          left_dot[index] = 1;
          break;
        case 2:
          right_dot[index] = 1;
          left_dot[index] = 0;
          break;
        case 3:
          right_dot[index] = 1;
          left_dot[index] = 1;
          break;
      }
//    }
  
//  ++count;
/*
  if(count < 25){
   nixie_num[0] = 1;
  }else{
   nixie_num[0] = 0;

   if(count == 50){
    count = 0;
   }
  }
  */

  /*
  nixie_num[1] = 10;
  nixie_num[2] = random(0,10);
  nixie_num[3] = random(0,10);
  nixie_num[4] = random(0,10);
  nixie_num[5] = random(0,10);
  nixie_num[6] = random(0,10);
  nixie_num[7] = random(0,10);
  */
//  delay(50);

}










