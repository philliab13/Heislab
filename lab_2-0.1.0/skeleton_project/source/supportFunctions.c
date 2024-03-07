#include "Supportfunctions.h"


extern bool isDoorOpen;
int totalOrders[10][2];



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
    printf("er i openDoor \n");

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
            //printf("kjorer \n");
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
    if(obstruction){
        while(obstruction){
            searchOrders();
            obstruction = elevio_obstruction();
        }
        printf("det er jeg som er dust \n");

        openDoor();
    }
    /*Update global variable to door is closed*/
    elevio_doorOpenLamp(0);
    isDoorOpen = false;
    return 1;
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
    elevio_floorIndicator(elevio_floorSensor());
    //elevatorReady();
}

void driveUp(int onFloor){
    elevio_motorDirection(1);
    while(onFloor < 0){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

void driveToFloor(int destinationFloor){
    /*Safety part*/
    /*the driving part*/
    int currentFloor = elevio_floorSensor();
    int previousFloor = currentFloor;
    int difference = destinationFloor - currentFloor;

    MotorDirection direction;
    if (difference > 0){
        direction = 1;
    }else if (difference < 0){
        direction = -1;
    }else{
        direction = 0;
    }
    elevio_motorDirection(direction);
    bool safe = safeToDrive();
    while (difference != 0 && safe){
        searchOrders();
        checkPassingFloors(previousFloor, 0, counter);
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
int findNearestFloor(int currentFloor, int * placement){
    int closest = 10; 
    int nextFloor;
    for (int i = 0; i < 4; ++i){
        if(targetFloor[i] != -1){
            if (abs(targetFloor[i] - currentFloor) < closest) { // Check if the value is not -1
                //check logic here
                closest = abs(targetFloor[i] - currentFloor); // Update closest if found a smaller value
                *placement = i;
                nextFloor = targetFloor[i];
            }
        }
    }
    targetFloor[*placement] = -1;
    return nextFloor;
}

void stopProcedure(){
    elevio_stopLamp(1);
    elevio_motorDirection(0);
    int floor = elevio_floorSensor();

    for(int i = 0; i < 10; ++i){
        deleteOrder(0);
    }


    if(floor >= 0){
        elevio_doorOpenLamp(1);
        nanosleep(&(struct timespec){1,0}, NULL);
        //nanosleep?
        while(true){
            if(elevio_stopButton()){
                elevio_stopLamp(0);
                break;
            }

        }
        printf("Jeg er ute av loop\n");
        openDoorForStopButton();
                printf("Jeg er etter openDoor\n");

        closeDoor();
                printf("Jeg er etter closeDoor\n");

        elevio_floorIndicator(elevio_floorSensor());
    }else{
        nanosleep(&(struct timespec){1,0}, NULL);
        while(true){
            if(elevio_stopButton()){
                elevio_stopLamp(0);
                break;
            }
        }
        driveUp(floor);
        elevio_floorIndicator(elevio_floorSensor());
    }
    printf("jeg kommer hit \n");
    nanosleep(&(struct timespec){1,0}, NULL);
    elevatorRunning();
}

void allLightsOff(){
    for (int f = 0; f < N_FLOORS; f++){
        for (int b = 0; b < N_BUTTONS; b++){
            elevio_buttonLamp(f, b, 0);
        }
    }
    elevio_stopLamp(0);
    elevio_doorOpenLamp(0);
}


/*returning 1 if sucsessfully opened door, 0 otherwise*/
int openDoorForStopButton(){
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
            //printf("kjorer \n");
            time(&end);
            if(difftime(end,start) >= 3.0){
                break;
            }
        }
        
        return 1;
    }
    else{
        return 0;
    }
}