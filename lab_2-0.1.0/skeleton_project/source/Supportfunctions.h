#pragma once
/*Inclusions:*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>
#include "lights.h"
#include <time.h>

int targetFloor[4];
int floor_index[4];
bool foundOrder;
int typeOfButton;
int direction;
int counter;


// Declerations
void printArray(int arr[10][2]);

int openDoor();
int closeDoor();
bool safeToDrive();
void driveToFloor(int);
void driveUp(int onFloor);
void startUp();
int findNearestFloor(int, int*);
void allLightsOff();
void stopProcedure();
int openDoorForStopButton();


void deleteOrder(int);
void elevatorRunning();
void searchOrders();
void checkPassingFloors(int, int, int);

/* 
#endif // SUPPORT_H
#ifndef SUPPORT_H // Header guards to prevent multiple inclusion
#define SUPPORT_H */