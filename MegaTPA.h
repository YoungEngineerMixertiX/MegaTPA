/**
 * @file MegaTPA.h
 * @author Qwen & 12yo Time Master (c) 2025
 * @brief Minimalist Arduino Library — Timers, Buttons, PWM
 * 
 * trm → Timer      : .end() / .oneEnd() — чистый if-стиль
 * bnm → Button     : .pressed() / .statechange() / .released()
 * stm → SoftPWM    : .set(0-255) — auto 15kHz
 * 
 * Никаких callback, reset, update — только if (timer.end()) { ... }
 */

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
};

#endif