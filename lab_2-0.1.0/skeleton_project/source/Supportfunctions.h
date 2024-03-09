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
#include "OrderHandeling.h"



// Declerations
void printArray(int arr[10][2]);

void startUp();
void stopProcedure();
void updateDirection();
void resetArrays();
void elevatorRunning();


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