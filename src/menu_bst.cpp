#include "menu_bst.h"
#include "file_handler.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

static void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

static void clearScreen() {
    for (int i = 0; i < 45; i++) cout << "\n";
}

static void playTransition(const char* message) {
    cout << "\n  " << message << "\n  [";
    for (int i = 0; i < 15; i++) {
        sleepMs(50);
        cout << "#" << flush;
    }
    cout << "] 100% Complete!\n" << endl;
    sleepMs(150);
}

static void pauseScreen() {
    cout << "\n  Press [Enter] to return to the menu...";
    cin.ignore(); 
    cin.get();    
}

static MenuBST menuTree;
static bool menuTreeBuilt = false;

static void clearInputLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static void readCString(const char* prompt, char* target, int maxLength) {
    cout << prompt;
    cin.getline(target, maxLength);
    trimWhitespace(target);
}

static int readInt(const char* prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputLine();
            return value;
        }
        cout << "  [ERROR] Please enter a valid number." << endl;
        clearInputLine();
    }
}

static int readIntInRange(const char* prompt, int minValue, int maxValue) {
    while (true) {
        int value = readInt(prompt);
        if (value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "  [ERROR] Please enter a number from "
             << minValue << " to " << maxValue << "." << endl;
    }
}

static int readMinInt(const char* prompt, int minValue) {
    while (true) {
        int value = readInt(prompt);
        if (value >= minValue) {
            return value;
        }
        cout << "  [ERROR] Please enter a number at least "
             << minValue << "." << endl;
    }
}

static double readDouble(const char* prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputLine();
            return value;
        }
        cout << "  [ERROR] Please enter a valid amount." << endl;
        clearInputLine();
    }
}

static double readPriceTwoDecimals(const char* prompt) {
    char input[50];

    while (true) {
        bool validFormat = true;
        bool hasDigit = false;
        bool hasDecimalPoint = false;
        int decimalPlaces = 0;

        readCString(prompt, input, 50);

        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] >= '0' && input[i] <= '9') {
                hasDigit = true;
                if (hasDecimalPoint) {
                    decimalPlaces++;
                }
            } else if (input[i] == '.' && !hasDecimalPoint) {
                hasDecimalPoint = true;
            } else {
                validFormat = false;
                break;
            }
        }

        if (!validFormat || !hasDigit || decimalPlaces > 2) {
            cout << "  [ERROR] Price must have maximum two decimal places, e.g. 8.50." << endl;
            continue;
        }

        double value = atof(input);
        if (value <= 0.0) {
            cout << "  [ERROR] Price must be greater than 0.00." << endl;
            continue;
        }

        return value;
    }
}


static void selectCategory(char* category) {
    int choice = readIntInRange("\n  Enter category (Food = 1, Beverage = 2, Dessert = 3): ", 1, 3);
    switch (choice) {
        case 1:
            strcpy(category, "Food");
            break;
        case 2:
            strcpy(category, "Beverage");
            break;
        case 3:
            strcpy(category, "Dessert");
            break;
    }
}

static bool readYesNo(const char* prompt) {
    char answer[10];
    while (true) {
        readCString(prompt, answer, 10);
        if (strlen(answer) == 1) {
            if (answer[0] == 'y' || answer[0] == 'Y') return true;
            if (answer[0] == 'n' || answer[0] == 'N') return false;
        }
        cout << "  [ERROR] Please enter only y or n." << endl;
    }
}

static bool equalsIgnoreCase(const char* first, const char* second) {
    while (*first && *second) {
        if (tolower((unsigned char)*first) != tolower((unsigned char)*second)) return false;
        first++; second++;
    }
    return *first == '\0' && *second == '\0';
}

static bool containsIgnoreCase(const char* text, const char* keyword) {
    if (keyword[0] == '\0') return true;
    int textLen = strlen(text);
    int keyLen = strlen(keyword);
    if (keyLen > textLen) return false;
    for (int i = 0; i <= textLen - keyLen; i++) {
        int j = 0;
        while (j < keyLen && tolower((unsigned char)text[i + j]) == tolower((unsigned char)keyword[j])) j++;
        if (j == keyLen) return true;
    }
    return false;
}

static bool containsComma(const char* text) { return strchr(text, ',') != nullptr; }

