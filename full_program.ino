#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <TimerOne.h>
#include "DHT.h"
#include <SPI.h>
#include <MFRC522.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define SS_PIN 10
#define RST_PIN 9
#define MOISTURE A2
#define ALARM 3
#define MQ4 A3
#define SS_PIN 10
#define RST_PIN 9

int gas_sensor = A3;   //Sensor pin 
float m = -0.318;     //Slope 
float b = 1.133;     //Y-Intercept 
float R0 = 4.0; 

double percentage;
int readSens;
float uvIntensity;
float suhu, kelembaban;

MFRC522 mfrc522(SS_PIN, RST_PIN); 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET     4 
#define NUMFLAKES     10 
int REF_3V3 = A1;
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int UVOUT = A0; //Output from the sensor uv


int menuitem = 1;
int frame = 1;
int page = 1;
int lastMenuItem = 1;

String menuItem1 = "SOIL SENSOR";
String menuItem2 = "DHT-11 SENSOR";
String menuItem3 = "UV & GAS SENSOR";
String menuItem4 = "BLOWER: ON ";
String menuItem5 = "POMPA: ON";
String menuItem6 = "Reset";

boolean backlight = true;
int contrast=60;
int volume = 50;


boolean up = false;
boolean down = false;
boolean middle = false;

#define BTN_UP 4
#define BTN_DOWN 5
#define BTN_OK 6
int16_t last, value;

#define OLED_RESET 4
//Adafruit_PCD8544 display = Adafruit_PCD8544( 5, 4, 3); //Download the latest Adafruit Library in order to use this constructor

void setup() {
  SPI.begin(); 
  mfrc522.PCD_Init();   // Initiate MFRC522
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(BTN_UP,INPUT_PULLUP);
  pinMode(BTN_DOWN,INPUT_PULLUP);
  pinMode(BTN_OK,INPUT_PULLUP);
  display.begin();      
  display.clearDisplay(); 
  setContrast();  
  
  POMPAOff();
  BLOWEROff();
   
  display.begin();      
  display.clearDisplay(); 
  setContrast(); 
  Timer1.initialize(1000);
  pinMode(ALARM,OUTPUT);
  dht.begin();
  Serial.begin(9600);     // Initiate  SPI bus
  displayString(2,"POLIJE",35,10);
  delay(3000);
  LOADING();
  //readRfid();
}

