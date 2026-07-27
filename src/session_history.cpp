#include "session_history.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>

using namespace std;

SessionHistoryStack::SessionHistoryStack() {
    backTop = -1;
    forwardTop = -1;
}

// Record a new action; a new action clears the forward history (like a browser)
bool SessionHistoryStack::recordStep(const SessionStep& step) {
    if (isFull()) {
        cout << "  [ERROR] Session limit reached (" << MAX_SESSION_HISTORY
             << " steps). Cannot record more." << endl;
        return false;
    }
    backTop++;
    backStack[backTop] = step;
    forwardTop = -1;
    return true;
}

// Browser back: move the current step onto the forward stack
bool SessionHistoryStack::goBack(SessionStep& out) {
    if (isEmpty()) {
        cout << "  [INFO] Nothing to go back to." << endl;
        return false;
    }
    out = backStack[backTop];
    forwardTop++;
    forwardStack[forwardTop] = backStack[backTop];
    backTop--;
    return true;
}

// Browser forward: re-visit the most recently undone step
bool SessionHistoryStack::goForward(SessionStep& out) {
    if (forwardTop == -1) {
        cout << "  [INFO] No step ahead to re-visit." << endl;
        return false;
    }
    out = forwardStack[forwardTop];
    backTop++;
    backStack[backTop] = forwardStack[forwardTop];
    forwardTop--;
    return true;
}

// Read the current active step without changing anything
bool SessionHistoryStack::peekCurrentState(SessionStep& out) const {
    if (isEmpty()) {
        cout << "  [INFO] No current step to show." << endl;
        return false;
    }
    out = backStack[backTop];
    return true;
}

// Modify the current re-visited step in place, so a wrong selection can be corrected
bool SessionHistoryStack::modifyCurrentState(const char* newItem, int newQty, const char* newState) {
    if (isEmpty()) {
        cout << "  [INFO] No current step to modify." << endl;
        return false;
    }
    if (newItem  != nullptr) strcpy(backStack[backTop].itemID, newItem);
    if (newQty   >= 0)       backStack[backTop].quantity = newQty;
    if (newState != nullptr) strcpy(backStack[backTop].stateSnapshot, newState);
    backStack[backTop].timeStamp = (long)time(nullptr);
    return true;
}

bool SessionHistoryStack::isEmpty() const {
    return backTop == -1;
}

bool SessionHistoryStack::isFull() const {
    return backTop == MAX_SESSION_HISTORY - 1;
}

int SessionHistoryStack::getSize() const {
    return backTop + 1;
}

// Print the full visited log, newest step first
void SessionHistoryStack::displayHistory() const {
    if (isEmpty()) {
        cout << "\n  [INFO] Session history is empty." << endl;
        return;
    }
    cout << "\n  Session History (most recent first)" << endl;
    cout << "  " << left
         << setw(4)  << "#"
         << setw(14) << "Step"
         << setw(12) << "Student"
         << setw(10) << "Item"
         << setw(5)  << "Qty"
         << "State" << endl;
    cout << "  ------------------------------------------------------------" << endl;

    for (int i = backTop; i >= 0; i--) {
        cout << "  " << left
             << setw(4)  << (backTop - i + 1)
             << setw(14) << backStack[i].stepType
             << setw(12) << backStack[i].studentID
             << setw(10) << (strlen(backStack[i].itemID) > 0 ? backStack[i].itemID : "-")
             << setw(5)  << backStack[i].quantity
             << backStack[i].stateSnapshot << endl;
    }
    cout << "  Total steps: " << getSize() << endl;
}

// Show the steps already visited and the steps still ahead
void SessionHistoryStack::displayNavigationTrace() const {
    if (isEmpty() && forwardTop == -1) {
        cout << "\n  [INFO] No navigation steps yet." << endl;
        return;
    }

    cout << "\n  Steps visited  : ";
    if (isEmpty()) {
        cout << "(none)";
    } else {
        for (int i = 0; i <= backTop; i++) {
            if (i == backTop) cout << "[" << backStack[i].stepType << "]"; // current step
            else              cout << backStack[i].stepType << " -> ";
        }
    }

    cout << "\n  Steps remaining: ";
    if (forwardTop == -1) {
        cout << "(none)";
    } else {
        for (int i = forwardTop; i >= 0; i--) {
            cout << forwardStack[i].stepType;
            if (i > 0) cout << " -> ";
        }
    }
    cout << endl;
}

