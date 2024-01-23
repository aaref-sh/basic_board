
bool waterOK = false;
unsigned long fillWaterStart = 0;

bool waterUnder50(){
    return bitRead(inputsState[waterLevel50 / 8], waterLevel50 % 8);
}

bool waterUnder80(){
    return bitRead(inputsState[waterLevel80 / 8], waterLevel80 % 8);
}

void requlateWaterLevel(){
    if(!fillWaterStart) fillWaterStart = millis();

    if(waterUnder50()){
        openValve1();

        if ((millis() - fillWaterStart)/1000 > 120) {
            // TODO: error ERR_NoWater
        }
    }
    else waterOK = 1;

    if(runningProgram.waterLevel == waterLevel50 && !waterUnder50() || !waterUnder80()){
        closeValve1();
    }
    
}