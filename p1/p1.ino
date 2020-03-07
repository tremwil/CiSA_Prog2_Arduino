#include "codes.h"

#define MODE_RECV 0
#define MODE_SEND 1

#define TIME_UNIT 100
#define BUZZER_PIN 6
#define SIGNAL_PIN A0
#define THRESHOLD 512
#define N_SAMPLES 100

const char mode = MODE_SEND;

char textBuffer[512];

int switchTimings[256];
int switchIndex = 0;
int cState = LOW;

short samples[N_SAMPLES];
int sampleIndex = 0;

void setup() {
    // put your setup code here, to run once:
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);
    Serial.begin(9600);
}

void loop() {
    if (mode == MODE_SEND)
    {
         if (Serial.available())
        {
            int sLen = Serial.readBytesUntil('\n', textBuffer, 1024);
            for (int i = 0; i < sLen; i++)
            {
                char chr = toUpperCase(textBuffer[i]);
                if (chr == ' ') { delay(4 * TIME_UNIT); }
                if (chr < '0' || chr > 'Z') { continue; }
                
                char code = codes[chr - '0'];
                if (code == 0) { continue; }
                // Get length of code
                int len = 7;
                while ((code & 1) == 0) { len--; code >>= 1; }
                code >>= 1;
                // Parse code
                for (int i = 0; i < len; i++)
                {
                    tone(BUZZER_PIN, 440);
                    delay(((code & 1)? 3 : 1) * TIME_UNIT);
                    noTone(BUZZER_PIN);
                    delay(((i+1 == len)? 3 : 1) * TIME_UNIT);
                    code >>= 1;
                }
            }
        }   
    }
    else
    {
        if (sampleIndex < N_SAMPLES)
        {
            samples[sampleIndex++] = analogRead(SIGNAL_PIN);
        }
        else
        {     
            sampleIndex = 0;
            int avg = 0;
            for (int i = 0; i < N_SAMPLES; i++)
                avg += samples[i]; 

            avg /= N_SAMPLES;

            if ((avg >= THRESHOLD) ^ cState)
            {
                switchTimings[switchIndex++] = millis();
                cState ^= 1;
            }
        }
    }
}
