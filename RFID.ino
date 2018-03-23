#include <Servo.h>
#include <MFRC522.h> // Include of the RC522 Library
#include <SPI.h> // Used for communication via SPI with the Module

// ulstrasound sensor pins
#define trigPinL 14
#define echoPinL 15
#define trigPinR 16
#define echoPinR 17

#define SDAPIN 10
#define RESETPIN 8

#define Buzzer 3 // Pin 3 connected to + pin of the Buzzer

byte FoundTag; // Variable used to check if Tag was found
byte ReadTag; // Variable used to store anti-collision value to read Tag information
byte TagData[MAX_LEN]; // Variable used to store Full Tag Data
byte TagSerialNumber[5]; // Variable used to store only Tag Serial Number
byte GoodTagSerialNumber[5] = {0x16, 0x69, 0x7, 0x9E}; // The Tag Serial number we are looking for

MFRC522 nfc(SDAPIN, RESETPIN); // Init of the library using the UNO pins declared above

// -------------------------------------------
// Servo
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;    // variable to store the servo position

void setup() {
  
  // ----------------------------------------
  // Servo Setup
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  // ----------------------------------------
  // Ultrasound sensor Setup
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);

  //------------------------------------------
  // RFID Setup
  pinMode(Buzzer, OUTPUT); // Set buzzer pin to an Output pin
  digitalWrite(Buzzer, LOW); // Buzzer Off at startup
  SPI.begin();
  Serial.begin(115200);

  // Start to find an RFID Module
  Serial.println("Looking for RFID Reader");
  nfc.begin();
  byte version = nfc.getFirmwareVersion(); // Variable to store Firmware version of the Module

  // If can't find an RFID Module 
  if (! version) { 
    Serial.print("Didn't find RC522 board.");
    while(1); //Wait until a RFID Module is found
  }

  // If found, print the information about the RFID Module
  Serial.print("Found chip RC522 ");
  Serial.print("Firmware version: 0x");
  Serial.println(version, HEX);
  Serial.println();
}

void loop() {
  //----------------------------------------------
  // Ultrasound sensor loop
  
  long duration, distance;
  digitalWrite(trigPinL, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPinL, HIGH);
  // delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPinL, LOW);
  duration = pulseIn(echoPinL, HIGH);
  distance = (duration/2) / 29.1;
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500);

  //-----------------------------------------------
  // RFID loop

  String GoodTag="False"; // Variable used to confirm good Tag Detected

  // Check to see if a Tag was detected
  // If yes, then the variable FoundTag will contain "MI_OK"
  FoundTag = nfc.requestTag(MF1_REQIDL, TagData);

  if (FoundTag == MI_OK) {
    delay(200);

    // Get anti-collision value to properly read information from the Tag
    ReadTag = nfc.antiCollision(TagData);
    memcpy(TagSerialNumber, TagData, 4); // Write the Tag information in the TagSerialNumber variable

    Serial.println("Tag detected.");
    Serial.print("Serial Number: ");
    for (int i = 0; i < 4; i++) { // Loop to print serial number to serial monitor
    Serial.print(TagSerialNumber[i], HEX);
    Serial.print(", ");
  }
  Serial.println("");

  // Check if detected Tag has the right Serial number we are looking for 
  for(int i=0; i < 4; i++){
    if (GoodTagSerialNumber[i] != TagSerialNumber[i]) {
      break; // if not equal, then break out of the "for" loop
    }
    if (i == 3) { // if we made it to 4 loops then the Tag Serial numbers are matching
      GoodTag="TRUE";
    } 
  }
  if (GoodTag == "TRUE"){
    Serial.write("RFIDAccept");
    delay(1500);
  }
  else {
    Serial.write("RFIDDenied");
    delay(500); 
  }
 }
  
}

/* End of Code */
