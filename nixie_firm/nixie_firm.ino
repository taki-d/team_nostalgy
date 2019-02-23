#include <Time.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <Wire.h>

#include "RTClib.h"
RTC_DS3231 rtc;

#include <SSCI_BME280.h>
SSCI_BME280 bme280;
uint8_t i2c_addr = 0x77; 

#include "ESPAsyncWebServer.h"

#include <TinyGPS.h>

// Timer Interrupt setting
hw_timer_t * timer = NULL;

volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

const char* ssid = "hogessid";
const char* pass = "fugapassword";

const int8_t timezone = 9;
unsigned int dpmode = 0;

AsyncWebServer server(80);

#define SW3 27
#define SW4 14
#define SW5 12

volatile bool anode_signal_pattern[8][3] = {
  {0,0,0},
  {0,0,1},
  {0,1,0},
  {0,1,1},
  {1,0,0},
  {1,0,1},
  {1,1,0},
  {1,1,1}
};

volatile char num_signal_pattern[11][2] = {
  {0b00000000, 0b10000000}, // 0
  {0b00000100, 0b00000000}, // 1
  {0b00001000, 0b00000000}, // 2
  {0b00000000, 0b00000001}, // 3
  {0b00000000, 0b00000010}, // 4
  {0b00000000, 0b00000100}, // 5
  {0b00000000, 0b00001000}, // 6
  {0b00000000, 0b00010000}, // 7
  {0b00000000, 0b00100000}, // 8
  {0b00000000, 0b01000000}, // 9
  {0b00000000, 0b00000000}, // _
};

volatile char func_btn[2] = {
  0,
  0,
};

// left dot
// 0b00000001

// right dot
// 0b00000010

volatile char dot_signal_pattern[4][2] = {
  {0b00000000, 0b00000000}, // 00
  {0b00000001, 0b00000000}, // 01
  {0b00000010, 0b00000000}, // 10
  {0b00000011, 0b00000000}, // 11
};

int cycleNum = 0;
char display_pattern[8][2] = {
  {0b00000000, 0b00000001}, // nix0
  {0b00000000, 0b00000010}, // nix1
  {0b00000000, 0b00000100}, // nix2
  {0b00000000, 0b00001000}, // nix3
  {0b00000000, 0b00010000}, // nix4
  {0b00000000, 0b00100000}, // nix5
  {0b00000000, 0b01000000}, // nix6
  {0b00000100, 0b00000000}, // nix7
};

String html ="<!DOCTYPE html> <html lang=\"ja\"> <head> <meta charset=\"utf-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Bootstrap Sample</title> <link href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" rel=\"stylesheet\"> <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.0/umd/popper.min.js\"></script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script> </head> <body> <header style=\"background-color:white\"></header> <form> <div class=\"container-fluid\"> <div class=\"row\"> <div class=\"container\"> <h3>ボタンのモード設定</h3> <table class=\"table table-bordered\"> <thead> <tr> <th>ボタンの名前</th> <th>モード</th> </tr> </thead> <tbody> <tr> <th scope=\"row\">func1</th> <td> <select name=\"func1\"> <option value=\"0\">日付</option> <option value=\"1\">時刻</option> <option value=\"2\">気圧</option> <option value=\"3\">気温、湿度</option> <option value=\"4\">緯度経度</option> <option value=\"5\">APIモード</option> <option value=\"6\">タイマー</option> </select> </td> </tr> <tr> <th scope=\"row\">func2</th> <td> <select name=\"func2\"> <option value=\"0\">日付</option> <option value=\"1\">時刻</option> <option value=\"2\">気圧</option> <option value=\"3\">気温、湿度</option> <option value=\"4\">緯度経度</option> <option value=\"5\">APIモード</option> <option value=\"6\">タイマー</option> </select> </td> </tr> </tbody> </table> </div> <br> <div class=\"container\"><br> <h3>Wifi設定</h3> <p>現在のIPアドレスは</p> <!-- Wifi情報をとってくる --> <p>現在接続中のWifiは</p> <!-- Wifi情報をとってくる --> <br> <div class=\"form-group\"> <h3>NTPアドレス</h3> <input type=\"email\" class=\"form-control\" id=\"exampleInputaddr\" placeholder=\"example\"> </div> </form> <br> <div class=\"checkbox\"> <h3>有効にするモード</h3> <label> <input type=\"checkbox\" name=\"date\"> 日付<br> <input type=\"checkbox\" name=\"clock\"> 時刻<br> <input type=\"checkbox\" name=\"pressure\"> 気圧<br> <input type=\"checkbox\" name=\"temp\"> 気温、湿度<br> <input type=\"checkbox\" name=\"gps\"> 緯度、経度<br> <input type=\"checkbox\" name=\"api\"> APIモード<br> <input type=\"checkbox\" name=\"timer\"> タイマー<br> </label> </div> <button type=\"submit\" class=\"btn btn-primary\">送信</button> </div> </div> </div> </form> <footer style=\"background-color:white\"></footer> </body> </html>";

