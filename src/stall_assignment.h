// ============================================================
// stall_assignment.h — Circular Queue Class Declaration
// Task 2: Stall Assignment Module
// By: Sherwin A/L Jesudass (TP075823)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================
// The Circular Queue is used here because stall assignment
// follows a round-robin pattern — orders are distributed to
// stalls in a continuous rotation that wraps back to the start
// after reaching the last stall. A circular queue naturally
// models this wrapping behaviour using modulo arithmetic,
// making it the ideal data structure for fair, rotating
// allocation without restarting the assignment cycle.
// ============================================================

#ifndef STALL_ASSIGNMENT_H
#define STALL_ASSIGNMENT_H

#include "structures.h"

class CircularQueue {
private:
    // ---- Circular queue internals ----
    Stall* queue[MAX_STALLS];   // Fixed-size circular array of stall pointers
    int front;                  // Index of the first element
    int rear;                   // Index of the last element
    int count;                  // Current number of stalls in queue

    // ---- Round-robin pointer ----
    // This pointer rotates through 0..count-1 and determines
    // which stall gets checked first for the next order.
    // It advances after every successful assignment, wrapping
    // back to 0 after reaching the last stall (circular behaviour).
    int assignPtr;

    // ---- Assignment history log ----
    AssignmentRecord history[MAX_STALL_HISTORY];
    int historyCount;

public:
    // Constructor
    CircularQueue();

    // ---- Core circular queue operations ----
    bool   enqueue(Stall* stall);         // Add stall to rear   — O(1)
    Stall* dequeue();                     // Remove from front    — O(1)
    Stall* peek() const;                  // View front element   — O(1)
    bool   isFull() const;                // Check if queue full  — O(1)
    bool  isEmpty() const;                // Check if queue empty — O(1)
    int   getSize() const;                // Get element count    — O(1)

    // ---- Round-robin stall assignment ----
    // Finds the next open, non-full stall and assigns the order.
    // Skips closed stalls and stalls at full capacity.
    // Returns true if assignment succeeded, false if all stalls unavailable.
    bool assignOrder(Order& order);

    // ---- Stall management ----
    int  findStall(const char* stallID) const;          // Find stall index — O(n)
    bool updateCapacity(const char* stallID, int delta); // Adjust capacity  — O(n)
    bool cancelOrderAssignment(const char* stallID);     // Restore +1 slot  — O(n)

    // ---- Display operations ----
    void displayAll() const;       // Show all stalls in queue       — O(n)
    void displayStatus() const;    // Show stall capacity overview   — O(n)
    void displayHistory() const;   // Show assignment history log    — O(n)
};

// ---- Module Entry Point ----
void runStallModule();

#endif // STALL_ASSIGNMENT_H
