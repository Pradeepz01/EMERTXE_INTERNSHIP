#include "micro_oven.h"

void power_on_screen(void) {
    for (unsigned char i = 0; i < 16; i++) {
        clcd_putch((const char)0xFF, LINE1(i));
        __delay_ms(50);
    }
    clcd_print("Powering ON", LINE2(2));
    clcd_print("Microwave Oven", LINE3(1));
    for (unsigned char i = 0; i < 16; i++) {
        clcd_putch((const char)0xFF, LINE4(i));
        __delay_ms(50);
    }
    __delay_ms(1500);
    clear_screen();
}

void clear_screen(void) {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

void display_main_menu(void) {
    clcd_print("1. MICRO", LINE1(0));
    clcd_print("2. GRILL", LINE2(0));
    clcd_print("3. CONVECTION", LINE3(0));
    clcd_print("4. START", LINE4(0));
}

void display_time_screen(void) {
    clcd_print("SET TIME (MM:SS)", LINE1(0));
    clcd_print("TIME: 00:00", LINE2(0));
    clcd_print("*:CLEAR  #:ENTER", LINE4(0));
}

void set_time(unsigned char key, unsigned int *min, unsigned int *sec) {
    static int pos = 0;
    
    if (key == '*') {
        *min = 0;
        *sec = 0;
        pos = 0;
    } else if (key <= 9) {
        if (pos == 0) {
            *min = (unsigned int)key * 10;
            pos++;
        } else if (pos == 1) {
            *min += (unsigned int)key;
            pos++;
        } else if (pos == 2) {
            *sec = (unsigned int)key * 10;
            pos++;
        } else if (pos == 3) {
            *sec += (unsigned int)key;
            pos = 0;
        }
    }
    
    char time_str[6];
    time_str[0] = (char)((*min / 10) + '0');
    time_str[1] = (char)((*min % 10) + '0');
    time_str[2] = ':';
    time_str[3] = (char)((*sec / 10) + '0');
    time_str[4] = (char)((*sec % 10) + '0');
    time_str[5] = '\0';
    
    clcd_print(time_str, LINE2(6));
}

void set_temp(unsigned char key, unsigned int *temp) {
    static int pos = 0;
    if (key == '*') {
        *temp = 0;
        pos = 0;
    } else if (key <= 9) {
        if (pos == 0) {
            *temp = (unsigned int)key * 100;
            pos++;
        } else if (pos == 1) {
            *temp += (unsigned int)key * 10;
            pos++;
        } else if (pos == 2) {
            *temp += (unsigned int)key;
            pos = 0;
        }
    }
    char temp_str[5];
    temp_str[0] = (char)((*temp / 100) + '0');
    temp_str[1] = (char)(((*temp / 10) % 10) + '0');
    temp_str[2] = (char)((*temp % 10) + '0');
    temp_str[3] = 'C';
    temp_str[4] = '\0';
    clcd_print(temp_str, LINE2(8));
}

void operation_mode(unsigned int min, unsigned int sec) {
    clcd_print("TIME = ", LINE1(0));
    char time_str[6];
    time_str[0] = (char)((min / 10) + '0');
    time_str[1] = (char)((min % 10) + '0');
    time_str[2] = ':';
    time_str[3] = (char)((sec / 10) + '0');
    time_str[4] = (char)((sec % 10) + '0');
    time_str[5] = '\0';
    clcd_print(time_str, LINE1(7));
    
    clcd_print("4.START/RESUME", LINE2(0));
    clcd_print("5.PAUSE", LINE3(0));
    clcd_print("6.STOP", LINE4(0));
}
