#define BLYNK_TEMPLATE_ID "TMPL3e-9RmmbY"
#define BLYNK_TEMPLATE_NAME "Water Flow Metering System"
#define BLYNK_AUTH_TOKEN "22HL5UOWCFPQaco40InX2pCTOcEaf7N1"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#define buz 13
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Realme GT 7";
char pass[] = "yfnz2629";

volatile int flow_frequency = 0;
float vol = 0.0, l_minute;

unsigned char flowsensor = 25;
unsigned long currentTime;
unsigned long cloopTime;

void IRAM_ATTR flow() {
  flow_frequency++;
}

void setup() {
  Serial.begin(9600);
  pinMode(buz,OUTPUT);
  pinMode(flowsensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, FALLING);

  Blynk.begin(auth, ssid, pass);

  currentTime = millis();
  cloopTime = currentTime;
}

void loop() {
  Blynk.run();
  currentTime = millis();

  if (currentTime - cloopTime >= 1000) {
    cloopTime = currentTime;

    // Noise filter
    if (flow_frequency < 5) {
      flow_frequency = 0;
      Serial.println("Flow rate = 0");
      return;
    }

    l_minute = flow_frequency / 7.5;
    l_minute = l_minute / 60.0;
    vol += l_minute;

    Serial.print("Flow: ");
    Serial.print(l_minute);
    Serial.println(" L/s");

    Serial.print("Volume: ");
    Serial.println(vol);

    if (vol >= 10) {
      Blynk.logEvent("alert");
      digitalWrite(buz,HIGH);
      delay(2000);
      digitalWrite(buz,LOW);
    }
    else{
      digitalWrite(buz,LOW);
    }

    flow_frequency = 0;
    Blynk.virtualWrite(V0, vol);
    Blynk.virtualWrite(V1, l_minute);
  }
}
