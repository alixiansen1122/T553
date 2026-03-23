#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stdbool.h"
//////////////////////////////////////////
//                                      //
//  Time Measurement Helper Functions   // 
//                                      //
//////////////////////////////////////////



typedef enum CalculationError_T {
    INSUFFICIENTDATA = -2,
    UNINITIALIZED = -3,
    INITIALIZATIONFAILURE = -4,
    DISCONTINUOUSCNT = -5
} CalculationError_T;


#endif
