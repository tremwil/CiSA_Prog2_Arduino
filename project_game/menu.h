#ifndef MENU_H
#define MENU_H

#include <LiquidCrystal.h>

#define MENU_SUBMENU 1
#define MENU_EXIT 2
#define MENU_ACTION 4
#define MENU_BOOL 8
#define MENU_NUM 16
#define MENU_READONLY 32

typedef void (*Action)();

byte UP_DOWN_ARROW[8] =
{
    0b00100,
    0b01010,
    0b10001,
    0b00000,
    0b00000,
    0b10001,
    0b01010,
    0b00100
};

class MenuPage
{
public:	
	const char* title;
    MenuPage* parent;
    
	byte nChild;
	MenuPage* children;	
	
	byte type;
	int* valuePtr;
	int defVal;

    // Create sub menu.
	MenuPage(const char* title, byte nChild, MenuPage* children)
	{
		this->title = title;
		this->parent = NULL;
      
		this->nChild = nChild;
		this->children = children;
		
		this->type = MENU_SUBMENU;
		this->valuePtr = NULL;
		this->defVal = 0;
	}
   
    // Create action menu
    MenuPage(const char* title, Action action)
    {
        this->title = title;
        this->parent = NULL;
      
        this->nChild = 0;
        this->children = NULL;
        
        this->type = MENU_ACTION;
        this->valuePtr = (int*)action;
        this->defVal = 0;
    }

    // Create special menu (exit/action) without value.
    MenuPage(const char* title, byte type)
    {
        this->title = title;
        this->parent = NULL;
      
        this->nChild = 0;
        this->children = NULL;
        
        this->type = type;
        this->valuePtr = NULL;
        this->defVal = 0;
    }

    // Create general menu.
	MenuPage(const char* title, byte type, int *valuePtr, int defVal)
	{
		this->title = title;
		this->parent = NULL;
      
		this->nChild = 0;
		this->children = NULL;
		
		this->type = type;
		this->valuePtr = valuePtr;
		this->defVal = defVal;
	}

    // Go through menu tree and assign parents
    void setParents(bool recurse)
    {
        for (int i = 0; i < nChild; i++)
        {
            children[i].parent = this;
            if (recurse) { children[i].setParents(true); }
        }
    }

    // Go through menu tree and return pointer to the menu page with the given title
    MenuPage* getChildrenByTitle(const char* title, bool recurse)
    {
        for (int i = 0; i < nChild; i++)
        {
            if (strcmp(children[i].title, title)) return &(children[i]);
            if (recurse) 
            {  
                MenuPage* res = children[i].getChildrenByTitle(title, true);
                if (res != NULL) { return res; }
            }
        }
        return NULL;
    }

    // Return if the entire menu tree from this point is on default values.
    bool isTreeDefault()
    {
        if (*valuePtr != defVal) return false;

        for (int i = 0; i < nChild; i++)
        {
            if (!children[i].isTreeDefault()) return false;
        }

        return true;
    }

    // Reset the menu to its default value (and all sub... menus if recurse is true).
    void reset(bool recurse)
    {
        if ((MENU_BOOL | MENU_NUM) & type) *valuePtr = defVal;

        if (recurse)
        {
            for (int i = 0; i < nChild; i++)
            {
                children[i].reset(true);
            }   
        }
    }
};

// Handles menu navigation
class MenuHandler
{
public:
    LiquidCrystal* lcd;
    MenuPage* currPage;
    int subIndex;

    // Create MenuHandler from LCD and MenuPage pointers
    MenuHandler(LiquidCrystal* lcd, MenuPage* currPage)
    {
        this->lcd = lcd;
        this->currPage = currPage;
        this->subIndex = 0;
    }

    // Render current menu on the LCD.
    void render()
    {
        lcd->createChar(0, UP_DOWN_ARROW);
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print((currPage->parent == NULL) ? "   " : "<< ");
        lcd->print(currPage->title);

        if (currPage->type & MENU_SUBMENU)
        {
            lcd->setCursor(0,1);
            lcd->print(">>");
            
            lcd->setCursor(3, 1);
            lcd->print(currPage->children[subIndex].title);
            
            lcd->setCursor(15, 1);
            lcd->write(byte(0));
        }
        else if (currPage->type & (MENU_NUM | MENU_BOOL))
        {
            lcd->setCursor(0,1);
            if (currPage->type & MENU_NUM) lcd->print(*currPage->valuePtr);
            if (currPage->type & MENU_BOOL) lcd->print(*currPage->valuePtr ? "True" : "False");

            if (*currPage->valuePtr == currPage->defVal) lcd->print(" (def)");
        }
    }

    // Set the current page of this MenuHandler
    void setPage(MenuPage *page)
    {
        currPage = page;
        subIndex = 0;
        render();
    }

    // Call when select button is pressed. Returns true if the menu should be closed.
    bool onSelect()
    {
        if (currPage->type & MENU_SUBMENU)
        {
            if (currPage->children[subIndex].type & MENU_EXIT) return true;

            if (currPage->children[subIndex].type & MENU_ACTION)
            {
               Action aptr = (Action)currPage->children[subIndex].valuePtr; aptr();
            }
            else
            {
                currPage = &currPage->children[subIndex];
                subIndex = 0;   
            }

            render();
        }
        
        return false;
    }

    // Call when escape button is pressed. Goes upward through the menu tree.
    void onEscape()
    {
        if (currPage->parent != NULL)
        {
            currPage = currPage->parent;
            subIndex = 0;
            render();
        }
    }

    // Call when reset button is pressed.
    void onReset()
    {
        currPage->reset(true);
        render();
    }

    // Call when moving through sub menus.
    void onMoveSubMenu(int shift)
    {
        if (currPage->type & MENU_SUBMENU)
        {
            subIndex = (subIndex + shift) % currPage->nChild;
            if (subIndex < 0) subIndex += currPage->nChild;
            render();
        }
    }

    // Call when toggle button is pressed.
    void onToggle()
    {
        if ((currPage->type & MENU_BOOL) && !(currPage->type & MENU_READONLY))
        {
            *currPage->valuePtr = !(*currPage->valuePtr);
            render();
        }
    }

    // Call when delete button is pressed.
    void onDelete()
    {
        if ((currPage->type & MENU_NUM) && !(currPage->type & MENU_READONLY))
        {
            *currPage->valuePtr /= 10;
            render();
        }
    }

    // Call when digit button (0-9) is pressed.
    void onDigit(int digit)
    {
        if ((currPage->type & MENU_NUM) && !(currPage->type & MENU_READONLY))
        {
            int new_val = 10*(*currPage->valuePtr) + digit;
            if (new_val < 0x8000) 
            {
                *currPage->valuePtr = new_val;
                render();
            }
        }
    }
};

#endif
