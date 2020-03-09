#ifndef _MENU_H
#define MENU_H

#include <LiquidCrystal.h>

#define MENU_SUBMENU 1
#define MENU_EXIT 2
#define MENU_ACTION 4
#define MENU_BOOL 8
#define MENU_NUM 16
#define MENU_READONLY 32

typedef void (*Action)();

class MenuPage
{
public:	
	const char* title;
    MenuPage* parent;
    
	byte nChild;
	MenuPage* children;	
	
	byte type;
	int value;
	int defVal;

    // Create sub menu.
	MenuPage(const char* title, byte nChild, MenuPage* children)
	{
		this->title = title;
		this->parent = NULL;
      
		this->nChild = nChild;
		this->children = children;
		
		this->type = MENU_SUBMENU;
		this->value = 0;
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
        this->value = (int)action;
        this->defVal = (int)action;
    }

    // Create general menu.
	MenuPage(const char* title, byte type, int val)
	{
		this->title = title;
		this->parent = NULL;
      
		this->nChild = 0;
		this->children = NULL;
		
		this->type = type;
		this->value = val;
		this->defVal = val;
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
        if (value != defVal) return false;

        for (int i = 0; i < nChild; i++)
        {
            if (!children[i].isTreeDefault()) return false;
        }

        return true;
    }

    // Reset the menu to its default value (and all sub... menus if recurse is true).
    void reset(bool recurse)
    {
        if (!(type & MENU_READONLY)) value = defVal;

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
        lcd->clear();
        lcd->setCursor(0,0);
        lcd->print((currPage->parent == NULL) ? "  " : "^ ");
        lcd->print(currPage->title);

        if (currPage->type & MENU_SUBMENU)
        {
            lcd->setCursor(0,1);
            lcd->print("< ");
            lcd->print(currPage->children[subIndex].title);
            lcd->setCursor(15, 1);
            lcd->print(">");
        }
        else if (currPage->type & (MENU_NUM | MENU_BOOL))
        {
            lcd->setCursor(0,1);
            if (currPage->type & MENU_NUM) lcd->print(currPage->value);
            if (currPage->type & MENU_BOOL) lcd->print(currPage->value ? "True" : "False");

            if (currPage->value == currPage->defVal) lcd->print(" (def)");
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
               Action aptr = (Action)currPage->children[subIndex].value; aptr();
            }
            else
            {
                currPage = &currPage->children[subIndex];
                subIndex = 0;   
            }
        }
        if ((currPage->type & MENU_BOOL) && !(currPage->type & MENU_READONLY))
        {
            currPage->value = !currPage->value;
        }

        render();
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

    // Call when delete button is pressed.
    void onDelete()
    {
        if ((currPage->type & MENU_NUM) && !(currPage->type & MENU_READONLY))
        {
            currPage->value /= 10;
            render();
        }
    }

    // Call when digit button (0-9) is pressed.
    void onDigit(int digit)
    {
        if ((currPage->type & MENU_NUM) && !(currPage->type & MENU_READONLY))
        {
            int new_val = 10*currPage->value + digit;
            if (new_val < 0x8000) 
            {
                currPage->value = new_val;
                render();
            }
        }
    }
};

#endif