static int findMenuItemIndex(const char* itemID) {
    for (int i = 0; i < menuItemCount; i++) {
        if (strcmp(menuItems[i].itemID, itemID) == 0) return i;
    }
    return -1;
}

static int findStallIndex(const char* stallID) {
    for (int i = 0; i < stallCount; i++) {
        if (strcmp(stallArray[i].stallID, stallID) == 0) return i;
    }
    return -1;
}

static void displayMenuHeader() {
    cout << "\n  ===================================================================================================" << endl;
    cout << "  " << left << setw(12) << "Item ID" << setw(30) << "Name" << setw(12) << "Category" << setw(10) << "Stock" << setw(9)  << "Prep" << setw(11) << "Stall" << setw(10) << "Price" << endl;
    cout << "  ---------------------------------------------------------------------------------------------------" << endl;
}

static void displayMenuItem(const MenuItem& item) {
    cout << "  " << left << setw(12) << item.itemID << setw(30) << item.itemName << setw(12) << item.category << setw(10) << (item.availability ? "In Stock" : "Out") << setw(9)  << item.prepTime << setw(11) << item.stallID << "RM" << fixed << setprecision(2) << item.price << endl;
}

static void displaySingleItem(const MenuItem& item) {
    int stallIdx = findStallIndex(item.stallID);
    cout << "\n  ---------------- MENU ITEM DETAILS ----------------" << endl;
    cout << "  Item ID      : " << item.itemID << endl;
    cout << "  Name         : " << item.itemName << endl;
    cout << "  Category     : " << item.category << endl;
    cout << "  Availability : " << (item.availability ? "In Stock" : "Out of Stock") << endl;
    cout << "  Prep Time    : " << item.prepTime << " minutes" << endl;
    cout << "  Stall ID     : " << item.stallID;
    if (stallIdx != -1) cout << " (" << stallArray[stallIdx].stallName << ")";
    cout << endl;
    cout << "  Price        : RM" << fixed << setprecision(2) << item.price << endl;
    cout << "  ---------------------------------------------------" << endl;
}

MenuBST::MenuBST() { root = nullptr; nodeCount = 0; }
MenuBST::~MenuBST() { clear(); }

