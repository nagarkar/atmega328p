/*$file${.::blinkyPinChangeMonitoring.ino} #################################*/
/*
* Model: blinkyPinChangeMonitoring.qm
* File:  ${.::blinkyPinChangeMonitoring.ino}
*
* This code has been generated by QM tool (https://state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*$endhead${.::blinkyPinChangeMonitoring.ino} ##############################*/
#include "qpn.h"     // QP-nano framework
#include "Arduino.h" // Arduino API
#include <PciManager.h>
#include <PciListenerImp.h>

//============================================================================
// declare all AO classes...
/*$declare${AOs::HCSR04} ###################################################*/
/*${AOs::HCSR04} ...........................................................*/
typedef struct HCSR04 {
/* protected: */
    QActive super;
} HCSR04;

/* protected: */
static QState HCSR04_initial(HCSR04 * const me);
static QState HCSR04_on(HCSR04 * const me);
static QState HCSR04_off(HCSR04 * const me);
/*$enddecl${AOs::HCSR04} ###################################################*/
//...

// AO instances and event queue buffers for them...
HCSR04 AO_HCSR04;

static QEvt l_HCSR04QSto[10]; // Event queue storage for Blinky
//...

//============================================================================
// QF_active[] array defines all active object control blocks ----------------
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                  },
    { (QActive *)&AO_HCSR04,  l_HCSR04QSto,     Q_DIM(l_HCSR04QSto) }
};

//============================================================================
// various constants for the application...
enum {
    BSP_TICKS_PER_SEC = 1000, // number of system clock ticks in one second
    LED_L = 13               // the pin number of the on-board LED (L)
};

void onPinChange(byte pin, byte changeKind);
PciListenerImp listener(LED_L, onPinChange);

void onPinChange(byte pin, byte changeKind) {
  // -- changeKind can be: CHANGEKIND_HIGH_TO_LOW, or CHANGEKIND_LOW_TO_HIGH HIGH
  Serial.print("pci on pin: ");
  Serial.print(pin);
  Serial.print("changeKind: ");
  Serial.println(changeKind);
}

//............................................................................
void setup() {
    // initialize the QF-nano framework
    QF_init(Q_DIM(QF_active));

    // initialize all AOs...
    QActive_ctor(&AO_HCSR04.super, Q_STATE_CAST(&HCSR04_initial));

    pinMode(LED_L, OUTPUT); // set the LED-L pin to output

    Serial.begin(9800);
    PciManager.registerListener(&listener);
    Serial.println("Ready.");
}

//............................................................................
void loop() {
    QF_run(); // run the QF-nano framework
}

//============================================================================
// interrupts...
ISR(TIMER2_COMPA_vect) {
    QF_tickXISR(0); // process time events for tick rate 0
}

//============================================================================
// QF callbacks...
void QF_onStartup(void) {
    // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
    // set Timer2 in CTC mode, start the timer ticking...
    TCCR2A = (1U << WGM21) | (0U << WGM20); // CTC mode
    // Set Timer2 prescaler to 1/1024
    TCCR2B = (1U << CS22 ) | (1U << CS21) | (1U << CS20); // 1/2^10, Clock / 1024

    //select asynchronous operation of timer2 (32,768kHz)
    // For more on ASSR, see https://goo.gl/nsi2DB
    ASSR  &= ~(1U << AS2);

    // enable TIMER2 compare Interrupt
    TIMSK2 = (1U << OCIE2A);

    // Reset timer count
    TCNT2  = 0U;

    // set the output-compare register based on the desired tick frequency
    // OCRn =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
    OCR2A  = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}
//............................................................................
void QV_onIdle(void) {   // called with interrupts DISABLED
    // Put the CPU and peripherals to the low-power mode. You might
    // need to customize the clock management for your application,
    // see the datasheet for your particular AVR MCU.
    SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts
}
//............................................................................
void Q_onAssert(char const Q_ROM * const file, int line) {
    // implement the error-handling policy for your application!!!
    QF_INT_DISABLE(); // disable all interrupts
    QF_RESET();  // reset the CPU
}

//============================================================================
// define all AO classes (state machine)...
/*$define${AOs::HCSR04} ####################################################*/
/* Check for the minimum required QP version */
#if ((QP_VERSION < 601) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8)))
#error qpn version 6.0.1 or higher required
#endif
/*${AOs::HCSR04} ...........................................................*/
/*${AOs::HCSR04::SM} .......................................................*/
static QState HCSR04_initial(HCSR04 * const me) {
    /*${AOs::HCSR04::SM::initial} */
        QActive_armX((QActive *)me, 0U,
                     BSP_TICKS_PER_SEC, BSP_TICKS_PER_SEC);
    return Q_TRAN(&HCSR04_on);
}
/*${AOs::HCSR04::SM::on} ...................................................*/
static QState HCSR04_on(HCSR04 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::HCSR04::SM::on} */
        case Q_ENTRY_SIG: {
            digitalWrite(LED_L, HIGH);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::HCSR04::SM::on::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&HCSR04_off);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${AOs::HCSR04::SM::off} ..................................................*/
static QState HCSR04_off(HCSR04 * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*${AOs::HCSR04::SM::off} */
        case Q_ENTRY_SIG: {
            digitalWrite(LED_L, LOW);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::HCSR04::SM::off::Q_TIMEOUT} */
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&HCSR04_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::HCSR04} ####################################################*/
//...