void loop() {
  readMQ();
  readUV_sensor();
  readDHT_sensor();
  readMoisture();
  debug();
  drawMenu();
  readRotaryEncoder();
   if (digitalRead(BTN_OK)==LOW) {  
       middle=true;
   }    
   else {
    digitalWrite(ALARM,LOW);
    }
  
  if (up && page == 1 ) {
     
    up = false;
    if(menuitem==2 && frame ==2)
    {
      frame--;
    }

     if(menuitem==4 && frame ==4)
    {
      frame--;
    }
      if(menuitem==3 && frame ==3)
    {
      frame--;
    }
    lastMenuItem = menuitem;
    menuitem--;
    if (menuitem==0)
    {
      menuitem=1;
    } 
  }
 
  if (down && page == 1) //We have turned the Rotary Encoder Clockwise
  {

    down = false;
    if(menuitem==3 && lastMenuItem == 2)
    {
      frame ++;
    }else  if(menuitem==4 && lastMenuItem == 3)
    {
      frame ++;
    }
     else  if(menuitem==5 && lastMenuItem == 4 && frame!=4)
    {
      frame ++;
    }
    lastMenuItem = menuitem;
    menuitem++;  
    if (menuitem==7) 
    {
      menuitem--;
    }
  
  }else if (down && page == 2 && menuitem==1) {
    down = false;
  }
  else if (down && page == 2 && menuitem==2) {
    down = false;
  }
   else if (down && page == 2 && menuitem==3 ) {
    down = false;
  }
  
  
  if (middle) //Middle Button is Pressed
  {
    middle = false;
   
    if (page == 1 && menuitem==5) // Backlight Control 
    {
      if (backlight) 
      {
        backlight = false;
        menuItem5 = "POMPA: OFF";
        POMPAOff();
        }
      else 
      {
        backlight = true; 
        menuItem5 = "POMPA: ON";
        POMPAOn();
       }
    }
  
  else if ( page == 2 && menuitem==4 ) {
   if (backlight) 
      {
        backlight = false;
        menuItem4 = "BLOWER: OFF";
        BLOWEROff();
        }
      else 
      {
        backlight = true; 
        menuItem4 = "BLOWER: ON";
        BLOWEROn();
       }
  } 

     
    if(page == 1 && menuitem ==6)// Reset
    {
      resetDefaults();
    }


    else if (page == 1 && menuitem<=4) {
      page=2;
     }
      else if (page == 2) 
     {
      page=1; 
     }
   }   
  }
  
  void drawMenu()
  {

  
  if (page==1) 
  {    
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE,BLACK );
    display.setCursor(15, 0);
    display.println("    MAIN MENU");
    display.drawFastHLine(0,10,128,WHITE);//BLACK

    if(menuitem==1 && frame ==1)
    {   
      displayMenuItem(menuItem1, 15,true);
      displayMenuItem(menuItem2, 25,false);
    }
    else if(menuitem == 2 && frame == 1)
    {
      displayMenuItem(menuItem1, 15,false);
      displayMenuItem(menuItem2, 25,true);
      displayMenuItem(menuItem3, 35,false);
    }
    else if(menuitem == 3 && frame == 1)
    {
      displayMenuItem(menuItem1, 15,false);
      displayMenuItem(menuItem2, 25,false);
      displayMenuItem(menuItem3, 35,true);
    }
     else if(menuitem == 4 && frame == 2)
    {
      displayMenuItem(menuItem2, 15,false);
      displayMenuItem(menuItem3, 25,false);
      displayMenuItem(menuItem4, 35,true);
    }

      else if(menuitem == 3 && frame == 2)
    {
      displayMenuItem(menuItem2, 15,false);
      displayMenuItem(menuItem3, 25,true);
      displayMenuItem(menuItem4, 35,false);
    }
    else if(menuitem == 2 && frame == 2)
    {
      displayMenuItem(menuItem2, 15,true);
      displayMenuItem(menuItem3, 25,false);
      displayMenuItem(menuItem4, 35,false);
    }
    
    else if(menuitem == 5 && frame == 3)
    {
      displayMenuItem(menuItem3, 15,false);
      displayMenuItem(menuItem4, 25,false);
      displayMenuItem(menuItem5, 35,true);
    }

    else if(menuitem == 6 && frame == 4)
    {
      displayMenuItem(menuItem4, 15,false);
      displayMenuItem(menuItem5, 25,false);
      displayMenuItem(menuItem6, 35,true);
    }
    
      else if(menuitem == 5 && frame == 4)
    {
      displayMenuItem(menuItem4, 15,false);
      displayMenuItem(menuItem5, 25,true);
      displayMenuItem(menuItem6, 35,false);
    }
      else if(menuitem == 4 && frame == 4)
    {
      displayMenuItem(menuItem4, 15,true);
      displayMenuItem(menuItem5, 25,false);
      displayMenuItem(menuItem6, 35,false);
    }
    else if(menuitem == 3 && frame == 3)
    {
      displayMenuItem(menuItem3, 15,true);
      displayMenuItem(menuItem4, 25,false);
      displayMenuItem(menuItem5, 35,false);
    }
        else if(menuitem == 2 && frame == 2)
    {
      displayMenuItem(menuItem2, 15,true);
      displayMenuItem(menuItem3, 25,false);
      displayMenuItem(menuItem4, 35,false);
    }
    else if(menuitem == 4 && frame == 3)
    {
      displayMenuItem(menuItem3, 15,false);
      displayMenuItem(menuItem4, 25,true);
      displayMenuItem(menuItem5, 35,false);
    }   
    display.display();
  }
  else if (page==2 && menuitem == 1) 
  {    
   displayIntMenuPage(menuItem1,readSens);
  }

  else if (page==2 && menuitem == 2) 
  {
   displayIntMenuPage2(menuItem2,suhu,kelembaban);
  }
   else if (page==2 && menuitem == 3) 
  {
   displayIntMenuPage1(menuItem3,uvIntensity,percentage);
  }
  }

  void resetDefaults()
  {
    contrast = 60;
    volume = 50;
    setContrast();
    backlight = true;
    menuItem5 = "POMPA: ON";
    menuItem4 = "BLOWER: ON";
    POMPAOff();
    BLOWEROff();
  }

  void setContrast()
  {
    //display.setContrast(contrast);
    display.display();
  }

  void POMPAOn()
  {
    digitalWrite(7,LOW);
  }

    void POMPAOff()
  {
    digitalWrite(7,HIGH);
  }
  void BLOWEROn()
  {
    digitalWrite(8,LOW);
  }

    void BLOWEROff()
  {
    digitalWrite(8,HIGH);
  }


