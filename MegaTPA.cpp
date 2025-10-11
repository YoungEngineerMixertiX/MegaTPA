#include "MegaTPA.h"

// =====================================================================
// trm — Timer (if-style)
// =====================================================================
trm::trm(unsigned long ms) {
  _interval = ms;
  _lastTrigger = millis();
  _oneShotDone = false;
}

bool trm::end() {
  unsigned long now = millis();
  if (now - _lastTrigger >= _interval) {
    _lastTrigger = now; // автоматический сброс для следующего цикла
    return true;
  }
  return false;
}

bool trm::oneEnd() {
  if (_oneShotDone) return false;
  if (end()) { // используем end() для проверки времени
    _oneShotDone = true;
    return true;
  }
  return false;
}

// =====================================================================
// bnm — Button
// =====================================================================
bnm::bnm(int pin, const char* mode) {
  _pin = pin;
  _pullup = (strcmp(mode, "pullup") == 0);
  pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
  _lastRaw = digitalRead(_pin);
  _lastStable = _lastRaw;
  _lastChange = millis();
  _toggleState = false;
}

bool bnm::pressed() {
  bool raw = digitalRead(_pin);
  unsigned long now = millis();

  if (raw != _lastRaw) {
    _lastChange = now;
  }

  if ((now - _lastChange) > 50) {
    if (raw != _lastStable) {
      _lastStable = raw;
      if (raw == (_pullup ? LOW : HIGH)) {
        _toggleState = !_toggleState;
      }
    }
  }

  _lastRaw = raw;
  return (_lastStable == (_pullup ? LOW : HIGH));
}

bool bnm::statechange() {
  pressed();
  return _toggleState;
}

bool bnm::released() {
  return !pressed();
}

