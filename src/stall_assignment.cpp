// ============================================================
// stall_assignment.cpp — Circular Queue Implementation
// Task 2: Stall Assignment Module
// By: Sherwin A/L Jesudass (TP075823)
// CT077-3-2-DSTR | Lab Work #2
// ============================================================
// This file implements a Circular Queue from scratch (no STL).
//
// WHY CIRCULAR QUEUE?
// The stall assignment follows a round-robin pattern: orders
// rotate through stalls 1 → 2 → 3 → 4 → 1 → 2 → ...
// A circular queue naturally wraps around using modulo
// arithmetic:  nextIndex = (current + 1) % count
// This avoids resetting the pointer and ensures fair, balanced
// distribution of orders across all stalls.
//
// CIRCULAR QUEUE DIAGRAM:
//   ┌──────┬──────┬──────┬──────┐
//   │ ST01 │ ST02 │ ST03 │ ST04 │  ← fixed-size array
//   └──────┴──────┴──────┴──────┘
//      ↑                    ↑
//    front                 rear
//           ↑
//        assignPtr (rotates after each assignment)
//
// When assignPtr reaches the end, it wraps to index 0.
// ============================================================

#include "stall_assignment.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

// ---- Constructor ----
CircularQueue::CircularQueue() {
    front        = 0;
    rear         = -1;
    count        = 0;
    assignPtr    = 0;
    historyCount = 0;
}

// ============================================================
// CORE CIRCULAR QUEUE OPERATIONS
// ============================================================

// Enqueue — Add a stall to the rear of the circular queue.
// Time: O(1) | Space: O(1)
bool CircularQueue::enqueue(const Stall& stall) {
    if (isFull()) {
        cout << "  [ERROR] Circular Queue is full. Cannot add stall." << endl;
        return false;
    }

    // Wrap rear index around using modulo (circular behaviour)
    rear = (rear + 1) % MAX_STALLS;
    queue[rear] = stall;
    count++;
    return true;
}

// Dequeue — Remove and return the stall at the front.
// Time: O(1) | Space: O(1)
Stall CircularQueue::dequeue() {
    Stall empty;
    memset(&empty, 0, sizeof(Stall));

    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty. Cannot dequeue." << endl;
        return empty;
    }

    Stall removed = queue[front];

    // Advance front index with wrapping
    front = (front + 1) % MAX_STALLS;
    count--;

    // Keep assignPtr within valid range
    if (count > 0) {
        assignPtr = assignPtr % count;
    } else {
        assignPtr = 0;
    }

    return removed;
}

// Peek — View the stall at the front without removing it.
// Time: O(1) | Space: O(1)
Stall CircularQueue::peek() const {
    Stall empty;
    memset(&empty, 0, sizeof(Stall));

    if (isEmpty()) {
        cout << "  [ERROR] Circular Queue is empty." << endl;
        return empty;
    }
    return queue[front];
}

// Time: O(1)
bool CircularQueue::isFull() const {
    return count == MAX_STALLS;
}

// Time: O(1)
bool CircularQueue::isEmpty() const {
    return count == 0;
}

// Time: O(1)
int CircularQueue::getSize() const {
    return count;
}

// ============================================================
// ROUND-ROBIN STALL ASSIGNMENT
// ============================================================

