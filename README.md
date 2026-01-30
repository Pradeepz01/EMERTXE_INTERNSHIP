# Microwave Oven Project Write-Up

## 1. Project Overview
**Title:** Microwave Oven Simulator using PIC Microcontroller
**Objective:** To simulate the operation of a microwave oven using a PIC16F877A microcontroller. The system features a user-friendly interface with a Character LCD (CLCD) and a Matrix Keypad, supporting multiple cooking modes like Micro, Grill, and Convection, along with a timer and temperature control.

## 2. Hardware Architecture
The system is built around the **PIC16F877A** microcontroller, interfacing with several peripherals:
*   **Microcontroller:** PIC16F877A (8-bit, RISC architecture).
*   **Display:** 16x4 Character LCD (CLCD) for displaying menus, time, and status messages.
*   **Input:** 4x4 Matrix Keypad for user interaction (selecting modes, setting time/temperature, start/stop).
*   **Actuators:**
    *   **Fan (RC2):** Simulates the microwave fan/magnetron, active during operation.
    *   **Buzzer (RC1):** Provides an audio alert when the cooking timer completes.
*   **Timer:** Internal **Timer2** is used to generate accurate 1-second delays for the countdown timer.

## 3. Software Architecture
The firmware is written in Embedded C and organized into modular files for better maintainability.

### File Structure
*   `main.c`: Finite State Machine (FSM) and Super Loop.
*   `micro_oven.c`: Application logic (Screens, Time Setup).
*   `isr.c`: Interrupt Service Routine for Timer.
*   `matrix_keypad.c`: Driver for reading key presses.
*   `clcd.c`: Driver for LCD display.

## 4. Operational Modes
The system supports four distinct operational modes:
1.  **Micro Mode (Mode 1)**: Standard 900W cooking.
2.  **Grill Mode (Mode 2)**: Grill element simulation.
3.  **Convection Mode (Mode 3)**: Temperature + Time control.
4.  **Quick Start (Mode 4)**: Immediate 30s start.

## 5. Detailed Code Walkthrough 

### Slide 1: Main Loop & State Machine
The core of the system is a Finite State Machine (FSM) inside the `while(1)` loop. It switches between modes based on the `state` variable.

```c
// main.c
void main(void) {
    init_config(); // Initialize peripherals
    
    while (1) {
        // Read key from keypad
        key = read_matrix_keypad(STATE);
        
        switch (state) {
            case MAIN_MENU:
                display_main_menu();
                if (key == 1) state = MICRO_MODE;
                else if (key == 2) state = GRILL_MODE;
                else if (key == 4) { 
                    // Quick Start Logic
                    sec = 30; run_timer = 1; FAN = 1; 
                    state = TIME_DISPLAY; 
                }
                break;
                
            case MICRO_MODE:
                set_time(key, &min, &sec); // User inputs time
                if (key == '#') {          // Start key
                    run_timer = 1; FAN = 1;
                    state = TIME_DISPLAY;
                }
                break;
                
            case TIME_DISPLAY:
                // Countdown logic & Stop/Pause handling
                operation_mode(min, sec);
                break;
        }
    }
}
```

### Slide 2: Timer Interrupt Logic (ISR)
We use **Timer2 Interrupts** to count time accurately without blocking the main loop.

```c
// isr.c
void __interrupt() isr(void) {
    if (TMR2IF == 1) { // Check Timer2 Flag
        if (run_timer) {
            if (++count == 450) { // 1 Second Calibration
                count = 0;
                if (sec > 0) sec--;
                else if (min > 0) { min--; sec = 59; }
            }
        }
        TMR2IF = 0; // Clear Flag
    }
}
```

### Slide 3: Keypad & LCD Drivers
*   **Keypad:** Uses column-scanning to detect button presses.
*   **LCD:** Uses 4-bit mode to send characters in two nibbles, saving IO pins.

---

## 6. Deep Dive: "Micro Mode" Behind the Scenes
This section explains exactly what happens inside the microcontroller when you use **Micro Mode**.

### Step 1: User Selects Mode
*   **Action:** User presses '1' in Main Menu.
*   **Code Event:** `main.c` detects `key == 1`.
*   **State Change:** `state` variable updates from `MAIN_MENU` to `MICRO_MODE`.
*   **Visual:** The `clear_screen()` function wipes the LCD, and print commands show "POWER: 900W".

### Step 2: Time Entry
*   **Action:** User types '1', '3', '0' (for 1 min 30 sec).
*   **Behind the Scenes:** The `set_time()` function captures each keypress.
    *   It shifts the digits into the `min` and `sec` variables.
    *   Example: Entering '1', '3', '0' results in `min = 1, sec = 30`.
    *   The LCD is updated after every keypress to show "01:30".

### Step 3: Starting the Oven
*   **Action:** User presses '#' (Start).
*   **Hardware Trigger:**
    1.  **Fan ON:** The pin `RC2` is set to High (`FAN = 1`). The motor driver circuit (simulated) activates.
    2.  **Timer Enable:** The flag `run_timer` is set to 1. This tells the ISR to start counting.
*   **State Change:** The system moves to `TIME_DISPLAY` mode.

### Step 4: During Cooking (The "Parallel" Processes)
While cooking, two things happen simultaneously:

1.  **Foreground (Main Loop):**
    *   The CPU continuously scans the matrix keypad in `operation_mode()`.
    *   It checks if you press '5' (Pause) or '6' (Stop).
    *   It updates the LCD with the current values of `min` and `sec`.

2.  **Background (Interrupts):**
    *   **Timer2** is hardware! It ticks independently of the code.
    *   Every few milliseconds, it overflows and triggers the `isr()` function.
    *   The `isr()` function momentarily pauses the Main Loop, increments a counter, and if 1 second has passed, it effectively "ticks" the clock down (e.g., 1:30 -> 1:29).
    *   **Crucial Detail:** This ensures accurate timing even if the Main Loop is busy updating the screen.

### Step 5: Completion
*   **Event:** `min` and `sec` both reach 0.
*   **Action:**
    1.  **Fan OFF:** `RC2` is cleared.
    2.  **Buzzer ON:** `RC1` is set High to beep.
    3.  **Visual:** "TIME UP" is displayed.
    4.  **Reset:** After 2 seconds, the Buzzer turns off and the state returns to `MAIN_MENU`.

---

## 7. Technical Highlights 
*   **Why use Interrupts?** If we used a simple `delay_ms(1000)` for the clock, the system would "freeze" for 1 second. You wouldn't be able to pause or stop the oven during that second. Interrupts allow the system to be **Responsive**.
*   **Debouncing:** The keypad uses a state check (`read_matrix_keypad`) to ensure one press is registered as one character, preventing "111" from appearing when you press "1" once.
