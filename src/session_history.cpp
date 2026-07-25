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

// Prompt for and record one navigation step
static void promptAndRecord(SessionHistoryStack& hist) {
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

    if (hist.recordStep(s))
        cout << "  Recorded: " << s.stepType << endl;
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
            case 0: running = false; break;
            default: cout << "\n  [ERROR] Invalid choice. Please enter 0-7." << endl;
        }
    }
}
