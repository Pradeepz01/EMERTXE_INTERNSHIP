#include "micro_oven.h"

#pragma config WDTE = OFF

unsigned int min = 0, sec = 0;
unsigned int temp = 0;
int run_timer = 0;
unsigned char state = MAIN_MENU;

static void init_config(void) {
    init_clcd();
    init_matrix_keypad();
    init_timer2();
    
    FAN_DDR = 0;
    FAN = 0;
    BUZZER_DDR = 0;
    BUZZER = 0;
    
    PEIE = 1;
    GIE = 1;
}

void main(void) {
    unsigned char key;
    
    init_config();
    power_on_screen();
    clear_screen();
    
    while (1) {
        key = read_matrix_keypad(STATE);
        
        switch (state) {
            case MAIN_MENU:
                display_main_menu();
                if (key == 1) { // Micro Mode
                    state = MICRO_MODE;
                    clear_screen();
                    clcd_print(" SET POWER ", LINE1(0));
                    clcd_print(" power = 900W ", LINE2(0));
                    __delay_ms(1500);
                    clear_screen();
                    display_time_screen();
                } else if (key == 2) { // Grill Mode
                    state = GRILL_MODE;
                    clear_screen();
                    clcd_print(" GRILL MODE ", LINE1(0));
                    __delay_ms(1000);
                    clear_screen();
                    display_time_screen();
                } else if (key == 3) { // Convection Mode
                    state = SET_TEMP_MODE;
                    clear_screen();
                    clcd_print(" SET TEMP (C) ", LINE1(0));
                    clcd_print(" TEMP: 000C ", LINE2(0));
                } else if (key == 4) { // Start Quick
                    min = 0;
                    sec = QUICK_START_TIME;
                    state = TIME_DISPLAY;
                    run_timer = 1;
                    FAN = 1;
                    clear_screen();
                }
                break;
                
            case MICRO_MODE:
            case GRILL_MODE:
            case CONVECTION_MODE:
                if (key == '#') {
                    state = TIME_DISPLAY;
                    run_timer = 1;
                    FAN = 1;
                    clear_screen();
                } else if (key == '*') {
                    min = 0;
                    sec = 0;
                    set_time(key, &min, &sec);
                } else if (key != ALL_RELEASED) {
                    set_time(key, &min, &sec);
                }
                break;
                
            case SET_TEMP_MODE:
                if (key != ALL_RELEASED) {
                    if (key == '#') {
                        state = CONVECTION_MODE;
                        clear_screen();
                        display_time_screen();
                    } else {
                        set_temp(key, &temp);
                    }
                }
                break;
                
            case TIME_DISPLAY:
                operation_mode(min, sec);
                
                if (min == 0 && sec == 0) {
                    run_timer = 0;
                    FAN = 0;
                    BUZZER = 1;
                    clear_screen();
                    clcd_print("  TIME UP!!  ", LINE2(2));
                    __delay_ms(2000);
                    BUZZER = 0;
                    clear_screen();
                    state = MAIN_MENU;
                }
                
                if (key == 4) { // Resume
                    run_timer = 1;
                    FAN = 1;
                } else if (key == 5) { // Pause
                    run_timer = 0;
                    FAN = 0;
                } else if (key == 6) { // Stop
                    run_timer = 0;
                    FAN = 0;
                    clear_screen();
                    state = MAIN_MENU;
                }
                break;
        }
    }
}