MenuNode* MenuBST::createNode(MenuItem* item) {
    MenuNode* node = new MenuNode;
    node->item = item;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

void MenuBST::clear(MenuNode* node) {
    if (node == nullptr) return;
    clear(node->left); clear(node->right);
    delete node;
}
void MenuBST::clear() { clear(root); root = nullptr; nodeCount = 0; }

bool MenuBST::insert(MenuNode*& node, MenuItem* item) {
    if (node == nullptr) {
        node = createNode(item);
        nodeCount++;
        return true;
    }
    int cmp = strcmp(item->itemID, node->item->itemID);
    if (cmp == 0) return false;
    if (cmp < 0) return insert(node->left, item);
    return insert(node->right, item);
}
bool MenuBST::insert(MenuItem* item) {
    if (item == nullptr) return false;
    return insert(root, item);
}

MenuItem* MenuBST::searchByID(MenuNode* node, const char* itemID) const {
    if (node == nullptr) return nullptr;
    int cmp = strcmp(itemID, node->item->itemID);
    if (cmp == 0) return node->item;
    if (cmp < 0) return searchByID(node->left, itemID);
    return searchByID(node->right, itemID);
}
MenuItem* MenuBST::searchByID(const char* itemID) const { return searchByID(root, itemID); }

MenuNode* MenuBST::findMin(MenuNode* node) const {
    while (node != nullptr && node->left != nullptr) node = node->left;
    return node;
}

MenuNode* MenuBST::removeByID(MenuNode* node, const char* itemID, bool& removed) {
    if (node == nullptr) return nullptr;
    int cmp = strcmp(itemID, node->item->itemID);
    if (cmp < 0) node->left = removeByID(node->left, itemID, removed);
    else if (cmp > 0) node->right = removeByID(node->right, itemID, removed);
    else {
        removed = true;
        if (node->left == nullptr) {
            MenuNode* rightChild = node->right; delete node; return rightChild;
        }
        if (node->right == nullptr) {
            MenuNode* leftChild = node->left; delete node; return leftChild;
        }
        MenuNode* successor = findMin(node->right);
        node->item = successor->item;
        bool ignored = false;
        node->right = removeByID(node->right, successor->item->itemID, ignored);
    }
    return node;
}
bool MenuBST::removeByID(const char* itemID) {
    bool removed = false;
    root = removeByID(root, itemID, removed);
    if (removed) nodeCount--;
    return removed;
}

void MenuBST::displayInOrder(MenuNode* node) const {
    if (node == nullptr) return;
    displayInOrder(node->left);
    displayMenuItem(*node->item);
    displayInOrder(node->right);
}
void MenuBST::displaySorted() const {
    if (isEmpty()) {
        cout << "\n  [INFO] BST is empty. Load or insert menu items first." << endl;
        return;
    }
    displayMenuHeader();
    displayInOrder(root);
    cout << "  ===================================================================================================" << endl;
}

void MenuBST::searchName(MenuNode* node, const char* keyword, int& found) const {
    if (node == nullptr) return;
    searchName(node->left, keyword, found);
    if (containsIgnoreCase(node->item->itemName, keyword)) {
        displayMenuItem(*node->item); found++;
    }
    searchName(node->right, keyword, found);
}
void MenuBST::searchByName(const char* keyword) const {
    if (isEmpty()) {
        cout << "\n  [INFO] BST is empty." << endl;
        return;
    }
    int found = 0; displayMenuHeader();
    searchName(root, keyword, found);
    cout << "  ===================================================================================================" << endl;
}

void MenuBST::searchCategory(MenuNode* node, const char* category, int& found) const {
    if (node == nullptr) return;
    searchCategory(node->left, category, found);
    if (equalsIgnoreCase(node->item->category, category)) {
        displayMenuItem(*node->item); found++;
    }
    searchCategory(node->right, category, found);
}
void MenuBST::searchByCategory(const char* category) const {
    if (isEmpty()) {
        cout << "\n  [INFO] BST is empty." << endl;
        return;
    }
    int found = 0; displayMenuHeader();
    searchCategory(root, category, found);
    cout << "  ===================================================================================" << endl;
}

bool MenuBST::isEmpty() const { return root == nullptr; }
int MenuBST::getSize() const { return nodeCount; }

static void rebuildMenuTree() {
    menuTree.clear();
    for (int i = 0; i < menuItemCount; i++) {
        menuTree.insert(&menuItems[i]);
    }
    menuTreeBuilt = true;
}

static void loadMenuModuleData() {
    if (stallCount == 0) stallCount = loadStalls("datasets/stalls.csv", stallArray, MAX_STALLS);
    if (menuItemCount == 0) menuItemCount = loadMenuItems("datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);
    rebuildMenuTree();
}

static bool ensureMenuTreeReady() {
    if (menuItemCount == 0) {
        loadMenuModuleData();
    } else if (!menuTreeBuilt || menuTree.getSize() != menuItemCount) {
        rebuildMenuTree();
    }
    return !menuTree.isEmpty();
}

static void saveMenuItemsToCSV() {
    ofstream file("datasets/menu_items.csv");
    if (!file.is_open()) {
        cout << "\n  [ERROR] Cannot write to datasets/menu_items.csv" << endl;
        return;
    }
    file << "itemID,itemName,category,availability,prepTime,stallID,price\n";
    file << fixed << setprecision(2);
    for (int i = 0; i < menuItemCount; i++) {
        file << menuItems[i].itemID << ',' << menuItems[i].itemName << ',' << menuItems[i].category << ',' << (menuItems[i].availability ? 1 : 0) << ',' << menuItems[i].prepTime << ',' << menuItems[i].stallID << ',' << menuItems[i].price << '\n';
    }
    file.close();
}

static void searchMenuByID() {
    if (!ensureMenuTreeReady()) return;
    char itemID[20];
    readCString("\n  Enter Item ID to search: ", itemID, 20);
    playTransition("Querying Binary Tree Index...");
    MenuItem* item = menuTree.searchByID(itemID);
    if (item == nullptr) {
        cout << "\n  [INFO] Item " << itemID << " was not found in the BST." << endl;
    } else {
        displaySingleItem(*item);
    }
}

static void searchMenuByName() {
    if (!ensureMenuTreeReady()) return;
    char keyword[100];
    readCString("\n  Enter item name keyword: ", keyword, 100);
    playTransition("Traversing tree nodes for matching Names...");
    menuTree.searchByName(keyword);
}

static void searchMenuByCategory() {
    if (!ensureMenuTreeReady()) return;
    char category[20];
    selectCategory(category);
    playTransition("Parsing category nodes in BST...");
    menuTree.searchByCategory(category);
}

static bool inputMenuItem(MenuItem& item, bool editingExisting) {
    if (!editingExisting) {
        readCString("\n  Item ID: ", item.itemID, 20);
        if (strlen(item.itemID) == 0) {
            cout << "  [ERROR] Item ID cannot be blank." << endl;
            return false;
        }
        if (menuTree.searchByID(item.itemID) != nullptr) {
            cout << "  [ERROR] Duplicate Item ID. BST keys must be unique." << endl;
            return false;
        }
    }

    readCString("  Item name: ", item.itemName, 100);
    selectCategory(item.category);
    if (strlen(item.itemName) == 0) {
        cout << "  [ERROR] Item name cannot be blank." << endl;
        return false;
    }
    if (containsComma(item.itemName)) {
        cout << "  [ERROR] Commas are not allowed in item names saved to CSV." << endl;
        return false;
    }
    int stock = readIntInRange("  Availability (1 = In Stock, 0 = Out of Stock): ", 0, 1);
    item.availability = (stock == 1);
    item.prepTime = readMinInt("  Prep time in minutes: ", 1);
    readCString("  Stall ID: ", item.stallID, 20);
    if (strlen(item.stallID) == 0 || containsComma(item.stallID)) {
        cout << "  [ERROR] Stall ID cannot be blank or contain commas." << endl;
        return false;
    }
    if (stallCount > 0 && findStallIndex(item.stallID) == -1) {
        cout << "  [ERROR] Stall ID does not exist in the shared stall records." << endl;
        return false;
    }
    item.price = readPriceTwoDecimals("  Price (RM): ");
    return true;
}

static void insertNewMenuItem() {
    ensureMenuTreeReady();
    if (menuItemCount >= MAX_MENU_ITEMS) {
        cout << "\n  [ERROR] Maximum menu item limit reached." << endl;
        return;
    }
    MenuItem newItem;
    memset(&newItem, 0, sizeof(MenuItem));
    cout << "\n  --- Insert New Menu Item into BST ---" << endl;
    if (!inputMenuItem(newItem, false)) return;

    menuItems[menuItemCount] = newItem;
    playTransition("Balancing tree and inserting item...");
    if (menuTree.insert(&menuItems[menuItemCount])) {
        menuItemCount++;
        menuTreeBuilt = true;
        cout << "\n  Item inserted successfully and indexed in the BST." << endl;
    }
}

static void updateMenuItem() {
    if (!ensureMenuTreeReady()) return;
    char itemID[20];
    readCString("\n  Enter Item ID to update: ", itemID, 20);
    MenuItem* item = menuTree.searchByID(itemID);
    if (item == nullptr) {
        cout << "\n  [INFO] Item not found." << endl;
        return;
    }
    displaySingleItem(*item);
    MenuItem updated = *item;
    if (!inputMenuItem(updated, true)) return;
    *item = updated;
    playTransition("Applying database updates...");
    cout << "\n  Menu item updated successfully." << endl;
}

static bool hasOrdersReferencingItem(const char* itemID) {
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].itemID, itemID) == 0 && strcmp(orders[i].orderStatus, "CANCELLED") != 0) return true;
    }
    return false;
}

