// диммер на симисторе с внешним детектором нуля
// в данном случае реализовано плавное включение/выключение нагрузки

#define SMOOTH_TIME 10000    // время "разгона", миллисекунды
#define HOLD_TIME 100       // время удержания кнопки, миллисекунды

#define ZERO_PIN 2     // пин детектора нуля
#define INT_NUM 0     // соответствующий ему номер прерывания
#define DIMMER_PIN 3  // управляющий пин симистора
#define BUTT_PIN 4    // пин кнопки

#include <GyverButton.h>  // библиотека для опроса кнопок
#include <GyverTimers.h>  // библиотека таймера

GButton start_bt(BUTT_PIN);

boolean dim_flag;         
int dimmer;               

void setup() {
  pinMode(ZERO_PIN, INPUT_PULLUP);
  pinMode(DIMMER_PIN, OUTPUT);
  attachInterrupt(INT_NUM, isr, RISING);  // для самодельной схемы ставь FALLING
  Timer2.enableISR();
  start_bt.setType(HIGH_PULL);
  start_bt.setDirection(NORM_OPEN);
  start_bt.setTimeout(HOLD_TIME);        // таймаут удержания кнопки
}

void loop() { // значение 500-9700, где 500 максимум мощности, 9700 выкл., в сети 50Гц
  start_bt.tick();               // опрос кнопки
  if (start_bt.isHold()) {       // кнопка удержана в течении HOLD_TIME
    dim_flag = !dim_flag; // переключение флага при нажатии кнопки, заретить/разрешить работу
    if (dim_flag) {
      Serial.println("ON");
      digitalWrite(13, HIGH); // включение индикаторного светодиода
      ON();                   // функция плавного включения 
    }
    if (!dim_flag) {
      digitalWrite(13, 0); // выключение индикаторного светодиода
      Serial.println("OFF");
      dim_flag = false;           // запретить работу
      OFF();
      digitalWrite(DIMMER_PIN, LOW); // вырубить ток на всякий случай
    }
  }
}

void ON() {   // плавное включение нагрузки
  int step_delay = SMOOTH_TIME / 255;      // расчёт задержки
  for (int i = 255; i >= 0 ; i--) {       // плавный пуск
    dimmer = map(i, 0, 255, 500, 9700);
    delay(step_delay);                    // при наличии какого то дополнительного фунционала, которй нужно отрабатвать без задержек delay нужно чем то заменить
  }
}

void OFF() {  // плавное выключение нагрузки
  int step_delay = SMOOTH_TIME / 255;    // расчёт задержки
  for (int i = 0; i < 255; i++) {       // плавное выключение
    dimmer = map(i, 0, 255, 500, 9700);
    delay(step_delay);                  // при наличии какого то дополнительного фунционала, которй нужно отрабатвать без задержек delay нужно чем то заменить
  }
}

void isr() { // прерывание детектора нуля
  static int lastDim;
  digitalWrite(DIMMER_PIN, 0);                               // выключаем симистор
  if (lastDim != dimmer) Timer2.setPeriod(lastDim = dimmer);  // если значение изменилось, устанавливаем новый период
  else Timer2.restart();                                     // если нет, то просто перезапускаем со старым
}

ISR(TIMER2_A) { // прерывание таймера
  digitalWrite(DIMMER_PIN, 1);  // включаем симистор
  Timer2.stop();                // останавливаем таймер
}
