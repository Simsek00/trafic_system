#include <Servo.h>
#include <Wire.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
SoftwareSerial BTserial(0, 1);
// Pin tanımlamaları
const int dataPin = 2;          // 74HC595 için veri pini (SER)
const int latchPin = 3;         // 74HC595 için yazmac saati pini (RCLK)
const int clockPin = 4;         // 74HC595 için saat pini (SRCLK)
const int lightSensorPin = A0;  // Fotorezistör pini
const int buzzerPin = 9;        // Buzzer pini
const int trigPin = 5;          // Ultrasonik sensör TRIG pini - değiştirildi
const int echoPin = 6;          // Ultrasonik sensör ECHO pini - değiştirildi
Servo barrierServo;
Servo barrierServo2;
Servo barrierServo3;
Servo barrierServo4;
const int servoPin = A1;
const int servoPin2 = 8; // Ek servo motor pini
const int barrierDownAngle = 0;
const int barrierUpAngle = 90;
String lastSerialOutOfLightStatus;

// TM1637 Display pin tanımlamaları
const int DISPLAY1_CLK = A4;  // Birinci Display CLK
const int DISPLAY1_DIO = A5;  // Birinci Display DIO
const int DISPLAY2_CLK = A2;  // İkinci Display CLK
const int DISPLAY2_DIO = A3;  // İkinci Display DIO
const int DISPLAY3_CLK = 10;  // Üçüncü Display CLK
const int DISPLAY3_DIO = 11;  // Üçüncü Display DIO
const int DISPLAY4_CLK = 12;  // Dördüncü Display CLK
const int DISPLAY4_DIO = 13;  // Dördüncü Display DIO

// Yaya butonları
//const int pedButton1 = 12;
//const int pedButton2 = 6;
const int pedButton3 = 7;
//const int pedButton4 = 8;

// 7-segment ekranlar
TM1637Display display1(DISPLAY1_CLK, DISPLAY1_DIO);
TM1637Display display2(DISPLAY2_CLK, DISPLAY2_DIO);
TM1637Display display3(DISPLAY3_CLK, DISPLAY3_DIO);
TM1637Display display4(DISPLAY4_CLK, DISPLAY4_DIO);

// Shift Register Tanımlamaları (LÜTFEN KENDİ DEVRENİZE GÖRE DOĞRULAYIN!)
// 1. Shift Register (U1)
#define CAR1_RED B00000001     // Bit 0
#define CAR1_YELLOW B00000010  // Bit 1
#define CAR1_GREEN B00000100   // Bit 2
#define CAR2_RED B00001000     // Bit 3
#define CAR2_YELLOW B00010000  // Bit 4
#define CAR2_GREEN B00100000   // Bit 5
#define CAR3_RED B01000000     // Bit 6 (3. Yol Kırmızı U1'de)
#define CAR3_YELLOW B10000000  // Bit 7 (3. Yol Sarı U1'de)

// 2. Shift Register (U2)
#define CAR3_GREEN_U2 B00000001  // Bit 0 (3. Yol Yeşil U2'de - ÖNEMLİ: Kendi devrenize göre doğrulayın!)
#define CAR4_RED B00000010       // Bit 1
#define CAR4_YELLOW B00000100    // Bit 2
#define CAR4_GREEN B00001000     // Bit 3
#define PED1_RED B00010000       // Bit 4
#define PED1_GREEN B00100000     // Bit 5
#define PED2_RED B01000000       // Bit 6
#define PED2_GREEN B10000000     // Bit 7

// 3. Shift Register (U3)
#define PED3_RED B00000001    // Bit 0
#define PED3_GREEN B00000010  // Bit 1
#define PED4_RED B00000100    // Bit 2
#define PED4_GREEN B00001000  // Bit 3
#define SERVO_PIN2 B00010000  // Bit 4
#define SERVO_PIN3 B00100000  // Bit 5
#define SERVO_PIN4 B01000000  // Bit 6

// Sistem ayarları
const int lightThreshold = 500;  // Gündüz/gece modu geçiş eşiği (Parlaklık ayarı devredışı olduğu için şu an etkisi az)

// Varsayılan zaman ayarları (milisaniye cinsinden)
unsigned long greenTime = 10000;
unsigned long yellowTime = 3000;
// unsigned long allRedTime = 1000; // Komutla ayarlanabilir, ama trafik döngüsünde aktif kullanımı yok

