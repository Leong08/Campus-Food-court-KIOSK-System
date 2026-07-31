#ifndef MENU_BST_H
#define MENU_BST_H

#include "structures.h"

struct MenuNode {
    MenuItem* item;
    MenuNode* left;
    MenuNode* right;
};

class MenuBST {
private:
    MenuNode* root;
    int nodeCount;

    MenuNode* createNode(MenuItem* item);
    void clear(MenuNode* node);
    bool insert(MenuNode*& node, MenuItem* item);
    MenuItem* searchByID(MenuNode* node, const char* itemID) const;
    MenuNode* removeByID(MenuNode* node, const char* itemID, bool& removed);
    MenuNode* findMin(MenuNode* node) const;
    void displayInOrder(MenuNode* node) const;
    void searchName(MenuNode* node, const char* keyword, int& found) const;
    void searchCategory(MenuNode* node, const char* category, int& found) const;

public:
    MenuBST();
    ~MenuBST();

    void clear();
    bool insert(MenuItem* item);
    bool removeByID(const char* itemID);
    MenuItem* searchByID(const char* itemID) const;
    void displaySorted() const;
    void searchByName(const char* keyword) const;
    void searchByCategory(const char* category) const;
    bool isEmpty() const;
    int getSize() const;
};

// Module Entry Point
void runMenuSearchModule();

#endif 