HardwareSerial serial0(0);
HardwareSerial gps_serial(2);
TinyGPS gps;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);

  digitalWrite(17,anode_signal_pattern[cycleNum][2]);
  digitalWrite(16,anode_signal_pattern[cycleNum][1]);
  digitalWrite(4,anode_signal_pattern[cycleNum][0]);

  shiftOut(19,18,MSBFIRST, display_pattern[cycleNum][0]);
  shiftOut(19,18,MSBFIRST, display_pattern[cycleNum][1]);
  digitalWrite(5,LOW);
  digitalWrite(5,HIGH);
  
  cycleNum++;
  if(cycleNum > 7) cycleNum = 0;
  
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void adjustByNTP(){
  configTzTime("JST-9", "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  rtc.adjust(DateTime(time(NULL))+TimeSpan(0, timezone, 0, 0));
}

void setup() {
  // put your setup code here, to run once:
  pinMode(17,OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(17,HIGH);
  digitalWrite(16,HIGH);
  digitalWrite(4,HIGH);

  //button setting
  pinMode(SW3,INPUT);
  pinMode(SW4,INPUT);
  pinMode(SW5,INPUT_PULLUP);

  serial0.begin(115200);
  gps_serial.begin(9600, SERIAL_8N1, 2, 15);

  bool isWiFiConnected = true;

  serial0.printf("Connecting to %s ", ssid);
  WiFi.disconnect(true);
  WiFi.begin(ssid, pass);
  unsigned long time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    serial0.print(".");
    /*
    if(millis()-time>10000){
      serial0.print("Can't connect Wi-Fi");
      WiFi.disconnect(true);
      isWiFiConnected = false;
      break;
    }
    */
  }
  serial0.println(" CONNECTED");
  

  //RTCのあれこれ
  if (! rtc.begin()) {
    serial0.println("Couldn't find RTC");
    while (1);
  }
  
  if(isWiFiConnected){
    adjustByNTP();
  }

  //bme280 set up
  uint8_t osrs_t = 1;             //Temperature oversampling x 1
  uint8_t osrs_p = 1;             //Pressure oversampling x 1
  uint8_t osrs_h = 1;             //Humidity oversampling x 1
  uint8_t bme280mode = 3;         //Normal mode
  uint8_t t_sb = 5;               //Tstandby 1000ms
  uint8_t filter = 0;             //Filter off
  uint8_t spi3w_en = 0;           //3-wire SPI Disable

  bme280.setMode(i2c_addr, osrs_t, osrs_p, osrs_h, bme280mode, t_sb, filter, spi3w_en);
  bme280.readTrim();
  
  server.on("/setting", HTTP_GET, [&](AsyncWebServerRequest *request){
 
    int paramsNr = request->params();
    serial0.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
      AsyncWebParameter* p = request->getParam(i);

      if(p->name() == "num"){
        String param = p->value();

        for(char i = 0; i < 8; i++){
          if(param[i] == ':'){
            display_pattern[i][0] = 0;
            display_pattern[i][1] = 0;
            continue;
          }
        
          memcpy(display_pattern[i], (void*)num_signal_pattern[param[i] - '0'], 2);
      
        }
        serial0.println(atoi(&param[1]));
      }

      if(p->name() == "mode"){
        dpmode = p->value()[0] - '0';
      }
    }
 
    request->send(200, "plain/text", "message received");
  });

  server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    int params_num = request->params();

    for(int i=0; i < params_num; ++i){
      AsyncWebParameter* p = request->getParam(i);

      if(p->name() == "func1"){
        func_btn[0] = p->value()[0] - '0';
      }

      if(p->name() == "func2"){
        func_btn[1] = p->value()[0] - '0';
      }
    }

    request->send(200, "text/html", html);
  });
  
  server.begin();
  
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

