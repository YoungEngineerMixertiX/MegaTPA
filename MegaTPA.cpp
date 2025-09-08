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