void displayIntMenuPage(String menuItem, float value)
{
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE,BLACK );
    display.setCursor(15, 0);
    display.println(menuItem);
    display.drawFastHLine(0,10,127,WHITE);//BLACK
    display.setCursor(5, 15);
    display.println("Value");
    display.setTextSize(2);
    display.setCursor(40, 14);
    display.println(value);
    display.setTextSize(2);
    display.display();
}
void displayIntMenuPage1(String menuItem, float valueA, float valueB)
{
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE,BLACK );
    display.setCursor(15, 0);
    display.println(menuItem);
    display.drawFastHLine(0,10,127,WHITE);//BLACK
    display.setCursor(15, 15);
    display.println("UV = ");
    display.setTextSize(1);
    display.setCursor(55, 15);
    display.println(valueA);
    display.setTextSize(1);
    display.setCursor(90, 15);
    display.println("mV/cm");
    display.setCursor(15, 25);
    display.println("Gas = ");
    display.setTextSize(1);
    display.setCursor(55, 25);
    display.println(valueB);
    display.setTextSize(1);
    display.setCursor(84, 25);
    display.println("PPM");
    display.setTextSize(1);
    display.display();
}
void displayIntMenuPage2(String menuItem, float valueA, float valueB)
{
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE,BLACK );
    display.setCursor(15, 0);
    display.println(menuItem);
    display.drawFastHLine(0,10,127,WHITE);//BLACK
    display.setCursor(15, 15);
    display.println("SUHU = ");
    display.setTextSize(1);
    display.setCursor(55, 15);
    display.println(valueA);
    display.setTextSize(1);
    display.setCursor(90, 15);
    display.println("C");
    display.setCursor(15, 25);
    display.println("KELEMBABAN = ");
    display.setTextSize(1);
    display.setCursor(95, 25);
    display.println(valueB);
    display.setTextSize(1);
    display.display();
}
void displayStringMenuPage(String menuItem, String value)
{
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor( WHITE,BLACK);
    display.setCursor(15, 0);
    display.println(menuItem);
    display.drawFastHLine(0,10,128,WHITE);//BLACK
    display.setCursor(5, 15);
    display.println("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.println(value);
    display.setTextSize(2);
    display.display();
}

void displayString(int size,String kata, int x, int y)
{
    display.setTextColor( WHITE,BLACK);
    display.setTextSize(size);
    display.clearDisplay();
    display.setCursor(x, y);
    display.println(kata); 
    display.setTextSize(size);
    display.display();
}

void displayMenuItem(String item, int position, boolean selected)
{
    if(selected)
    {
      display.setTextColor(BLACK,WHITE);//, BLACK
    }else
    {
      display.setTextColor(WHITE);//BLACK,
    }
    display.setCursor(0, position);
    display.print(">"+item);
}

void readRotaryEncoder()
{
  int readUP = 0;
  int readDOWN = 0;
  if(digitalRead(BTN_UP)==LOW){
    digitalWrite(ALARM,HIGH);
    up = true;
    delay(50);
    }
  if (digitalRead(BTN_DOWN)==LOW) {
    digitalWrite(ALARM,HIGH);
    last = value/2;
    down = true;
    delay(50);
  }
  else{
  digitalWrite(ALARM,LOW);
  }
}
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float readUV_sensor()
{
  int uvLevel = analogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);
  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  delay(100);
}
void readDHT_sensor() {
  kelembaban = dht.readHumidity();
  suhu = dht.readTemperature();


  if (isnan(suhu) || isnan(kelembaban)) {
    return;
  }
   delay(100);
}
void readRfid() {
while(1){
// Look for new cards
  
  while( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    displayString(1,"Tap your Card",30,10);
  }
  // Select one of the cards
  while( ! mfrc522.PICC_ReadCardSerial()) 
  {
   displayString(1,"Tap your Card",30,10);
  }
  
  //Show UID on serial monitor
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Serial.println(content.substring(1));
  content.toUpperCase(); 
  if (content.substring(1).toInt() > 0) //change here the UID of the card/cards that you want to give access
  {
    displayString(1,"Authorized access",30,10);
    LOADING2();
    displayString(1,"Authorized access",20,10);
    delay(1000);
    displayString(1,"Mashfukh",25,10);
    delay(3000);
    break;
  }
  else{
     for(int i=0;i<2;i++){
    digitalWrite(ALARM,HIGH);
    delay(80);
    digitalWrite(ALARM,LOW);
    delay(80);
    }
    displayString(1,"Access denied !!",30,10);
    delay(2000);
    }
 }
}
void readMoisture(){
  int readOne = analogRead(MOISTURE);
  readSens = map(readOne,0,1023,100,0);
  delay(100);
 }
