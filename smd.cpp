#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <WiFiUdp.h>           // For NTP
#include <NTPClient.h>         // NTP client library
#include <Wire.h>
#include <RtcDS3231.h>         //RTC library
#include <EEPROM.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>
#include <Base64.h>
#include <WiFiClientSecure.h>


#define WIFI_SSID "Galaxy M31"
#define WIFI_PASSWORD "mokshda2"
// #define API_KEY "AIzaSyC5wcEzJyJKvC-HVZeRm-TocDbQMNNMB08"
// #define DATABASE_URL "https://dhanvantari-95014-default-rtdb.firebaseio.com " //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define FIREBASE_HOST "https://dhanvantari-95014-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyC5wcEzJyJKvC-HVZeRm-TocDbQMNNMB08"


// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // 19800 seconds for UTC +5:30

//RtcDS3231 rtcObject;              //Uncomment for version 1.0.1 of the rtc library
RtcDS3231<TwoWire> rtcObject(Wire); //Uncomment for version 2.0.0 of the rtc library

SoftwareSerial mySoftwareSerial(D6, D7); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
// Create a Firebase Data object
FirebaseData firebaseData;

// Firebase Configuration object
FirebaseConfig firebaseConfig;

// Servo objects for controlling the medicine compartments
Servo servo1;
Servo servo2;
Servo servo3;

// Pin assignments for servos
const int servoPin1 = D1;
const int servoPin2 = D2;
const int servoPin3 = D5;


int AV1, AV2, AV3, pDay;
int AVR1, AVR2, AVR3;
int Years, Months, Days, Hours, Min, Sec,timeStamp, date;

String lang,k, m1, m2, m3, iv;
int MS1[] = {1,3,0};
int MS2[] = {1,2,0};
int MS3[] = {1,3,4};
int M1, M2, M3;
int ind1, ind2, ind3;


int AH1, AM1, AH2, AM2, AH3, AM3, W;
int AHR1, AMR1, AHR2, AMR2, AHR3, AMR3;
int onv=1, ofv=0;

// value to be adjust according to your choice
int rt=1;// repeat time gap in min
unsigned long lm, HT = 5000; // time is in milisecond//// **It is the time how much the led will glow after alarm

//MED COUNT
int ctr1=0;
int ctr2=0;
int ctr3=0;

