

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
    ERR_
};
