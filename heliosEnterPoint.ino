

void readInputTask(xTask task_, xTaskParm parm_)
{
  readInputRegister();
  bool valve1Opened = bitRead(registerState[0],2);
  bool valve2Opened = bitRead(registerState[0],3);
  if( (valve1Opened || valve2Opened) && !waterUnder80()){
    closeValve1();
    closeValve2();
  }
  return;
}

void displayUpdate(xTask task_, xTaskParm parm_)
{
  return;
}

void regulateWaterTask(xTask task_, xTaskParm parm_)
{
  requlateWaterLevel();
}

void initOS()
{

  pinMode(LED_BUILTIN, OUTPUT);

  if (ERROR(xSystemInit()))
  {
    xSystemHalt();
  }

  xTask DisplayUpdate;
  xTask ReadInput;
  xTask RegulateWater;

  int noParam = 0;
  if (ERROR(xTaskCreate(&DisplayUpdate, (const xByte *)"displayUpdate", displayUpdate, &noParam)) ||
      ERROR(xTaskCreate(&ReadInput, (const xByte *)"readInputTask", readInputTask, &noParam)) ||
      ERROR(xTaskCreate(&RegulateWater, (const xByte *)"regulateWaterTask", regulateWaterTask, &noParam)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskWait(DisplayUpdate)) || 
      ERROR(xTaskWait(ReadInput)) || 
      ERROR(xTaskWait(RegulateWater)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskChangePeriod(DisplayUpdate, 10)) || 
      ERROR(xTaskChangePeriod(ReadInput, 10)) || 
      ERROR(xTaskChangePeriod(RegulateWater, 100)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskStartScheduler()))
  {
    xSystemHalt();
  }

  xSystemHalt();
}
