

enum outRegister
{
    RELAY,      // motor relay pin
    TORIGHT,
    door_lock,  // lock the door
    valve1,     // main valve
    valve2,     // second valve
};


enum inRegister
{
    door_stat,      // high when door is locked
    waterLevel50,   // first level of water
    waterLevel80,   // second level of water

    BTN_Power = 8,
    BTN_Start,
    BTN_Program,
    BTN_Tempreture,
    BTN_Spin,
    BTN_Options,
    BTN_Remove_Stain
};

enum errorCodes{
    ERR_NoWater,
    ERR_MotorStuck,
    ERR_Door,
    ERR_LowPower,
    ERR_HighPower
};

enum waterLevels{
    Low,    // must wash with 50% water
    High    // must wash with 80% water
};

enum spinSpeed{
    RPM0,
    RPM400,
    RPM600,
    RPM800,
    RPMMAX,
};

enum Tempreture{
    Cold=(uint8_t)0,
    C30=(uint8_t)30,
    C40=(uint8_t)40,
    C60=(uint8_t)60,
    C95=(uint8_t)95,
};
