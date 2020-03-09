#include <LiquidCrystal.h>
#include "nec_ir.h"
#include "menu.h"

#define FPS 20

#define PIN_IR 3
#define PIN_S A1
#define PIN_Y A2
#define PIN_X A3

int JOY_CX = 512, JOY_CY = 512;
void calibrate()
{
    JOY_CX = analogRead(PIN_X);
    JOY_CY = analogRead(PIN_Y);
}

// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

MenuPage menuRoot = MenuPage("- LCD GAME - ", 3, new MenuPage[3]
{
    MenuPage("Play", MENU_EXIT, 0),
    MenuPage("Highscore", MENU_NUM | MENU_READONLY, 0),
    MenuPage("Settings", 3, new MenuPage[3]
    {
        MenuPage("Calibrate", calibrate),
        MenuPage("Game Speed", MENU_NUM, 10),
        MenuPage("Debug mode", MENU_BOOL, 0)
    })
});

bool inMenu = true;
MenuHandler menu(&lcd, &menuRoot);

byte chars[8] { };

long t_last;

byte dgCodes[10] { 22, 12, 24, 94, 8, 28, 90, 66, 82, 74 };

void setup() {
    Serial.begin(9600);
    
    pinMode(PIN_IR, INPUT);
    pinMode(PIN_X, INPUT);
    pinMode(PIN_Y, INPUT);
    pinMode(PIN_S, INPUT);
    
    lcd.begin(16, 2);
    lcd.clear();

    IR_begin(PIN_IR);

    t_last = millis();

    menuRoot.setParents(true);
    menu.render();
}

const long frame_ms = 1000 / FPS;
void loop() 
{
    long t_curr = millis();
    if (t_curr - t_last >= frame_ms)
    {
        t_last = t_curr;
        if (!inMenu) gameTick();
    }

    if (!IR_available()) { return; }
    
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

    if (cmd.error || cmd.repeat) { return; }

    if (inMenu)
    {
        if (cmd.command == 64 && menu.onSelect()) inMenu = false;
        else if (cmd.command == 9) menu.onEscape();
        else if (cmd.command == 25) menu.onReset();
        else if (cmd.command == 67) menu.onMoveSubMenu(+1);
        else if (cmd.command == 68) menu.onMoveSubMenu(-1);
        else if (cmd.command == 13) menu.onDelete();
        else
        {
            for (int i = 0; i < 10; i++)
            {
                if (dgCodes[i] == cmd.command) 
                {
                    menu.onDigit(i);
                    break;
                }
            }  
        }   
    }
    else if (cmd.command == 64)
    {
        inMenu = true;
        menu.setPage(&menuRoot);
    }
}

void gameTick()
{

}

void createChars()
{
    
}
