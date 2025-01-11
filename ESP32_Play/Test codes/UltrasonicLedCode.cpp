#include <Arduino.h>

const int trigPin = 38;
const int echoPin = 37;
const int L1 = 6;  //Pin for Led Red / Led 1
const int L2 = 18;  //Pin for Led Green / Led 2
const int L3 = 17;
const int L4 = 16;
const int Bz1 = 5;

long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(L3,OUTPUT); //Onboard Led
  pinMode(L1, OUTPUT);  //Showing output on Led 1
  pinMode(L2, OUTPUT);  // Showing output on Led 2
  pinMode(L4, OUTPUT);
  Serial.begin(115200); //Defining the baud rate 
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH); // Reading the time delay
  distance = duration * 0.034 / 2; // calculating the distance with the formula
  Serial.print("Distance: ");
  Serial.println(distance);

  if(distance <= 5) {
    digitalWrite(L1, HIGH);   
    delay(300);              
    digitalWrite(L1, LOW);    
    delay(300);  
  } else if (distance > 5 && distance <= 10) {
    digitalWrite(L2, HIGH);   
    delay(300);              
    digitalWrite(L2, LOW);    
    delay(300);  
  } else if (distance > 10 && distance <= 15) {  
    digitalWrite(L3, HIGH);   
    delay(300);              
    digitalWrite(L3, LOW);    
    delay(300);     
  } else if (distance > 15 && distance <= 20) {
    digitalWrite(L4, HIGH);   
    delay(300);              
    digitalWrite(L4, LOW);    
    delay(300);
  } else if (distance > 20 && distance <= 29) {
    digitalWrite(L4, HIGH);   
    delay(300);              
    digitalWrite(L4, LOW);    
    delay(300);
   } else if (distance > 30) {
    digitalWrite(Bz1, LOW);   
    delay(500);              
    digitalWrite(Bz1, HIGH);    
    delay(500);   
}

 delay(100);
}