// Yaya geçiş ayarları
unsigned long pedestrianGreenTime = 10000;
unsigned long pedestrianDelay = 10000;  // Yaya butonu basıldıktan sonra bekleme süresi

// Durum takip değişkenleri
unsigned long previousMillis = 0;
int currentPhase = 0;   // 0-7 arası trafik fazları
bool isDayMode = true;  // Parlaklık için (şu an etkisi yok)
String inputString = "";

// Yaya geçiş kontrol değişkenleri
bool pedestrianRequested = false;
unsigned long pedestrianRequestTime = 0;
bool pedestrianPhaseActive = false;
unsigned long pedestrianPhaseStart = 0;
int activePedButton = 0;
bool waitingForPedDelay = false;

// Buzzer kontrol değişkenleri
unsigned long lastBeepTime = 0;
int buzzerPattern = 0;  // 0=normal, 1=uyarı (son 5sn)
const int normalBeepInterval = 1000;
const int warningBeepInterval = 300;
const unsigned long warningPeriod = 5000;  // Son 5 saniye için uyarı süresi

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600); 
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(lightSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  barrierServo.attach(servoPin); barrierServo.write(barrierUpAngle);
  barrierServo2.attach(servoPin2);  barrierServo2.write(barrierUpAngle);

  //pinMode(pedButton1, INPUT_PULLUP);
  // pinMode(pedButton2, INPUT_PULLUP);
  pinMode(pedButton3, INPUT_PULLUP);
  //pinMode(pedButton4, INPUT_PULLUP);

  display1.setBrightness(7);  // 0-7 arası parlaklık
  display2.setBrightness(7);
  display3.setBrightness(7);
  display4.setBrightness(7);
  display1.clear();
  display2.clear();
  display3.clear();
  display4.clear();

  Serial.println("Akilli Trafik Isigi Sistemi Baslatildi");
  Serial.println("------------------------------------");
  Serial.println("Komut: SABAH/AKSAM/ELLE: [YesilSn] [SariSn] [KirmiziHepsiSn]");
  // Başlangıçta tüm ışıklar kırmızı, yayalar kırmızı
  setAllLights(0, false);  // 0: araçlar kırmızı, false: yayalar aktif değil (kırmızı)
}

void loop() {
  checkSerialInput();
  checkBluetoothSerialInput();
  checkLightLevel();  // Şu an sadece seri portta ışık seviyesini raporlar
  checkPedestrianButtons();
  checkPedestrianDelay();
  updateTrafficLights();
  updateDisplays();
  updateBuzzer();
  checkUltrasonicSensor();  // Her döngüde mesafeyi kontrol eder
}

void setServo2Angle(int angle) {
  byte reg3_data = getRegister3Value();
  if (angle > 0) {
    reg3_data |= SERVO_PIN2;
  } else {
    reg3_data &= ~SERVO_PIN2;
  }
  updateRegister3(reg3_data);
}

void setServo3Angle(int angle) {
  byte reg3_data = getRegister3Value();
  if (angle > 0) {
    reg3_data |= SERVO_PIN3;
  } else {
    reg3_data &= ~SERVO_PIN3;
  }
  updateRegister3(reg3_data);
}

void setServo4Angle(int angle) {
  byte reg3_data = getRegister3Value();
  if (angle > 0) {
    reg3_data |= SERVO_PIN4;
  } else {
    reg3_data &= ~SERVO_PIN4;
  }
  updateRegister3(reg3_data);
}

byte getRegister3Value() {
  // Bu fonksiyon 3. register'ın mevcut değerini döndürmeli
  // Gerçek uygulamada global bir değişkende tutulabilir
  static byte reg3_current = 0;
  return reg3_current;
}

void updateRegister3(byte reg3_data) {
  // Sadece 3. register'ı güncelle
  static byte reg1_current = 0;
  static byte reg2_current = 0;

  // Mevcut değeri sakla
  static byte reg3_current = 0;
  reg3_current = reg3_data;

  // Shift register'ı güncelle
  updateShiftRegister(reg1_current, reg2_current, reg3_current);
}

