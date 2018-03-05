#include <Adafruit_NeoPixel.h>
#define SERIALCOMMANDBUFFER 32
#include <SerialCommand.h>
#include <TimerOne.h>

//#define WS2812B_MODE

volatile int nixie_num[8] = {
  2,
  10,
  7,
  1,
  8,
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

#ifdef WS2812B_MODE
volatile int led_color[8] = {
  20,
  20,
  20,
  20,
  20,
  20,
  20,
  20
};
#endif

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

Adafruit_NeoPixel ledtape = Adafruit_NeoPixel(8,3,NEO_RGB + NEO_KHZ800);

uint32_t rotateColor(byte WheelPos) {
  if(WheelPos < 85) {
   return ledtape.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ledtape.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ledtape.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void call(){
 uint16_t i, j;
 
 for(j=0; j <256; j++) {

 }
}


int nixie_index = 0;

void display(){
    ++nixie_index;

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
}

volatile int timer_count = 0;

void flash(){
  timer_count++;
  switch(timer_count){
    case 5:
      display();
#ifdef WS2812B_MODE
      led_loop();
#endif
      break;
    case 11:
      end_display();
      break;
    case 24:
      timer_count = 0;
      break;
  } 
}

SerialCommand SCmd;

void echo_back_args(){
  char* arg = SCmd.next();
  Serial.println("command test");
  Serial.print("input:");
  Serial.print(arg);

  arg = SCmd.next();
  Serial.println(arg);
}

void error(){
  Serial.println("ubnrecongized command");
}

void set_num(){
  char* arg = SCmd.next();

  nixie_num[0] = calc_num(arg[0]);
  nixie_num[1] = calc_num(arg[1]);
  nixie_num[2] = calc_num(arg[2]);
  nixie_num[3] = calc_num(arg[3]);
  nixie_num[4] = calc_num(arg[4]);
  nixie_num[5] = calc_num(arg[5]);
  nixie_num[6] = calc_num(arg[6]);
  nixie_num[7] = calc_num(arg[7]);
}

void set_dot(){
  char* arg = SCmd.next();
  int dot = 0;

  for(int i = 0; i < 8; ++i){
    dot = calc_num(arg[i]);
    
    switch(dot){
      case 0:
        right_dot[i] = 0;
        left_dot[i] = 0;
        break;
      case 1:
        right_dot[i] = 0;
        left_dot[i] = 1;
        break;
      case 2:
        right_dot[i] = 1;
        left_dot[i] = 0;
        break;
      case 3:
        right_dot[i] = 1;
        left_dot[i] = 1;
        break;
    }
  }
}

#ifdef WS2812B_MODE
void set_led_color(){
  char* arg = SCmd.next();
  int led_index = atoi(arg);

  arg = SCmd.next();
  int led_color_code = atoi(arg);

  led_color[led_index] = led_color_code;
}
#endif

#ifdef WS2812B_MODE
//int led_color = 0;
void led_loop(){
  int i;
  for(i=0; i < ledtape.numPixels(); i++) {
    ledtape.setPixelColor(i, rotateColor(led_color[i]));
  }
  ledtape.show();
}
#endif

void setup(){
#ifdef WS2812B_MODE
  ledtape.begin();
  ledtape.setBrightness(40);
  ledtape.show();
  SCmd.addCommand("setled",set_led_color);
#endif
  
  SCmd.addCommand("echo",echo_back_args);
  SCmd.addCommand("setnum",set_num);
  SCmd.addCommand("setdot",set_dot);

  SCmd.addDefaultHandler(error);
  
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
  
  Serial.begin(9600);

  Serial.println("Ready");
  
  Timer1.initialize(100);
  Timer1.attachInterrupt(flash);
}

void loop(){
  SCmd.readSerial();
}