void SessionHistoryStack::clear() {
    backTop = -1;
    forwardTop = -1;
}

// Map a menu number to a canonical STEP_* label
static const char* pickStepType(int n) {
    switch (n) {
        case 1: return STEP_LOGIN;
        case 2: return STEP_BROWSE_MENU;
        case 3: return STEP_SEARCH_ITEM;
        case 4: return STEP_SELECT_ITEM;
        case 5: return STEP_UPDATE_QTY;
        case 6: return STEP_VIEW_CART;
        case 7: return STEP_PLACE_ORDER;
        case 8: return STEP_CANCEL_ORDER;
        default: return STEP_BROWSE_MENU;
    }
}

// Item ID only applies to steps that act on a specific menu item
static bool stepNeedsItem(const char* type) {
    return strcmp(type, STEP_SEARCH_ITEM)  == 0 ||
           strcmp(type, STEP_SELECT_ITEM)  == 0 ||
           strcmp(type, STEP_UPDATE_QTY)   == 0 ||
           strcmp(type, STEP_CANCEL_ORDER) == 0;
}

// Quantity only applies when the student is setting an item amount
static bool stepNeedsQuantity(const char* type) {
    return strcmp(type, STEP_SELECT_ITEM) == 0 ||
           strcmp(type, STEP_UPDATE_QTY)  == 0;
}

// Prompt for and record one navigation step, asking only for fields the step uses
static void promptAndRecord(SessionHistoryStack& hist) {
    cout << "\n  Step type:\n"
         << "    1.LOGIN 2.BROWSE_MENU 3.SEARCH_ITEM 4.SELECT_ITEM\n"
         << "    5.UPDATE_QTY 6.VIEW_CART 7.PLACE_ORDER 8.CANCEL_ORDER\n"
         << "  Choice: ";
    int t;
    if (!(cin >> t)) { cin.clear(); cin.ignore(1000, '\n'); cout << "  [ERROR] Invalid.\n"; return; }

    SessionStep s;
    strcpy(s.stepType, pickStepType(t));
    s.itemID[0] = '\0';
    s.quantity  = 0;

    cout << "  Student ID: ";
    cin >> setw(20) >> s.studentID;

    if (stepNeedsItem(s.stepType)) {
        cout << "  Item ID: ";
        cin >> setw(20) >> s.itemID;
    }
    if (stepNeedsQuantity(s.stepType)) {
        cout << "  Quantity: ";
        if (!(cin >> s.quantity)) { cin.clear(); cin.ignore(1000, '\n'); s.quantity = 0; }
    }

    cin.ignore(1000, '\n');
    cout << "  State note: ";
    cin.getline(s.stateSnapshot, 100);
    s.timeStamp = (long)time(nullptr);

    if (hist.recordStep(s))
        cout << "  Recorded: " << s.stepType << endl;
}

// Re-visit the current step and correct its item, quantity and note (error recovery)
static void promptAndModify(SessionHistoryStack& hist) {
    SessionStep cur;
    if (!hist.peekCurrentState(cur)) return;

    cout << "\n  Modifying current step: " << cur.stepType
         << " | item=" << (strlen(cur.itemID) > 0 ? cur.itemID : "-")
         << " | qty=" << cur.quantity
         << " | " << cur.stateSnapshot << endl;

    char newItem[20];
    cout << "  New Item ID (- to keep): ";
    cin >> setw(20) >> newItem;

    int newQty;
    cout << "  New Quantity (-1 to keep): ";
    if (!(cin >> newQty)) { cin.clear(); newQty = -1; }
    cin.ignore(1000, '\n');

    char newState[100];
    cout << "  New state note (blank to keep): ";
    cin.getline(newState, 100);

    const char* itemArg  = (strcmp(newItem, "-") == 0) ? nullptr : newItem;
    const char* stateArg = (newState[0] == '\0')       ? nullptr : newState;

    if (hist.modifyCurrentState(itemArg, newQty, stateArg))
        cout << "  Current state corrected." << endl;
}

