/*
 * Title: Spot Welder Control Code
 * Author: David Fernie
 * Description: Arduino script used in the custom-developped spot welder for in-house use by Space Concordia. This spot welder uses the dual-pulse method, with the first pulse being a fraction of the length of the second main pulse.
 * 
 * Copyright 2018 Space Concordia
 */
#define SHORT_RATIO     10
#define DEBOUNCE_DELAY  10    //ms delay before start of weld to let contact bounce die down
#define INTERIM_DELAY   20    //ms delay between the two pulses
#define SAFETY_DELAY    1000  //ms delay following the weld sequence to prevent continual activation
#define BLANK           10    //extra character for digits (0-9)
/*   
 *   _A
 * F|_|B
 * E|_|C
 *   D
 * See display datasheet for more details
 */
#define BUTTON_PIN 2
#define WELD_SIGNAL_PIN 3
#define A_PIN 4
#define B_PIN 5
#define C_PIN 6
#define D_PIN 7
#define E_PIN 8
#define F_PIN 9
#define G_PIN 10
#define D1_PIN 11
#define D2_PIN 12
#define D3_PIN 13
#define D4_PIN 14 //A0

/* On-Off states for element pins. A high output results in an element being off, and a low output results in an element being illuminated (common anode construction) */
const int digits[11][7] = {{0,0,0,0,0,0,1}, //0
                           {1,0,0,1,1,1,1}, //1
                           {0,0,1,0,0,1,0}, //2
                           {0,0,0,0,1,1,0}, //3
                           {1,0,0,1,1,0,0}, //4
                           {0,1,0,0,1,0,0}, //5
                           {0,1,0,0,0,0,0}, //6
                           {0,0,0,1,1,1,1}, //7
                           {0,0,0,0,0,0,0}, //8
                           {0,0,0,1,1,0,0}, //9
                           {1,1,1,1,1,1,1}};//BLANK

int dial_period,period,short_period;  //For calculating period
int dig3,dig2,dig1,dig0;              //Left digit->Right digit for display

void weldISR();
void printDig(int);
void printPeriod();

void setup() {
  for (int i = 3; i <= D4_PIN; i++) pinMode(i,OUTPUT);                //Setup all outputs
  for (int i = 11; i<= D4_PIN; i++) digitalWrite(i,LOW);              //Turn all digits off
  pinMode(BUTTON_PIN,INPUT);                                          //Setup input button for initiating welds
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),weldISR,FALLING); //Set the input button as an interrupt, attach to the ISR
}

void loop() {
  printPeriod();
}

void weldISR(){
  delay(DEBOUNCE_DELAY);
  digitalWrite(WELD_SIGNAL_PIN,HIGH);
  delay(short_period);
  digitalWrite(WELD_SIGNAL_PIN,LOW);
  delay(INTERIM_DELAY);
  digitalWrite(WELD_SIGNAL_PIN,HIGH);
  delay(period);
  digitalWrite(WELD_SIGNAL_PIN,LOW);
  delay(SAFETY_DELAY);
  return;
}

void printDig(int dig){
  for(int i = 4; i <=10; i++){
    digitalWrite(i,digits[dig][i-4]); 
  }
}

void printPeriod(){
  dial_period = analogRead(A5);
  period = map(dial_period,0,1023,1,500);
  short_period = period / SHORT_RATIO;
  
  //dig3, period/1000, if = 0, don't display
  dig3 = period/1000;
  (dig3 == 0)? printDig(BLANK):printDig(dig3);
  digitalWrite(D4_PIN,HIGH);
  delay(15);
  digitalWrite(D4_PIN,LOW);
  
  //dig2, period%1000/100, if =dig3=0, dont display
  dig2 = period%1000/100;
  (dig2 == 0 && dig3 == 0)? printDig(BLANK):printDig(dig2);
  digitalWrite(D3_PIN,HIGH);
  delay(15);
  digitalWrite(D3_PIN,LOW);
  
  //dig1, period%100/10, if =dig3=dig2=0, dont display
  dig1 = period%100/10;
  (dig1 == 0 && dig2 == 0 && dig3 == 0)? printDig(BLANK):printDig(dig1);
  digitalWrite(D2_PIN,HIGH);
  delay(15);
  digitalWrite(D2_PIN,LOW);
  
  //dig0, period%10
  dig0 = period%10;
  printDig(dig0);
  digitalWrite(D1_PIN,HIGH);
  delay(15);
  digitalWrite(D1_PIN,LOW);
}

