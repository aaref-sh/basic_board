
#pragma region "motor"
#define PULSE 2            // number of triac trigger pulse width counts. One count is 16 microseconds
#define TACHOPULSES 8      // number of pulses per revolution 
#pragma endregion

#pragma region "interrupts"
#define DETECT 2                // zero cross detect pin
#pragma endregion

#pragma region "Motor"
#define TACHO 3                 // tacho signals input pin
#define Triac 4                 // TRIAC gate pin
#pragma endregion

#pragma region "4x7 sigments digital display"

#pragma endregion   

#pragma region "outputs registers"
#define OUTPUT_DATA_PIN 8       // DS pin
#define OUTPUT_LATCH_PIN 9      // ST_CP pin
#define OUTPUT_CLOCK_PIN 10     // SH_CP pin
#pragma endregion

#pragma region "input registers"
#define INPUT_DATA_PIN 2        // Q7 pin
#define INPUT_CLOCK_PIN 3       // SH_CP pin
#define INPUT_LOAD_PIN 4        // PL pin
#pragma endregion