void checkUltrasonicSensor() {
  static unsigned long lastDetectionTime = 0;
  float distance = measureDistance();

  if (distance < 5 && distance > 0) {            // 0-5 cm arası algılama
    if (millis() - lastDetectionTime >= 1000) {  // Saniyede bir raporla
      Serial.println("Arac algilandi! Mesafe: " + String(distance) + " cm");
      lastDetectionTime = millis();
      // Burada araç sayısını artırabilirsiniz:
      // carCount++;
      // Serial.println("Gecen arac sayisi: " + String(carCount));
    }
  }
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout ekledim
  if (duration == 0) return 999;                   // Timeout veya hata durumu
  return (duration / 2.0) * 0.0343;
}

void updateBuzzer() {
  if (pedestrianPhaseActive && (millis() - pedestrianPhaseStart < pedestrianGreenTime)) {
    unsigned long currentTime = millis();
    if (buzzerPattern == 1) {  // Uyarı modu (son 5 saniye)
      if (currentTime - lastBeepTime >= warningBeepInterval) {
        lastBeepTime = currentTime;
        tone(buzzerPin, 2000, 150);
      }
    } else {  // Normal mod
      if (currentTime - lastBeepTime >= normalBeepInterval) {
        lastBeepTime = currentTime;
        tone(buzzerPin, 1000, 200);
      }
    }
  }
  // Yaya fazı dışındaki sesler (buton basma, faz bitişi) kendi fonksiyonlarında yönetiliyor.
}

void checkSerialInput() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {  // Hem newline hem de carriage return kabul et
      if (inputString.length() > 0) {
        processCommand(inputString);
        inputString = "";
      }
    } else {
      inputString += inChar;
    }
  }
}
void checkBluetoothSerialInput() {
  while (BTserial.available()) {
    char inChar = (char)BTserial.read();
    if (inChar == '\n' || inChar == '\r') {  // Hem newline hem de carriage return kabul et
      if (inputString.length() > 0) {
        processCommand(inputString);
        inputString = "";
      }
    } else {
      inputString += inChar;
    }
  }
}

void processCommand(String command) {
  command.trim();
  Serial.print("Gelen Komut: '");
  Serial.print(command);
  Serial.println("'");

  if (command.startsWith("SABAH:") || command.startsWith("AKSAM:") || command.startsWith("ELLE:")) {
    int firstSpace = command.indexOf(':');  // ':' karakterine göre ayır
    if (firstSpace != -1) {
      String mode = command.substring(0, firstSpace);
      String valuesPart = command.substring(firstSpace + 1);
      valuesPart.trim();

      long val[3] = { 0, 0, 0 };  // long kullandım, süreler büyük olabilir
      int currentValIndex = 0;
      String tempNum = "";

      for (int i = 0; i < valuesPart.length(); i++) {
        if (valuesPart.charAt(i) == ' ') {
          if (tempNum.length() > 0 && currentValIndex < 3) {
            val[currentValIndex++] = tempNum.toInt();
            tempNum = "";
          }
        } else {
          tempNum += valuesPart.charAt(i);
        }
      }
      if (tempNum.length() > 0 && currentValIndex < 3) {  // Son sayıyı da al
        val[currentValIndex++] = tempNum.toInt();
      }

      if (currentValIndex == 3 && val[0] > 0 && val[1] > 0 && val[2] >= 0) {  // val[2] (allRedTime) 0 olabilir
        greenTime = val[0] * 1000UL;
        yellowTime = val[1] * 1000UL;
        // allRedTime = val[2] * 1000UL; // Bu değişkenin trafik döngüsüne entegrasyonu size bağlı
        Serial.print("Mod: ");
        Serial.println(mode);
        Serial.print("Yeni Sureler -> Yesil: ");
        Serial.print(greenTime);
        Serial.print("ms, Sari: ");
        Serial.print(yellowTime);
        // Serial.print("ms, Kirmizi (hepsi): "); Serial.print(allRedTime);
        Serial.println("ms");
      } else {
        Serial.println("Hata: Gecersiz sure veya eksik parametre!");
      }
    } else {
      Serial.println("Hata: Komut formatinda ':' eksik!");
    }
  }
  if (command == "ping") {
     BTserial.println("pong"); // "pong" mesajını gönder
    Serial.println("pong");
  } else {
    Serial.println("Geçersiz komut.");
  }
}