// Build one step inline for the scripted demo
static SessionStep makeStep(const char* type, const char* sid, const char* item, int qty, const char* snap) {
    SessionStep s;
    strcpy(s.stepType, type);
    strcpy(s.studentID, sid);
    strcpy(s.itemID, item);
    s.quantity  = qty;
    s.timeStamp = (long)time(nullptr);
    strcpy(s.stateSnapshot, snap);
    return s;
}

// Auto-record a realistic kiosk journey so steps are captured in sequence, as during real navigation
static void runSampleSession(SessionHistoryStack& hist) {
    hist.clear();
    const char* sid = "TP075570";
    SessionStep flow[] = {
        makeStep(STEP_LOGIN,       sid, "",         0, "Scanned student ID at kiosk"),
        makeStep(STEP_BROWSE_MENU, sid, "",         0, "Browsing Malay stall menu"),
        makeStep(STEP_SELECT_ITEM, sid, "ITEM_M01", 1, "Selected Nasi Lemak Ayam"),
        makeStep(STEP_UPDATE_QTY,  sid, "ITEM_M01", 2, "Changed quantity to 2"),
        makeStep(STEP_SELECT_ITEM, sid, "ITEM_M02", 1, "Added Teh Tarik"),
        makeStep(STEP_VIEW_CART,   sid, "",         0, "Reviewing cart"),
        makeStep(STEP_PLACE_ORDER, sid, "",         0, "Order placed")
    };
    int n = sizeof(flow) / sizeof(flow[0]);

    cout << "\n  Simulating a kiosk session (steps auto-recorded in sequence):\n";
    for (int i = 0; i < n; i++) {
        if (hist.recordStep(flow[i]))
            cout << "  [auto] " << left << setw(14) << flow[i].stepType
                 << flow[i].stateSnapshot << endl;
    }
    hist.displayNavigationTrace();
    cout << "  Tip: now try Back / Forward to retrace this session." << endl;
}

// Interactive menu driver invoked from main.cpp
void runSessionHistory() {
    SessionHistoryStack hist;
    SessionStep out;
    bool running = true;

    while (running) {
        cout << "\n  ---------------------------------------\n";
        cout << "    Session History Module - Main menu\n";
        cout << "\n  ---------------------------------------\n";
        cout << "    1. Record a navigation step\n";
        cout << "    2. Back (undo one step)\n";
        cout << "    3. Forward (re-visit a step)\n";
        cout << "    4. Show current state\n";
        cout << "    5. Display full history\n";
        cout << "    6. Display navigation trace\n";
        cout << "    7. Clear history\n";
        cout << "    8. Run sample kiosk session (auto-record)\n";
        cout << "    9. Modify current state (fix a mistake)\n";
        cout << "    0. Exit\n";
        cout << "    Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n  [ERROR] Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: promptAndRecord(hist); break;
            case 2:
                if (hist.goBack(out))
                    cout << "  Went back from: " << out.stepType << endl;
                break;
            case 3:
                if (hist.goForward(out))
                    cout << "  Re-visited: " << out.stepType << endl;
                break;
            case 4:
                if (hist.peekCurrentState(out))
                    cout << "  Current state: " << out.stepType
                         << " | " << out.stateSnapshot << endl;
                break;
            case 5: hist.displayHistory(); break;
            case 6: hist.displayNavigationTrace(); break;
            case 7: hist.clear(); cout << "  History cleared." << endl; break;
            case 8: runSampleSession(hist); break;
            case 9: promptAndModify(hist); break;
            case 0: running = false; break;
            default: cout << "\n  [ERROR] Invalid choice. Please enter 0-9." << endl;
        }
    }
}
