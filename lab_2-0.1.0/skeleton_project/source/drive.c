#include "Drive.h"

extern bool isDoorOpen;
extern int direction;
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
            elevio_motorDirection(0);
            return false;
        }
        if(currentFloor == destinationFloor){
            elevio_motorDirection(0);
            break;
        }else if(direction == 1 && currentFloor == 3){
            elevio_motorDirection(0);
            resetArrays();
            elevatorRunning();
            break;
        }else if(direction == -1 && currentFloor == 0){
            elevio_motorDirection(0);
            resetArrays();
            elevatorRunning();
            break;
        }
        safe = safeToDrive();        
    }
    elevio_motorDirection(0);
    return true;
}

/*checking if both the door is closed and the emergency stop button is not pressed before it is safe to drive*/
bool safeToDrive(){
    bool stopButton = elevio_stopButton();
    return !isDoorOpen && !stopButton;
}

void driveUp(){
    elevio_motorDirection(1);
    int onFloor = elevio_floorSensor();
    while(onFloor < 0){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

void updateDirection(){
    if(previousFloor - targetFloor[0] < 0){
        direction = 1;
    }else if(previousFloor - targetFloor[0] > 0){
        direction = -1;
    }else{
        direction = 0;
    }
}