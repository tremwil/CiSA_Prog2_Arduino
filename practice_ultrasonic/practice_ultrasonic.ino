#define VCC 5
#define TRIG 4
#define ECHO 3
#define GND 2

int microLast;
const float c = 340;

void setup() {
    Serial.begin(9600);

    pinMode(VCC, OUTPUT);
    pinMode(GND, OUTPUT);
    digitalWrite(GND, LOW);
    digitalWrite(VCC, HIGH);

    pinMode(ECHO, INPUT);
    pinMode(TRIG, OUTPUT);
}

void loop() {
    PORTD &= ~(1 << TRIG);
    delayMicroseconds(10);
    PORTD |= (1 << TRIG);
    delayMicroseconds(10);
    PORTD &= ~(1 << TRIG);
    
//    while ((PIND & (1 << ECHO)) == 1) { }
//    microLast = micros();
//    while ((PIND & (1 << ECHO)) == 0) { }
    int microDT = pulseIn(ECHO, HIGH);
    
    float dist = (float)microDT * c / 2e6;

    //Serial.println(microDT);
    if (dist < 0) Serial.println("NO READING");
    else Serial.println(dist);
    delay(100);
}
