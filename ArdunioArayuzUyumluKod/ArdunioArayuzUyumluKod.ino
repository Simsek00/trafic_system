// Pin tanımlamaları
const int dataPin = 2;
const int latchPin = 3;
const int clockPin = 4;
const int lightSensorPin = A0;

// Yaya butonları
const int pedButton1 = 5;
const int pedButton2 = 6;
const int pedButton3 = 7;
const int pedButton4 = 8;

const int dayBrightness = 255;
const int nightBrightness = 100;
String lastSerialOutOfLightStatus;

// 1. Shift Register
#define CAR1_RED    B00000001
#define CAR1_YELLOW B00000010
#define CAR1_GREEN  B00000100
#define CAR2_RED    B00001000
#define CAR2_YELLOW B00010000
#define CAR2_GREEN  B00100000
#define CAR3_RED    B01000000
#define CAR3_YELLOW B10000000

// 2. Shift Register
#define CAR3_GREEN  B00000001
#define CAR4_RED    B00000010
#define CAR4_YELLOW B00000100
#define CAR4_GREEN  B00001000
#define PED1_RED    B00010000
#define PED1_GREEN  B00100000
#define PED2_RED    B01000000
#define PED2_GREEN  B10000000

// 3. Shift Register
#define PED3_RED    B00000001
#define PED3_GREEN  B00000010
#define PED4_RED    B00000100
#define PED4_GREEN  B00001000

const int lightThreshold = 500;

unsigned long greenTime = 5000;
unsigned long yellowTime = 2000;
unsigned long allRedTime = 1000;

unsigned long previousMillis = 0;
int currentPhase = 0;
int currentBrightness = dayBrightness;
bool isDayMode = true;
String inputString = "";

// Yaya geçiş kontrol değişkenleri
bool pedestrianRequested = false;
unsigned long pedestrianRequestTime = 0;
const unsigned long pedestrianMaxWait = 30000;
const unsigned long pedestrianGreenTime = 15000;
bool pedestrianPhaseActive = false;
unsigned long pedestrianPhaseStart = 0;

void setup() {
  Serial.begin(9600);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(lightSensorPin, INPUT);

  // Yaya buton pinleri
  pinMode(pedButton1, INPUT_PULLUP);
  pinMode(pedButton2, INPUT_PULLUP);
  pinMode(pedButton3, INPUT_PULLUP);
  pinMode(pedButton4, INPUT_PULLUP);

  Serial.println("Trafik Işığı Sistemi Başlatıldı");
  Serial.println("--------------------------------");
}

void loop() {
  checkSerialInput();
  checkLightLevel();
  checkPedestrianButtons();
  updateTrafficLights();
}

void checkSerialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      processCommand(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

void processCommand(String command) {
  command.trim();
  Serial.print("Gelen Komut: ");
  Serial.println(command);

  if (command.startsWith("SABAH:") || command.startsWith("AKSAM:") || command.startsWith("ELLE:")) {
    int firstSpace = command.indexOf(' ');
    if (firstSpace != -1) {
      String mode = command.substring(0, firstSpace);
      String values = command.substring(firstSpace + 1);

      int val1 = values.substring(0, values.indexOf(' ')).toInt();
      values = values.substring(values.indexOf(' ') + 1);
      int val2 = values.substring(0, values.indexOf(' ')).toInt();
      int val3 = values.substring(values.indexOf(' ') + 1).toInt();

      greenTime = val1;
      yellowTime = val2;
      allRedTime = val3;

      Serial.print("Mod: ");
      Serial.println(mode);
      Serial.print("Yeni Süreler -> Yeşil: ");
      Serial.print(greenTime);
      Serial.print("ms, Sarı: ");
      Serial.print(yellowTime);
      Serial.print("ms, Kırmızı (hepsi): ");
      Serial.print(allRedTime);
      Serial.println("ms");
    }
  }
  if(command=="ping"){
    Serial.println("pong");
  } 
  else 
  {
    Serial.println("Geçersiz komut.");
  }
}

void checkLightLevel() {
  int lightValue = analogRead(lightSensorPin);

  if (lightValue > lightThreshold && !isDayMode) {
    isDayMode = true;
    currentBrightness = dayBrightness;
    Serial.println("Gündüz Modu Aktif - LED Parlaklığı Yüksek");
  } 
  else if (lightValue <= lightThreshold && isDayMode) {
    isDayMode = false;
    currentBrightness = nightBrightness;
    Serial.println("Gece Modu Aktif - LED Parlaklığı Düşük");
  }

  static unsigned long lastLightPrint = 0;
  if (millis() - lastLightPrint >= 10000) {
    lastLightPrint = millis();
    Serial.print("Işık Seviyesi: ");
    Serial.print(lightValue);
    Serial.print(" - Mod: ");
    Serial.println(isDayMode ? "Gündüz" : "Gece");
  }
}

void checkPedestrianButtons() {
  if (!pedestrianRequested) {
    if (digitalRead(pedButton1) == LOW || digitalRead(pedButton2) == LOW ||
        digitalRead(pedButton3) == LOW || digitalRead(pedButton4) == LOW) {
      pedestrianRequested = true;
      pedestrianRequestTime = millis();
      Serial.println("Yaya geçişi talebi alındı.");
    }
  }

  if (pedestrianRequested && !pedestrianPhaseActive && millis() - pedestrianRequestTime >= pedestrianMaxWait) {
    activatePedestrianPhase();
  }

  if (pedestrianPhaseActive && millis() - pedestrianPhaseStart >= pedestrianGreenTime) {
    pedestrianPhaseActive = false;
    pedestrianRequested = false;
    Serial.println("Yaya geçişi tamamlandı. Normal döngüye dönülüyor.");
    previousMillis = millis();
  }
}

void activatePedestrianPhase() {
  pedestrianPhaseActive = true;
  pedestrianPhaseStart = millis();
  Serial.println("Yaya geçişi başlatıldı. Araçlar duruyor.");
  setLights(0, 0, 0, 0);  // Araçlar kırmızı
}

void updateTrafficLights() {
  if (pedestrianPhaseActive) return;

  unsigned long currentMillis = millis();

  switch (currentPhase) {
    case 0:
      if (currentMillis - previousMillis >= greenTime) {
        previousMillis = currentMillis;
        currentPhase = 1;
      } else setLights(1, 0, 0, 0);
      break;
    case 1:
      if (currentMillis - previousMillis >= yellowTime) {
        previousMillis = currentMillis;
        currentPhase = 2;
      } else setLights(2, 2, 0, 0);
      break;
    case 2:
      if (currentMillis - previousMillis >= greenTime) {
        previousMillis = currentMillis;
        currentPhase = 3;
      } else setLights(0, 1, 0, 0);
      break;
    case 3:
      if (currentMillis - previousMillis >= yellowTime) {
        previousMillis = currentMillis;
        currentPhase = 4;
      } else setLights(0, 2, 2, 0);
      break;
    case 4:
      if (currentMillis - previousMillis >= greenTime) {
        previousMillis = currentMillis;
        currentPhase = 5;
      } else setLights(0, 0, 1, 0);
      break;
    case 5:
      if (currentMillis - previousMillis >= yellowTime) {
        previousMillis = currentMillis;
        currentPhase = 6;
      } else setLights(0, 0, 2, 2);
      break;
    case 6:
      if (currentMillis - previousMillis >= greenTime) {
        previousMillis = currentMillis;
        currentPhase = 7;
      } else setLights(0, 0, 0, 1);
      break;
    case 7:
      if (currentMillis - previousMillis >= yellowTime) {
        previousMillis = currentMillis;
        currentPhase = 0;
      } else setLights(2, 0, 0, 2);
      break;
  }
}

void setLights(int road1, int road2, int road3, int road4) {
  byte reg[3] = {0, 0, 0};
  String serialoutput;
  

  if (road1 == 0) {
    reg[0] |= CAR1_RED;
    serialoutput += "C1_R ";
    }
  else if (road1 == 1) {
    reg[0] |= CAR1_GREEN;
    serialoutput += "C1_G ";
    }
  else if (road1 == 2) {
    reg[0] |= CAR1_YELLOW;
    serialoutput += "C1_Y ";
    }

  if (road2 == 0) {
    reg[0] |= CAR2_RED;
    serialoutput += "C2_R ";
    }
  else if (road2 == 1) {
    reg[0] |= CAR2_GREEN;
    serialoutput += "C2_G ";
    }
  else if (road2 == 2) {
    reg[0] |= CAR2_YELLOW;
    serialoutput += "C2_Y ";
    }

  if (road3 == 0) {
    reg[0] |= CAR3_RED;
    serialoutput += "C3_R ";
    }
  else if (road3 == 1) {
    reg[1] |= CAR3_GREEN;
    serialoutput += "C3_G ";
    }
  else if (road3 == 2) {
    reg[0] |= CAR3_YELLOW;
    serialoutput += "C3_Y ";
    }

  if (road4 == 0) {
    reg[1] |= CAR4_RED;
    serialoutput += "C4_R ";
    }
  else if (road4 == 1) {
    reg[1] |= CAR4_GREEN;
    serialoutput += "C4_G ";
    }
  else if (road4 == 2) {
    reg[1] |= CAR4_YELLOW;
    serialoutput += "C4_Y ";
    }

  if (road1 == 1) {
    reg[1] |= PED2_GREEN;
    reg[1] |= PED1_RED; reg[2] |= PED3_RED | PED4_RED;
    serialoutput += "P1_R P2_G P3_R P4_R ";
  } else if (road2 == 1) {
    reg[2] |= PED3_GREEN;
    reg[1] |= PED1_RED | PED2_RED; reg[2] |= PED4_RED;
    serialoutput += "P1_R P2_R P3_G P4_R ";
  } else if (road3 == 1) {
    reg[2] |= PED4_GREEN;
    reg[1] |= PED1_RED | PED2_RED; reg[2] |= PED3_RED;
    serialoutput += "P1_R P2_R P3_R P4_G ";
  } else if (road4 == 1) {
    reg[1] |= PED1_GREEN;
    reg[1] |= PED2_RED; reg[2] |= PED3_RED | PED4_RED;
    serialoutput += "P1_G P2_R P3_R P4_R ";
  } else if (pedestrianPhaseActive) {
    reg[1] |= PED1_GREEN;
    reg[2] |= PED3_GREEN | PED4_GREEN;
    reg[1] |= PED2_GREEN;
    serialoutput += "P1_G P2_G P3_G P4_G ";
  } else {
    reg[1] |= PED1_RED | PED2_RED;
    reg[2] |= PED3_RED | PED4_RED;
    serialoutput += "P1_R P2_R P3_R P4_R ";
  }
  if(lastSerialOutOfLightStatus != serialoutput)
   Serial.println("SETLIGHTS "+ serialoutput);
  lastSerialOutOfLightStatus=serialoutput;
  serialoutput.remove(0);
  updateShiftRegister(reg[0], reg[1], reg[2]);
}

void updateShiftRegister(byte reg1, byte reg2, byte reg3) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, reg3);
  shiftOut(dataPin, clockPin, MSBFIRST, reg2);
  shiftOut(dataPin, clockPin, MSBFIRST, reg1);
  digitalWrite(latchPin, HIGH);

  if (currentBrightness < 255) {
    delayMicroseconds(currentBrightness * 10);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    delayMicroseconds((255 - currentBrightness) * 10);
  }
}
