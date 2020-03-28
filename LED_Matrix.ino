// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - RGB matrix Panel Library: https://github.com/2dom/PxMatrix
// - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
// - esp8266 library (nodemcu) found at https://github.com/esp8266/Arduino
// - package_esp8266com_index.json found at http://arduino.esp8266.com/stable/package_esp8266com_index.json
// Find All "Great Projects" Videos : https://www.youtube.com/channel/UCCC8DuqicBtP3A_aC53HYDQ/videos

#include "DHT.h"
#define DHTPIN 0     //GPIO-0 D3 pin of nodemcu
#define DHTTYPE DHT22   // DHT 11
#include "Arduino.h"
#include <ESP8266WiFi.h>
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);
#include <Adafruit_GFX.h>
#include "FreeMonoBold12pt7b.h"
#include "kongtext4pt7b.h"
#include <PxMatrix.h>
#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_OE 2

// Pins for LED MATRIX

#define matrix_width 64
#define matrix_height 32

int h,m,s,d;
int16_t   hue = 0;
String dow;
String day;
String month;
String year;
String date;
  float rh = 0;
  float t = 0;
uint8_t r=0, g=0, b=0;
unsigned int NewRTCh = 24;
unsigned int NewRTCm = 60;
unsigned int NewRTCs = 10;
char szTime[4];    // 00
char szMesg[10] = "";
  static bool Mode = true; 
const char* ssid     = "Lotelle";     // SSID of local network
const char* password = "lotelle241ap";   // Password on network

// This defines the 'on' time of the display is us. The larger this number,
// the brighter the display. If too large the ESP will crash
uint8_t display_draw_time=10; //10-50 is usually fine

//PxMATRIX display(matrix_width,matrix_height,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(255, 255, 255);

uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};


#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(display_draw_time);
}
#endif



void display_update_enable(bool is_enable)
{

#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.002, display_updater);
  else
    display_ticker.detach();
#endif

}

void(* resetFunc) (void) = 0;//declare reset function at address 0

void getTemperature(char *psz)
{
          dtostrf(t, 3, 1, szMesg);
  display.setCursor(11, 0);
  display.fillRect(0, 0, 64, 8, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myRED);
    display.print(t);
    display.print("C");
  display.setFont();
  rh = dht.readHumidity();
}
void getHumidity(char *psz)
// Code for reading date
{
  display.setCursor(11, 0);
  display.fillRect(0, 0, 64, 8, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myBLUE);
    display.print(rh);
    display.print("%");
  display.setFont();
  t = dht.readTemperature();
} 
void getDate()
// Code for reading date
{
    if (Mode) 
   {
        display.setCursor(4,8);
  display.fillRect(0, 9, 64, 7, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myGREEN);
  display.print(dow);
  display.setTextColor(myRED);
  display.print(day);
  display.setTextColor(myGREEN);
  display.print(month);
  display.setFont();
   } else
   {
        display.setCursor(0,8);
  display.fillRect(0, 9, 64, 7, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myRED);
  display.print(day);
  display.setTextColor(myGREEN);
  display.print(month);
  display.setTextColor(myRED);
  display.print(year);
  display.setFont();
   }
  
} 
void getRTCh(char *psz)
// Code for reading clock time
{
  sprintf(psz, "%02d", h);
  display.setCursor(0, 24); 
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(myMAGENTA);
  display.fillRect(0, 16, 24, 15, display.color565(0, 0, 0));
  display.print(szTime);
  display.setFont();
  NewRTCh=h;
}

