#include <SPI.h>
#include <RFID.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define rfidOUTPUT 7
#define rfidStatus 6
#define fingerPrintOUTPUT 5
#define fingerPrintStatus 4

RFID rfid(SS_PIN, RST_PIN);
SoftwareSerial mySerial(2, 3);

int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
int led = 4;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.init();
  pinMode(rfidStatus, OUTPUT);
  pinMode(fingerPrintStatus, OUTPUT);
  pinMode(rfidOUTPUT, OUTPUT);
  pinMode(fingerPrintOUTPUT, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(rfidStatus, LOW);
  digitalWrite(rfidOUTPUT, LOW);
  digitalWrite(fingerPrintStatus,LOW);
  digitalWrite(fingerPrintOUTPUT,LOW);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()
{
  getRFID();
  getFingerprintID();
}

void getRFID(){
  if (rfid.isCard()) {
    digitalWrite(rfidStatus,HIGH);
    if (rfid.readCardSerial()) {
      /* With a new cardnumber, show it. */
      Serial.println(" ");
      Serial.println("Card found");
      serNum0 = rfid.serNum[0];
      serNum1 = rfid.serNum[1];
      serNum2 = rfid.serNum[2];
      serNum3 = rfid.serNum[3];
      serNum4 = rfid.serNum[4];

      //Serial.println(" ");
      Serial.println("Cardnumber:");
      Serial.print("Dec: ");
      Serial.print(rfid.serNum[0], DEC);
      Serial.print(", ");
      Serial.print(rfid.serNum[1], DEC);
      Serial.print(", ");
      Serial.print(rfid.serNum[2], DEC);
      Serial.print(", ");
      Serial.print(rfid.serNum[3], DEC);
      Serial.print(", ");
      Serial.print(rfid.serNum[4], DEC);
      Serial.println(" ");

      Serial.print("Hex: ");
      Serial.print(rfid.serNum[0], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[1], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[2], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[3], HEX);
      Serial.print(", ");
      Serial.print(rfid.serNum[4], HEX);
      Serial.println(" ");
    }
    if (rfid.serNum[0] == 163 && rfid.serNum[1] == 206 && rfid.serNum[2] == 134 && rfid.serNum[3] == 18 && rfid.serNum[4] == 249) {
      digitalWrite(rfidOUTPUT, HIGH);
      Serial.println("Matched");
      delay(300);
      digitalWrite(rfidOUTPUT,LOW);
    }
    else {
      digitalWrite(rfidOUTPUT, LOW);
      Serial.println("Not Match");
    }
    digitalWrite(rfidStatus,LOW);
  }
  rfid.halt();
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); ;
  digitalWrite(fingerPrintOUTPUT,HIGH);
  delay(300);
  digitalWrite(fingerPrintOUTPUT,LOW);
  return finger.fingerID;
}

int8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
