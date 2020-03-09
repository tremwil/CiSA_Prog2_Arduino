#define STEPS 50

int pColor[3] { 0x00, 0x00, 0x00 };
int cColor[3] { 0x80, 0xFF, 0x80 };

int cStep = 0;
int cPins[3] = {6, 5, 3};

void setup() {
    Serial.begin(9600);
    
    // put your setup code here, to run once:
    for (int i = 0; i < 3; i++)
    {
        pinMode(cPins[i], OUTPUT);
        digitalWrite(cPins[i], LOW);
    }
}

void loop() {
    // put your main code here, to run repeatedly:

    if (Serial.available())
    {
        pColor[0] = cColor[0];
        pColor[1] = cColor[1];
        pColor[2] = cColor[2];
        
        cColor[0] = Serial.parseInt();
        cColor[1] = Serial.parseInt();
        cColor[2] = Serial.parseInt();
        Serial.read();

        cStep = 0;
    }
    
    if (cStep <= STEPS)
    {
        for (int i = 0; i < 3; i++)
        {
            analogWrite(cPins[i], (pColor[i] * (STEPS - cStep) + cColor[i] * cStep) / STEPS);
        }
        
        cStep++;
    }

    delay(20);
}
