// Libraries
#include <Ultrasonic.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"
#include "animation.h"

// Constants
#define PIN_TRIG 4
#define PIN_ECHO 5
#define PIN_SERVO 6
#define PIN_BUZZER 8
bool DEBUG = true;


//OLED liga no SCL e SDA
#define SCREEN_WIDTH 128  //OLED
#define SCREEN_HEIGHT 64  //OLED
#define OLED_RESET -1 // Mine doesn't have it
#define SCREEN_ADRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialization
Ultrasonic ultrasonic(PIN_TRIG, PIN_ECHO);
Servo servo;

// Timers
elapsedMillis timerServo;
elapsedMillis timerBuzzer;

// -- Global variables -- //

// Variávei para OLED
int frame = 0;

// Variáveis para Sensor
int distance;

// Variáveis para Servo
bool isRotating = true;                 // Variável de controle para parar ou não o servo  
int angle = 0;                          // Ângulo atual (position)
int direction = 1;                      // Direção da rotação: 1 para aumentar, -1 para diminuir
const long servoInterval = 20;          // Intervalo para dar step no servo (ms)
   

// Variáveis para buzzer
bool isBeeping = true;                    // Variável de controle para dar beep ou não
unsigned long previousMillisBuzzer = 0;   // Armazena o tempo da última vez que tocou
const long buzzerInterval = 3000;         // Intervalo entre os beeps (ms)

// Variáveis para lógica de detecção
bool isDetected = false;                  // Vairável de controle para detecção

unsigned long previousMillisDetect;       // Armazena o tempo da última vez que o periodo começou
bool isInDetectPeriod = false;            // Controle do periodo de ação
unsigned long actionTime = 5000;          // Tamanho do periodo de ação (5s)
const long actionInterval = 500;

// Música
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4,8,8,4,4,4,4,4
};

// -- Funções -- //

void startupSong() {
  
  // iterate over the notes of the melody

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8);
  }
}

void servo_setup() {
  servo.attach(PIN_SERVO);
}

void oled_setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  } else {
    display.display();
  }
}

void setup() {
  // Serial
  Serial.begin(9600);

  // Inputs and Outputs
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  servo_setup();

  // Tocar música de inicizalização
  oled_setup();
  startupSong();
  display.clearDisplay();
}

void loop() {

  // Ler distância do sensor
  int distance = ultrasonic.read();
  delay(5);
  
  // Rotacionar o Servo motor
  if(isRotating) {

    // Check if it's time to update the angle
    if (timerServo >= servoInterval ) {
      timerServo = timerServo - servoInterval; // Reset the timer
      
      // Update the angle based on the current direction
      angle += direction;
      
      // Reverse direction at 0 or 180 degrees
      if (angle >= 180 || angle <= 0) {
        direction = -direction;
      }

      // Move servo to the new angle
      servo.write(angle);

      if (DEBUG) {
      Serial.println("Angle: ");
      Serial.println(angle);
      }
    }
  } else {
    if (DEBUG)
    Serial.println("Servo parado");
  }

  if(isBeeping) {
    if (timerBuzzer >= buzzerInterval) {
      timerBuzzer =  timerBuzzer - buzzerInterval;

      tone(PIN_BUZZER, NOTE_B5, 500);
    }
  } else {
    tone(PIN_BUZZER, NOTE_C4, 100);
    delay(100);
    tone(PIN_BUZZER, NOTE_C4, 100);
    noTone(PIN_BUZZER);
  }


  // Mostar informações no display OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Detecção de objeto (Parar servo motor, apitar buzzer, mostrar no lcd)
  if (distance < 20) {
    isRotating = false;
    isBeeping = false;

    display.setCursor(0, 0);
    display.println("Distance:");
    display.println(distance);
    display.display();

  } else {
    isRotating = true;
    isBeeping = true;
  }
  
  display.setCursor(0, 0);
  display.println("Scanning...");
  display.display();


  // Mandar informações para o Serial e consequentemente para Processing
  //Serial.print(angle);
  //Serial.print(",");
  //Serial.print(distance);
  //Serial.print(".");
  
}