void setup() 
{
Serial.begin(9600);
mySoftwareSerial.begin(9600);
timeClient.begin();
Serial.println("NTP Client initialized.");
EEPROM.begin(4096);
Serial.println();
delay(4000);
  
//DF player begin________________________________________________
   
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println("Unable to begin:");
    Serial.println("1.Please recheck the connection!");
    Serial.println("2.Please insert the SD card!");
    while(true)
    {
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(("DFPlayer Mini online."));
  myDFPlayer.volume(28);  //Set volume value. From 0 to 30
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  delay(4000);
  
  if (WiFi.status() == WL_CONNECTED)
 {
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
 
 

 // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;  // Use legacy token for authentication

  // Initialize Firebase connection
  Firebase.begin(&firebaseConfig, nullptr);
  Firebase.reconnectWiFi(true);  // Automatically reconnect if Wi-Fi is lost




  // Optional: Test Firebase connection
  if (Firebase.beginStream(firebaseData, "/testStream")) {
    Serial.println("Connected to Firebase!");
      // Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback); 
  } else {
    Serial.println("Failed to connect to Firebase");
    Serial.println(firebaseData.errorReason());
  }
  // Test connection and write data to Firebase
  Serial.println("Trying to write data to Firebase...");
  if (Firebase.setInt(firebaseData, "/test/data", 42)) {
    Serial.println("Data successfully written to Firebase.");
  } else {
    Serial.println("Failed to write data to Firebase.");
    Serial.print("Error: ");
    Serial.println(firebaseData.errorReason()); // This will print the detailed error
  }


  
 if (Firebase.ready()) {
 Firebase.getString(fbdo, "DHANVANTARI/Language") ;
        lang= fbdo.to<String>();
        lang.trim(); // Trim any leading/trailing spaces or special characters
lang.replace("\\", ""); // Remove any backslashes
lang.remove(0, 1); // Remove the first "
lang.remove(lang.length() - 1, 1); // Remove the last "

        Serial.print("Language Selected: ");
        Serial.println(lang);
           //starting play
          start();
      
  
//For Medicine set 1____________________
 if (Firebase.getString(fbdo, "DHANVANTARI/MS/MS1")) {
        k = fbdo.to<String>();
        Serial.print("Retrieved MS1: ");
        Serial.println(k); // Debug print
        // Continue with parsing the data...
    } else {
        Serial.println("Failed to retrieve MS1");
         Serial.println(fbdo.errorReason());
    }
    Firebase.getString(fbdo, "DHANVANTARI/MS/MS1");
    k = fbdo.to<String>();
    
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS1[0]=M1; EEPROM.write(40,MS1[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS1[1]=M2; EEPROM.write(41,MS1[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS1[2]=M3; EEPROM.write(42,MS1[2]);
    Serial.print("MS1 - "); Serial.print(MS1[0]); Serial.print(" - "); Serial.print(MS1[1]); Serial.print(" - "); Serial.println(MS1[2]);
     
//For Medicine set 2____________________
   Firebase.getString(fbdo, "DHANVANTARI/MS/MS2");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS2[0]=M1; EEPROM.write(20,MS2[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS2[1]=M2; EEPROM.write(21,MS2[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS2[2]=M3; EEPROM.write(22,MS2[2]); 
    Serial.print("MS2 - "); Serial.print(MS2[0]); Serial.print(" - "); Serial.print(MS2[1]); Serial.print(" - "); Serial.println(MS2[2]);
     

//For Medicine set 3____________________
   Firebase.getString(fbdo, "DHANVANTARI/MS/MS3");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS3[0]=M1; EEPROM.write(30,MS3[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS3[1]=M2; EEPROM.write(31,MS3[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS3[2]=M3; EEPROM.write(32,MS3[2]);
    Serial.print("MS3 - "); Serial.print(MS3[0]); Serial.print(" - "); Serial.print(MS3[1]); Serial.print(" - "); Serial.println(MS3[2]);
     
    //For Alarm 1___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH1"); iv = fbdo.to<String>();
    AH1 = iv.toInt(); EEPROM.write(10,AH1);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM1"); iv = fbdo.to<String>();
    AM1 = iv.toInt(); EEPROM.write(11,AM1);
      // Firebase.setStreamCallback(AH1,AM1, streamCallback, streamTimeoutCallback); 

    if((AM1+rt) == 60) { AHR1=AH1+1; AMR1=0; }
    else               { AHR1=AH1; AMR1=AM1+rt; }
    
//For Alarm 2___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH2"); iv = fbdo.to<String>();
    AH2 = iv.toInt(); EEPROM.write(12,AH2);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM2"); iv = fbdo.to<String>();
    AM2 = iv.toInt(); EEPROM.write(13,AM2);
      // Firebase.setStreamCallback(AH2,AM2, streamCallback, streamTimeoutCallback); 

    if((AM2+rt) == 60) { AHR2=AH2+1; AMR2=0; }
    else               { AHR2=AH2; AMR2=AM2+rt; }

//For Alarm 3___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH3"); iv = fbdo.to<String>();
    AH3 = iv.toInt(); EEPROM.write(14,AH3);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM3"); iv = fbdo.to<String>();
    AM3 = iv.toInt(); EEPROM.write(15,AM3);
      // Firebase.setStreamCallback(AH3,AM3, streamCallback, streamTimeoutCallback); 

    if((AM3+rt) == 60) { AHR3=AH3+1; AMR3=0; }
    else               { AHR3=AH3; AMR3=AM3+rt; }
    
  }//fb ready
 }// have network

// ______________________________________no network________________________________________
else{
 Serial.println("No network");
  }// no net end

EEPROM.commit();

Serial.print(EEPROM.read(10)); Serial.print(":"); Serial.println(EEPROM.read(11));
Serial.print(EEPROM.read(12)); Serial.print(":"); Serial.println(EEPROM.read(13));
Serial.print(EEPROM.read(14)); Serial.print(":"); Serial.println(EEPROM.read(15));

Serial.print(EEPROM.read(40)); Serial.print(" - "); Serial.print(EEPROM.read(41)); Serial.print(" - "); Serial.println(EEPROM.read(42));
Serial.print(EEPROM.read(20)); Serial.print(" - "); Serial.print(EEPROM.read(21)); Serial.print(" - "); Serial.println(EEPROM.read(22));
Serial.print(EEPROM.read(30)); Serial.print(" - "); Serial.print(EEPROM.read(31)); Serial.print(" - "); Serial.println(EEPROM.read(32));

   Serial.print("Repeat Alarm1-  "); Serial.print(AHR1); Serial.print(":"); Serial.println(AMR1);
   Serial.print("Repeat Alarm2-  "); Serial.print(AHR2); Serial.print(":"); Serial.println(AMR2);
   Serial.print("Repeat Alarm3-  "); Serial.print(AHR3); Serial.print(":"); Serial.println(AMR3);
 

 Serial.println("wait for some moment...");
 delay(5000);

// Initialize servos
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);

  // Set servos to initial position
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
}//setup

void loop() 

{
   if (Firebase.ready()) 

  { 
//For Medicine set 1____________________
 if (Firebase.getString(fbdo, "DHANVANTARI/MS/MS1")) {
        k = fbdo.to<String>();
        Serial.print("Retrieved MS1: ");
        Serial.println(k); // Debug print
        // Continue with parsing the data...
    } else {
        Serial.println("Failed to retrieve MS1");
         Serial.println(fbdo.errorReason());
    }
    Firebase.getString(fbdo, "DHANVANTARI/MS/MS1");
    k = fbdo.to<String>();
    
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS1[0]=M1; EEPROM.write(40,MS1[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS1[1]=M2; EEPROM.write(41,MS1[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS1[2]=M3; EEPROM.write(42,MS1[2]);
    Serial.print("MS1 - "); Serial.print(MS1[0]); Serial.print(" - "); Serial.print(MS1[1]); Serial.print(" - "); Serial.println(MS1[2]);
     
//For Medicine set 2____________________
   Firebase.getString(fbdo, "DHANVANTARI/MS/MS2");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS2[0]=M1; EEPROM.write(20,MS2[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS2[1]=M2; EEPROM.write(21,MS2[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS2[2]=M3; EEPROM.write(22,MS2[2]); 
    Serial.print("MS2 - "); Serial.print(MS2[0]); Serial.print(" - "); Serial.print(MS2[1]); Serial.print(" - "); Serial.println(MS2[2]);
     

//For Medicine set 3____________________
   Firebase.getString(fbdo, "DHANVANTARI/MS/MS3");
    k = fbdo.to<String>();
    ind1 = k.indexOf(',');           m1 = k.substring(1, ind1);      M1 = m1.toInt(); MS3[0]=M1; EEPROM.write(30,MS3[0]);
    ind2 = k.indexOf(',', ind1+1 );  m2 = k.substring(ind1+1, ind2); M2 = m2.toInt(); MS3[1]=M2; EEPROM.write(31,MS3[1]);
    ind3 = k.indexOf(']', ind2+1 );  m3 = k.substring(ind2+1, ind3); M3 = m3.toInt(); MS3[2]=M3; EEPROM.write(32,MS3[2]);
    Serial.print("MS3 - "); Serial.print(MS3[0]); Serial.print(" - "); Serial.print(MS3[1]); Serial.print(" - "); Serial.println(MS3[2]);
     
    //For Alarm 1___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH1"); iv = fbdo.to<String>();
    AH1 = iv.toInt(); EEPROM.write(10,AH1);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM1"); iv = fbdo.to<String>();
    AM1 = iv.toInt(); EEPROM.write(11,AM1);
    if((AM1+rt) == 60) { AHR1=AH1+1; AMR1=0; }
    else               { AHR1=AH1; AMR1=AM1+rt; }
    
    
//For Alarm 2___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH2"); iv = fbdo.to<String>();
    AH2 = iv.toInt(); EEPROM.write(12,AH2);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM2"); iv = fbdo.to<String>();
    AM2 = iv.toInt(); EEPROM.write(13,AM2);
    if((AM2+rt) == 60) { AHR2=AH2+1; AMR2=0; }
    else               { AHR2=AH2; AMR2=AM2+rt; }

//For Alarm 3___________________
    Firebase.getString(fbdo, "DHANVANTARI/T/AH3"); iv = fbdo.to<String>();
    AH3 = iv.toInt(); EEPROM.write(14,AH3);
    Firebase.getString(fbdo, "DHANVANTARI/T/AM3"); iv = fbdo.to<String>();
    AM3 = iv.toInt(); EEPROM.write(15,AM3);
    if((AM3+rt) == 60) { AHR3=AH3+1; AMR3=0; }
    else               { AHR3=AH3; AMR3=AM3+rt; }

    
  }//fb ready
  EEPROM.commit();

Serial.print(EEPROM.read(10)); Serial.print(":"); Serial.println(EEPROM.read(11));
Serial.print(EEPROM.read(12)); Serial.print(":"); Serial.println(EEPROM.read(13));
Serial.print(EEPROM.read(14)); Serial.print(":"); Serial.println(EEPROM.read(15));

Serial.print(EEPROM.read(40)); Serial.print(" - "); Serial.print(EEPROM.read(41)); Serial.print(" - "); Serial.println(EEPROM.read(42));
Serial.print(EEPROM.read(20)); Serial.print(" - "); Serial.print(EEPROM.read(21)); Serial.print(" - "); Serial.println(EEPROM.read(22));
Serial.print(EEPROM.read(30)); Serial.print(" - "); Serial.print(EEPROM.read(31)); Serial.print(" - "); Serial.println(EEPROM.read(32));

   Serial.print("Repeat Alarm1-  "); Serial.print(AHR1); Serial.print(":"); Serial.println(AMR1);
   Serial.print("Repeat Alarm2-  "); Serial.print(AHR2); Serial.print(":"); Serial.println(AMR2);
   Serial.print("Repeat Alarm3-  "); Serial.print(AHR3); Serial.print(":"); Serial.println(AMR3);

    delay(1000);
    timeClient.update();  // Update time from NTP server

    // Display the current time in IST (Indian Standard Time)
    Serial.print("Current time (IST): ");
    Serial.println(timeClient.getFormattedTime());  // Returns time as HH:MM:SS

    delay(1000);  // Wait 1 second before next update


    // RtcDateTime currentTime = rtcObject.GetDateTime();    //get the time from the RTC
 
  //  Years= currentTime.Year(); Months= currentTime.Month(); Days= currentTime.Day();
   Hours= timeClient.getHours(); Min= timeClient.getMinutes(); Sec= timeClient.getSeconds();
    
  
    if (Hours == AH1 && Min == AM1 && AV1 == 0) // for alarm1
    {

       its_time ();// Medicine alert
      for(int i = 0; i<sizeof(MS1) / sizeof(MS1[0]); i++)
      {
       Serial.print("  Medicine No. ");
       Serial.print(MS1[i]);
       W = MS1[i];
       work(); // medicine no.
      }
      AV1=1;
      dont_forget(); // dont forget
    }
    
   else if (Hours == AH2 && Min == AM2 && AV2 == 0)
    {
      its_time();
      for(int i = 0; i<sizeof(MS2) / sizeof(MS2[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS2[i]);
        W = MS2[i];
       work();
      }
      AV2=1;
      dont_forget();
    }

   else if (Hours == AH3 && Min == AM3 && AV3 == 0)
    {
     its_time();
      for(int i = 0; i<sizeof(MS3) / sizeof(MS3[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS3[i]);
        W = MS3[i];
        work();
      }
      AV3=1;
      dont_forget();
    }
//Repeat alarm___________________________________________________

 else if (Hours == AHR1 && Min == AMR1 && AVR1 == 0) // for alarm1
    {
      its_time(); // Medicine alert
      for(int i = 0; i<sizeof(MS1) / sizeof(MS1[0]); i++)
      {
       Serial.print("  Medicine No. ");
       Serial.print(MS1[i]);
       W = MS1[i];
       
      }
      AVR1=1;
      dont_forget(); // dont forget
    }
    
   else if (Hours == AHR2 && Min == AMR2 && AVR2 == 0)
    {
      its_time();
      for(int i = 0; i<sizeof(MS2) / sizeof(MS2[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS2[i]);
        W = MS2[i];
       
      }
      AVR2=1;
      dont_forget();
    }

   else if (Hours == AHR3 && Min == AMR3 && AVR3 == 0)
    {
      its_time();
      for(int i = 0; i<sizeof(MS3) / sizeof(MS3[0]); i++)
      {
        Serial.print("  Medicine No. ");
        Serial.print(MS3[i]);
        W = MS3[i];
       
      }
      AVR3=1;
      dont_forget();
    }

}// loop

//_void functions_______________________________
void work()
{
  
  if (W == 0){}
  else if (W == 1)
  {  myDFPlayer.play(11);
    servo1.write(180);  // Rotate 180 degrees
    delay(3000);        // Wait for dispensing
    servo1.write(0);    // Reset to initial position
   delay(3000); 
   ctr1=ctr1+1;

   // Test connection and write data to Firebase
  if (Firebase.setInt(firebaseData, "DHANVANTARI/MEDCOUNT/MED1", ctr1))
  {
    Serial.println("Number of pills of med-1 dispensed-");
    Serial.println(ctr1);
  } else {
    Serial.println("Failed to write data to Firebase.");
    Serial.print("Error: ");
    Serial.println(firebaseData.errorReason()); // This will print the detailed error    
   }
     Serial.println(" Medicine1");
  }
  else if(W == 2)
  {  myDFPlayer.play(12); 
  servo2.write(180);  // Rotate 180 degrees
    delay(3000);        // Wait for dispensing
    servo2.write(0);    // Reset to initial position
  delay(3000);
  ctr2= ctr2+1;
    if (Firebase.setInt(firebaseData, "DHANVANTARI/MEDCOUNT/MED2", ctr2))
  {
    Serial.println("Number of pills of med-2 dispensed-");
    Serial.println(ctr2);
  } else {
    Serial.println("Failed to write data to Firebase.");
    Serial.print("Error: ");
    Serial.println(firebaseData.errorReason()); // This will print the detailed error    
   }
   Serial.println(" Medicine2"); 
    
   }
  else if(W == 3)
  {  myDFPlayer.play(1); 
  servo3.write(180);  // Rotate 180 degrees
    delay(3000);        // Wait for dispensing
    servo3.write(0);    // Reset to initial position
  delay(3000); 
  ctr3=ctr3+1;
    if (Firebase.setInt(firebaseData, "DHANVANTARI/MEDCOUNT/MED3", ctr3))
  {
    Serial.println("Number of pills of med-3 dispensed-");
    Serial.println(ctr3);
  } else {
    Serial.println("Failed to write data to Firebase.");
    Serial.print("Error: ");
    Serial.println(firebaseData.errorReason()); // This will print the detailed error    
   }
  Serial.println(" Medicine3");
  
    }

  }

 void its_time (){
  if(lang =="Hindi"){
    Serial.println("Medicine Alert");
  myDFPlayer.play(2);
  delay(5000);
  // myDFPlayer.play(1); // Tring Medicine Alert It's the time for
  // delay(4050);
}
  else if(lang =="English"){
     Serial.println("Medicine Alert");
  myDFPlayer.play(6);
  delay(5000);
  // myDFPlayer.play(1); // Tring Medicine Alert It's the time for
  // delay(4050);
  }
  else if(lang =="Bengali"){
     Serial.println("Medicine Alert");
  myDFPlayer.play(9);
  delay(5000);
  }
else{
  Serial.println("no matched language");
}
 } 

void dont_forget(){
  if(lang =="Hindi"){
    myDFPlayer.play(3); // This is the last reminder for the medicine don't forget to take your medicine 
  delay(3200);
  Serial.println();
  Serial.println("F-------------");
  lm=millis();
  }
  else if(lang =="English"){
     myDFPlayer.play(7); // This is the last reminder for the medicine don't forget to take your medicine 
  delay(3200);
  Serial.println();
  Serial.println("F-------------");
  lm=millis();
  }
  else if(lang =="Bengali"){
    myDFPlayer.play(10); // This is the last reminder for the medicine don't forget to take your medicine 
  delay(3200);
  Serial.println();
  Serial.println("F-------------");
  lm=millis();
  }
else{
  Serial.println("no matched language");
}
}

  
 void start(){
            
  if(lang =="Hindi"){
    myDFPlayer.play(4); // This is the last reminder for the medicine don't forget to take your medicine 
  
  }
  else if(lang =="English"){
     myDFPlayer.play(5); // This is the last reminder for the medicine don't forget to take your medicine 
  
  }
  else if(lang =="Bengali"){
    myDFPlayer.play(8); // This is the last reminder for the medicine don't forget to take your medicine 
  
  }
else{
  Serial.println("no matched language");
}
            }