// assignOrder — The core function of this module.
//
// Uses a rotating pointer (assignPtr) to distribute orders
// fairly across all stalls. The pointer advances after each
// assignment, wrapping back to the start (circular behaviour).
//
// LOGIC:
//   1. Start from the current assignPtr position
//   2. Check if the stall is OPEN and has CAPACITY
//   3. If yes → assign order, update capacity, advance pointer
//   4. If no  → skip to next stall, repeat
//   5. If all stalls checked → system overload error
//
// Time: O(n) worst case (all stalls full, must check every one)
// Space: O(1)
bool CircularQueue::assignOrder(Order& order) {
    if (isEmpty()) {
        cout << "  [ERROR] No stalls in queue. Load stalls first." << endl;
        return false;
    }

    int stallsChecked = 0;
    int startPtr = assignPtr;  // Remember start in case of failure

    cout << "\n  [Round-Robin Assignment]" << endl;
    cout << "  Starting from position " << assignPtr << endl;

    while (stallsChecked < count) {
        // Calculate the actual array index using circular wrapping
        int actualIdx = (front + assignPtr) % MAX_STALLS;
        Stall& stall  = queue[actualIdx];

        cout << "  -> Checking " << stall.stallName
             << " (" << stall.stallID << "): ";

        // --- Skip closed stalls ---
        if (!stall.opening) {
            cout << "CLOSED - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        // --- Skip full stalls (capacity boundary check) ---
        if (stall.maxCapacity <= 0) {
            cout << "FULL (0 capacity) - Skipping" << endl;
            assignPtr = (assignPtr + 1) % count;
            stallsChecked++;
            continue;
        }

        // --- Stall is available — assign the order ---
        cout << "AVAILABLE (capacity: " << stall.maxCapacity
             << ") - ASSIGNED!" << endl;

        // Set the order's assigned stall
        strcpy(order.stallID, stall.stallID);

        // Symmetrical synchronization:
        // maxCapacity-- and currentQueueLength++ must happen together
        stall.maxCapacity--;
        stall.currentQueueLength++;

        // Record this assignment in the history log
        if (historyCount < MAX_HISTORY) {
            strcpy(history[historyCount].orderID, order.orderID);
            strcpy(history[historyCount].stallID, stall.stallID);
            strcpy(history[historyCount].stallName, stall.stallName);
            history[historyCount].timestamp = order.timeStamp;
            historyCount++;
        }

        cout << "\n  Result: Order " << order.orderID
             << " -> " << stall.stallName << endl;
        cout << "  Updated: Capacity=" << stall.maxCapacity
             << " | Queue Length=" << stall.currentQueueLength << endl;

        // Advance pointer for the NEXT call (round-robin rotation)
        assignPtr = (assignPtr + 1) % count;

        return true;
    }

    // All stalls were either closed or at full capacity
    cout << "\n  [WARNING] System overload! All stalls are closed or at"
         << " full capacity." << endl;
    cout << "  Order " << order.orderID << " could not be assigned." << endl;
    assignPtr = startPtr;  // Reset pointer since no assignment was made
    return false;
}

// ============================================================
// STALL MANAGEMENT
// ============================================================

// Find a stall's array index by its ID.
// Returns the actual queue array index, or -1 if not found.
// Time: O(n)
int CircularQueue::findStall(const char* stallID) const {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        if (strcmp(queue[idx].stallID, stallID) == 0) {
            return idx;
        }
    }
    return -1;
}

// Update a stall's capacity by a delta value.
// delta = -1 when an order is assigned (decrease capacity)
// delta = +1 when an order is cancelled (restore capacity)
// Time: O(n)
bool CircularQueue::updateCapacity(const char* stallID, int delta) {
    int idx = findStall(stallID);
    if (idx == -1) {
        cout << "  [ERROR] Stall " << stallID << " not found in queue." << endl;
        return false;
    }

    queue[idx].maxCapacity       += delta;
    queue[idx].currentQueueLength -= delta;
    return true;
}

// Cancel an order's stall assignment — restore one capacity slot.
// This implements the "Cancellation and Recovery Wipes" rule
// from the dataset dictionary.
// Time: O(n)
bool CircularQueue::cancelOrderAssignment(const char* stallID) {
    return updateCapacity(stallID, 1);  // Restore +1 slot
}

// ============================================================
// DISPLAY OPERATIONS
// ============================================================