void setDisplayTime(DateTime now){
  memcpy(display_pattern[0], (void*)num_signal_pattern[now.hour()%100/10], 2);
  memcpy(display_pattern[1], (void*)num_signal_pattern[now.hour()%10], 2);
  memcpy(display_pattern[2], (void*)dot_signal_pattern[2], 2);
  memcpy(display_pattern[3], (void*)num_signal_pattern[now.minute()%100/10], 2);
  memcpy(display_pattern[4], (void*)num_signal_pattern[now.minute()%10], 2);
  memcpy(display_pattern[5], (void*)dot_signal_pattern[2], 2);
  memcpy(display_pattern[6], (void*)num_signal_pattern[now.second()%100/10], 2);
  memcpy(display_pattern[7], (void*)num_signal_pattern[now.second()%10], 2);

}

void setDisplayDate(DateTime now){
  memcpy(display_pattern[0], (void*)num_signal_pattern[now.year()%100/10], 2);
  memcpy(display_pattern[1], (void*)num_signal_pattern[now.year()%10], 2);
  memcpy(display_pattern[2], (void*)dot_signal_pattern[2], 2);
  memcpy(display_pattern[3], (void*)num_signal_pattern[now.month()%100/10], 2);
  memcpy(display_pattern[4], (void*)num_signal_pattern[now.month()%10], 2);
  memcpy(display_pattern[5], (void*)dot_signal_pattern[2], 2);
  memcpy(display_pattern[6], (void*)num_signal_pattern[now.day()%100/10], 2);
  memcpy(display_pattern[7], (void*)num_signal_pattern[now.day()%10], 2);

}

void setDisplayThermoHumidity(double temperature, double humidity){
  
  memcpy(display_pattern[0], (void*)num_signal_pattern[(int)temperature%100/10], 2);
  memcpy(display_pattern[1], (void*)num_signal_pattern[(int)temperature%10/1], 2);
  memcpy(display_pattern[2], (void*)num_signal_pattern[(int)round(temperature*10)%10], 2);
  memcpy(display_pattern[3], (void*)num_signal_pattern[10], 2);
  memcpy(display_pattern[4], (void*)num_signal_pattern[10], 2);
  memcpy(display_pattern[5], (void*)num_signal_pattern[(int)humidity%100/10], 2);
  memcpy(display_pattern[6], (void*)num_signal_pattern[(int)humidity%10/1], 2);
  memcpy(display_pattern[7], (void*)num_signal_pattern[(int)round(humidity*10)%10], 2);

  display_pattern[2][0] |= dot_signal_pattern[1][0]; 
  display_pattern[7][0] |= dot_signal_pattern[1][0]; 
}

void setDisplayPressure(double pressure){
  memcpy(display_pattern[0], (void*)num_signal_pattern[10], 2);
  memcpy(display_pattern[1], (void*)num_signal_pattern[10], 2);
  memcpy(display_pattern[2], ((int)pressure%10000/1000 == 0 ? (void*)num_signal_pattern[10] : (void*)num_signal_pattern[(int)pressure%10000/1000]), 2);
  memcpy(display_pattern[3], (void*)num_signal_pattern[(int)pressure%1000/100], 2);
  memcpy(display_pattern[4], (void*)num_signal_pattern[(int)pressure%100/10], 2);
  memcpy(display_pattern[5], (void*)num_signal_pattern[(int)pressure%10/1], 2);
  memcpy(display_pattern[6], (void*)num_signal_pattern[(int)(pressure*10)%10], 2);
  memcpy(display_pattern[7], (void*)num_signal_pattern[(int)(pressure*100)%10], 2);

  display_pattern[6][0] |= dot_signal_pattern[1][0];
}

double temp_act, press_act, hum_act; //最終的に表示される値を入れる変数

void loop() {
  if(!digitalRead(SW3)){
    while(!digitalRead(SW3));
    dpmode++;
    if(dpmode>4) dpmode = 0;
  }

  if(!digitalRead(SW4)){
    while(!digitalRead(SW4));

    dpmode = func_btn[0];
  }

  if(!digitalRead(SW5)){
    while(!digitalRead(SW5));

    dpmode = func_btn[1];
  }

  DateTime now = rtc.now();
  bme280.readData(&temp_act, &press_act, &hum_act);
  switch(dpmode){
    case 0:
      setDisplayTime(now);
      break;
    case 1:
      setDisplayDate(now);
      break;
    case 2:
      setDisplayThermoHumidity(temp_act, hum_act);
      delay(100);
      break;
    case 3:
      setDisplayPressure(press_act);
      delay(100);
      break;
    case 4: // API Mode
      delay(100);
      break;

    default:
      break; 
  }
}
