void setup()
{
  Serial.begin(9600);
}

void loop()
{
  measurement(30);
}

void measurement(int times)
{
  double total = 0.00;
  for (int i = 0; i < times; i++)
  {
    double volt = float(analogRead(A0)) * 5.0 / 1024.0;
    double lux = (-1750.8 * (volt * volt)) + (15154 * volt) - 1178.7;
    total = total + volt;
    Serial.print("A0 = ");
    Serial.print(lux);
    Serial.println(" lux");
//    Serial.print(volt);
//    Serial.println(" volts");
    delay(1000);
  }
  double average = total / times;
  Serial.print("Average = ");
  Serial.println(average);
}
