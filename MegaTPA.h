#ifndef MEGATPA_H
#define MEGATPA_H

#include <Arduino.h>
#include <string.h>

// =====================================================================
// CLASS: trm — Timer (чистый if-стиль)
// =====================================================================
class trm {
  private:
    unsigned long _interval;
    unsigned long _lastTrigger;
    bool _oneShotDone;

  public:
    trm(unsigned long ms);
    bool end();        // true каждый раз, когда прошло время (для повторов)
    bool oneEnd();     // true только один раз за всё время
};

// =====================================================================
// CLASS: bnm — Button
// =====================================================================
class bnm {
  private:
    int _pin;
    bool _pullup;
    bool _lastRaw;
    bool _lastStable;
    unsigned long _lastChange;
    bool _toggleState;

  public:
    bnm(int pin, const char* mode = "pullup");
    bool pressed();
    bool statechange();
    bool released();
};

// =====================================================================
// CLASS: stm — Software PWM (15 kHz)
// =====================================================================
class stm {
  private:
    int _pin;
    byte _duty;
    unsigned long _lastToggle;
    bool _state;
    static const unsigned long PERIOD_US = 66;

  public:
    stm(int pin);
    void set(byte duty);
    byte getDuty() const { return _duty; }; // Добавлен метод для получения текущего значения duty
};

// =====================================================================
// CLASS: bldc — BLDC Motor Driver
// =====================================================================
class bldc {
private:
    int _pins[3];         // Пины для управления мотором (например, 2, 3, 4)
    byte _speed;          // Текущая скорость (0-255)
    byte _minSpeed;       // Минимальная скорость (по умолчанию 5)
    byte _maxSpeed;       // Максимальная скорость (по умолчанию 255)
    bool _loopMode;       // Режим циклического запуска
    bool _paused;         // Флаг паузы (если loop() был вызван, но resume() еще не был)

public:
    // Конструктор: пины мотора
    bldc(int pin1, int pin2, int pin3);

    // Управление скоростью
    void writespeed(byte speed); // Установить скорость (0-255)
    void maxs();                  // Установить максимальную скорость
    void mins();                  // Установить минимальную скорость
    void loop();                 // Включить режим циклического запуска
    void resume();               // Возобновить работу
    void setMinSpeed(byte minSpeed); // Установить минимальную скорость
    void setMaxSpeed(byte maxSpeed); // Установить максимальную скорость
    byte getSpeed() const { return _speed; }; // Получить текущую скорость
    bool isLooping() const { return _loopMode; };
    bool isPaused() const { return _paused; };

    // Обновление состояния мотора (если нужно)
    void update(); // Вызывается в loop() для управления мотором
};

#endif