// Display all stalls currently in the circular queue.
// Shows the internal queue state (front, rear, assignPtr).
// Time: O(n)
void CircularQueue::displayAll() const {
    if (isEmpty()) {
        cout << "\n  [INFO] Circular Queue is empty. No stalls loaded." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "           STALLS IN CIRCULAR QUEUE                    " << endl;
    cout << "  ====================================================" << endl;
    cout << "  " << left
         << setw(4)  << "Pos"
         << setw(12) << "Stall ID"
         << setw(18) << "Name"
         << setw(10) << "Cuisine"
         << setw(8)  << "Open"
         << setw(10) << "Capacity"
         << setw(10) << "QueueLen"
         << endl;
    cout << "  ----------------------------------------------------" << endl;

    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        const Stall& s = queue[idx];

        cout << "  " << left
             << setw(4)  << i
             << setw(12) << s.stallID
             << setw(18) << s.stallName
             << setw(10) << s.cuisineType
             << setw(8)  << (s.opening ? "Yes" : "No")
             << setw(10) << s.maxCapacity
             << setw(10) << s.currentQueueLength;

        // Mark the current assignment pointer position
        if (i == assignPtr) {
            cout << " <-- NEXT";
        }
        cout << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Queue Size: " << count
         << " | Front: " << front
         << " | Rear: " << rear
         << " | Assign Ptr: " << assignPtr << endl;
    cout << "  ====================================================" << endl;
}

// Display detailed stall status with visual capacity bars.
// Time: O(n)
void CircularQueue::displayStatus() const {
    if (isEmpty()) {
        cout << "\n  [INFO] No stalls loaded." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "              STALL STATUS OVERVIEW                    " << endl;
    cout << "  ====================================================" << endl;

    int totalCapacity = 0;
    int totalUsed     = 0;

    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_STALLS;
        const Stall& s = queue[idx];

        cout << "\n  " << s.stallName << " (" << s.stallID << ")" << endl;
        cout << "    Status:    " << (s.opening ? "OPEN" : "CLOSED") << endl;
        cout << "    Cuisine:   " << s.cuisineType << endl;
        cout << "    Capacity:  " << s.maxCapacity << " remaining / 15 total" << endl;
        cout << "    In Queue:  " << s.currentQueueLength << " orders" << endl;

        // Visual capacity bar [####------] style
        cout << "    Load:      [";
        int maxSlots = 15;
        int filled   = s.currentQueueLength;
        if (filled > maxSlots) filled = maxSlots;

        for (int j = 0; j < maxSlots; j++) {
            if (j < filled) cout << "#";
            else            cout << "-";
        }
        cout << "] " << s.currentQueueLength << "/" << maxSlots << endl;

        totalCapacity += 15;
        totalUsed     += s.currentQueueLength;
    }

    cout << "\n  ----------------------------------------------------" << endl;
    cout << "  Overall Load: " << totalUsed << "/" << totalCapacity
         << " (" << (totalCapacity > 0 ? (totalUsed * 100 / totalCapacity) : 0)
         << "%)" << endl;
    cout << "  ====================================================" << endl;
}

// Display the assignment history log.
// Shows every order-to-stall assignment made during this session.
// Time: O(n)
void CircularQueue::displayHistory() const {
    if (historyCount == 0) {
        cout << "\n  [INFO] No assignments recorded yet." << endl;
        return;
    }

    cout << "\n  ====================================================" << endl;
    cout << "            ASSIGNMENT HISTORY LOG                     " << endl;
    cout << "  ====================================================" << endl;
    cout << "  " << left
         << setw(5)  << "#"
         << setw(15) << "Order ID"
         << setw(12) << "Stall ID"
         << setw(20) << "Stall Name" << endl;
    cout << "  ----------------------------------------------------" << endl;

    for (int i = 0; i < historyCount; i++) {
        cout << "  " << left
             << setw(5)  << (i + 1)
             << setw(15) << history[i].orderID
             << setw(12) << history[i].stallID
             << setw(20) << history[i].stallName << endl;
    }

    cout << "  ====================================================" << endl;
    cout << "  Total Assignments: " << historyCount << endl;
    cout << "  ====================================================" << endl;
}
