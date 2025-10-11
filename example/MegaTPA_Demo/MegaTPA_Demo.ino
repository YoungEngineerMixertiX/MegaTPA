/*
  🚀 MegaTPA Full Demo — by 12yo Embedded Master
  Включает:
    - trm: таймеры (повторяющиеся и одноразовые)
    - bnm: кнопка с антидребезгом
    - stm: софт-PWM для LED
    - bldc: управление BLDC-мотором
    - shs: софт-сериал для связи между Arduino

  Подключение:
    - Кнопка: пин 2 → GND (внутренний подтяжка включена)
    - LED: пин 9 (через резистор 220 Ом)
    - BLDC: пины 3, 4, 5 → драйвер мотора (например, IR2104 + MOSFETs)
    - shs (софт-сериал):
        TX → пин 6
        RX ← пин 7   (подключается к TX другой Arduino)

  Для теста shs:
    - Загрузи этот скетч на Arduino #1
    - На Arduino #2 загрузи простой приёмник (см. комментарий ниже)
*/

#include <MegaTPA.h>

// Таймеры
trm timer1(1000);        // каждую секунду
trm timer2(250);         // одноразово через 250 мс
trm motorRunTimer(3000); // переключение мотора каждые 3 сек

// Кнопка на пине 2 (с подтяжкой)
bnm button(2, "pullup");

// PWM для LED на пине 9
stm led(9);

// BLDC мотор на пинах 3, 4, 5 (без датчика оборотов)
bldc motor(3, 4, 5);

// Софт-сериал: RX=7, TX=6
shs link(7, 6);

// Состояние мотора
bool motorRunning = false;

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 MegaTPA Full Demo — Timer + BLDC + shs");
  Serial.println("📡 shs (софт-сериал) работает на пинах: RX=7, TX=6");
  Serial.println("💬 Отправляю тестовое сообщение по shs...");

  // Инициализация shs
  link.begin(4800);
  link.sendString("Hello from MegaTPA!");
  link.send(0x42); // отправляем байт как подтверждение
}

void loop() {
  // === Таймеры ===
  if (timer1.end()) {
    Serial.println("⏰ [timer1.end()] — Прошла 1 секунда!");
    link.sendString("Ping from timer1");
  }

  if (timer2.oneEnd()) {
    Serial.println("🎯 [timer2.oneEnd()] — 250 мс прошли — больше не повторится!");
  }

  // === Кнопка ===
  if (button.pressed()) {
    Serial.println("🖱️  Кнопка нажата!");
    motor.writespeed(100); // Устанавливаем скорость
    link.sendString("Button pressed!");
  }

  // === LED по состоянию кнопки ===
  led.set(button.held() ? 200 : 50); // held() = удержание
  led.update(); // ВАЖНО: вызывать update() для stm!

  // === Управление мотором ===
  if (motorRunTimer.end()) {
    motorRunning = !motorRunning;

    if (motorRunning) {
      Serial.println("⚡ Включаем мотор (режим loop)");
      motor.writespeed(30);
      motor.loop(); // мотор работает в фоне
    } else {
      Serial.println("🛑 Останавливаем мотор");
      motor.writespeed(0);
      // loop() уже выключен, доп. вызов не нужен
    }
  }

  // Обязательно обновлять мотор в loop!
  motor.update();

  // === Приём данных по shs ===
  int b = link.read();
  if (b != -1) {
    Serial.print("📥 Получен байт по shs: 0x");
    Serial.println(b, HEX);
    // Можно реагировать: например, включить LED
    if (b == 0xFF) led.set(255);
  }

  // === Приём строки по shs ===
  char* msg = link.readString();
  if (strlen(msg) > 0) {
    Serial.print("📨 Получена строка: ");
    Serial.println(msg);
    link.sendString("ACK: got your message!");
  }

  delay(10); // небольшая пауза для стабильности Serial
}
