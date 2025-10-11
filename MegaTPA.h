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
    int _rpmPin;          // Пин датчика оборотов (может быть -1, если не используется)
    int _gndPin;          // Пин GND (если нужен)
    byte _speed;          // Текущая скорость (0-255)
    byte _minSpeed;       // Минимальная скорость (по умолчанию 5)
    byte _maxSpeed;       // Максимальная скорость (по умолчанию 255)
    bool _calibrated;     // Флаг калибровки
    byte _calibratedMaxSpeed; // Значение максимальной скорости после калибровки
    bool _loopMode;       // Режим циклического запуска
    bool _paused;         // Пауза
    unsigned long _lastUpdate; // Время последнего обновления
    unsigned long _rpmCounter; // Счетчик оборотов
    unsigned long _rpmLastUpdate; // Время последнего обновления RPM
    unsigned long _rpm;   // Текущие обороты (примерно)

public:
    // Конструктор: пины мотора, пин датчика оборотов (если есть), пин GND (если нужен)
    bldc(int pin1, int pin2, int pin3, int rpmPin = -1, int gndPin = -1);

    // Управление скоростью
    void writespeed(byte speed); // Установить скорость (0-255)
    void maxs();                  // Установить максимальную скорость
    void mins();                  // Установить минимальную скорость
    void cal();                  // Калибровка (повышение скорости до достижения стабильных оборотов)
    void loop();                 // Включить режим циклического запуска
    void resume();               // Возобновить работу
    void pause();                // Приостановить работу
    void setMinSpeed(byte minSpeed); // Установить минимальную скорость
    void setMaxSpeed(byte maxSpeed); // Установить максимальную скорость
    byte getSpeed() const { return _speed; }; // Получить текущую скорость
    byte getCalibratedMaxSpeed() const { return _calibratedMaxSpeed; }; // Получить калиброванное значение макс. скорости
    bool isCalibrated() const { return _calibrated; };
    bool isLooping() const { return _loopMode; };
    bool isPaused() const { return _paused; };

    // Обновление состояния мотора
    void update(); // Вызывается в loop() для управления мотором
    void updateRPM(); // Обновление счетчика оборотов (если есть датчик)
    unsigned long getRPM() const { return _rpm; }; // Получить текущие обороты
};
// =====================================================================
// CLASS: shs — Software Serial (SerialHi-style, up to 4800 baud)
// =====================================================================
class shs {
private:
    uint8_t _rxPin;
    uint8_t _txPin;
    long _baud;
    unsigned long _bitTime; // microseconds per bit

    void sendByte(uint8_t byte);
    int readByte();

public:
    shs(uint8_t rxPin, uint8_t txPin);
    void begin(long baud);
    void sendString(const char* str);
    char* readString(); // Returns static buffer (max 63 chars + null)
    void send(uint8_t byte);   // отправить один байт
    int read();                // прочитать один байт (-1 если нет данных)
};
#endif

