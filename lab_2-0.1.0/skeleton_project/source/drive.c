#include "Drive.h"

extern bool isDoorOpen;
extern MotorDirection direction;
extern int previousFloor;
extern int targetFloor[4];

bool driveToFloor(int destinationFloor){
    int currentFloor = elevio_floorSensor();
    int difference = destinationFloor - previousFloor;

    elevio_motorDirection(direction);
    bool safe = safeToDrive();

    while(difference != 0 && safe){
        currentFloor = elevio_floorSensor();
        /*Updating the lights*/
        switch (currentFloor){
            case 0:
                elevio_floorIndicator(0);
                previousFloor = 0;
                break;
            case 1:
            elevio_floorIndicator(1);
            previousFloor = 1;
            break;
            case 2:
                elevio_floorIndicator(2);
                previousFloor = 2;
                break;
            case 3:
                elevio_floorIndicator(3);
                previousFloor = 3;
                break;
            default:
                break;
        }
        
        searchOrders();

        if(checkPassingFloors(previousFloor)){
            elevio_motorDirection(DIRN_STOP);
            return false;
        }
        if(currentFloor == destinationFloor){
            elevio_motorDirection(DIRN_STOP);
            break;
        }else if(direction == DIRN_UP && currentFloor == 3){
            elevio_motorDirection(DIRN_STOP);
            resetArrays();
            elevatorRunning();
            break;
        }else if(direction == DIRN_DOWN && currentFloor == 0){
            elevio_motorDirection(DIRN_STOP);
            resetArrays();
            elevatorRunning();
            break;
        }
        safe = safeToDrive();        
    }
    elevio_motorDirection(DIRN_STOP);
    return true;
}

/*checking if both the door is closed and the emergency stop button is not pressed before it is safe to drive*/
bool safeToDrive(){
    bool stopButton = elevio_stopButton();
    return !isDoorOpen && !stopButton;
}

void driveUp(){
    elevio_motorDirection(DIRN_UP);
    int onFloor = elevio_floorSensor();
    while(onFloor < 0){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(DIRN_STOP);
}
void driveDown(){
    elevio_motorDirection(DIRN_DOWN);
    int onFloor = elevio_floorSensor();
    while(onFloor < 0){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(DIRN_STOP);
}
void updateDirection(){
    if(previousFloor - targetFloor[0] < 0){
        direction = DIRN_UP;
    }else if(previousFloor - targetFloor[0] > 0){
        direction = DIRN_DOWN;
    }else{
        direction = DIRN_STOP;
    }
}