void checkLightLevel() {
  // Parlaklık ayarı şu an için sadece seri monitöre raporlama yapıyor
  static unsigned long lastLightPrint = 0;
  if (millis() - lastLightPrint >= 10000) {  // Her 10 saniyede bir
    int lightValue = analogRead(lightSensorPin);
    lastLightPrint = millis();
    Serial.print("Isik Seviyesi: ");
    Serial.println(lightValue);
    // Eskiden burada parlaklık ayarı vardı, şimdilik devredışı.
    // if (lightValue > lightThreshold && !isDayMode) { isDayMode = true; ... }
  }
}

void checkPedestrianButtons() {
  if (!pedestrianRequested && !waitingForPedDelay && !pedestrianPhaseActive) {
    bool buttonPressedThisCycle = false;  // Butona bir kez basılmasını sağlamak için
    // if (digitalRead(pedButton1) == LOW) { activePedButton = 1; buttonPressedThisCycle = true; }
    // if (digitalRead(pedButton2) == LOW) { activePedButton = 2; buttonPressedThisCycle = true; }
    if (digitalRead(pedButton3) == LOW) {
      activePedButton = 3;
      buttonPressedThisCycle = true;
    } 
    // else if (digitalRead(pedButton4) == LOW) { activePedButton = 4; buttonPressedThisCycle = true;}

    if (buttonPressedThisCycle) {
      pedestrianRequested = true;
      pedestrianRequestTime = millis();
      waitingForPedDelay = true;
      Serial.println(String(activePedButton) + " Nolu yaya talebi " + String(pedestrianDelay / 1000) + "sn bekleme basladi.");
      BTserial.println("3 Nolu yaya talebi");
      tone(buzzerPin, 1500, 500);  // Buton basma sesi
    }
  }

  if (pedestrianPhaseActive && (millis() - pedestrianPhaseStart >= pedestrianGreenTime)) {
    pedestrianPhaseActive = false;
    pedestrianRequested = false;
    waitingForPedDelay = false;
    activePedButton = 0;

    barrierServo.write(barrierUpAngle);
    barrierServo2.write(barrierUpAngle);
    setServo3Angle(barrierUpAngle);
    setServo4Angle(barrierUpAngle);

    Serial.println("Yaya gecisi tamamlandi. Normal dongu.");
    previousMillis = millis();  // Trafik fazı zamanlayıcısını sıfırla
    // currentPhase kaldığı yerden devam eder.

    noTone(buzzerPin);  // Aktif yaya sesini kes
    // Yaya geçişi bitiş özel sesi
    tone(buzzerPin, 2500, 150);
    delay(200);  // delay'ler kısa süreli, idealde millis() ile yapılmalı
    tone(buzzerPin, 2000, 150);
    delay(200);
    tone(buzzerPin, 1500, 300);
    lastBeepTime = millis();  // Bir sonraki olası buzzer sesi için
    buzzerPattern = 0;
  }
}

void checkPedestrianDelay() {
  if (waitingForPedDelay && (millis() - pedestrianRequestTime >= pedestrianDelay)) {
    waitingForPedDelay = false;
    if (pedestrianRequested) {  // Talep hala geçerliyse
      activatePedestrianPhase();
    }
  }
}

void activatePedestrianPhase() {
  pedestrianPhaseActive = true;
  pedestrianPhaseStart = millis();
  lastBeepTime = millis();  // Buzzer zamanlayıcısını sıfırla
  buzzerPattern = 0;        // Normal ses paterniyle başla

  barrierServo.write(barrierDownAngle);
  barrierServo2.write(barrierDownAngle);
  setServo3Angle(barrierDownAngle);
  setServo4Angle(barrierDownAngle);

  Serial.println(String(activePedButton) + " Nolu yaya gecisi aktif. Araclar durduruldu.");
  setAllLights(0, true);  // Tüm araçlar kırmızı, yayalar yeşil
}