// =====================================================================
// stm — Software PWM
// =====================================================================
stm::stm(int pin) {
  _pin = pin;
  _duty = 0;
  _state = LOW;
  _lastToggle = micros();
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void stm::set(byte duty) {
  _duty = duty;
  unsigned long now = micros();
  unsigned long elapsed = now - _lastToggle;

  if (_duty == 0) {
    digitalWrite(_pin, LOW);
    return;
  }
  if (_duty == 255) {
    digitalWrite(_pin, HIGH);
    return;
  }

  unsigned long onTime  = (PERIOD_US * _duty) / 255;
  unsigned long offTime = PERIOD_US - onTime;

  if (_state == HIGH && elapsed >= onTime) {
    _state = LOW;
    digitalWrite(_pin, LOW);
    _lastToggle = now;
  } else if (_state == LOW && elapsed >= offTime) {
    _state = HIGH;
    digitalWrite(_pin, HIGH);
    _lastToggle = now;
  }
}

// =====================================================================
// bldc — BLDC Motor Driver
// =====================================================================
bldc::bldc(int pin1, int pin2, int pin3, int rpmPin, int gndPin) {
  _pins[0] = pin1;
  _pins[1] = pin2;
  _pins[2] = pin3;
  _rpmPin = rpmPin;
  _gndPin = gndPin;
  
  _speed = 0;
  _minSpeed = 5;
  _maxSpeed = 255;
  _calibrated = false;
  _calibratedMaxSpeed = 255;
  _loopMode = false;
  _paused = false;
  
  _lastUpdate = 0;
  _rpmCounter = 0;
  _rpmLastUpdate = 0;
  _rpm = 0;
  
  // Инициализация пинов
  for (int i = 0; i < 3; i++) {
    pinMode(_pins[i], OUTPUT);
    digitalWrite(_pins[i], LOW);
  }
  
  if (_gndPin != -1) {
    pinMode(_gndPin, OUTPUT);
    digitalWrite(_gndPin, LOW);
  }
  
  if (_rpmPin != -1) {
    pinMode(_rpmPin, INPUT);
  }
}

void bldc::writespeed(byte speed) {
  _speed = speed;
  if (!_calibrated) {
    _calibratedMaxSpeed = speed;
  }
}

void bldc::maxs() {
  _speed = _calibrated ? _calibratedMaxSpeed : _maxSpeed;
}

void bldc::mins() {
  _speed = _minSpeed;
}

void bldc::cal() {
  // Простая реализация калибровки
  // Можно улучшить, добавив логику с датчиком оборотов
  _calibrated = true;
  _calibratedMaxSpeed = _speed; // Предполагаем, что текущая скорость - это максимальная
  // Для более точной калибровки нужно реализовать чтение датчика оборотов
  // и учитывать, насколько скорость соответствует оборотам
}

void bldc::loop() {
  _loopMode = true;
  _paused = false;
}

void bldc::resume() {
  _paused = false;
}

void bldc::pause() {
  _paused = true;
}

void bldc::setMinSpeed(byte minSpeed) {
  _minSpeed = minSpeed;
}

void bldc::setMaxSpeed(byte maxSpeed) {
  _maxSpeed = maxSpeed;
}

void bldc::update() {
  if (_paused || !_loopMode) return;
  
  // Здесь будет логика управления мотором
  // Простая реализация: включаем мотор с заданной скоростью
  // В реальной реализации может потребоваться более сложная логика
  // для управления пинами и частотой переключения
  
  // Например, можно использовать STM для генерации PWM с нужной частотой
  // и управлять пинами для переключения полюсов
  // Для простоты, просто устанавливаем скорость в STM (если используется)
  // Но поскольку мы не используем STM напрямую здесь, просто обновляем состояние
  // и можно использовать другие методы для управления мотором
  
  // Для примера, можно создать дополнительный STM объект для управления мотором
  // или просто управлять пинами напрямую
}

void bldc::updateRPM() {
  if (_rpmPin == -1) return;
  
  // Чтение датчика оборотов (пример)
  // Это упрощенная версия
  unsigned long now = millis();
  if (now - _rpmLastUpdate > 100) { // Обновление каждые 100 мс
    // Здесь должна быть логика чтения датчика
    // Например, если датчик подключен к прерыванию:
    // int rpmValue = digitalRead(_rpmPin);
    // Если используется аналоговый датчик:
    // int rpmValue = analogRead(_rpmPin);
    
    // Простая имитация счетчика
    _rpmCounter++;
    _rpm = _rpmCounter * 60 / 10; // Упрощенный расчет оборотов
    
    _rpmLastUpdate = now;
  }
}
// =====================================================================
// shs — Software Serial (SerialHi implementation)
// =====================================================================
shs::shs(uint8_t rxPin, uint8_t txPin) {
    _rxPin = rxPin;
    _txPin = txPin;
    pinMode(_txPin, OUTPUT);
    digitalWrite(_txPin, HIGH); // idle = HIGH
    pinMode(_rxPin, INPUT_PULLUP);
}

void shs::begin(long baud) {
    _baud = baud;
    _bitTime = 1000000 / baud;
}

void shs::sendByte(uint8_t byte) {
    // Start bit
    digitalWrite(_txPin, LOW);
    delayMicroseconds(_bitTime);

    // 8 data bits (LSB first)
    for (int i = 0; i < 8; i++) {
        digitalWrite(_txPin, (byte & 1) ? HIGH : LOW);
        delayMicroseconds(_bitTime);
        byte >>= 1;
    }

    // Stop bit
    digitalWrite(_txPin, HIGH);
    delayMicroseconds(_bitTime);
}

void shs::sendString(const char* str) {
    while (*str) {
        sendByte(*str++);
    }
    sendByte('\n'); // auto newline
}

int shs::readByte() {
    unsigned long start = micros();
    // Wait for start bit (LOW)
    while (digitalRead(_rxPin) == HIGH) {
        if (micros() - start > _bitTime * 10) return -1;
    }

    delayMicroseconds(_bitTime + _bitTime / 2); // sample middle of bit

    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data >>= 1;
        if (digitalRead(_rxPin) == HIGH) data |= 0x80;
        delayMicroseconds(_bitTime);
    }

    return data;
}

char* shs::readString() {
    static char buf[64];
    int idx = 0;
    unsigned long last = millis();

    while (millis() - last < 100) { // 100ms timeout
        int b = readByte();
        if (b == -1) {
            delay(1);
            continue;
        }
        last = millis();

        if (b == '\n' || b == '\r') break;
        if (idx < 63) buf[idx++] = (char)b;
    }
    buf[idx] = '\0';
    return buf;
}
void shs::send(uint8_t byte) {
    sendByte(byte);
}

int shs::read() {
    return readByte();
}

