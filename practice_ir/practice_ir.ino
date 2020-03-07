// Using IR sensor implementing NEC IR protocol
// Info: https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
#include "nec_ir.h"

void setup() {
    Serial.begin(9600);

    pinMode(4, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(2, INPUT);
    
    pinMode(8, OUTPUT);
    pinMode(10, OUTPUT);
    digitalWrite(8, LOW);
    digitalWrite(10, LOW);
    
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);

    IR_begin(2);
}

bool buzz = false;
int freq = 440;

int tempFreq = 0;
byte dgCodes[10]
{
    22, 12, 24, 94, 8, 28, 90, 66, 82, 74
};

void loop() {
    delay(10);
    if (!IR_available()) return;

    IrCmd cmd = IR_readCurrent();

    Serial.println(F("IR PACKET:"));
    Serial.print(F("    Device ID : "));
    Serial.println(cmd.deviceID);
    Serial.print(F("    Command   : "));
    Serial.println(cmd.command);
    Serial.print(F("    Is repeat : "));
    Serial.println(cmd.repeat);
    Serial.print(F("    Error code: "));
    Serial.println(cmd.error);
    
    if (cmd.error) 
    { 
        return; 
    }
    
    if (cmd.command == 64 && !cmd.repeat)
    {
        buzz = !buzz;
        if (buzz && tempFreq != 0) 
        {
            freq = tempFreq;
            tempFreq = 0;
        }
        Serial.println(buzz? F("BUZZER ON") : F("BUZZER OFF"));
        if (buzz) tone(10, freq);
        else noTone(10);
    }
    else if (cmd.command == 13)
    {
        tempFreq = 0;    
    }
    
    for (int i = 0; i < 10; i++)
    {
        if (dgCodes[i] == cmd.command && !cmd.repeat)
        {
            tempFreq = 10 * tempFreq + i;
            break;
        }
    }
}
