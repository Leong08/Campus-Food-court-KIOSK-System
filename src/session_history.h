#ifndef SESSION_HISTORY_H
#define SESSION_HISTORY_H


const int MAX_SESSION_HISTORY = 100;  // max navigation steps per session

struct SessionStep {
    char stepType[20];        // "BROWSE", "SELECT_ITEM", "UPDATE_QTY", "CHECKOUT"
    char studentID[20];       
    char itemID[20];          
    int  quantity;           
    long timeStamp;           
    char stateSnapshot[100];  
};

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

#endif 
