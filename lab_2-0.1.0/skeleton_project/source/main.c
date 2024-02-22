#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>
/*Global variables*/
bool isDoorOpen = false;


/*Declaring functions so the layout does not matter*/
bool safeToDrive();
int elevatorReady();
void driveToFLoor(int destinationFloor);
/*end of declerations*/




 void driveUp(int onFloor){
    MotorDirection direction = 1;
    elevio_motorDirection(direction);
    while(onFloor == -1){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

void startUp(){
    int onFloor = elevio_floorSensor();
    if(onFloor == -1){
        driveUp(onFloor);
    }
    elevatorReady();
}

int elevatorReady(){
    ButtonType button = 0;
    elevio_buttonLamp(2, button, 1);
    return 0;
}



int main(){
    elevio_init();

    startUp();

    while (true) {}
    return 0;
} 


void driveToFLoor(int destinationFloor){
    /*Safety part*/

    /*the driving part*/
    int currentFloor = elevio_floorSensor();
    int difference = destinationFloor - currentFloor;
    MotorDirection direction;
    if(difference > 0){
        direction = 1;
    }else{
        direction = -1;
    }
    bool safe = safeToDrive();
    elevio_motorDirection(direction);
    while(difference != 0 && safe){
        
        currentFloor = elevio_floorSensor();
        difference = destinationFloor - currentFloor;
        safe = safeToDrive();
    }
    elevio_motorDirection(0);
}


/*returning 1 if sucsessfully opened door, 0 otherwise*/
int openDoor(){
    int floor = elevio_floorSensor();
    /*checking if it safe to open door*/
    if(floor > 0){
        /*Update global variable to door is open*/
        isDoorOpen = true;
        /*set open door light on*/
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
        sleep(3);
        return 1;
    }else{
        return 0;
    }
}
/*returning 1 if sucsessfully opened door, 0 otherwise*/
int closeDoor(){
    /*check if there is an obstruction*/
    bool obstruction = elevio_obstruction();
    if(obstruction){
        return 0;
    }else{
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