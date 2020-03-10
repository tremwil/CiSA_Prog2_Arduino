#ifndef GAME_H
#define GAME_H

#include <LiquidCrystal.h>

typedef unsigned long long uint64;

#define FPS 20

// Indexed with (next << 2 | curr)
const byte LVL_TO_CHAR[16]
{
     ' ', 0x03, 0x05, 0x01, 
    0x02,  '!',  '!',  '!', 
    0x04,  '!',  '!',  '!', 
    0x00,  '!',  '!', 0xff
};

class LcdGame
{
private:

    // Shift from aligning with characters - 0 to 4
    byte levelShift;
    
    // Level data stored as columns, each being 4 bits
    byte level[17];
    
    // Char data ordering:
    // 0-5: Block data: 1st bit is left (0) / right (1),
    // 2 other bits are full (0), top (1), bottom (2).
    // 6: Player top
    // 7: Player bottom
    byte charData[64];

    void shiftLevel()
    {
        levelShift = (levelShift + 1) % 5;

        if (levelShift == 0)
        {
            for (int i = 0; i < 16; i++)
            {
                level[i] = level[i+1];
            }

            byte next = 0b1111, prv = level[15];
            while ((next | prv) == 0b1111)
            {
                next = random((prv >> 2)? 0 : 4) << 2 | random((prv & 3)? 0 : 4);   
            } 
            Serial.println(next, BIN);
            level[16] = next;
        }
    }
public:
    LiquidCrystal *lcd;

    int highScore;
    int startSpeed;
    int gameSpeed;
    int playerX;

    int itCnt;
    int score;

    bool gameOver;
    int incSpeed; // used as bool but must be int size

    int secForInc;

    LcdGame(LiquidCrystal *lcd) : charData{ }, level{ }
    {
        this->lcd = lcd;
        this->highScore = 0;
        this->startSpeed = 5;
        this->playerX = 2;
        this->incSpeed = true;
        this->secForInc = 30;
    }

    void start()
    {
        itCnt = 0;
        score = 0;
        levelShift = 0;
        gameSpeed = startSpeed;
        gameOver = false;
        memset(level, 0, sizeof(level));
    }

    void tick(int playerY)
    {
        if (gameOver)
        {
            lcd->clear();
            if (itCnt < 2*FPS) lcd->print("G A M E O V E R");
            else
            {
                lcd->print("Score:");
                lcd->print(score);

                lcd->setCursor(0,1);
                lcd->print("Speed:");
                lcd->print(gameSpeed);
                if (highScore == score) lcd->print(" (BEST!)");
            }
        }
        else
        {
            // Char generation
    
            byte leftFill = (1 << levelShift) - 1;
            byte currFill = ~leftFill & 0b11111;
            
            for (unsigned int i = 0; i < 16; i++)
            {
                charData[i] = (i / 8) ? currFill : leftFill;
            }
            for (unsigned int i = 0, n = 0x0F0F; i < 16; i++, n >>= 1)
            {
                charData[i+16] =  (n & 1) ? ((i / 8) ? currFill : leftFill) : 0;
            }
            for (unsigned int i = 0, n = 0xF0F0; i < 16; i++, n >>= 1)
            {
                charData[i+32] =  (n & 1) ? ((i / 8) ? currFill : leftFill) : 0;
            }
    
            // Create player data
    
            uint64 player_top = (uint64)0x0606 << (8*playerY);
            uint64 player_btm = (uint64)0x0606 >> max(0, 64 - 8*playerY) << max(0, 8*playerY - 64);
    
            byte char_top = LVL_TO_CHAR[(level[playerX+1] & 3) << 2 | (level[playerX] & 3)];
            byte char_btm = LVL_TO_CHAR[(level[playerX+1] >> 2) << 2 | (level[playerX] >> 2)];
            
            uint64 bit_top = (char_top == ' ')? 0 : ((uint64*)charData)[char_top];
            uint64 bit_btm = (char_btm == ' ')? 0 : ((uint64*)charData)[char_btm];
    
            ((uint64*)charData)[6] = bit_top | player_top;
            ((uint64*)charData)[7] = bit_btm | player_btm;
    
            // Render level with generated char data
            
            for (int i = 0; i < 8; i++)
            {
                lcd->createChar(i, charData + 8*i);
            }
            lcd->clear();
    
            for (int i = 0; i < 16; i++)
            {
                if (i == playerX)
                {
                    lcd->setCursor(i, 0);
                    lcd->write(byte(6));
                    lcd->setCursor(i, 1);
                    lcd->write(byte(7));
                }
                else
                {
                    lcd->setCursor(i, 0);
                    lcd->write(LVL_TO_CHAR[(level[i+1] & 3) << 2 | (level[i] & 3)]);
                    lcd->setCursor(i, 1);
                    lcd->write(LVL_TO_CHAR[(level[i+1] >> 2) << 2 | (level[i] >> 2)]);   
                }
            }
    
            // Player lost
            if ((bit_top & player_top) || (bit_btm & player_btm))
            {
                gameOver = true;
                itCnt = 0;
            }
            // Shift level
            else if (itCnt % (FPS / gameSpeed) == 0) 
            {
                shiftLevel();
                score++;
                if (score > highScore) { highScore = score; }
            }
            if (incSpeed && (itCnt+1) % (secForInc * FPS) == 0)
            {
                gameSpeed = min(gameSpeed + 1, FPS);
            }
        }

        itCnt++;
    }
};

#endif
