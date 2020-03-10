#include <LiquidCrystal.h>
#include "nec_ir.h"
#include "menu.h"
#include "game.h"

// Hardware stuff

#define PIN_IR 3
#define PIN_S A1
#define PIN_Y A2
#define PIN_X A3
#define PIN_RAND A5

// (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

int JOY_CX = 512, JOY_CY = 512;
void calibrate()
{
    lcd.clear();
    lcd.print(" Calibrating...");

    delay(1000);
    
    JOY_CX = analogRead(PIN_X);
    JOY_CY = analogRead(PIN_Y);

    lcd.clear();
    lcd.print("    Complete");

    delay(1000);
}

int DEBUG = 0;

long t_last;
byte dgCodes[10] { 22, 12, 24, 94, 8, 28, 90, 66, 82, 74 };

// Game stuff

LcdGame game(&lcd);

// Menu stuff

MenuPage menuRoot = MenuPage(" LCD GAME", 3, new MenuPage[3]
{
    MenuPage("Play", MENU_EXIT, NULL, 0),
    MenuPage("Highscore", MENU_NUM | MENU_READONLY, &game.highScore, 0),
    MenuPage("Settings", 3, new MenuPage[3]
    {
        MenuPage("Gameplay", 4, new MenuPage[4]
        {
            MenuPage("Game Speed", MENU_NUM, &game.startSpeed, 5),
            MenuPage("Inc. Speed", MENU_BOOL, &game.incSpeed, true),
            MenuPage("Inc. Period", MENU_NUM, &game.secForInc, 30),
            MenuPage("Player X", MENU_NUM, &game.playerX, 2)
        }),
        MenuPage("Calibrate", calibrate),
        MenuPage("Debug mode", MENU_BOOL, &DEBUG, false)
    })
});

bool inMenu = true;
MenuHandler menu(&lcd, &menuRoot);

void setup() {
    // Begin Serial comm
    Serial.begin(9600);
    // Set modes of important pins
    pinMode(PIN_IR, INPUT);
    pinMode(PIN_X, INPUT);
    pinMode(PIN_Y, INPUT);
    pinMode(PIN_S, INPUT);

    // Begin LCD display
    lcd.begin(16, 2);
    lcd.clear();

    // Seed random number generator using analog pin noise
    lcd.print("Seeding RNG...");
    delay(500);
    long seed = 0;
    for (long i = 0; i < 16; i++)
    {
        long c_seed = 0;
        for (long j = 0; i < 16; i++)
        {
            c_seed = c_seed << 2 | (analogRead(PIN_RAND) & 3);
            delay(50);
        }
        seed ^= c_seed;
    }
    lcd.setCursor(0,1);
    lcd.print(seed);
    delay(2000);
    randomSeed(seed);

    // Begin NEC IR reciever
    IR_begin(PIN_IR);
    // Set last time
    t_last = millis();
    // Set menu parents and render menu
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
        if (!inMenu) 
        {
            int range = min(1023 - JOY_CX, JOY_CX);
            int joyBias = min(max(analogRead(PIN_X) - JOY_CX, -range), range);
            int playerPos = 7 - joyBias * 7 / range;
            if (DEBUG) 
            {
                Serial.print(F("PLAYER POS: "));
                Serial.println(playerPos);
            }
            game.tick(playerPos);
        }
    }

    if (!IR_available()) { return; }
    
    IrCmd cmd = IR_readCurrent();

    if (DEBUG)
    {
        Serial.println(F("IR PACKET:"));
        Serial.print(F("    Device ID : "));
        Serial.println(cmd.deviceID);
        Serial.print(F("    Command   : "));
        Serial.println(cmd.command);
        Serial.print(F("    Is repeat : "));
        Serial.println(cmd.repeat);
        Serial.print(F("    Error code: "));
        Serial.println(cmd.error);   
    }

    if (cmd.error || cmd.repeat) { return; }

    if (inMenu)
    {
        if (cmd.command == 67 && menu.onSelect()) 
        {
            inMenu = false;
            game.start();
        }
        else if (cmd.command == 68) menu.onEscape();
        else if (cmd.command == 25) menu.onReset();
        else if (cmd.command == 7 ) menu.onMoveSubMenu(+1);
        else if (cmd.command == 9 ) menu.onMoveSubMenu(-1);
        else if (cmd.command == 64) menu.onToggle();
        else if (cmd.command == 13) menu.onDelete();
        else if (cmd.command == 71) menu.setPage(&menuRoot);
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
