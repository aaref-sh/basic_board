
bool waterUnder50(){
    return bitRead(inputsState[waterLevel50 / 8], waterLevel50 % 8);
}

bool waterUnder80(){
    return bitRead(inputsState[waterLevel80 / 8], waterLevel80 % 8);
}