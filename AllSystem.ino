#include "MQ135.h"
#include "DHT.h"
#include "Adafruit_CCS811.h"
#include <Wire.h>
#include "U8glib.h"
#include "string.h"
#include "SharpGP2Y10.h"

#define PIN_MQ135 A0
#define PIN_MQ7 A1
#define PIN_MQ2 A2

#define DHTPIN 3
#define DHTTYPE DHT11   // DHT 11

#define Dust_V0_Pin  A3
#define Dust_ledPin  4

DHT dht(DHTPIN, DHTTYPE);
//MQ135 mq135_sensor = MQ135(PIN_MQ135);
Adafruit_CCS811 ccs;
SharpGP2Y10 dustSensor(Dust_V0_Pin, Dust_ledPin);
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST);  // Dev 0, Fast I2C / TWI

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  Serial.println("CCS811 Reading CO2 and VOC");
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
  
}

void loop() {
  float dustDensity = dustSensor.getDustDensity();
  delay(1000) ;  
  Serial.print("Dust: ");
  Serial.println(dustDensity);
  float temperature = dht.readTemperature(); 
  float humidity = dht.readHumidity(); 
  float f = dht.readTemperature(true);
  if (isnan(humidity) || isnan(temperature) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 21 ; 
    humidity = 75 ;
  } 
//  delay(2000) ;  
  Serial.println("Temp : "+ (String)temperature); 
  Serial.println("Humi : "+ (String)humidity); 
  
//  float ppm = mq135_sensor.getPPM();
//  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
//  Serial.print("MQ135 PPM: ");
//  Serial.print(ppm);
//  Serial.print("\t Corrected PPM: ");
//  Serial.print(correctedPPM);
//  Serial.println("ppm");
  float ppm = analogRead(A0);
  Serial.print("MQ135 PPM: ");
  Serial.println(ppm);
  
  float CO = analogRead(A1) ; 
  Serial.print("CO PPM: ");
  Serial.println(CO);
  
  float GAS = analogRead(A2) ; 
  Serial.print("GAS PPM: ");
  Serial.println(GAS);
  //---------Read CO2 tVOC from CCS sensor---------// 
  String CO2 ; 
  String tVOC ; 
  String CCS_Temper ;
  if(ccs.available()){
    float CCS_Temp = ccs.calculateTemperature();
    if(!ccs.readData()){
       CO2 = String(ccs.geteCO2()) ; 
       delay(100);
       tVOC = String(ccs.getTVOC()) ; 
       delay(100);
       CCS_Temper = String(CCS_Temp) ; 
       delay(100);
      Serial.println("CO2: "+ CO2 +" ppm, TVOC: "+ tVOC+ " ppb   CCS_Temp: "+ CCS_Temper);
    }
    else{
      Serial.println("Sensor read ERROR!");
      ccs.readData();
    }
  }
  Serial.println(" ");
  Serial.println(" ");
  String S_temperature = (String)temperature ; 
  String S_humidity = (String)(humidity) ;
  String S_ppm = (String)(ppm) ;
  String S_CO = (String)(CO) ;
  String S_GAS = (String)(GAS) ;
  String S_Dust = (String)(dustDensity) ;
  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_5x8);      //`
    u8g.drawStr( 0, 9,  "Temp  :"); u8g.drawStr( 40, 9, S_temperature.c_str());   u8g.drawStr( 75, 9, "*C");
    u8g.drawStr( 0, 18, "Humi  :"); u8g.drawStr( 40, 18, S_humidity.c_str());     u8g.drawStr( 75, 18, "%");
    u8g.drawStr( 0, 27, "AirQ  :"); u8g.drawStr( 40, 27, S_ppm.c_str());          u8g.drawStr( 75, 27, "PPM");
    u8g.drawStr( 0, 36, "CO    :"); u8g.drawStr( 40, 36, S_CO.c_str());           u8g.drawStr( 75, 36, "PPM");
    u8g.drawStr( 0, 45, "CO2   :"); u8g.drawStr( 40, 45, CO2.c_str());            u8g.drawStr( 75, 45, "PPM");
    u8g.drawStr( 0, 54, "tVOC  :"); u8g.drawStr( 40, 54, tVOC.c_str());           u8g.drawStr( 75, 54, "PPB");
    u8g.drawStr( 0, 63, "Dust  :"); u8g.drawStr( 40, 63, S_Dust.c_str());         u8g.drawStr( 75, 63, "ug/l");
  } while( u8g.nextPage() );
  
  delay(5000) ;
}
