#ifndef MICRO_OVEN_H
#define	MICRO_OVEN_H

#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"

/* State Definitions */
#define POWER_ON_SCREEN         0x01
#define MAIN_MENU               0x02
#define MICRO_MODE              0x03
#define GRILL_MODE              0x04
#define CONVECTION_MODE         0x05
#define START_MODE              0x06
#define TIME_DISPLAY            0x07
#define PAUSE_MODE              0x08
#define STOP_MODE               0x09
#define SET_TEMP_MODE           0x0A

/* Microwave Constants */
#define DEFAULT_POWER           900
#define QUICK_START_TIME        30

/* Peripheral Mapping */
#define FAN                     RC2
#define FAN_DDR                 TRISC2
#define BUZZER                  RC1
#define BUZZER_DDR              TRISC1

/* Prototypes */
void power_on_screen(void);
void clear_screen(void);
void display_main_menu(void);
void display_time_screen(void);
void set_time(unsigned char key, unsigned int *min, unsigned int *sec);
void set_temp(unsigned char key, unsigned int *temp);
void operation_mode(unsigned int min, unsigned int sec);

#endif	/* MICRO_OVEN_H */