void updateTrafficLights() {
  if (pedestrianPhaseActive) {
    // Yaya fazı aktifken ışıklar activatePedestrianPhase ve setAllLights ile yönetiliyor.
    // Ek bir şey yapmaya gerek yok, sadece diğer fazlara geçişi engelle.
    return;
  }

  unsigned long currentMillis = millis();
  unsigned long currentPhaseDuration = (currentPhase % 2 == 0) ? greenTime : yellowTime;  // 0,2,4,6 Yeşil; 1,3,5,7 Sarı

  if (currentMillis - previousMillis >= currentPhaseDuration) {
    previousMillis = currentMillis;
    currentPhase = (currentPhase + 1) % 8;  // 8 faz (0-7)
  }

  // Işıkları mevcut faza göre ayarla
  switch (currentPhase) {
    case 0: setLights(1, 0, 0, 0); break;  // Yol 1 Yeşil, diğerleri Kırmızı
    case 1: setLights(2, 2, 0, 0); break;  // Yol 1 Sarı, Yol 2 Sarı, Yol 3 Kırmızı, Yol 4 Kırmızı
    case 2: setLights(0, 1, 0, 0); break;  // Yol 2 Yeşil, diğerleri Kırmızı
    case 3: setLights(0, 2, 2, 0); break;  // Yol 1 Kırmızı, Yol 2 Sarı, Yol 3 Sarı, Yol 4 Kırmızı
    case 4: setLights(0, 0, 1, 0); break;  // Yol 3 Yeşil, diğerleri Kırmızı
    case 5: setLights(0, 0, 2, 2); break;  // Yol 1 Kırmızı, Yol 2 Kırmızı, Yol 3 Sarı, Yol 4 Sarı
    case 6: setLights(0, 0, 0, 1); break;  // Yol 4 Yeşil, diğerleri Kırmızı
    case 7: setLights(2, 0, 0, 2); break;  // Yol 1 Sarı, Yol 2 Kırmızı, Yol 3 Kırmızı, Yol 4 Sarı
  }
}

// Tüm araç trafik ışıklarını aynı duruma getir (Genellikle yaya modu için)
// carLightState: 0=Kırmızı
// pedestrianActiveState: true=Yayalar Yeşil, false=Yayalar Kırmızı
void setAllLights(int carLightState, bool pedestrianActiveState) {
  byte reg[3] = { 0, 0, 0 };

  // Araç ışıkları
  if (carLightState == 0) {  // Tüm araçlar Kırmızı
    reg[0] |= CAR1_RED | CAR2_RED | CAR3_RED;
    reg[1] |= CAR4_RED;
    Serial.println("SETLIGHTS C1_R C2_R C3_R C4_R");
    BTserial.println("SETLIGHTS C1_R C2_R C3_R C4_R");
  }
  // Başka toplu araç durumları (örn: hepsi sarı yanıp sönme) buraya eklenebilir.

  // Yaya ışıkları
  if (pedestrianActiveState) {  // Yayalar aktif (Yeşil)
    reg[1] |= PED1_GREEN | PED2_GREEN;
    reg[2] |= PED3_GREEN | PED4_GREEN;
    Serial.println("SETLIGHTS P1_G P2_G P3_G P4_G");
    BTserial.println("SETLIGHTS P1_G P2_G P3_G P4_G");
  } else {  // Yayalar pasif (Kırmızı)
    reg[1] |= PED1_RED | PED2_RED;
    reg[2] |= PED3_RED | PED4_RED;
    Serial.println("SETLIGHTS P1_R P2_R P3_R P4_R");
    BTserial.println("SETLIGHTS P1_R P2_R P3_R P4_R");
  }
  updateShiftRegister(reg[0], reg[1], reg[2]);
}

