#include <xc.h>
#include "micro_oven.h"

extern unsigned int min, sec;
extern int run_timer;

void __interrupt() isr(void) {
    static unsigned int count = 0;
    
    if (TMR2IF == 1) {
        if (run_timer) {
            if (++count == 450) { // Recalibrated: 1250 * (18/50) = 450 for correct 1s timing
                count = 0;
                if (sec > 0) {
                    sec--;
                } else if (min > 0) {
                    min--;
                    sec = 59;
                }
            }
        }
        TMR2IF = 0;
    }
}
