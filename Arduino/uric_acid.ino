#include <Wire.h>
#include <LiquidCrystal.h>
#include "Adafruit_TCS34725.h"

// Define LCD pin connections: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Create a TCS34725 sensor instance with integration time of 50ms and gain of 4x.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define M1 2
#define M2 3

#define SENSOR1 A0
#define SENSOR2 A3

uint16_t clear, red, green, blue;

void setup() {
  Serial.begin(9600);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);

  lcd.begin(16, 2); // Initialize LCD as 16x2
  lcd.print("     PROJECT    "); // Display message on the first row
  delay(2000);

  Serial.println("[INFO]...URIC ACID DETECTION DEVICE FROM URIN SAMPLE");
  lcd.setCursor(0, 0);
  lcd.print("    URIC ACID   "); // Display message on the first row
  lcd.setCursor(0, 1);
  lcd.print("    DETECTION   ");
  delay(2000);

  if (tcs.begin()) {
    Serial.println("Found TCS34725 sensor");
    lcd.clear();
    lcd.print("Sensor Found");
  } 
  else {
    Serial.println("No TCS34725 sensor found");
    lcd.clear();
    lcd.print("Sensor Error!");
    while (1);
  }

  Serial.println("[INFO]...Device Initialized");
  lcd.setCursor(0, 0);
  lcd.print("     Device     "); 
  lcd.setCursor(0, 1);
  lcd.print("   Initialized  "); 
  delay(2000);

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("    URIC ACID   "); 
  int data = digitalRead(SENSOR1);
  lcd.setCursor(0, 1);
  lcd.print("PERSON : "); 
  if (data == 0){
    lcd.setCursor(9, 1);
    lcd.print("Yes    "); 
    Serial.println("[INFO]...Person Detected ");
    Serial.println(data);

    for (int i=1; i<=5; i+=1){  // Wait for 5 sec
      lcd.setCursor(0, 0);
      lcd.print("Collecting Urin ");
      lcd.setCursor(0, 1);
      lcd.print("Sample wait, "+String(i)+"s ");
      Serial.println("[INFO]...wait "+String(i));
      delay(1000);
    }

    Serial.println("[INFO]...Moving for Recognition ");
    lcd.setCursor(0, 0);
    lcd.print("   Moving for   ");
    lcd.setCursor(0, 1);
    lcd.print("  Recognition   ");
    clocwise();
    delay(1150);

    while(1){                 // Waiting for correct position
      int obj = digitalRead(SENSOR2);
      if (obj == 1){
        Serial.println("[INFO]... Event Detected"+String(obj));
        stopmotor();
        break;
      }
    }

    delay(1000);

    for (int i=0; i<3; i+=1){
      clear, red, green, blue = get_color_data();
      Serial.print("R: "); Serial.print(red);
      Serial.print("\tG: "); Serial.print(green);
      Serial.print("\tB: "); Serial.print(blue);
      Serial.print("\tC: "); Serial.println(clear);

      lcd.setCursor(0, 0);
      lcd.print(" C   R   G   B  ");
      lcd.setCursor(0, 1);
      lcd.print(String(clear)+" "+String(red)+" "+String(green)+" "+String(blue)+" ");
      delay(1000);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Your Test Result");

    int A_C_THR = 678; // Abnormal 
    int A_R_THR = 138;
    int A_G_THR = 258;
    int A_B_THR = 678;

    int N_C_THR = 685; // Seviour
    int N_R_THR = 139;
    int N_G_THR = 260;
    int N_B_THR = 685;

    // Varish you need to work on this logic, by performing testing. 
  
    if ((clear == N_C_THR) && (red == N_R_THR) && (green == N_G_THR) && (blue == N_B_THR)){
      lcd.setCursor(0, 1);
      Serial.println("[INFO]... SEVIOUR");
      lcd.print("    SEVIOUR     ");
    }
    else if ((clear == A_C_THR) && (red == A_R_THR) && (green == A_G_THR) && (blue == A_B_THR)){
      lcd.setCursor(0, 1);
      Serial.println("[INFO]... ABNORMAL");
      lcd.print("    ABNORMAL    ");
    }
    else{
      lcd.setCursor(0, 1);
      Serial.println("[INFO]... NORMAL");
      lcd.print("     NORMAL     ");
    }
    delay(5000);

  }
  else{
    lcd.setCursor(9, 1);
    lcd.print("No     "); 
  }
}

void clocwise(){
  Serial.println("[INFO]...Clockwise");
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
}

void stopmotor(){
  Serial.println("[INFO]...Stoped Motor");
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
}

void anti_clocwise(){
  Serial.println("[INFO]...AntiClockwise");
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
}

int get_color_data(){

  tcs.getRawData(&red, &green, &blue, &clear);
  return red, green, blue, clear;
}
