#ifndef _MENU_H
#define MENU_H

#define MENU_SUBMENU 0
#define MENU_ACTION 1
#define MENU_BOOL 2
#define MENU_NUM 4
#define MENU_EXIT 8
#define MENU_READONLY 16

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
    MenuPage(const char* title, void (*action)())
    {
        this->title = title;
        this->parent = NULL;
      
        this->nChild = 0;
        this->children = NULL;
        
        this->type = MENU_ACTION;
        this->value = (int)action;
        this->defVal = NULL;
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
};

#endif
