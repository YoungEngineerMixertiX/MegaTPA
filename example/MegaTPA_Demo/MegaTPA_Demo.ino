#include <MegaTPA.h>

trm  timer1(1000);   // 1 сек
trm  timer2(250);    // 250 мс
bnm  button(2, "pullup");
stm  led(9);

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 MegaTPA Timer Demo — if-style by 12yo Master");
}

void loop() {
  // Повторяющееся событие — каждую секунду
  if (timer1.end()) {
    Serial.println("⏰ [timer1.end()] — Прошла 1 секунда!");
  }

  // Одноразовое событие — только один раз
  if (timer2.oneEnd()) {
    Serial.println("🎯 [timer2.oneEnd()] — Прошло 250 мс — и больше никогда!");
  }

  // Кнопка
  if (button.pressed()) {
    Serial.println("🖱️  Кнопка нажата!");
  }

  // LED — яркость по кнопке
  led.set(button.pressed() ? 200 : 50);

  delay(10); // для Serial
}
