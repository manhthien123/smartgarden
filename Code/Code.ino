#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <AccelStepper.h>

int DHT11Pin = 2;
#define DHTType DHT11
DHT HT(DHT11Pin,DHTType);
float humi;
float tempC;
float tempF;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int Water_Sensor = A0;
int Water_Sensor_Value;
#define FULLSTEP 4
AccelStepper stepper_1(FULLSTEP, 8, 9, 10, 11);
AccelStepper stepper_2(FULLSTEP, 11, 10, 9, 8);

#define Sensor 5
#define Relay 6
int Sensor_value;

#define Light_Sensor 3
int Infrared_Sensor = 4;
int Led= 12; //12345678890
int Light_Sensor_Value, Infrared_Sensor_Value;

void setup() {
  Serial.begin(9600);
  HT.begin();
  delay(1000);
  humi = HT.readHumidity();
  tempC = HT.readTemperature();
  tempF = HT.readTemperature(true);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hu:");
  lcd.print(humi, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(tempC, 1);
  lcd.print("C");
  lcd.print("~");
  lcd.print(tempF, 1);
  lcd.print("F");

  stepper_1.setMaxSpeed(300.0);
  stepper_1.setAcceleration(50.0);
  stepper_1.setSpeed(200);
  stepper_1.setCurrentPosition(0);
  stepper_1.moveTo(4096);

  stepper_2.setMaxSpeed(300.0);
  stepper_2.setAcceleration(50.0);
  stepper_2.setSpeed(200);
  stepper_2.setCurrentPosition(0);
  stepper_2.moveTo(4096);

  pinMode(Sensor, INPUT);
  pinMode(Relay, OUTPUT);

  pinMode (Light_Sensor, INPUT);
  pinMode (Infrared_Sensor, INPUT);
  pinMode (Led, OUTPUT);
}

void loop() {
  Water_Sensor_Value = analogRead(Water_Sensor);
  if (Water_Sensor_Value < 200) { // Do là cảm biến mực nước không phải cảm biến mưa, nếu giá trị nhỏ hơn 200 thì không có mưa
    Serial.println("khong mua");
    if (stepper_1.distanceToGo() == 0) { // nếu vòng quay quay về số 0 thì sẽ ngưng
      stepper_1.moveTo(stepper_1.currentPosition()); 
    }
    stepper_1.run(); // động cơ bước quay
  }
  else {
    Serial.println("mua");
    if (stepper_2.distanceToGo() == 0) {
      stepper_2.moveTo(stepper_2.currentPosition());
    }
    stepper_2.run();
   }
  Sensor_value = digitalRead(Sensor);
  if (Sensor_value == HIGH ) { //Giá trị sáng nghĩa là độ ẩm trong đất thấp
    Serial.println("Do am thap"); 
    digitalWrite(Relay, 1); // Độ ẩm thấp cần bơm nước
  }
  else {
    Serial.println("Do am cao");
    digitalWrite(Relay, 0); // Độ ẩm cao tắt bơm nước 
  }

  Light_Sensor_Value = digitalRead(Light_Sensor);
  Infrared_Sensor_Value = digitalRead(Infrared_Sensor);
  if(Light_Sensor_Value == HIGH) { //Nếu giá trị của cảm biến ánh sáng sáng nghĩa là trời sáng
  if (Infrared_Sensor_Value == HIGH) {// nếu giá trị của cảm biến hồng ngoại sáng nghĩ là có chuyển động
    digitalWrite(Led, HIGH); // Có chuyển động bật đèn
    }
    else {
      digitalWrite(Led, LOW); // Không có chuyển động tắt đèn
    }
  }
else
  digitalWrite(Led, LOW); // Buổi sáng tắt đèn
}