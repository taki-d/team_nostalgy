

int pattern_t[8][4] = {
  {0,0,0,0},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,1,1},
  {0,1,0,1},
  {0,1,0,0},
  {0,1,1,0},
  {0,1,1,1}
};

int pattern[10][4] = {
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
};

void setup(){
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);

  const int display_num = 0;

  digitalWrite(10,pattern[display_num][3]);
  digitalWrite(11,pattern[display_num][2]);
  digitalWrite(12,pattern[display_num][1]);
  digitalWrite(13,pattern[display_num][0]);
}

void loop(){
  for(int nixie_num = 0; nixie_num < 8; ++nixie_num){
    digitalWrite(6,pattern_t[nixie_num][3]);
    digitalWrite(7,pattern_t[nixie_num][2]);
    digitalWrite(8,pattern_t[nixie_num][1]);
    digitalWrite(9,pattern_t[nixie_num][0]);

    digitalWrite(10,pattern[nixie_num][3]);
    digitalWrite(11,pattern[nixie_num][2]);
    digitalWrite(12,pattern[nixie_num][1]);
    digitalWrite(13,pattern[nixie_num][0]);

    delayMicroseconds(500);

    digitalWrite(13,1);
    delayMicroseconds(1000);
  }
}






