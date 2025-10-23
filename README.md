"Dhanvantari - Your Med Saarthi":

A smart medication adherence system, empowering elderly care and a seamless medication management access for their caregivers. 
Dhanvantari comprises:
• A smart, automatic pill dispenser 
• Mobile app to be used for medication management such as medicine combination to be dispensed, alarm time and native language reminder
• Email and SMS alert for missed dosage and medicine intake confirmation 
• Analytics dashboard for tracking medicine adherence and providing insights for healthcare professionals 
• Remote access for caregivers providing hassle free coordination between them and their elders

 Features

- **Automated Medicine Dispensing**: Dispenses medicines from up to 3 compartments at scheduled times.  
- **Custom Alarms**: Plays audio reminders in multiple languages using DFPlayer Mini.  
- **User Control via Mobile App**: Set alarm times, select medicines, and preferred language through a Kodular-based mobile app.  
- **Real-Time Logging**: Medicine intake logged to **Google Sheets** via Apps Script or Firebase.  
- **Alert System**: Sends notifications or emails if medicines are not taken.  
- **Multi-Language Support**: Audio alarms for English, Hindi, or other languages.  

---

## Hardware Components

| Component             | Purpose                                           |
|----------------------|-------------------------------------------------|
| NodeMCU ESP8266       | Main controller for alarms, dispensing, and logging |
| Servo Motors (3)      | Dispense medicines from compartments            |
| RTC Module            | Real-time clock to schedule alarms             |
| DFPlayer Mini         | Plays audio reminders for medicines            |
| Push Button           | Confirms that medicine has been taken          |
| Power Supply          | Powers NodeMCU and connected peripherals       |

---

## Software Components

- **Arduino IDE**: Program NodeMCU and servo control logic  
- **Kodular App**: Mobile interface to set alarms and select medicines  
- **Firebase Realtime Database**: Stores alarm times and medicine selection  
- **Google Apps Script**: Logs medicine intake to Google Sheets  
- **Twilio Alerts**: Sends sms notifications if medicine is not taken  

---

## Data Flow

1. **Alarm Time Setup**: User sets time and medicine combination in the mobile app.  
2. **NodeMCU Fetches Data**: Primary NodeMCU retrieves alarm data from Firebase.  
3. **Medicine Dispensing**: At alarm time, servo motors rotate to dispense selected medicines.  
4. **Audio Reminder**: DFPlayer Mini plays the corresponding alarm sound in the selected language.  
5. **Confirmation & Logging**: User presses a button to confirm intake; data is logged to Google Sheets/Firebase.  
6. **Alert System**: If button is not pressed within 10 minutes, an email alert is sent to the caregiver.  

---

## Project Setup

### Hardware Connections
- **NodeMCU D1 & D2** → RTC Module  
- **NodeMCU D3** → Servo Motor(s)  
- **NodeMCU D6 & D7** → DFPlayer Mini  

### Software Steps
1. Install necessary **Arduino libraries**: `Servo.h`, `RTClib.h`, `ESP8266WiFi.h`, `ESP8266HTTPClient.h`, `DFRobotDFPlayerMini.h`.  
2. Upload NodeMCU firmware for alarm, dispensing, and logging.  
3. Configure Firebase with your database structure:  

