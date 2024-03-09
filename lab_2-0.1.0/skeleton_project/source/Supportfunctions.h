#pragma once
/*Inclusions:*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "Door.h"
#include "Drive.h"
#include "HandleOrders.h"
#include "lights.h"
#include "TakeOrders.h"

// Declerations
void elevatorRunning();
void resetArrays();
void startUp();
void stopProcedure();
void bubbleSort(int , bool );