#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>
#include "lights.h"
#include <time.h>


/*Global variables*/
bool isDoorOpen = false;

int totalOrders[10][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};



/*Declaring functions so the layout does not matter*/
bool safeToDrive();
int elevatorReady();
void driveToFloor(int);
void allFloorLightsOff();
void executeOrder();
void stopAtFloor(int);
void addOrder(int, int);
int openDoor();
int closeDoor();
void searchOrders();
void deleteOrder(int indexInArray);
void checkPassingFloors(int[] , int , int , int[]);
int findNearestFloor(int [], int);
int findOrder(int);
void printArray(int [][]);


/*end of declerations*/

typedef struct
{
    int location;
    int direction;
} elevatorOrder;

int elevatorReady(){
    while (true)    {
        executeOrder();
    }
    return 0;
}

/*Scans all the buttons to see if any of them are pressed in that instance, if yes call addOrder*/
void searchOrders(){
    /*f iterates the different floors 0-3, b iterates the different buttontypes on that floor*/
    for (int f = 0; f < N_FLOORS; f++){
        for (int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            /*add order*/
            if (btnPressed){
                addOrder(f, b);
            }
        }
    }
}


/*Add the order detected from searchOrders to the global array totalOrders*/
/*Need failsafe for identical orders*/

void addOrder(floor, button){
    /*First the function checks if the order already exist and ignores it if it does*/
    for(int i = 0; i < 10; ++i){
        if(totalOrders[i][0] == floor && totalOrders[i][1] == button){
            return;
        }
    }
    /*Looking for the first empty spot in the array to insert the new order*/
    for (int i = 0; i < 10; i++) {
        if(totalOrders[i][0] == -1){
            totalOrders[i][0] = floor;
            totalOrders[i][1] = button;
        /*Sets the button lamp on for the order*/
            elevio_buttonLamp(floor, button, 1);
            break;
        }
    }
}

/*Denne skal slette elementet på index i arrayen og flytte alt bak et hakk mot venstre  */

void deleteOrder(int indexInArray){
    /*
    for (int i = 0; i < 2; i++)    {

        totalOrders[indexInArray][i] = -1;
        elevio_floorIndicator(indexInArray);
    }
    */
    elevio_buttonLamp(totalOrders[indexInArray][0], totalOrders[indexInArray][1], 0);
    for (int i = indexInArray; i < 9; i++){
        totalOrders[i][0] = totalOrders[i + 1][0];
        totalOrders[i][1] = totalOrders[i + 1][1];
    }
    totalOrders[9][0] = -1;
    totalOrders[9][1] = -1;
}

/*This function checks the totalOrder array and checks if there are any orders going the same direction on passing floors*/
/*If yes, updates the arrays targetFloor with what floor the order is on and index for location in totalOrder*/
/*defyning that going up equals a positive number for direction*/

void checkPassingFloors(int targetFloor[], int currentFloor, int typeOfButton, int index[]){
    int direction = targetFloor[0] - currentFloor;
    int counter = 1;
    if(direction > 1){
        for (int i = 0; i < 10; i++){
            if((totalOrders[i][1] == 0) && ((totalOrders[i][0]==currentFloor) || (totalOrders[i][0]<targetFloor[i]))){
                targetFloor[counter] = totalOrders[i][0];
                ++counter;
                index[counter] = i;
            }
        }
    }
    else if (direction <= -1){
        for (int i = 0; i < 10; i++){
            /*Need to check the logic for this if-test*/
            if((totalOrders[i][1] == 1) && ((totalOrders[i][0]==currentFloor) || (totalOrders[i][0]>targetFloor[i]))){
                targetFloor[counter] = totalOrders[i][0];
                ++counter;
                index[counter] = i;
            }
        }
    }
}

/*This function is for finding the right floor to drive the elevator to first*/
/*This was created by chatGPT -> remember for report*/

int findNearestFloor(int targetFloors[], int currentFloor){
    int placement;
    int closest = 10; 
    int nextFloor;
    for (int i = 0; i < 3; ++i){
        if(targetFloors[i] != -1){
            if (abs(targetFloors[i] - currentFloor) < closest) { // Check if the value is not -1
                closest = targetFloors[i] - currentFloor; // Update closest if found a smaller value
                placement = i;
                nextFloor = targetFloors[i];
            }
        }
    }
    targetFloors[placement] = -1;
    return nextFloor;
}
/*TODO: create findNearestFloor*/


/*Find if there happens to be an order for the floor we are stopping at*/
int findOrder(int floor){
    for (int i = 0; i < 10; ++i){
        if ((totalOrders[i][0] != floor) && (totalOrders[i][1] == 2)){
            return i;
        }
    }
    return -1;
}


