#include "session_history.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <ctime>

using namespace std;

SessionHistoryStack::SessionHistoryStack() {
    topIndex = -1;  // empty stack
}

bool SessionHistoryStack::pushStep(const SessionStep& step) {
    if (isFull()) {
        cout << "  [ERROR] Session limit reached (" << MAX_SESSION_HISTORY
             << " steps). Cannot record more." << endl;
        return false;
    }
    topIndex++;
    steps[topIndex] = step;  
    return true;
}

bool SessionHistoryStack::undoStep(SessionStep& out) {
    if (isEmpty()) {
        cout << "  [INFO] Nothing to undo." << endl;
        return false;
    }
    out = steps[topIndex];
    topIndex--;
    return true;
}

bool SessionHistoryStack::peekCurrentState(SessionStep& out) const {
    if (isEmpty()) {
        cout << "  [INFO] No current step to show." << endl;
        return false;
    }
    out = steps[topIndex];
    return true;
}

bool SessionHistoryStack::isEmpty() const {
    return topIndex == -1;
}

bool SessionHistoryStack::isFull() const {
    return topIndex == MAX_SESSION_HISTORY - 1;
}

int SessionHistoryStack::getSize() const {
    return topIndex + 1;
}

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

    // walk from the top down so the newest step prints first
    for (int i = topIndex; i >= 0; i--) {
        cout << "  " << left
             << setw(4)  << (topIndex - i + 1)
             << setw(14) << steps[i].stepType
             << setw(12) << steps[i].studentID
             << setw(10) << (strlen(steps[i].itemID) > 0 ? steps[i].itemID : "-")
             << setw(5)  << steps[i].quantity
             << steps[i].stateSnapshot << endl;
    }
    cout << "  Total steps: " << getSize() << endl;
}

void SessionHistoryStack::displayNavigationTrace() const {
    if (isEmpty()) {
        cout << "\n  [INFO] No navigation steps yet." << endl;
        return;
    }
    cout << "\n  Navigation trace: ";
    // walk from the bottom up for chronological order
    for (int i = 0; i <= topIndex; i++) {
        cout << steps[i].stepType;
        if (i < topIndex) cout << " -> ";
    }
    cout << endl;
}

void SessionHistoryStack::clear() {
    topIndex = -1;  // reset without touching the array contents
}

// maps a menu number to a canonical STEP_* label
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

// prompts for and records one navigation step
static void recordStep(SessionHistoryStack& hist) {
    cout << "\n  Step type:\n"
         << "    1.LOGIN 2.BROWSE_MENU 3.SEARCH_ITEM 4.SELECT_ITEM\n"
         << "    5.UPDATE_QTY 6.VIEW_CART 7.PLACE_ORDER 8.CANCEL_ORDER\n"
         << "  Choice: ";
    int t;
    if (!(cin >> t)) { cin.clear(); cin.ignore(1000, '\n'); cout << "  [ERROR] Invalid.\n"; return; }

    SessionStep s;
    strcpy(s.stepType, pickStepType(t));

    cout << "  Student ID: ";
    cin >> setw(20) >> s.studentID;
    cout << "  Item ID (- for none): ";
    cin >> setw(20) >> s.itemID;
    if (strcmp(s.itemID, "-") == 0) s.itemID[0] = '\0';
    cout << "  Quantity: ";
    if (!(cin >> s.quantity)) { cin.clear(); cin.ignore(1000, '\n'); s.quantity = 0; }
    cin.ignore(1000, '\n');
    cout << "  State note: ";
    cin.getline(s.stateSnapshot, 100);
    s.timeStamp = (long)time(nullptr);

    if (hist.pushStep(s))
        cout << "  Recorded: " << s.stepType << endl;
}

// interactive driver invoked from main.cpp
void runSessionHistory() {
    SessionHistoryStack hist;
    SessionStep out;
    bool running = true;

    while (running) {
        cout << "\n  ============================================\n";
        cout << "    SESSION HISTORY MODULE - MAIN MENU\n";
        cout << "  ============================================\n";
        cout << "    1. Record a navigation step\n";
        cout << "    2. Undo last step (back)\n";
        cout << "    3. Show current state\n";
        cout << "    4. Display full history\n";
        cout << "    5. Display navigation trace\n";
        cout << "    6. Clear history\n";
        cout << "    0. Exit\n";
        cout << "  ============================================\n";
        cout << "    Enter choice: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n  [ERROR] Invalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: recordStep(hist); break;
            case 2:
                if (hist.undoStep(out))
                    cout << "  Went back from: " << out.stepType << endl;
                break;
            case 3:
                if (hist.peekCurrentState(out))
                    cout << "  Current state: " << out.stepType
                         << " | " << out.stateSnapshot << endl;
                break;
            case 4: hist.displayHistory(); break;
            case 5: hist.displayNavigationTrace(); break;
            case 6: hist.clear(); cout << "  History cleared." << endl; break;
            case 0: running = false; break;
            default: cout << "\n  [ERROR] Invalid choice. Please enter 0-6." << endl;
        }
    }
}
