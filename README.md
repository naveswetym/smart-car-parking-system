# 🚗 Smart Car Parking System

An Arduino-based automated parking management system with individual slot tracking, automatic gate control, per-slot LED indicators, and a real-time LCD display. Entry is denied automatically when the lot is full.

> Simulated using **Wokwi**

---

## 📌 Problem Statement

Manual parking management leads to congestion, wasted time, and inefficient space usage. This system automates slot monitoring and gate control, providing real-time status of every parking slot at a glance.

---

## ⚙️ How It Works

1. **IR sensors** at entry and exit detect vehicle presence via beam-break (LOW signal)
2. On valid **entry** → first available slot is auto-assigned, gate opens for 2 seconds
3. On **exit** → first occupied slot is released, gate opens for 2 seconds
4. If lot is **full** → entry gate stays closed, long buzzer tone alerts the driver
5. Each slot has a **Green/Red LED pair** — Green = empty, Red = occupied
6. **Entrance LEDs** show overall availability at the gate
7. **LCD** displays car count, free slots, and per-slot status (E/F) in real time

---

## 🔧 Components Used

| Component | Purpose |
|---|---|
| Arduino UNO | Main microcontroller |
| IR Sensor × 2 | Entry & exit vehicle detection |
| SG90 Servo Motor | Barrier gate control |
| Green + Red LED × 4 | 3 slot indicators + 1 entrance indicator |
| 16x2 I2C LCD | Car count & slot status display |
| Buzzer | Entry / exit / denial audio feedback |

---

## 🗺️ Pin Configuration

| Pin | Component |
|---|---|
| 2 | Entry IR Sensor |
| 3 | Exit IR Sensor |
| 9 | Servo Motor (gate) |
| 10 | Buzzer |
| 11 | Entrance Green LED |
| 12 | Entrance Red LED |
| A0 | Slot 1 Green LED |
| A1 | Slot 1 Red LED |
| A2 | Slot 2 Green LED |
| A3 | Slot 2 Red LED |
| 7 | Slot 3 Green LED |
| 8 | Slot 3 Red LED |
| SDA/SCL | I2C LCD (0x27) |

---

## 🔊 Buzzer Tones

| Event | Tone |
|---|---|
| Entry allowed | Short high beep (1000 Hz, 300ms) |
| Exit confirmed | Short high beep (1500 Hz, 300ms) |
| Entry denied (full) | Long low beep (500 Hz, 1000ms) |

---

## 💻 Simulation

Built and tested on **Wokwi** online simulator.

---

## 📚 Libraries Required

- `Servo.h` — built into Arduino IDE
- `Wire.h` — built into Arduino IDE
- `LiquidCrystal_I2C` — install via Arduino Library Manager

---

## 👤 Author

**Naveswety M**  
B.E. – Electronics and Communication Engineering  
Sathyabama Institute of Science and Technology, Chennai
