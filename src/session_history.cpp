#include "session_history.h"
#include <iostream>
#include <iomanip>
#include <cstring>

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