static void removeMenuItem() {
    if (!ensureMenuTreeReady()) return;
    char itemID[20];
    readCString("\n  Enter Item ID to remove: ", itemID, 20);
    int removeIndex = findMenuItemIndex(itemID);
    if (removeIndex == -1) {
        cout << "\n  [INFO] Item not found." << endl;
        return;
    }
    displaySingleItem(menuItems[removeIndex]);
    if (hasOrdersReferencingItem(itemID)) cout << "\n  [WARNING] Existing non-cancelled orders reference this item." << endl;
    if (!readYesNo("  Confirm remove from menu database? (y/n): ")) return;

    playTransition("Running recursive deletion of tree node...");
    menuTree.removeByID(itemID);
    for (int i = removeIndex; i < menuItemCount - 1; i++) menuItems[i] = menuItems[i + 1];
    menuItemCount--;
    rebuildMenuTree();
    cout << "\n  Item removed successfully." << endl;
}

static void showBSTExplanation() {
    cout << "\n  ---------------- BST JUSTIFICATION ----------------" << endl;
    cout << "  Structure : Binary Search Tree" << endl;
    cout << "  Key       : MenuItem.itemID, the shared primary key" << endl;
    cout << "  Search    : ID lookup follows left/right comparisons, average O(log n)" << endl;
    cout << "  Insert    : New records are placed by itemID order, average O(log n)" << endl;
    cout << "  Remove    : BST node deletion handles leaf, one-child, and two-child cases" << endl;
    cout << "  Sort      : In-order traversal prints menu items in itemID order" << endl;
    cout << "  Note      : Name/category searches traverse the same tree because those" << endl;
    cout << "              fields are not unique primary keys in the shared dataset." << endl;
    cout << "  ---------------------------------------------------" << endl;
}

