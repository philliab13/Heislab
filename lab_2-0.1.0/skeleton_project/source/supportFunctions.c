#include "Supportfunctions.h"


 extern bool isDoorOpen;

 extern int  totalOrders[10][2];
 extern int targetFloor[4];
 extern int floor_index[4];
 extern bool foundOrder;
 extern int direction;
 //extern int counter;
 extern int placement;
 extern int previousFloor;


void resetArrays(){
    for(int i = 0; i < 4; ++i){
        targetFloor[i] = -1;
        floor_index[i] = -1;
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
            time(&end);
            if(difftime(end,start) >= 3.0){
                break;
            }
            searchOrders();
        }
        return 1;
    }else{
        return 0;
    }
}

int closeDoor(){
    /*check if there is an obstruction*/
    bool obstruction = elevio_obstruction();
    if(obstruction){
        while(obstruction){
            searchOrders();
            obstruction = elevio_obstruction();
        }
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
        driveUp();
    }
    elevio_floorIndicator(elevio_floorSensor());
    previousFloor = elevio_floorSensor();
}

void driveUp(){
    elevio_motorDirection(1);
    int onFloor = elevio_floorSensor();
    while(onFloor < 0){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

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


void stopProcedure(){
    elevio_stopLamp(1);
    elevio_motorDirection(0);
    int floor = elevio_floorSensor();

    for(int i = 0; i < 10; ++i){
        deleteOrder(0);
    }

    if(floor >= 0){
        elevio_doorOpenLamp(1);
        /*nanosleep so the program does not confuse a slow button press for two*/
        nanosleep(&(struct timespec){1,0}, NULL);
        while(true){
            if(elevio_stopButton()){
                elevio_stopLamp(0);
                break;
            }

        }
        openDoorForStopButton();
        closeDoor();
        elevio_floorIndicator(elevio_floorSensor());
    }else{
        nanosleep(&(struct timespec){1,0}, NULL);
        while(true){
            if(elevio_stopButton()){
                elevio_stopLamp(0);
                break;
            }
        }
        driveUp();
        elevio_floorIndicator(elevio_floorSensor());
    }
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
            time(&end);
            if(difftime(end,start) >= 3.0){
                break;
            }
        }
        return 1;
    }else{
        return 0;
    }
}

/*written by chat-gpt  --> for report*/
void bubbleSort(int size, bool ascending) {
    // Loop through each element of the array
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            // Determine if a swap should occur
            bool shouldSwap = false;

            if (ascending) {
                // For ascending order, move -1 to the end and sort the rest
                if ((targetFloor[j] == -1 && targetFloor[j + 1] != -1) || 
                    (targetFloor[j] != -1 && targetFloor[j + 1] != -1 && targetFloor[j] > targetFloor[j + 1])) {
                    shouldSwap = true;
                }
            } else {
                // For descending order, ensure -1 stays at the end and sort the rest
                if ((targetFloor[j] == -1 && targetFloor[j + 1] == -1) || 
                    (targetFloor[j + 1] != -1 && targetFloor[j] < targetFloor[j + 1])) {
                    shouldSwap = true;
                }
            }

            // Perform the swap if necessary
            if (shouldSwap) {
                int temp = targetFloor[j];
                targetFloor[j] = targetFloor[j + 1];
                targetFloor[j + 1] = temp;

                // Assuming you want to swap the floor_index values in parallel to maintain linkage
                // Uncomment the following lines if floor_index swapping is needed
                temp = floor_index[j];
                floor_index[j] = floor_index[j + 1];
                floor_index[j + 1] = temp;
            }
        }
    }
}

