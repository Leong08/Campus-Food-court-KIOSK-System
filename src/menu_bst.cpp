#include "menu_bst.h"
#include "file_handler.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <limits>

using namespace std;

static MenuBST menuTree;
static bool menuTreeBuilt = false;

// SMALL INPUT / DISPLAY HELPERS

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

static bool selectCategory(char* category) {
    int choice = readInt("\n  Enter category (Food = 1, Beverage = 2, Dessert = 3): ");
    switch (choice) {
        case 1:
            strcpy(category, "Food");
            return true;
        case 2:
            strcpy(category, "Beverage");
            return true;
        case 3:
            strcpy(category, "Dessert");
            return true;
        default:
            cout << "  [ERROR] Invalid category choice. Please enter 1-3." << endl;
            category[0] = '\0';
            return false;
    }
}

static bool readYesNo(const char* prompt) {
    char answer[10];

    while (true) {
        readCString(prompt, answer, 10);

        if (strlen(answer) == 1) {
            if (answer[0] == 'y' || answer[0] == 'Y') {
                return true;
            }
            if (answer[0] == 'n' || answer[0] == 'N') {
                return false;
            }
        }

        cout << "  [ERROR] Please enter only y or n." << endl;
    }
}

static bool equalsIgnoreCase(const char* first, const char* second) {
    while (*first && *second) {
        if (tolower((unsigned char)*first) != tolower((unsigned char)*second)) {
            return false;
        }
        first++;
        second++;
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
        while (j < keyLen &&
               tolower((unsigned char)text[i + j]) ==
               tolower((unsigned char)keyword[j])) {
            j++;
        }
        if (j == keyLen) return true;
    }
    return false;
}

static bool containsComma(const char* text) {
    return strchr(text, ',') != nullptr;
}

static int findMenuItemIndex(const char* itemID) {
    for (int i = 0; i < menuItemCount; i++) {
        if (strcmp(menuItems[i].itemID, itemID) == 0) {
            return i;
        }
    }
    return -1;
}

static int findStallIndex(const char* stallID) {
    for (int i = 0; i < stallCount; i++) {
        if (strcmp(stallArray[i].stallID, stallID) == 0) {
            return i;
        }
    }
    return -1;
}

static void displayMenuHeader() {
    cout << "\n  ===================================================================================================" << endl;
    cout << "  " << left
         << setw(12) << "Item ID"
         << setw(30) << "Name"
         << setw(12) << "Category"
         << setw(10) << "Stock"
         << setw(9)  << "Prep"
         << setw(11) << "Stall"
         << setw(10) << "Price" << endl;
    cout << "  ---------------------------------------------------------------------------------------------------" << endl;
}

static void displayMenuItem(const MenuItem& item) {
    cout << "  " << left
         << setw(12) << item.itemID
         << setw(30) << item.itemName
         << setw(12) << item.category
         << setw(10) << (item.availability ? "In Stock" : "Out")
         << setw(9)  << item.prepTime
         << setw(11) << item.stallID
         << "RM" << fixed << setprecision(2) << item.price << endl;
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
    if (stallIdx != -1) {
        cout << " (" << stallArray[stallIdx].stallName << ")";
    }
    cout << endl;
    cout << "  Price        : RM" << fixed << setprecision(2) << item.price << endl;
    cout << "  ---------------------------------------------------" << endl;
}

// MENU BST CLASS IMPLEMENTATION

MenuBST::MenuBST() {
    root = nullptr;
    nodeCount = 0;
}

MenuBST::~MenuBST() {
    clear();
}

MenuNode* MenuBST::createNode(MenuItem* item) {
    MenuNode* node = new MenuNode;
    node->item = item;
    node->left = nullptr;
    node->right = nullptr;
    return node;
}

