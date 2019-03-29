/*
 * This code is made for further development, a simple beginning for robotics with rtos.
 * 
 * A simple autonomous tank compiled with Arduino IDE on STM32-Nucleo.
 * Make sure that the correct FreeRTOS as shown in the #include is installed
 * and tested.
 * No schematic available but should be quite simple to wire by following the sketch.
 * 
 * For information or questions, conctact @ https://github.com/jonimoi
 */

#define INCLUDE_vTaskDelay 1
#include <STM32FreeRTOS.h>;
#include <Servo.h>
#include <HC_SR04.h>
#define servo_max 140
#define servo_min 45
#define servo_mid 90
#define TRIG_PIN D7
#define ECHO_PIN D8
#define ECHO_INT 0
#define IRMASKr D4
#define IRMASK D2
#define LEFTBACK D6
#define LEFTFORWARD D5
#define RIGHTBACK D3
#define RIGHTFORWARD D9


HC_SR04 sensor(TRIG_PIN, ECHO_PIN, ECHO_INT);

Servo SERVOKORV;
int iRead, oRead;
long duration, cmLeft, cmRight;
void MOTOR(void *pvParameters);
void IR(void *pvParameters);
void IRr(void *pvParameters);
void SERVODISTANCE(void *pvParameters);

typedef enum
{
    STATE_1 = 0,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_5,
    STATE_6
} my_state_t;

my_state_t state = STATE_1;

void setup() {
  // put your main code here, to run repeatedly:
    Serial.begin(9600);
    xTaskCreate(
        MOTOR
        ,  (const portCHAR *)"MOTOR" // A name just for humans
        ,  configMINIMAL_STACK_SIZE+100         // Stack size
        ,  NULL
        ,  tskIDLE_PRIORITY + 2           // Priority, with 3, and 0 being the lowest.
        ,  NULL ); 

    xTaskCreate(
        IR
        ,  (const portCHAR *)"IR" // A name just for humans
        ,  configMINIMAL_STACK_SIZE+100      // Stack size
        ,  NULL
        ,  tskIDLE_PRIORITY + 2           // Priority, with 3, and 0 being the lowest.
        ,  NULL );
        
    xTaskCreate(
        IRr
        ,  (const portCHAR *)"IRr" // A name just for humans
        ,  configMINIMAL_STACK_SIZE+100      // Stack size
        ,  NULL
        ,  tskIDLE_PRIORITY + 2           // Priority, with 3, and 0 being the lowest.
        ,  NULL );

    xTaskCreate(
        SERVODISTANCE
        ,  (const portCHAR *)"SERVO" // A name just for humans
        ,  configMINIMAL_STACK_SIZE+100      // Stack size
        ,  NULL
        ,  tskIDLE_PRIORITY + 2           // Priority, with 3, and 0 being the lowest.
        ,  NULL );

    vTaskStartScheduler();  
}

void loop() {}

// Motortask
void MOTOR(void *pvParameters) {
    const TickType_t xDelay = 500;
    pinMode(LEFTBACK,       OUTPUT);
    pinMode(LEFTFORWARD,    OUTPUT);
    pinMode(RIGHTBACK,      OUTPUT);
    pinMode(RIGHTFORWARD,   OUTPUT);

while(true) {
delay(100);
if (oRead == 0 || iRead == 0) {  
  state = STATE_6;
}
 // States for the tank
 switch(state)
    {
        case STATE_1:                  
            forward();
            if (cmLeft < 20 || cmRight < 20) {
              state = STATE_2;
            }
            break;
        case STATE_2:
            if (cmLeft < 20) {
              state = STATE_3;
            }
            else {
              state = STATE_4;
            }
            break;
        case STATE_3:  
            left();
            delay(1000);
            state = STATE_1;
            break;
        case STATE_4:
            right();
            delay(1000);
            state = STATE_1;
            break;
        case STATE_5:
            break;
        case STATE_6:
            fullstop();
            delay(200);
            back();
            delay(1500);
            state = STATE_1;
            break;           
    }
}
} 

void SERVODISTANCE(void *pvParameters) {
    const TickType_t xDelay = 500;
    sensor.begin();
    SERVOKORV.attach(10);
    sensor.start();
     
  while(true) {        
  SERVOKORV.write(servo_min);
  delay(400);
  if(sensor.isFinished()){
    // Do something with the range...
    delay(100);
    cmLeft = sensor.getRange();
    delay(100);
    Serial.println(cmLeft);
    sensor.start();
    }
  SERVOKORV.write(servo_max);
  delay(400);
  if(sensor.isFinished()){
    // Do something with the range...
    delay(100);
    cmRight = sensor.getRange();
    delay(100);
    Serial.println(cmRight);
    sensor.start();
    }
      
}
}
// The IRsensors could be put together to a single task
void IR(void *pvParameters) {
    const TickType_t xDelay = 500;
    pinMode(IRMASK, INPUT);
    
  while(true) {
      iRead = digitalRead (IRMASK);

    if (iRead == HIGH) {
      //Serial.println("+++");
      delay(100);
    }
    else 
    {
      //Serial.println("---");
      delay(100);
    }
              
  }
}

void IRr(void *pvParameters) {
    const TickType_t xDelay = 500;
    pinMode(IRMASKr, INPUT);
    
  while(true) {
      oRead = digitalRead (IRMASKr);

    if (oRead == HIGH) {
      //Serial.println("+++");
      delay(100);
    }
    else 
    {
      //Serial.println("---");
      delay(100);
    }              
  }
}
// Motorfunctions for direction
void forward() {
  digitalWrite(RIGHTFORWARD, HIGH);
  digitalWrite(LEFTFORWARD, HIGH);
  digitalWrite(RIGHTBACK, LOW);
  digitalWrite(LEFTBACK, LOW);
}
void back() {
  digitalWrite(RIGHTFORWARD, LOW);
  digitalWrite(LEFTFORWARD, LOW);
  digitalWrite(LEFTBACK, HIGH);
  digitalWrite(RIGHTBACK, HIGH);
}
void left() {
  digitalWrite(RIGHTFORWARD, HIGH);
  digitalWrite(LEFTFORWARD, LOW);
  digitalWrite(RIGHTBACK, LOW);
  digitalWrite(LEFTBACK, LOW);
}
void right() {
  digitalWrite(RIGHTFORWARD,LOW);
  digitalWrite(RIGHTBACK, LOW);
  digitalWrite(LEFTFORWARD, HIGH);
  digitalWrite(LEFTBACK, LOW);
}
void fullstop() {
  digitalWrite(RIGHTFORWARD,LOW);
  digitalWrite(RIGHTBACK, LOW);
  digitalWrite(LEFTFORWARD, LOW);
  digitalWrite(LEFTBACK, LOW);
}
