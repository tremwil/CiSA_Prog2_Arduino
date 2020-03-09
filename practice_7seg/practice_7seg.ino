#define DUTY_ON 200 // Delay (us) the LEDs are ON
#define DUTY_OFF 2000 // Delay (us) the LEDs are off
#define NUM_DIGITS 4   // Number of digits in the 7-segment display

// Uncomment this line and set SINGLE_PIN_MODE to a pin number
// to make the display print the digital signal on this pin (debug)
//#define SINGLE_PIN_MODE 3

// Output pins for the anodes (segments) of the display (order ABCDEFG)
byte SEGMENT_PINS[] = { 6, 2, 10, 12, 13, 5, 9 };
// Ouput pins for the cathodes (digits) of the display (right to left)
byte DIGIT_PINS[] = { 8, 3, 4, 7 };

// Segments on for each digit, with LSB => A and MSB => G.
byte NUM_CODES[] = {
    B00111111, // 0          AAA
    B00000110, // 1         F   B
    B01011011, // 2         F   B
    B01001111, // 3          GGG
    B01100110, // 4         E   C
    B01101101, // 5         E   C
    B01111101, // 6          DDD
    B00000111, // 7
    B01111111, // 8
    B01101111, // 9
};

void setup() 
{   
    // Set all pins to appropritate modes
    for (int i = 0; i < 7; i++) 
    {
        pinMode(SEGMENT_PINS[i], OUTPUT);
    }
    for (int i = 0; i < NUM_DIGITS; i++) 
    {
        pinMode(DIGIT_PINS[i], OUTPUT);
        digitalWrite(DIGIT_PINS[i], HIGH);
    }
}

void loop() 
{
    int n = analogRead(A0);

    for (int cd = 0; cd < NUM_DIGITS; cd++) 
    {   // Set current digit to LOW, allowing current flow
        for (int i = 0; i < 7; i++) 
        {   // Read the 7-segment code for n and light appropriate segments
            digitalWrite(SEGMENT_PINS[i], LOW);
        }
        digitalWrite(DIGIT_PINS[cd], n == 0 && cd != 0);
        // Read the last digit of n
        byte dcode = NUM_CODES[n % 10];
        for (int i = 0; i < 7; i++) 
        {   // Read the 7-segment code for n and light appropriate segments
            digitalWrite(SEGMENT_PINS[i], dcode & 1);
            dcode >>= 1;
        }
        delayMicroseconds(DUTY_ON);    // Wait to allow off LEDs to dim
        n /= 10;                            // Skip to the next digit of n 
        // Set current digit back to HIGH, stopping current flow
        digitalWrite(DIGIT_PINS[cd], HIGH);
        delayMicroseconds(DUTY_OFF);    // Wait to allow off LEDs to dim
    }
}
