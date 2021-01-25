int Laser = D7;

int Detector = D6;

void setup()

{ 

Serial.begin (9600);

pinMode(Laser, OUTPUT);

pinMode(Detector, INPUT);

}

void loop()

{

digitalWrite(Laser, HIGH);

boolean val = digitalRead(Detector);

Serial.println(val);

}
