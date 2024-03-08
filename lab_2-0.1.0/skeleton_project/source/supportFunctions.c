#include "Supportfunctions.h"


 extern bool isDoorOpen;

 extern int  totalOrders[10][2];
 extern int targetFloor[4];
 extern int floor_index[4];
 extern bool foundOrder;
 extern int direction;
 //extern int counter;
 extern int placement;



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

bool driveToFloor(int destinationFloor){
    printf("Jeg kommer til driveToFloor \n");
    /*Safety part*/
    /*the driving part*/
    int currentFloor = elevio_floorSensor();
    int previousFloor = currentFloor;
    int difference = destinationFloor - currentFloor;

    elevio_motorDirection(direction);
    bool safe = safeToDrive();

    while (difference != 0 && safe){
        currentFloor = elevio_floorSensor();
        searchOrders();
        if(checkPassingFloors(previousFloor)){
            elevio_motorDirection(0);
            printf("Jeg returnerer fra driveToFloor \n");
            return false;
        }
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
    return true;
}

/*This function is for finding the right floor to drive the elevator to first*/
int findNearestFloor(int currentFloor){
    int closest = 10; 
    int nextFloor;
    for (int i = 0; i < 4; ++i){
        if(targetFloor[i] != -1){
            if (abs(targetFloor[i] - currentFloor) < closest) { // Check if the value is not -1
                //check logic here
                closest = abs(targetFloor[i] - currentFloor); // Update closest if found a smaller value
                placement = i;
                nextFloor = targetFloor[i];
            }
        }
    }
    targetFloor[placement] = -1;
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


/*This function should be working -> tested*/

// void bubbleSort(int size, bool ascending) {
// // Loop through each element of the array
//     for (int i = 0; i < size - 1; i++) {
//         for (int j = 0; j < size - i - 1; j++) {
//             // Determine if a swap should occur
//             bool shouldSwap = false;

//             if (ascending) {
//                 // For ascending order, move -1 to the end and sort the rest
//                 if ((targetFloor[j] == -1 && targetFloor[j + 1] != -1) ||
//                     (targetFloor[j] != -1 && targetFloor[j + 1] != -1 && targetFloor[j] > targetFloor[j + 1])) {
//                     shouldSwap = true;
//                 }
//             } else {
//                 // For descending order, ensure -1 stays at the end and sort the rest
//                 if ((targetFloor[j] != -1 && targetFloor[j + 1] == -1) ||
//                     (targetFloor[j] != -1 && targetFloor[j + 1] != -1 && targetFloor[j] < targetFloor[j + 1])) {
//                     shouldSwap = true;
//                 }
//             }

//             // Perform the swap if necessary
//             if (shouldSwap) {
//                 int temp = targetFloor[j];
//                 targetFloor[j] = targetFloor[j + 1];
//                 targetFloor[j + 1] = temp;

//                 // Swap the floor_index values in parallel to maintain linkage
//                 temp = floor_index[j];
//                 floor_index[j] = floor_index[j + 1];
//                 floor_index[j + 1] = temp;
//             }
//         }
//     }
// }

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



// void printArraysss(int size) {
//     printf("targetFloor: ");
//     for (int i = 0; i < size; i++) {
//         printf("%d ", targetFloor[i]);
//     }
//     printf("\n");

//     printf("floor_index: ");
//     for (int i = 0; i < size; i++) {
//         printf("%d ", floor_index[i]);
//     }
//     printf("\n\n");
// }
//     int size = sizeof(targetFloor) / sizeof(targetFloor[0]);
//     // Test sorting in ascending order
//     printf("Sorting in ascending order:\n");
//     bubbleSort(size, 1);
//     printArraysss(size);
//     // Test sorting in descending order
//     printf("Sorting in descending order:\n");
//     bubbleSort(size, 0);
//     printArraysss(size);