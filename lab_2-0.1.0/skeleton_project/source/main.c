#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>

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
    while(difference != 0 && safe){
        elevio_motorDirection(direction);
        currentFloor = elevio_floorSensor();
        difference = destinationFloor - currentFloor;
        safe = okayToDrive();
    }
}

 void driveUp(int onFloor){
    MotorDirection direction = 1;
    while(onFloor == -1){
        elevio_motorDirection(direction);
        onFloor = elevio_floorSensor();
    }
    print("checking if git is setup right");
}

void startUp(){
    int onFloor = elevio_floorSensor();
    if(onFloor == -1){
        driveUp(onFloor);
    }
    elevatorReady();
}

int elevatorReady(){
    return 0;
}



int main(){
    elevio_init();

    startUp();

    


    return 0;
}  

/*returning 1 if sucsessfully opened door, 0 otherwise*/
int openDoor(){
    int floor = elevio_floorSensor();
    if(floor > 0){
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
        elevio_doorOpenLamp(0);
        return 1;
    }
}






bool safeToDrive(){
    bool door = isDoorOpen();
    bool stopButton = isStopButton();
    return door && stopButton;
}
bool isDoorOpen(){
    /*check if the door open lamp is lit*/
    bool lit = false;
    return lit;
}
bool isStopButton(){
    /*check if the stop lamp is lit*/
    bool lit = false;
    return lit;
}
int getCurrentFloor(){
    /*check floor lamp is lit*/

    return 0;
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