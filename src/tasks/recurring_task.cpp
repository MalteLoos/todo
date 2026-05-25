#include "recurring_task.h"
#include <iostream>
 
// CONSTRUCTOR

recurringTask::recurringTask()
    : Task(), // => superclass constructor
    recurrence(Recurrence::None),
    nextOcurrence(deadline){} 