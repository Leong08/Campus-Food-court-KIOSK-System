#ifndef SESSION_HISTORY_H
#define SESSION_HISTORY_H

struct SessionStep {
    char actionName[50];
    char details[100];
};

struct HistoryNode {
    SessionStep step;
    HistoryNode* next;
};

class SessionHistoryStack {
private:
    HistoryNode* topNode;
    int count;

public:
    SessionHistoryStack();
    ~SessionHistoryStack();

    void push(const char* action, const char* details);
    bool pop(SessionStep& poppedStep);
    bool peek(SessionStep& currentStep) const;
    bool isEmpty() const;
    int getSize() const;
    void clear();

    void displayCurrentState() const;
    void displayHistoryTrace() const;
};

void runHistoryModule();

#endif // SESSION_HISTORY_H
