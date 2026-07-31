#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include "structures.h"

struct OrderNode {
    Order* data;
    OrderNode* next;
};

class OrderQueue {
private:
    OrderNode* front;
    OrderNode* rear;  
    int count;     

    OrderNode* historyHead; 

public:
    OrderQueue();
    ~OrderQueue();


    bool isEmpty() const;
    void enqueue(Order* newOrder);
    Order* dequeue();
    Order* peek() const;
    int getQueueSize() const;


    void displayPendingOrders() const;
    void displayCompletedOrders() const;
};


void runOrderQueueModule();

#endif