// Belirli bir trafik ışığı düzenini ayarla
// roadX: 0=Kırmızı, 1=Yeşil, 2=Sarı
// Bu fonksiyon çağrıldığında yaya ışıkları otomatik olarak araç durumuna göre ayarlanır.
void setLights(int road1State, int road2State, int road3State, int road4State) {
  byte reg[3] = { 0, 0, 0 };  // Shift register byte'ları
  String serialoutput;
  // 1. Araç Yolu
  if (road1State == 1) {
    reg[0] |= CAR1_GREEN;
    serialoutput += "C1_G ";
  } else if (road1State == 2) {
    reg[0] |= CAR1_YELLOW;
    serialoutput += "C1_Y ";
  } else {
    reg[0] |= CAR1_RED;
    serialoutput += "C1_R ";
  }

  // 2. Araç Yolu
  if (road2State == 1) {
    reg[0] |= CAR2_GREEN;
    serialoutput += "C2_G ";
  } else if (road2State == 2) {
    reg[0] |= CAR2_YELLOW;
    serialoutput += "C2_Y ";
  } else {
    reg[0] |= CAR2_RED;
    serialoutput += "C2_R ";
  }

  // 3. Araç Yolu (Yeşil U2'de, Kırmızı/Sarı U1'de olduğunu varsayıyoruz - KONTROL EDİN!)
  if (road3State == 1) {
    reg[1] |= CAR3_GREEN_U2;
    serialoutput += "C3_G ";
  }  // Yeşil U2'de
  else if (road3State == 2) {
    reg[0] |= CAR3_YELLOW;
    serialoutput += "C3_Y ";
  }  // Sarı U1'de
  else {
    reg[0] |= CAR3_RED;
    serialoutput += "C3_R ";
  }  // Kırmızı U1'de

  // 4. Araç Yolu
  if (road4State == 1) {
    reg[1] |= CAR4_GREEN;
    serialoutput += "C4_G ";
  } else if (road4State == 2) {
    reg[1] |= CAR4_YELLOW;
    serialoutput += "C4_Y ";
  } else {
    reg[1] |= CAR4_RED;
    serialoutput += "C4_R ";
  }

  // Yaya Işıkları (Normal trafik akışında araçlara göre otomatik ayarlanır)
  // Mantık: Bir araç yolu yeşilse, o yola paralel yaya geçitleri de yeşil olabilir.
  // Diğer tüm yaya geçitleri kırmızı olur.
  // Bu mantığı kendi kavşak düzeninize göre detaylandırmanız gerekebilir.
  // Örnek:
  if (road1State == 1) {   // Yol 1 Yeşil ise Yaya 1&Paralelindekiler Yeşil
    reg[1] |= PED1_GREEN;  // PED1'in Yol 1'e paralel olduğunu varsayalım
    reg[2] |= PED4_RED;    // Diğerleri kırmızı
    reg[1] |= PED2_RED;
    reg[2] |= PED3_RED;
    serialoutput += "P1_G P2_R P3_R P4_R ";
  } else if (road2State == 1) {  // Yol 2 Yeşil ise Yaya 2&Paralelindekiler Yeşil
    reg[1] |= PED2_GREEN;
    reg[1] |= PED1_RED;
    reg[2] |= PED3_RED;
    reg[2] |= PED4_RED;
    serialoutput += "P1_R P2_G P3_R P4_R ";
  } else if (road3State == 1) {  // Yol 3 Yeşil ise Yaya 3&Paralelindekiler Yeşil
    reg[2] |= PED3_GREEN;
    reg[1] |= PED1_RED;
    reg[1] |= PED2_RED;
    reg[2] |= PED4_RED;
    serialoutput += "P1_R P2_R P3_G P4_R ";
  } else if (road4State == 1) {  // Yol 4 Yeşil ise Yaya 4&Paralelindekiler Yeşil
    reg[2] |= PED4_GREEN;
    reg[1] |= PED1_RED;
    reg[1] |= PED2_RED;
    reg[2] |= PED3_RED;
    serialoutput += "P1_R P2_R P3_R P4_G ";
  } else {  // Hiçbir araç yolu yeşil değilse (sarı veya hepsi kırmızı), tüm yayalar kırmızı
    reg[1] |= PED1_RED | PED2_RED;
    reg[2] |= PED3_RED | PED4_RED;
    serialoutput += "P1_R P2_R P3_R P4_R ";
  }
  if (lastSerialOutOfLightStatus != serialoutput)
    Serial.println("SETLIGHTS " + serialoutput);
    BTserial.println("SETLIGHTS " + serialoutput);

  lastSerialOutOfLightStatus = serialoutput;
  serialoutput.remove(0);
  updateShiftRegister(reg[0], reg[1], reg[2]);
}

void updateShiftRegister(byte reg1_data, byte reg2_data, byte reg3_data) {
  digitalWrite(latchPin, LOW);
  // Veriler LSB'den MSB'ye doğru mu yoksa tam tersi mi shift edilecek?
  // Genellikle en son gönderilen byte, Arduino'ya en uzak olan register'a gider (daisy chain).
  // Eğer U3 en uzaktaysa:
  shiftOut(dataPin, clockPin, MSBFIRST, reg3_data);  // U3 (PED3, PED4)
  shiftOut(dataPin, clockPin, MSBFIRST, reg2_data);  // U2 (CAR3_G, CAR4, PED1, PED2)
  shiftOut(dataPin, clockPin, MSBFIRST, reg1_data);  // U1 (CAR1, CAR2, CAR3_R, CAR3_Y)
  digitalWrite(latchPin, HIGH);

  // YANIP SÖNME SORUNUNUN KAYNAĞI OLAN PARLAKLIK KONTROLÜ DEVRE DIŞI BIRAKILDI.
  // Işıklar sabit yandıktan sonra bu konu tekrar değerlendirilebilir (donanımsal PWM daha iyi bir çözüm olabilir).
}