static void displayMenuBanner() {
    cout << endl;
    cout << "  ====================================================" << endl;
    cout << "  |                                                  |" << endl;
    cout << "  |   CAMPUS FOOD COURT SELF-ORDER KIOSK SYSTEM      |" << endl;
    cout << "  |   Task 4: Menu Item Search and Management        |" << endl;
    cout << "  |   Data Structure: Binary Search Tree (BST)       |" << endl;
    cout << "  |                                                  |" << endl;
    cout << "  ====================================================" << endl;
}

void runMenuSearchModule() {
    clearScreen();
    static bool bstAutoLoaded = false;
    if (!bstAutoLoaded) {
        playTransition("Initializing Menu BST Index Structures...");
        loadMenuModuleData();
        bstAutoLoaded = true;
        clearScreen();
    }
    displayMenuBanner();

    bool running = true;
    while (running) {
        cout << "\n  =================================================" << endl;
        cout << "    MENU ITEM SEARCH MODULE - MAIN MENU       " << endl;
        cout << "  =================================================" << endl;
        cout << "    1. Display Menu Items Sorted by Item ID" << endl;
        cout << "    2. Search Item by ID" << endl;
        cout << "    3. Search Items by Name Keyword" << endl;
        cout << "    4. Search Items by Category" << endl;
        cout << "    5. Insert New Menu Item" << endl;
        cout << "    6. Update Menu Item" << endl;
        cout << "    7. Remove Menu Item" << endl;
        cout << "    8. Save Current Menu Database to CSV" << endl;
        cout << "    9. Show BST Data Structure Justification" << endl;
        cout << "    0. Return to Main Menu" << endl;
        cout << "  =================================================" << endl;
        cout << "    Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            clearScreen();
            playTransition("Handling input error...");
            clearScreen();
            cout << "\n  [ERROR] Invalid input. Please enter a valid number." << endl;
            sleepMs(1200);
            clearScreen();
            continue;
        }
        clearInputLine();

        switch (choice) {
            case 1:
                clearScreen();
                playTransition("Executing In-order tree traversal (Left-Root-Right)...");
                clearScreen();
                if (ensureMenuTreeReady()) menuTree.displaySorted();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 2:
                clearScreen();
                searchMenuByID();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 3:
                clearScreen();
                searchMenuByName();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 4:
                clearScreen();
                searchMenuByCategory();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 5:
                clearScreen();
                insertNewMenuItem();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 6:
                clearScreen();
                updateMenuItem();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 7:
                clearScreen();
                removeMenuItem();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 8:
                clearScreen();
                playTransition("Writing binary tree indexes into CSV dataset...");
                clearScreen();
                saveMenuItemsToCSV();
                cout << "  [SUCCESS] Menu database sync complete." << endl;
                sleepMs(1500);
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 9:
                clearScreen();
                showBSTExplanation();
                pauseScreen();
                clearScreen();
                playTransition("Returning to BST Controller...");
                clearScreen();
                break;
            case 0:
                clearScreen();
                playTransition("Safely releasing BST memory and returning to Main Menu...");
                clearScreen();
                running = false;
                break;
            default:
                clearScreen();
                cout << "\n  [ERROR] Invalid option choice. Self-redirecting..." << endl;
                sleepMs(1200);
                clearScreen();
        }
    }
}
