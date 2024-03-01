#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>

// Declerations
void printArray(int arr[10][2]);

int openDoor();
int closeDoor();
bool safeToDrive();
void driveToFloor(int);
void driveUp(int onFloor);
void startUp();
int findNearestFloor(int [], int);
