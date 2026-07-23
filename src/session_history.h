#ifndef SESSION_HISTORY_H
#define SESSION_HISTORY_H

#include "structures.h"

const int MAX_SESSION_HISTORY = 100;

// Canonical session step types, team must use these labels 
const char* const STEP_LOGIN        = "LOGIN";        
const char* const STEP_BROWSE_MENU  = "BROWSE_MENU";  
const char* const STEP_SEARCH_ITEM  = "SEARCH_ITEM";  
const char* const STEP_SELECT_ITEM  = "SELECT_ITEM";  
const char* const STEP_UPDATE_QTY   = "UPDATE_QTY";  
const char* const STEP_VIEW_CART    = "VIEW_CART";    
const char* const STEP_PLACE_ORDER  = "PLACE_ORDER";  
const char* const STEP_CANCEL_ORDER = "CANCEL_ORDER"; 

// Array-based Stack (LIFO) for undo/backtrack navigation
class SessionHistoryStack {
private:
    SessionStep steps[MAX_SESSION_HISTORY];  
    int topIndex;                           

public:
    SessionHistoryStack();

    bool pushStep(const SessionStep& step);        // push onto top; false if full
    bool undoStep(SessionStep& out);               // pop top into out; false if empty
    bool peekCurrentState(SessionStep& out) const; // read top into out; false if empty

    bool isEmpty() const;
    bool isFull() const;
    int  getSize() const;

    void displayHistory() const;         // most-recent first
    void displayNavigationTrace() const; // oldest -> newest breadcrumb
    void clear();                        // reset to empty
};

// Entry point called from main.cpp menu
void runSessionHistory();

#endif
