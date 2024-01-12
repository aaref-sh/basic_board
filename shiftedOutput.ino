
/* outputs:
 * ====== register 1 ======
 * 0 motor relay
 * 1 motor direction
 * 2 switch 
 * 3 value1
 * 4 value2
 * 5
 * 6
 * 7 door locked led
 * 
 * ====== register 2 ======
 * 8 - 15 LEDs (5 heat - 3 stage)
 * 
 * ====== register 3 ======
 * 16 - 23 LEDS (6 program - 1 removePoints - 1 others)
 * 
 * ====== register 4 ======
 * 24 - 31 LEDs (5 spin - 3 others)
 * 
*/ 


// Define the number of shift registers
#define NUM_OUTPUT_REGISTERS 4

// Create an array to store the state of each output pin
byte registerState[NUM_OUTPUT_REGISTERS];

// A function to write a single pin of the shift registers
void regWrite(int pin, bool state) {
  // Determine which register and which pin
  int reg = pin / 8;
  int actualPin = pin - (8 * reg);

  // Write the state to the register state array
  bitWrite(registerState[reg], actualPin, state);

  // Update the shift registers with the new state
  updateRegisters();
}

// A function to update the shift registers
void updateRegisters() {
  // Set the latch pin low to start sending data
  digitalWrite(OUTPUT_LATCH_PIN, LOW);

  // Send the data to the shift registers
  // The first register in the array is the last in the chain
  for (int i = NUM_OUTPUT_REGISTERS - 1; i >= 0; i--) {
    shiftOut(OUTPUT_DATA_PIN, OUTPUT_CLOCK_PIN, MSBFIRST, registerState[i]);
  }

  // Set the latch pin high to stop sending data
  digitalWrite(OUTPUT_LATCH_PIN, HIGH);
}


void initOutputRegisters(){
    
  // Set the pins as outputs
  pinMode(OUTPUT_DATA_PIN, OUTPUT);
  pinMode(OUTPUT_LATCH_PIN, OUTPUT);
  pinMode(OUTPUT_CLOCK_PIN, OUTPUT);

  // Initialize the register state array to zero
  for (int i = 0; i < NUM_OUTPUT_REGISTERS; i++) {
    registerState[i] = 0;
  }

  // Update the shift registers with the initial state
  updateRegisters();

}

void setOutValue(int pin, bool state){
    regWrite(pin, state);
}
