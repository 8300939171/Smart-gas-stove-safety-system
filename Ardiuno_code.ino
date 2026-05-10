#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int buttonPin = 7;
const int flamePin = 4;
const int gasSensorPin = A0;
const int servoPin = 9;
const int buzzerPin = 8;
const int potPin = A1;
const int modeSwitchPin = 6;

Servo myServo;

bool servoState = false;
bool buttonWasPressed = false;
bool buzzerActive = false;

unsigned long pressStartTime = 0;
unsigned long buzzerStartTime = 0;
unsigned long cookingStartTime = 0;

int cookingTimeMinutes = 0;
unsigned long cookingTimeMillis = 0;

void setup() {

  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(flamePin, INPUT_PULLUP);
  pinMode(modeSwitchPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(gasSensorPin, INPUT);

  myServo.attach(servoPin);
  myServo.write(0);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,20);
  display.println("SYSTEM");
  display.println("READY");
  display.display();

  Serial.println("System Ready");

  delay(2000);
}

void loop() {

  bool buttonPressed = (digitalRead(buttonPin) == LOW);
  bool flameDetected = (digitalRead(flamePin) == LOW);
  bool timerMode = (digitalRead(modeSwitchPin) == LOW);

  int gasValue = analogRead(gasSensorPin);
  bool gasDetected = (gasValue > 300);

  int potValue = analogRead(potPin);
  cookingTimeMinutes = map(potValue, 0, 1023, 5, 30);
  cookingTimeMillis = cookingTimeMinutes * 60000UL;

  Serial.print("Timer Set: ");
  Serial.print(cookingTimeMinutes);
  Serial.println(" min");

  // BUTTON PRESS START
  if (buttonPressed && !buttonWasPressed) {
    pressStartTime = millis();
  }

  // BUTTON RELEASE
  if (!buttonPressed && buttonWasPressed) {

    unsigned long pressDuration = millis() - pressStartTime;

    if (pressDuration >= 3000 && !servoState) {

      servoState = true;
      myServo.write(90);
      cookingStartTime = millis();

      Serial.println("Gas ON");

    }

    else if (pressDuration < 3000 && servoState) {

      servoState = false;
      myServo.write(0);
      digitalWrite(buzzerPin, LOW);
      buzzerActive = false;

      Serial.println("Gas OFF (Manual)");
    }
  }

  buttonWasPressed = buttonPressed;

  // TIMER MODE
  if (servoState && timerMode) {

    if (millis() - cookingStartTime >= cookingTimeMillis) {

      myServo.write(0);
      servoState = false;

      Serial.println("Cooking Time Completed");
    }
  }

  // FLAME SAFETY
  if (servoState) {

    if (!flameDetected && !buzzerActive) {

      digitalWrite(buzzerPin, HIGH);
      buzzerStartTime = millis();
      buzzerActive = true;

      Serial.println("Flame NOT detected");
    }

    if (flameDetected) {

      digitalWrite(buzzerPin, LOW);
      buzzerActive = false;
    }

    if (buzzerActive && millis() - buzzerStartTime >= 10000) {

      digitalWrite(buzzerPin, LOW);
      myServo.write(0);
      servoState = false;
      buzzerActive = false;

      Serial.println("Gas OFF (No Flame)");
    }
  }

  // GAS LEAKAGE ALERT
  if(gasDetected)
  {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Gas Leakage Detected!");
  }

  // OLED DISPLAY
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("SMART GAS SYSTEM");

  display.setCursor(0,12);
  display.print("MODE: ");
  display.println(timerMode ? "TIMER" : "MANUAL");

  display.setCursor(0,24);
  display.print("TIME: ");
  display.print(cookingTimeMinutes);
  display.println(" MIN");

  display.setCursor(0,36);
  display.print("GAS: ");
  display.println(servoState ? "ON" : "OFF");

  display.setCursor(0,48);
  display.print("FLAME: ");
  display.println(flameDetected ? "YES" : "NO");

  display.setCursor(70,48);
  display.print("LEAK:");
  display.println(gasDetected ? "YES" : "NO");

  display.display();

  delay(200);
}
