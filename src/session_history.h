// ============================================================
// session_history.h — Stack (LIFO) Class Declaration
// Task 3: Kiosk Session History and Navigation Module
// CT077-3-2-DSTR | Lab Work #2
// ============================================================

#ifndef SESSION_HISTORY_H
#define SESSION_HISTORY_H

#include "structures.h" // 👈 这里已经包含了 structures.h 里的 SessionStep 定义

// 栈节点
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

    // 核心栈操作
    void push(const char* action, const char* details);
    bool pop(SessionStep& poppedStep);
    bool peek(SessionStep& currentStep) const;
    bool isEmpty() const;
    int getSize() const;
    void clear();

    // 状态和历史追踪显示
    void displayCurrentState() const;
    void displayHistoryTrace() const;
};

// 外部调用入口
void runHistoryModule();

#endif // SESSION_HISTORY_H