#include <LiquidCrystal.h>

// (rs, e, d4, d5, d6, d7)
LiquidCrystal lcd(9, 8, A0, A1, A2, A3);

void setup()
{
    lcd.begin(16, 2);
    lcd.clear();
}

void loop()
{
    lcd.setCursor(0,0);
    lcd.print(millis()/1000);    
}