void printArray(int arr[10][2]){
    int rows = 10;
    int cols = 2;
    for(int i = 0; i < rows; i++) {
        printf("[");
        for(int j = 0; j < cols; j++) {
            printf("%d", arr[i][j]);
            if(j != cols - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }
}


void executeOrder(){
    int targetFloor[3]={-1,-1,-1};
    int typeOfButton;
    int index[3] = {-1, -1, -1};
    int currentFloor = elevio_floorSensor();
    bool foundOrder = false;
    printArray(totalOrders);
/*Iterates through the order array and picks an order to execute.*/
/*In this loop we are looking for orders from inside the elevator, these are prioritized*/
/*Sets foundOder = true so that we don't look for more orders once one is found*/
    elevio_motorDirection(0);
    for (int i = 0; i < 10; ++i) {
        if((totalOrders[i][0] != -1) && (totalOrders[i][1] == 2)){
            targetFloor[0] = totalOrders[i][0];
            typeOfButton = totalOrders[i][1];
            index[0] = i;
            foundOrder = true;
            break;
        }
    }
    /*This section is for handling orders when they come from inside the cab*/
    if (foundOrder){
        if (currentFloor == targetFloor[0]){
            openDoor();
            elevio_doorOpenLamp(1);
            deleteOrder(index[0]);
            closeDoor();
        }
        checkPassingFloors(targetFloor, currentFloor, typeOfButton, index);
        /*We should now have an main order to execute and all the floors worth stopping by in the array targetFloor*/
        /*The elevator can now drive to the target floors, when it has stopped by all of them the order is completed*/
        for (int i = 0; i < 3; ++i){
            if (targetFloor[i] != -1){
                closeDoor();
                elevio_doorOpenLamp(0);
                driveToFloor(findNearestFloor(targetFloor, currentFloor));
                openDoor();
                //deleteOrder(index[i]);
                int passBy = findOrder(currentFloor);
                if (passBy != -1){
                    deleteOrder(passBy);
                }
                closeDoor();
            }
        }
    }

    /*this section is for handling orders from outside the cab*/
    else{
        /*Iterates through the order array and picks an order to execute.*/
        /*This loop should only be able to pick orders from outside the cab*/
        for (int i = 0; i < 10; i++){
            if (totalOrders[i][0] != -1){
                targetFloor[0] = totalOrders[i][0];
                typeOfButton = totalOrders[i][1];
                index[0] = i;
                foundOrder = true;
                break;
            }
        }

        if (foundOrder){
            closeDoor();
            driveToFloor(targetFloor[0]);
            openDoor();
            deleteOrder(index[0]);
            closeDoor();
        }
    }
}

int main(){
    elevio_init();
   /*  startUp(); */
    while (true) {
        searchOrders();
        executeOrder();

        /* if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        } */
    }

    return 0;
}

void driveUp(int onFloor){
    MotorDirection direction = 1;
    elevio_motorDirection(direction);
    while (onFloor == -1){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

void startUp(){
    int onFloor = elevio_floorSensor();
    if (onFloor == -1){
        driveUp(onFloor);
    }
    elevatorReady();
}

void driveToFloor(int destinationFloor){
    /*Safety part*/
    /*the driving part*/
    int currentFloor = elevio_floorSensor();
    int difference = destinationFloor - currentFloor;
    MotorDirection direction;
    if (difference > 0){
        direction = 1;
    }
    else if (difference < 0){
        direction = -1;
    }
    else{
        direction = 0;
    }
    elevio_motorDirection(direction);
    bool safe = safeToDrive();
    while (difference != 0 && safe){
        searchOrders();
        currentFloor = elevio_floorSensor();
        if (currentFloor == destinationFloor){
            // difference = destinationFloor - currentFloor;
            elevio_motorDirection(0);
            break;
        }
        safe = safeToDrive();

        /*Updating the lights*/
        switch (currentFloor){
            case 0:
                elevio_floorIndicator(0);
                break;
            case 1:
            elevio_floorIndicator(1);
            break;
            case 2:
                elevio_floorIndicator(2);
                break;
            case 3:
                elevio_floorIndicator(3);
                break;
            default:
                break;
        }
    }
    elevio_motorDirection(0);
    switch (currentFloor){
        case 0:
            elevio_floorIndicator(0);
            break;
        case 1:
        elevio_floorIndicator(1);
        break;
        case 2:
            elevio_floorIndicator(2);
            break;
        case 3:
            elevio_floorIndicator(3);
            break;
        default:
            break;
        }
}

void allFloorLightsOff(){
    for (int i = 0; i < N_FLOORS; ++i){
        elevio_floorIndicator(i);
    }
}

/*returning 1 if sucsessfully opened door, 0 otherwise*/
int openDoor(){
    int floor = elevio_floorSensor();
    time_t start, end;
    /*checking if it safe to open door*/
    if (floor >= 0){
        /*Update global variable to door is open*/
        isDoorOpen = true;
        /*set open door light on*/
        elevio_doorOpenLamp(1);
        /*Keeps the door open for 3 seconds, and searches for orders at the same time*/
        /*This is created by chat-gpt -> for report*/
        time(&start);
        while(true){
            printf("kjorer \n");
            time(&end);
            if(difftime(end,start) >= 3.0){
                break;
            }
            searchOrders();
        }
        
        return 1;
    }
    else{
        return 0;
    }
}
/*returning 1 if sucsessfully closed door, 0 otherwise*/
int closeDoor(){
    /*check if there is an obstruction*/
    bool obstruction = elevio_obstruction();
    if (obstruction){
        return 0;
    }
    else{
        /*Update global variable to door is closed*/
        isDoorOpen = false;
        elevio_doorOpenLamp(0);
        return 1;
    }
}

/*checking if both the door is closed and the emergency stop button is not pressed before it is safe to drive*/
bool safeToDrive(){
    bool stopButton = elevio_stopButton();
    return !isDoorOpen && !stopButton;
}




/*
int main(){
    elevio_init();

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    elevio_motorDirection(DIRN_UP);

    while(1){
        int floor = elevio_floorSensor();

        if(floor == 0){
            elevio_motorDirection(DIRN_UP);
        }

        if(floor == N_FLOORS-1){
            elevio_motorDirection(DIRN_DOWN);
        }


        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                int btnPressed = elevio_callButton(f, b);
                elevio_buttonLamp(f, b, btnPressed);
            }
        }

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }

        if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        }

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}

 */