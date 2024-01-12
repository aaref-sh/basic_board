

/* outputs:
 * ====== register 1 ======
 * 0 door_stat
 * 1 waterLevel_50
 * 2 waterLevel_80
 * 3 
 * 4 
 * 5
 * 6
 * 7 
 *
 * ====== register 2 ======
 * 8 power
 * 9 start/pause
 * 10 program
 * 11 spin
 * 12 tempreture
 * 13 option
 * 14 remove stains
 * 15
 */


void initInputRegisters()
{
    // Set the pins as inputs or outputs
    pinMode(INPUT_DATA_PIN, INPUT);
    pinMode(INPUT_CLOCK_PIN, OUTPUT);
    pinMode(INPUT_LOAD_PIN, OUTPUT);

    // Set the load pin high to disable reading
    digitalWrite(INPUT_LOAD_PIN, HIGH);
}

void readInputRegister()
{
    // Pulse the parallel load pin to read the input pins
    digitalWrite(INPUT_LOAD_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(INPUT_LOAD_PIN, HIGH);

    for (int i = 0; i < NUM_INPUT_REGISTERS; i++)
    {
        inputsState[i] = shiftIn(INPUT_DATA_PIN, INPUT_CLOCK_PIN, MSBFIRST);
    }
}