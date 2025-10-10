#include <MegaTPA.h>

trm  timer1(1000);   // 1 сек
trm  timer2(250);    // 250 мс
trm motorRunTimer(2500); // 2.5 сек
bnm  button(2, "pullup");
stm  led(9);
bldc motor(3, 4, 5); // Пины для мотора

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 MegaTPA Timer Demo + BLDC Motor — if-style by 12yo Master");
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
    motor.setSpeed(100); // Установка скорости при нажатии кнопки
  }

  // LED — яркость по кнопке
  led.set(button.pressed() ? 200 : 50);
  
  // Каждые 3 секунды переключаем состояние мотора
  if (motorRunTimer.end()) {
    motorRunning = !motorRunning;
    
    if (motorRunning) {
      Serial.println("⚡ Включаем мотор на скорости 20");
      motor.setSpeed(20);
      motor.loop(); // Мотор не отвечает на команды
    } else {
      Serial.println("🛑 Выключаем мотор");
      motor.setSpeed(0);
      motor.resume(); // Теперь мотор снова отвечает на команды
    }
  }

  // Если мотор в режиме loop(), он не отвечает на команды
  // Если мотор в режиме resume(), он отвечает на команды

  delay(10); // для Serial
}
