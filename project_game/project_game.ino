#include <LiquidCrystal.h>
#include "nec_ir.h"
#include "menu.h"

#define FPS 20

#define PIN_IR 3
#define PIN_S A1
#define PIN_Y A2
#define PIN_X A3

int JOY_CX, JOY_CY;
void calibrate()
{
    JOY_CX = analogRead(PIN_X);
    JOY_CY = analogRead(PIN_Y);
}

MenuPage menuRoot = MenuPage("- LCD GAME - ", 3, new MenuPage[3]
{
    MenuPage("Play", MENU_EXIT, 0),
    MenuPage("High Score", MENU_NUM | MENU_READONLY, 0),
    MenuPage("Settings", 0, NULL)
});

// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

byte chars[8] { };

void setup() {
    menuRoot.setParents(true);
    
    pinMode(PIN_IR, INPUT);
    pinMode(PIN_X, INPUT);
    pinMode(PIN_Y, INPUT);
    pinMode(PIN_S, INPUT);
    
    lcd.begin(16, 2);
    lcd.clear();

    IR_begin(PIN_IR);
}

const long frame_ms = 1000 / FPS;
void loop() {
    long t_last = millis();

    

    long dt = millis() - t_last;
    if (dt < frame_ms) delay(frame_ms - dt);
}

void createChars()
{
    
}
