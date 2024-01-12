

void readInput(xTask task_, xTaskParm parm_)
{
  readInputRegister();
  return;
}

void displayUpdate(xTask task_, xTaskParm parm_)
{
  display.update();
  return;
}

void initOS()
{

  for (int i = 2; i > 0; i--)
  {
    // store number and show it for 400ms
    display.set(i, ALIGN_RIGHT);
    display.show(400);
    // add dot to stored number and show it for 400ms
    display.changeDot(1);
    display.show(400);
  }
  display.set("GO"); // store "GO"
  pinMode(LED_BUILTIN, OUTPUT);

  if (ERROR(xSystemInit()))
  {
    xSystemHalt();
  }

  xTask DisplayUpdate;
  xTask ReadInput;

  int noParam = 0;
  if (ERROR(xTaskCreate(&DisplayUpdate, (const xByte *)"displayUpdate", displayUpdate, &noParam)) ||
      ERROR(xTaskCreate(&ReadInput, (const xByte *)"readInput", readInput, &noParam)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskWait(DisplayUpdate)) || ERROR(xTaskWait(ReadInput)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskChangePeriod(DisplayUpdate, 10)) || ERROR(xTaskChangePeriod(ReadInput, 10)))
  {
    xSystemHalt();
  }

  if (ERROR(xTaskStartScheduler()))
  {
    xSystemHalt();
  }

  xSystemHalt();
}
