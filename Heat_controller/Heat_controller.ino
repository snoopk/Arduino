#include <DHT.h>
#include <GyverWDT.h>

#define DHTPIN 8     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define HeaterPIN 2
#define min_t 0 // температура включения нагревателя
#define max_t 4 // температура выключения нагревателя

boolean heater_flag;
uint32_t myTimer;
int period = 2000; // период опроса датчиков

DHT dht(DHTPIN, DHTTYPE);

void setup() {
//  Serial.begin(9600);
  dht.begin();
  heater_flag = 0;
  pinMode(HeaterPIN, OUTPUT);
  //  pinMode(13, OUTPUT);
  Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024); // Режим сторжевого сброса , таймаут ~8с
  myTimer = millis();
}

void loop() {
  if (millis() - myTimer >= period)
  {
    myTimer = millis();
    float t = dht.readTemperature();
 //   if (isnan(t)) { // Check if any reads failed and exit early (to try again).
 //     Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Heating(t);
    /* Serial.print(F("Temperature: "));
       Serial.print(t);
       Serial.println(F("°C "));
    */
  }
  Watchdog.reset();
}

void Heating(float temp) {
  if ((temp < min_t) && (heater_flag == 0)) {
    digitalWrite(HeaterPIN, HIGH); // Нагрев включен
    heater_flag = 1; 
    //    Serial.println("Нагрев включен");
    //    digitalWrite(13, HIGH);
  }
  if ((max_t < temp ) && (heater_flag == 1)) {
    digitalWrite(HeaterPIN, LOW); // Нагрев выключен
    heater_flag = 0;
    //    Serial.println("Нагрев выключен");
    //    digitalWrite(13, LOW);
  }
}
