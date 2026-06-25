/*
 * ============================================================
 *   SMART CAR PARKING SYSTEM
 * ============================================================
 * Author     : Naveswety M
 * College    : Sathyabama Institute of Science and Technology
 * Board      : Arduino UNO
 * Simulation : Wokwi
 *
 * Description:
 *   An automated car parking management system that tracks
 *   3 parking slots individually using boolean slot states.
 *   Entry/exit is detected via IR sensors. The servo gate
 *   opens automatically on valid entry or exit. Each slot
 *   has its own Green/Red LED pair. LCD displays car count,
 *   free slots, and individual slot status in real time.
 *   Entry is denied with a buzzer tone when the lot is full.
 *
 * Features:
 *   - Auto slot assignment (first available slot on entry)
 *   - Auto slot release (first occupied slot on exit)
 *   - Per-slot dual LED indicators (Green = Empty, Red = Full)
 *   - Entrance dual LED (Green = space available, Red = full)
 *   - LCD shows car count, free slots, and S1/S2/S3 status
 *   - Buzzer tones differ for entry, exit, and denial
 *
 * Pin Configuration:
 *   Pin 2       → Entry IR Sensor
 *   Pin 3       → Exit IR Sensor
 *   Pin 9       → Servo Motor (gate)
 *   Pin 10      → Buzzer
 *   Pin 11      → Entry Green LED
 *   Pin 12      → Entry Red LED
 *   Pin A0      → Slot 1 Green LED
 *   Pin A1      → Slot 1 Red LED
 *   Pin A2      → Slot 2 Green LED
 *   Pin A3      → Slot 2 Red LED
 *   Pin 7       → Slot 3 Green LED
 *   Pin 8       → Slot 3 Red LED
 *   SDA/SCL     → I2C LCD (address 0x27)
 * ============================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ── LCD & Servo ──────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gate;

// ── IR Sensor Pins ───────────────────────────────────────────
#define ENTRY_SENSOR  2
#define EXIT_SENSOR   3

// ── Output Pins ──────────────────────────────────────────────
#define SERVO_PIN     9
#define BUZZER        10

// Entrance LEDs
#define ENTRY_GREEN   11
#define ENTRY_RED     12

// Per-slot LEDs
#define SLOT1_GREEN   A0
#define SLOT1_RED     A1
#define SLOT2_GREEN   A2
#define SLOT2_RED     A3
#define SLOT3_GREEN   7
#define SLOT3_RED     8

// ── Constants ────────────────────────────────────────────────
const int TOTAL_SLOTS      = 3;
const int GATE_OPEN_ANGLE  = 90;
const int GATE_CLOSE_ANGLE = 0;
const int GATE_OPEN_MS     = 2000;   // Gate stays open 2 seconds

// ── State Variables ──────────────────────────────────────────
int  carCount     = 0;
bool slot1Occupied = false;
bool slot2Occupied = false;
bool slot3Occupied = false;
bool prevEntry    = HIGH;
bool prevExit     = HIGH;

// ============================================================
void setup() {
  pinMode(ENTRY_SENSOR, INPUT_PULLUP);
  pinMode(EXIT_SENSOR,  INPUT_PULLUP);

  pinMode(ENTRY_GREEN, OUTPUT);
  pinMode(ENTRY_RED,   OUTPUT);

  pinMode(SLOT1_GREEN, OUTPUT);
  pinMode(SLOT1_RED,   OUTPUT);
  pinMode(SLOT2_GREEN, OUTPUT);
  pinMode(SLOT2_RED,   OUTPUT);
  pinMode(SLOT3_GREEN, OUTPUT);
  pinMode(SLOT3_RED,   OUTPUT);

  gate.attach(SERVO_PIN);
  gate.write(GATE_CLOSE_ANGLE);  // Start with gate closed

  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
  Serial.println("Smart Car Parking System Ready.");
}

// ── Open gate for fixed duration then close ──────────────────
void openGate() {
  gate.write(GATE_OPEN_ANGLE);
  delay(GATE_OPEN_MS);
  gate.write(GATE_CLOSE_ANGLE);
}

// ============================================================
void loop() {

  // ── Count occupied & free slots ──────────────────────────
  int occupied = (slot1Occupied ? 1 : 0)
               + (slot2Occupied ? 1 : 0)
               + (slot3Occupied ? 1 : 0);
  int empty = TOTAL_SLOTS - occupied;

  // ── Update per-slot LEDs ──────────────────────────────────
  // Green = empty, Red = occupied
  digitalWrite(SLOT1_GREEN, !slot1Occupied);
  digitalWrite(SLOT1_RED,    slot1Occupied);
  digitalWrite(SLOT2_GREEN, !slot2Occupied);
  digitalWrite(SLOT2_RED,    slot2Occupied);
  digitalWrite(SLOT3_GREEN, !slot3Occupied);
  digitalWrite(SLOT3_RED,    slot3Occupied);

  // ── Update entrance LEDs ──────────────────────────────────
  digitalWrite(ENTRY_GREEN, empty > 0 ? HIGH : LOW);
  digitalWrite(ENTRY_RED,   empty > 0 ? LOW  : HIGH);

  // ── Handle Entry ─────────────────────────────────────────
  bool currentEntry = digitalRead(ENTRY_SENSOR);

  if (prevEntry == HIGH && currentEntry == LOW) {
    if (empty > 0) {
      // Assign first available slot
      carCount++;
      if      (!slot1Occupied) slot1Occupied = true;
      else if (!slot2Occupied) slot2Occupied = true;
      else if (!slot3Occupied) slot3Occupied = true;

      Serial.print("Car entered. Total: "); Serial.println(carCount);
      tone(BUZZER, 1000, 300);   // Short high beep = entry allowed
      openGate();

    } else {
      // Lot full — deny entry
      Serial.println("Parking FULL. Entry denied.");
      tone(BUZZER, 500, 1000);   // Long low beep = entry denied
    }
  }
  prevEntry = currentEntry;

  // ── Handle Exit ──────────────────────────────────────────
  bool currentExit = digitalRead(EXIT_SENSOR);

  if (prevExit == HIGH && currentExit == LOW) {
    if (carCount > 0) {
      // Release first occupied slot
      carCount--;
      if      (slot1Occupied) slot1Occupied = false;
      else if (slot2Occupied) slot2Occupied = false;
      else if (slot3Occupied) slot3Occupied = false;

      Serial.print("Car exited. Total: "); Serial.println(carCount);
      tone(BUZZER, 1500, 300);   // Short high beep = exit confirmed
      openGate();
    }
  }
  prevExit = currentExit;

  // ── Update LCD ────────────────────────────────────────────
  lcd.clear();

  if (empty == 0) {
    // Lot full — show warning
    lcd.setCursor(0, 0);
    lcd.print("PARKING FULL");
    lcd.setCursor(0, 1);
    lcd.print("Cars: ");
    lcd.print(carCount);

  } else {
    // Show car count + free slots on row 0
    lcd.setCursor(0, 0);
    lcd.print("Cars:");
    lcd.print(carCount);
    lcd.print(" F:");
    lcd.print(empty);

    // Show per-slot status on row 1 (E = Empty, F = Full)
    lcd.setCursor(0, 1);
    lcd.print("S1:");
    lcd.print(slot1Occupied ? "F" : "E");
    lcd.print(" S2:");
    lcd.print(slot2Occupied ? "F" : "E");
    lcd.print(" S3:");
    lcd.print(slot3Occupied ? "F" : "E");
  }

  delay(200);
}
