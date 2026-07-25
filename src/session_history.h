#ifndef SESSION_HISTORY_H
#define SESSION_HISTORY_H

#include "structures.h"

const int MAX_SESSION_HISTORY = 100;

// Canonical session step labels, all modules must use these
const char* const STEP_LOGIN        = "LOGIN";
const char* const STEP_BROWSE_MENU  = "BROWSE_MENU";
const char* const STEP_SEARCH_ITEM  = "SEARCH_ITEM";
const char* const STEP_SELECT_ITEM  = "SELECT_ITEM";
const char* const STEP_UPDATE_QTY   = "UPDATE_QTY";
const char* const STEP_VIEW_CART    = "VIEW_CART";
const char* const STEP_PLACE_ORDER  = "PLACE_ORDER";
const char* const STEP_CANCEL_ORDER = "CANCEL_ORDER";

// Browser-style navigation history built from two stacks (back + forward)
class SessionHistoryStack {
private:
    SessionStep backStack[MAX_SESSION_HISTORY];    // steps already visited
    SessionStep forwardStack[MAX_SESSION_HISTORY]; // undone steps, available to re-visit
    int backTop;
    int forwardTop;

public:
    SessionHistoryStack();

    bool recordStep(const SessionStep& step);      // record a new action, clears forward history
    bool goBack(SessionStep& out);                 // step back one action (browser back)
    bool goForward(SessionStep& out);              // re-visit an undone action (browser forward)
    bool peekCurrentState(SessionStep& out) const; // read the current active step

    bool isEmpty() const;
    bool isFull() const;
    int  getSize() const;

    void displayHistory() const;         // full visited log, newest first
    void displayNavigationTrace() const; // steps visited and steps remaining
    void clear();
};

// Entry point called from main.cpp menu
void runSessionHistory();

#endif