void updateDisplays() {
  unsigned long currentMillis = millis();

  if (pedestrianPhaseActive) {
    // Yaya fazı aktifken tüm ekranlarda kalan süreyi göster
    long remainingTimeMs = pedestrianGreenTime - (currentMillis - pedestrianPhaseStart);
    int remainingTimeSec = remainingTimeMs / 1000;
    if (remainingTimeSec < 0) remainingTimeSec = 0;

    // Son 5 saniye için uyarı modunu ayarla
    if (remainingTimeSec <= 5 && buzzerPattern == 0) {
      buzzerPattern = 1;  // Uyarı modu
      Serial.println("Yaya: Son 5sn, buzzer uyari.");
    } else if (remainingTimeSec > 5 && buzzerPattern == 1) {
      buzzerPattern = 0;  // Normal moda dön
    }

    // Tüm displaylere aynı değeri yaz
    uint8_t digits[] = { 0, 0, 0, 0 };
    digits[0] = remainingTimeSec / 10;
    digits[1] = remainingTimeSec % 10;

    display1.showNumberDecEx(remainingTimeSec, 0, true);  // true = başa sıfır ekle
    display2.showNumberDecEx(remainingTimeSec, 0, true);
    display3.showNumberDecEx(remainingTimeSec, 0, true);
    display4.showNumberDecEx(remainingTimeSec, 0, true);

  } else {
    // Normal trafik döngüsü - her ekran kendi yolunun süresini göstersin
    buzzerPattern = 0;  // Normal mod

    // Şu anki fazın toplam süresi
    unsigned long currentPhaseTotalTime = (currentPhase % 2 == 0) ? greenTime : yellowTime;
    unsigned long elapsedTimeInPhase = currentMillis - previousMillis;
    long currentPhaseRemainingTimeMs = currentPhaseTotalTime - elapsedTimeInPhase;
    int currentPhaseRemainingTimeSec = currentPhaseRemainingTimeMs / 1000;
    if (currentPhaseRemainingTimeSec < 0) currentPhaseRemainingTimeSec = 0;

    // Her yolun göstereceği süreleri hesapla
    // Burada karışık bir mantık var: Her yol bir sonraki yeşile ne kadar kaldığını göstermeli
    int countdownTimes[4] = { 0, 0, 0, 0 };  // Yol1, Yol2, Yol3, Yol4 için geri sayım süreleri

    // Her bir yol için bir sonraki yeşile ne kadar kaldığını hesapla
    for (int road = 0; road < 4; road++) {
      int nextGreenPhase = road * 2;  // Her yol için yeşil fazın indeksi (0, 2, 4, 6)
      int phasesToGo;

      if (currentPhase <= nextGreenPhase)
        phasesToGo = nextGreenPhase - currentPhase;
      else
        phasesToGo = 8 - currentPhase + nextGreenPhase;

      // Toplam bekleyeceği süreyi hesapla
      int totalWaitTimeInSec = currentPhaseRemainingTimeSec;

      // Araya giren fazların sürelerini ekle
      for (int i = 1; i < phasesToGo; i++) {
        int intermediatePhase = (currentPhase + i) % 8;
        totalWaitTimeInSec += ((intermediatePhase % 2 == 0) ? greenTime : yellowTime) / 1000;
      }

      countdownTimes[road] = totalWaitTimeInSec;

      // Eğer şu an bu yolun fazındaysak, direkt olarak kalan süreyi göster
      if ((currentPhase % 8) == nextGreenPhase) {  // Yeşil faz
        countdownTimes[road] = currentPhaseRemainingTimeSec;

      } else if ((currentPhase % 8) == (nextGreenPhase + 1) % 8) {  // Sarı faz
        countdownTimes[road] = currentPhaseRemainingTimeSec;
      }
    }

    // Her ekranı güncelle
    display1.showNumberDecEx(countdownTimes[0], 0, true);
    display2.showNumberDecEx(countdownTimes[1], 0, true);
    display3.showNumberDecEx(countdownTimes[2], 0, true);
    display4.showNumberDecEx(countdownTimes[3], 0, true);
  }
}
