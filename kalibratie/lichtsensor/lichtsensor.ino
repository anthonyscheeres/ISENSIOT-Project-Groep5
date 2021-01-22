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
  float total = 0.00;
  for (int i = 0; i < times; i++)
  {
    float volt = float(analogRead(A0)) * 5.0 / 1024.0;
    total = total + volt;
    Serial.print("A0 = ");
    Serial.print(volt);
    Serial.println(" volts");
    delay(1000);
  }
  float average = total / times;
  Serial.print("Average = ");
  Serial.println(average);
}
