#include <FlashStorage.h>

//typedef struct {
//  double temp;
//  double pressure;
//  double alt;
//  float batt;
//  int csum;
//} FlashVarsStruct;
FlashStorage(myAge, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(8000);
  if (myAge.read() == 0) {
    readAndStoreUserAge();
  }
}

void loop() {
  delay(3000);
  Serial.println(myAge.read());
  //delay(10000);
  // put your main code here, to run repeatedly:
//  if (Serial.available() >0){
//    char c = Serial.read();
//    if (c == 'W') {
//      readAndStoreUserAge();
//    } else {
//      Serial.print("My age is ");
//      Serial.print(myAge.read());
//    }
//  }
//  delay(10);
}

void readAndStoreUserAge() {
  Serial.println("Please enter your age:");
  delay(4000);
  String age = Serial.readStringUntil('\n');
  delay(2000);
  myAge.write(age.toInt());  // <-- save the age
}