void alarm(){
  digitalWrite(ALARM,HIGH);
  delay(50);
  digitalWrite(ALARM,LOW);
  delay(50);
  }
void readMQ() {
  float sensor_volt;              //Define variable for sensor voltage 
  float RS_gas;                  //Define variable for sensor resistance  
  float ratio;                  //Define variable for ratio
  float sensorValue = analogRead(MQ4);    //Read analog values of sensor  
  sensor_volt = sensorValue*(5.0/1023.0);       //Convert analog values to voltage 
  RS_gas = ((5.0*10.0)/sensor_volt)-10.0;      //Get value of RS in a gas
  ratio = RS_gas/R0;                          // Get ratio RS_gas/RS_air
  double ppm_log = (log10(ratio)-b)/m;     //Get ppm value in linear scale according to the the ratio value  
  double ppm = pow(10, ppm_log);          //Convert ppm value to log scale 
  percentage = ppm/10000;  
  delay(100);
}
void drawProgressbar(int x,int y, int width,int height, int progress)
{
   float bar = ((float)(width-1) / 100) * progress;
   display.drawRect(x, y, width, height, WHITE);
   display.fillRect(x+2, y+2, bar , height-4, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
  }
void LOADING(){
  for(int i=0;i<100;i++){  
    digitalWrite(ALARM,HIGH);
    delay(8);
    digitalWrite(ALARM,LOW);
    delay(8);
    display.clearDisplay();    
    drawProgressbar(0,10,120,10,i);
    display.display();
    delay(8);
    }
   for(int i=0;i<2;i++){
    digitalWrite(ALARM,HIGH);
    delay(80);
    digitalWrite(ALARM,LOW);
    delay(80);
    }
  }
void LOADING2(){
  for(int i=0;i<100;i++){  
    digitalWrite(ALARM,HIGH);
    delay(2);
    digitalWrite(ALARM,LOW);
    delay(2);
    display.clearDisplay();    
    drawProgressbar(0,10,120,10,i);
    display.display();
    delay(2);
    }
   for(int i=0;i<2;i++){
    digitalWrite(ALARM,HIGH);
    delay(80);
    digitalWrite(ALARM,LOW);
    delay(80);
    }
  }
 void debug(){
  Serial.print(percentage);
  Serial.print(",");
  Serial.print(readSens);
  Serial.print(",");
  Serial.print(uvIntensity);
  Serial.print(",");
  Serial.print(suhu);
  Serial.print(",");
  Serial.println(kelembaban);
}