void MenuBST::clear(MenuNode* node) {
    if (node == nullptr) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

void MenuBST::clear() {
    clear(root);
    root = nullptr;
    nodeCount = 0;
}

bool MenuBST::insert(MenuNode*& node, MenuItem* item) {
    if (node == nullptr) {
        node = createNode(item);
        nodeCount++;
        return true;
    }

    int cmp = strcmp(item->itemID, node->item->itemID);
    if (cmp == 0) {
        return false;
    }
    if (cmp < 0) {
        return insert(node->left, item);
    }
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

MenuItem* MenuBST::searchByID(const char* itemID) const {
    return searchByID(root, itemID);
}

MenuNode* MenuBST::findMin(MenuNode* node) const {
    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

MenuNode* MenuBST::removeByID(MenuNode* node, const char* itemID, bool& removed) {
    if (node == nullptr) return nullptr;

    int cmp = strcmp(itemID, node->item->itemID);
    if (cmp < 0) {
        node->left = removeByID(node->left, itemID, removed);
    } else if (cmp > 0) {
        node->right = removeByID(node->right, itemID, removed);
    } else {
        removed = true;

        if (node->left == nullptr) {
            MenuNode* rightChild = node->right;
            delete node;
            return rightChild;
        }
        if (node->right == nullptr) {
            MenuNode* leftChild = node->left;
            delete node;
            return leftChild;
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
    cout << "  Total indexed menu items: " << nodeCount << endl;
}

void MenuBST::searchName(MenuNode* node, const char* keyword, int& found) const {
    if (node == nullptr) return;
    searchName(node->left, keyword, found);
    if (containsIgnoreCase(node->item->itemName, keyword)) {
        displayMenuItem(*node->item);
        found++;
    }
    searchName(node->right, keyword, found);
}

void MenuBST::searchByName(const char* keyword) const {
    if (isEmpty()) {
        cout << "\n  [INFO] BST is empty. Load or insert menu items first." << endl;
        return;
    }
    int found = 0;
    displayMenuHeader();
    searchName(root, keyword, found);
    cout << "  ===================================================================================" << endl;
    cout << "  Matches found: " << found << endl;
}

void MenuBST::searchCategory(MenuNode* node, const char* category, int& found) const {
    if (node == nullptr) return;
    searchCategory(node->left, category, found);
    if (equalsIgnoreCase(node->item->category, category)) {
        displayMenuItem(*node->item);
        found++;
    }
    searchCategory(node->right, category, found);
}

void MenuBST::searchByCategory(const char* category) const {
    if (isEmpty()) {
        cout << "\n  [INFO] BST is empty. Load or insert menu items first." << endl;
        return;
    }
    int found = 0;
    displayMenuHeader();
    searchCategory(root, category, found);
    cout << "  ===================================================================================" << endl;
    cout << "  Matches found: " << found << endl;
}

bool MenuBST::isEmpty() const {
    return root == nullptr;
}

int MenuBST::getSize() const {
    return nodeCount;
}

// MODULE DATA LOADING / REBUILDING

static void rebuildMenuTree() {
    menuTree.clear();
    for (int i = 0; i < menuItemCount; i++) {
        menuTree.insert(&menuItems[i]);
    }
    menuTreeBuilt = true;
}

static void loadMenuModuleData() {
    cout << "\n  --- Loading / Rebuilding Menu Module Data ---" << endl;

    if (stallCount == 0) {
        stallCount = loadStalls("datasets/stalls.csv", stallArray, MAX_STALLS);
        cout << "  Stalls loaded:     " << stallCount << endl;
    } else {
        cout << "  Using shared stalls already loaded: " << stallCount << endl;
    }

    if (menuItemCount == 0) {
        menuItemCount = loadMenuItems("datasets/menu_items.csv", menuItems, MAX_MENU_ITEMS);
        cout << "  Menu items loaded: " << menuItemCount << endl;
    } else {
        cout << "  Using shared menu items already loaded: " << menuItemCount << endl;
    }

    rebuildMenuTree();
    cout << "  Menu BST built with " << menuTree.getSize()
         << " itemID-indexed node(s)." << endl;
}

static bool ensureMenuTreeReady() {
    if (menuItemCount == 0) {
        cout << "\n  [INFO] Menu data is not loaded yet. Preparing shared menu data..." << endl;
        loadMenuModuleData();
    } else if (!menuTreeBuilt || menuTree.getSize() != menuItemCount) {
        rebuildMenuTree();
    }

    if (menuTree.isEmpty()) {
        cout << "\n  [ERROR] No menu records are available." << endl;
        return false;
    }
    return true;
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
        file << menuItems[i].itemID << ','
             << menuItems[i].itemName << ','
             << menuItems[i].category << ','
             << (menuItems[i].availability ? 1 : 0) << ','
             << menuItems[i].prepTime << ','
             << menuItems[i].stallID << ','
             << menuItems[i].price << '\n';
    }

    file.close();
    cout << "\n  Menu database saved to datasets/menu_items.csv" << endl;
}

// MENU MODULE OPERATIONS

static void searchMenuByID() {
    if (!ensureMenuTreeReady()) return;

    char itemID[20];
    readCString("\n  Enter Item ID to search: ", itemID, 20);

    MenuItem* item = menuTree.searchByID(itemID);
    if (item == nullptr) {
        cout << "\n  [INFO] Item " << itemID << " was not found in the BST." << endl;
        return;
    }
    displaySingleItem(*item);
}

static void searchMenuByName() {
    if (!ensureMenuTreeReady()) return;

    char keyword[100];
    readCString("\n  Enter item name keyword: ", keyword, 100);
    menuTree.searchByName(keyword);
}

static void searchMenuByCategory() {
    if (!ensureMenuTreeReady()) return;

    char category[20];
    if (!selectCategory(category)) return;
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
    if (!selectCategory(item.category)) {
        return false;
    }

    if (strlen(item.itemName) == 0) {
        cout << "  [ERROR] Item name cannot be blank." << endl;
        return false;
    }

    if (containsComma(item.itemName)) {
        cout << "  [ERROR] Commas are not allowed in item names saved to CSV." << endl;
        return false;
    }

    int stock = readInt("  Availability (1 = In Stock, 0 = Out of Stock): ");
    if (stock != 0 && stock != 1) {
        cout << "  [ERROR] Availability must be 1 or 0." << endl;
        return false;
    }
    item.availability = (stock == 1);

    item.prepTime = readInt("  Prep time in minutes: ");
    if (item.prepTime < 1) {
        cout << "  [ERROR] Prep time must be at least 1 minute." << endl;
        return false;
    }

    readCString("  Stall ID: ", item.stallID, 20);
    if (strlen(item.stallID) == 0 || containsComma(item.stallID)) {
        cout << "  [ERROR] Stall ID cannot be blank or contain commas." << endl;
        return false;
    }
    if (stallCount > 0 && findStallIndex(item.stallID) == -1) {
        cout << "  [ERROR] Stall ID does not exist in the shared stall records." << endl;
        return false;
    }

    item.price = readDouble("  Price (RM): ");
    if (item.price <= 0) {
        cout << "  [ERROR] Price must be greater than 0.00." << endl;
        return false;
    }

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
    if (!inputMenuItem(newItem, false)) {
        cout << "  Insert cancelled." << endl;
        return;
    }

    menuItems[menuItemCount] = newItem;
    if (menuTree.insert(&menuItems[menuItemCount])) {
        menuItemCount++;
        menuTreeBuilt = true;
        cout << "\n  Item inserted successfully and indexed in the BST." << endl;
    } else {
        cout << "\n  [ERROR] BST insertion failed." << endl;
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
    cout << "\n  Enter replacement details. Item ID remains unchanged." << endl;

    MenuItem updated = *item;
    if (!inputMenuItem(updated, true)) {
        cout << "  Update cancelled." << endl;
        return;
    }

    *item = updated;
    cout << "\n  Menu item updated successfully." << endl;
}

static bool hasOrdersReferencingItem(const char* itemID) {
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].itemID, itemID) == 0 &&
            strcmp(orders[i].orderStatus, "CANCELLED") != 0) {
            return true;
        }
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

    if (hasOrdersReferencingItem(itemID)) {
        cout << "\n  [WARNING] Existing non-cancelled orders reference this item." << endl;
    }

    if (!readYesNo("  Confirm remove from menu database? (y/n): ")) {
        cout << "  Remove cancelled." << endl;
        return;
    }

    menuTree.removeByID(itemID);

    for (int i = removeIndex; i < menuItemCount - 1; i++) {
        menuItems[i] = menuItems[i + 1];
    }
    menuItemCount--;

    rebuildMenuTree();
    cout << "\n  Item removed from the shared menu database and BST rebuilt." << endl;
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

// ============================================================
// MODULE ENTRY POINT
// ============================================================

void runMenuSearchModule() {
    displayMenuBanner();

    bool running = true;
    while (running) {
        cout << "\n  =================================================" << endl;
        cout << "    MENU ITEM SEARCH MODULE - MAIN MENU       " << endl;
        cout << "  =================================================" << endl;
        cout << "    1. Load / Rebuild Menu BST from Shared Data" << endl;
        cout << "    2. Display Menu Items Sorted by Item ID" << endl;
        cout << "    3. Search Item by ID" << endl;
        cout << "    4. Search Items by Name Keyword" << endl;
        cout << "    5. Search Items by Category" << endl;
        cout << "    6. Insert New Menu Item" << endl;
        cout << "    7. Update Menu Item" << endl;
        cout << "    8. Remove Menu Item" << endl;
        cout << "    9. Save Current Menu Database to CSV" << endl;
        cout << "   10. Show BST Data Structure Justification" << endl;
        cout << "    0. Return to Main Menu" << endl;
        cout << "  =================================================" << endl;
        cout << "    Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            cout << "\n  [ERROR] Invalid input. Please enter a number." << endl;
            clearInputLine();
            continue;
        }
        clearInputLine();

        switch (choice) {
            case 1:  loadMenuModuleData();       break;
            case 2:  if (ensureMenuTreeReady()) menuTree.displaySorted(); break;
            case 3:  searchMenuByID();           break;
            case 4:  searchMenuByName();         break;
            case 5:  searchMenuByCategory();     break;
            case 6:  insertNewMenuItem();        break;
            case 7:  updateMenuItem();           break;
            case 8:  removeMenuItem();           break;
            case 9:  saveMenuItemsToCSV();       break;
            case 10: showBSTExplanation();       break;
            case 0:
                running = false;
                break;
            default:
                cout << "\n  [ERROR] Invalid choice. Please enter 0-10." << endl;
                break;
        }
    }
}
