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
#include "HandleOrders.h"
#include "lights.h"
#include "OrderHandeling.h"
#include "Supportfunctions.h"

bool driveToFloor(int);
bool safeToDrive();
void driveUp();
void updateDirection();