void getRTCm(char *psz)
// Code for reading clock time
{
  sprintf(psz, "%02d", m);
  display.setCursor(26, 24); //26
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(myMAGENTA);
  display.fillRect(26, 16, 25, 15, display.color565(0, 0, 0));
  display.print(szTime);
  display.setFont();
  NewRTCm=m;
}
void getTim(char *psz, bool f = true)
// Code for reading clock time
{
  if (NewRTCs != s/10) 
  {
      
    if (Mode) 
   {
        getTemperature(szMesg);
   } else
   {
        getHumidity(szMesg);
   }
   getDate();
   Mode = !Mode;
  display.setCursor(20, 16); 
  display.setTextSize(2);
  display.setTextColor(myCOLORS[g]);
  display.fillRect(24, 20, 2, 6, display.color565(0, 0, 0));
    display.print(f ? ':' : ' ');
  display.setCursor(54, 18); 
  display.setTextSize(1);
  display.fillRect(54, 18, 10, 6, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myCOLORS[b]);
    display.print(f ? ' ' : '*');
  display.setFont();
  display.setCursor(50, 24); 
  display.setTextSize(1);
  display.setTextColor(myCOLORS[r]);
  sprintf(psz, "%02d", s);
  display.fillRect(50, 25, 13, 6, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
    display.print(szTime);
  display.setFont();
      NewRTCs=s/10;
    }
    else
  {
  display.setCursor(20, 16); 
  display.setTextSize(2);
  display.setTextColor(myCOLORS[g]);
  display.fillRect(24, 20, 2, 6, display.color565(0, 0, 0));
    display.print(f ? ':' : ' ');
  display.setCursor(54, 18); 
  display.setTextSize(1);
  display.fillRect(54, 18, 10, 6, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
  display.setTextColor(myCOLORS[r]);
    display.print(f ? ' ' : '*');
  display.setFont();
  display.setCursor(50, 24); 
  display.setTextColor(myCOLORS[b]);
  sprintf(psz, "%02d", s);
  display.fillRect(58, 25, 6, 6, display.color565(0, 0, 0));
  display.setFont(&kongtext4pt7b);
    display.print(szTime);
  display.setFont();
    }
}
void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text)
{ uint16_t text_length = 70;
    display.setTextWrap(false); 
    // Asuming 5 pixel average character width
    display.setFont(&kongtext4pt7b);
  display.setTextColor(myCOLORS[r]);
    for (int xpos=matrix_width; xpos>-120; xpos--)
    {
      display.setCursor(xpos,ypos);
      display.fillRect(0, 9, 64, 7, display.color565(0, 0, 0));
      display.println(text);
      delay(scroll_delay);
      yield();

      // This might smooth the transition a bit if we go slow
      // display.setTextColor(display.color565(colorR/4,colorG/4,colorB/4));
      // display.setCursor(xpos-1,ypos);
      // display.println(text);
    }
  display.setFont();
   getDate();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
   t = dht.readTemperature();
   rh = dht.readHumidity();
    delay(2000);
  // Define your display layout here, e.g. 1/8 step
  display.begin(16);

  // Define your scan pattern here {LINE, ZIGZAG, ZAGGIZ, WZAGZIG, VZAG} (default is LINE)
  //display.setScanPattern(LINE);

  // Define multiplex implemention here {BINARY, STRAIGHT} (default is BINARY)
  //display.setMuxPattern(BINARY);

  display.setFastUpdate(true);
    display.setRotation(0); // we don't wrap text so it scrolls nicely
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.println("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
  }
  display.clearDisplay();
  display.println(""); 
  display.print("   WiFi OK"); 
  display_update_enable(true);
  delay(3000);
    getTime();
}

void loop() {
  

  static uint32_t lastTime = 0; // millis() memory
  static bool flasher = false;  // seconds passing flasher
  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();
    updateTime();
    getTim(szTime, flasher);
    flasher = !flasher;
    if (NewRTCh != h) 
   {
    if (h >= 24) 
   {
      resetFunc();
   }
        getRTCh(szTime);
   }
    if (NewRTCm != m) 
   {
        getRTCm(szTime);
  scroll_text(14,30,date);
   }
      r++;
      if(r == 8) {
        r = 0;
        g++;
        if(g == 8) {
          g = 0;
          b++;
        if(b == 8) {
          b = 0;
        }
        }
      }
  }
}

long localEpoc = 0;

void getTime()
{
  display.clearDisplay();
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
  display.println("connection to google failed");
    return;
  }
  display.setCursor(0, 0);
  display.println("connection to google");
  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
  display.print(".");
    repeatCounter++;
  }

  String line;
  client.setNoDelay(false);
  while(client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    if (line.startsWith("DATE: ")) {
      dow = line.substring(6, 9).c_str();
      day = line.substring(11, 13).c_str();
      month = line.substring(14, 17).c_str();
      year = line.substring(18, 22).c_str();
      date = line.substring(6, 22).c_str();
      h = line.substring(23, 25).toInt();
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt();
      localEpoc = (h * 60 * 60 + m * 60 + s);
    }
  }
  client.stop();
  display.clearDisplay();
}

void updateTime()
{
  s = localEpoc + (millis()/1000);
  m = s/60;
  h = m/60;
  m %= 60;
  s %= 60;
}

uint16_t Wheel(byte WheelPos) {
  if(WheelPos < 1) {
   return display.color565(255, 255, 0);
  } else if(WheelPos < 2) {
   WheelPos -= 1;
   return display.color565(0, 255, 255);
  } else if(WheelPos < 3) {
   WheelPos -= 2;
   return display.color565(255, 0, 255);
  } else {
   WheelPos -= 3;
   return display.color565(255, 255, 0);
  } 
}
