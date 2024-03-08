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
#include <assert.h>

// int targetFloor[4];
// int floor_index[4];
// bool foundOrder;
// int typeOfButton;
// int direction;
// int counter;
// int placement;


// Declerations
void printArray(int arr[10][2]);

int openDoor();
int closeDoor();
bool safeToDrive();
bool driveToFloor(int);
void driveUp(int onFloor);
void startUp();
int findNearestFloor(int);
void allLightsOff();
void stopProcedure();
int openDoorForStopButton();
void updateDirection();


void deleteOrder(int);
void elevatorRunning();
void searchOrders();
bool checkPassingFloors(int);
int findOrderOnFloor(int);

void completeNextOrder(int);
void bubbleSort(int , bool );

/* 
#endif // SUPPORT_H
#ifndef SUPPORT_H // Header guards to prevent multiple inclusion
#define SUPPORT_H */