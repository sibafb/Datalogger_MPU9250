#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include "Arduino_ST7789.h" // Hardware-specific library for ST7789 (with or without CS pin)
#include <SPI.h>
#include <SD.h>

#include <MPU9250_asukiaaa.h>

#define TFT_DC    8
#define TFT_RST   9 
#define TFT_MOSI  7   
#define TFT_SCLK  10  

const int chipSelect = 4;

#define Threshold (0.4)

Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK);

MPU9250 mpu9250;

uint8_t sensorId;
float aX, aY, aZ;
float aX_past, aY_past, aZ_past;
String dataString;

void setup() {
  while(!Serial);
  Serial.begin(115200);
  Wire.begin();

  mpu9250.setWire(&Wire);
  mpu9250.beginAccel();

  sensorId = mpu9250.readId();

  tft.fillScreen(BLACK);
  tft.init(240, 240);
  tft.fillScreen(BLACK);
  delay(40);
  //文字サイズの調整
  tft.setTextSize(3);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   dataFile.println("\tACC X\tACC Y\tACC Z\t TIME STAMP ");
   dataFile.close();
}

void loop() {
  mpu9250.accelUpdate();
  aX = mpu9250.accelX();
  aY = mpu9250.accelY();
  aZ = mpu9250.accelZ();
  dataString = "";

  //IPS液晶（SPI）の通信速度が遅い(ArduinoPromini由来？)ので、変化が一定以下なら表示しない。
  //変化幅が0.3以下の値ではIPS表示が変化しない
  if((aX-aX_past) * (aX-aX_past) > Threshold ||
     (aY-aY_past) * (aY-aY_past) > Threshold ||
     (aZ-aZ_past) * (aZ-aZ_past) > Threshold ){
    
      tft.setCursor(0, 30);
      tft.fillRect(100, 30,  100, 72, BLACK);
      tft.setCursor(0, 30);
      tft.setTextColor(BLUE);
      tft.println("ACC X:" + String(aX));
      tft.setTextColor(GREEN);
      tft.println("ACC Y:" + String(aY));
      tft.setTextColor(RED);
      tft.println("ACC Z:" + String(aZ));

      aX_past = aX;
      aY_past = aY;
      aZ_past = aZ;

      delay(500);
   }

  dataString = "\t" + String(aX) + "\t" + String(aY) + "\t" + String(aZ) + "\t" + String(millis());
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.println(dataString);
    dataFile.close();
  }
  delay(30);
}
