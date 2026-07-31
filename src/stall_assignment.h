#ifndef STALL_ASSIGNMENT_H
#define STALL_ASSIGNMENT_H

#include "structures.h"

class CircularQueue {
private:
    // Circular queue internals 
    Stall* queue[MAX_STALLS];   
    int front;                  
    int rear;                 
    int count;                  

    //  Round-robin pointer 
    int assignPtr;

    // Assignment history log 
    AssignmentRecord history[MAX_STALL_HISTORY];
    int historyCount;

public:
    // Constructor
    CircularQueue();

    // Core circular queue operations 
    bool   enqueue(Stall* stall);         // Add stall to rear  
    Stall* dequeue();                     // Remove from front    
    Stall* peek() const;                  // View front element 
    bool   isFull() const;                // Check if queue full  
    bool  isEmpty() const;                // Check if queue empty
    int   getSize() const;                // Get element count   

    // Round-robin stall assignment 
    // Finds the next open, non-full stall and assigns the order.
    // Skips closed stalls and stalls at full capacity.
    // Returns true if assignment succeeded, false if all stalls unavailable.
    bool assignOrder(Order& order);

    // Stall management
    int  findStall(const char* stallID) const;          // Find stall index 
    bool updateCapacity(const char* stallID, int delta); // Adjust capacity 
    bool cancelOrderAssignment(const char* stallID);     // Restore +1 slot  

    // Display operations 
    void displayAll() const;       // Show all stalls in queue     
    void displayStatus() const;    // Show stall capacity overview 
    void displayHistory() const;   // Show assignment history log    
};

// Module Entry Point
void runStallModule();

#endif 
