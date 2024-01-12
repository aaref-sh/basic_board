
void setup()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("press start");

    Serial.begin(115200);

    initOutputRegisters();
    // set up pins
    pinMode(BUTTON, INPUT); // set the button pin
    pinMode(DETECT, INPUT); // set the zero cross detect pin
    pinMode(Triac, OUTPUT); // set the TRIAC gate control pin
    pinMode(TACHO, INPUT);  // set the tacho pulses detect pin

    digitalWrite(BUTTON, HIGH); // turn on pullup resistors

    Input = 200;    // asiign initial value for PID
    Setpoint = 200; // asiign initial value for PID

    // turn the PID on
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(minoutputlimit, maxoutputlimit);
    myPID.SetSampleTime(sampleRate); // Sets the sample rate

    // set up Timer1
    OCR1A = 100;   // initialize the comparator
    TIMSK1 = 0x03; // enable comparator A and overflow interrupts
    TCCR1A = 0x00; // timer control registers set for
    TCCR1B = 0x00; // normal operation, timer disabled

    // set up zero crossing interrupt IRQ0 on pin 2.
    // set up tacho sensor interrupt IRQ1 on pin3
    attachInterrupt(0, zeroCrossingInterrupt, RISING);
    attachInterrupt(1, tacho, FALLING);
   

    initOS();

}
