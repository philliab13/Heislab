#include <Supportfunctions.h>
extern bool isDoorOpen = false;



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

void startUp(){
    int onFloor = elevio_floorSensor();
    if (onFloor == -1){
        driveUp(onFloor);
    }
    elevatorReady();
}

void driveUp(int onFloor){
    MotorDirection direction = 1;
    elevio_motorDirection(direction);
    while (onFloor == -1){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
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


/*This function is for finding the right floor to drive the elevator to first*/
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




/*void allFloorLightsOff();
void allFloorLightsOff(){
    for (int i = 0; i < N_FLOORS; ++i){
        elevio_floorIndicator(i);
    }
}*/