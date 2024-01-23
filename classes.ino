
class Stage{
public:
    int duration;
    bool withRinse;

};
bool withrinse = 0;
class Program{
public:
    uint8_t waterLevel;
    unsigned short maxSpinSpeed;
    unsigned short tempreture;
    bool withRemoveStain = false;
    Stage stages[];

    Program(){};

    Program(unsigned short waterLevel, unsigned short maxSpinSpeed, unsigned short tempreture, bool withRemoveStain){
        this->maxSpinSpeed = maxSpinSpeed;
        this->waterLevel = waterLevel;
        this->withRemoveStain = withRemoveStain;
        this->tempreture = tempreture;
    };

    void run(){
        // uint8_t stageIndex = 0;
        // for(;stageIndex<stages.length;stageIndex++){
        //     withrinse = stages[stageIndex].withRinse;
        // }
    }
};

