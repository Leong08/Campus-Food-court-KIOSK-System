========================================================================
           CAMPUS FOOD COURT SELF-ORDER KIOSK SYSTEM (PART 2)
                 DATASET DICTIONARY & RELATIONSHIP MANUAL
========================================================================
Directory Target : /datasets/
Serves As        : Flat-File Database Dictionary & Validation Rules
Module Code      : CT077-3-2-DSTR (Data Structures)
========================================================================

------------------------------------------------------------------------
1. DATASET SCHEMA & VARIABLE DEFINITIONS
------------------------------------------------------------------------
All fields inside the CSV files must follow these explicit string formatting
and numeric boundaries during parsing and streaming loops:

1. Student Registry Database (students.csv)
   - studentID     : std::string (Primary Key, alphanumeric format: "TPXXXXXX")
   - fullName      : std::string (Alphabetical string for student name validation)
   - contactNumber : std::string (Hyphenated string format, e.g., "01X-XXXXXXX")
   - accBalance    : double      (Kiosk digital wallet balance; must be >= 0.00)
   - status        : std::string (Active, Suspended, Graduated)

2. Stall Registry Database (stalls.csv)
   - stallID            : std::string (Primary Key, unique identifier: "STALL_XX")
   - stallName          : std::string (Descriptive string vendor name)
   - cuisineType        : std::string (Cuisine class: Chinese, Indian, Malay, Western)
   - opening            : bool        (1 = Open/Accepting Orders, 0 = Closed/Resting)
   - maxCapacity        : int         (Workload throttle index. Initial baseline = 15.
                                       Decrements by 1 per individual food item ordered. 
                                       System locks out new entries when value hits 0)
   - currentQueueLength : int         (Total active food items currently processing by stall. 
                                       Maintains an inverted linear relationship with maxCapacity: 
                                       [currentQueueLength = 15 - maxCapacity])

3. Menu Items Master Database (menu_items.csv)
   - itemID       : std::string (Primary Key, unique item flag: "ITEM_XXX")
   - itemName     : std::string (Descriptive name of the food item)
   - category     : std::string (Food, Beverage, Dessert)
   - availability : bool        (1 = In Stock, 0 = Out of Stock)
   - prepTime     : int         (Suggested preparation duration in minutes for customer wait)
   - stallID      : std::string (Foreign Key referencing stalls.csv)
   - price        : double      (Monetary value cost; must be > 0.00)

4. Transaction Processing Registry (orders.csv)
   - orderID       : std::string (Primary Key, unique tracking token: "ORD_XXXXX")
   - timeStamp     : long        (Unix epoch timestamp tracking transaction time)
   - studentID     : std::string (Foreign Key referencing students.csv)
   - stallID       : std::string (Foreign Key referencing stalls.csv)
   - itemID        : std::string (Foreign Key referencing menu_items.csv)
   - quantity      : int         (Item quantity count within order; must be >= 1)
   - totalPrice    : double      (Derived total field calculated as: [quantity * menu_item.price])
   - paymentStatus : std::string (PAID, REFUNDED, FAILED)
   - priorityFlag  : int         (Scheduling modifier code: 1 = Priority/Express, 0 = Normal)
   - pickupTime    : long        (Unix epoch timestamp target for food collection alerts)
   - order_status  : std::string (PENDING, PREPARING, READY, FULFILLED, CANCELLED)

------------------------------------------------------------------------
2. INTEGRITY & CONDITIONAL DATA VALIDATION CHECKS
------------------------------------------------------------------------
To qualify for an 'Excellent' mark under technical proficiency and error trapping, 
the global `filehandler.cpp` wrapper must implement code traps checking these rules:

- Financial Sufficiency: Kiosk transactions must verify that the target student's 
  `accBalance` >= the transaction `totalPrice` prior to instantiating an order.
- Capacity Boundaries: An order entry sequence targeting a matching `stallID` must 
  evaluate `maxCapacity`. If `maxCapacity` == 0, the kiosk interface must instantly 
  drop the order and alert the user with a workload congestion warning.
- Symmetrical Synchronization: Runtimes must synchronize state updates symmetrically. 
  An in-flight order element successfully joining a stall's queue structure must cause 
  `maxCapacity--` and `currentQueueLength++` across internal memory layouts simultaneously.
- Cancellation and Recovery Wipes: If an order's `order_status` transitions to `"CANCELLED"`, 
  the system must roll back state variables by processing a monetary refund to the student's 
  `accBalance` and restoring the stall's workload slots via `maxCapacity++` and `currentQueueLength--`.
- Malformed Line Drops: Any row failing regex pattern matching on formatting rules 
  or featuring data anomalies (e.g., negative prices, zero quantities) must be logged 
  as an invalid skip and ignored to avoid heap pointer segmentation memory faults.

------------------------------------------------------------------------
3. CROSS-MEMBER STRUCTURAL COUPLING BLUEPRINT
------------------------------------------------------------------------
These dataset attributes serve as the unified pipeline binding all compulsory 
individual structures into a singular program loop:

- Singly Linked List Queue (Role 1): Consumes `orders.csv` records marked as "PENDING"
  and sequences them in fair chronological time slots.
- Array or List Stack (Role 2): Leverages state history records tracing kiosk transition
  flows and changes to `quantity` inputs within a session for undo/backtrack functions.
- Binary Search Tree (Role 3): Indexes `menu_items.csv` records by `price` or `itemID` 
  to run rapid logarithmic binary node searches or extract sorted inventory lists.
- Tree/Graph Layout (Role 3/4): Implements a hierarchical parent-child structural model 
  or adjacency matrix mapping navigation paths from floor layouts down to individual stall tokens.
========================================================================