#define N_BITS 4

int bitPins[N_BITS] = { A3, A2, A1, A0 };
int incPin = A4;
int resetPin = A5;

int c = 0;

void setup() 
{
    Serial.begin(9600);
    
    for (int i = 0; i < N_BITS; i++)
    {
        pinMode(bitPins[i], OUTPUT);
        digitalWrite(bitPins[i], LOW);
    }

    pinMode(incPin, INPUT);
    pinMode(resetPin, INPUT);
}

void loop() 
{
    if (digitalRead(incPin)) { c++; }
    else if (digitalRead(resetPin)) { c = 0; }
    else { return; }

    int t = c;
    for (int i = 0; i < N_BITS; i++)
    {
        digitalWrite(bitPins[i], t & 1);
        t >>= 1;
    }

    while (digitalRead(resetPin) || digitalRead(incPin)) { };
}
