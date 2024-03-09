#include "Supportfunctions.h"

extern int targetFloor[4];
extern int floor_index[4];
 extern int direction;
extern int previousFloor;

void elevatorRunning(){
    while (true) {
        searchOrders();
        executeOrder();
    }
}


void resetArrays(){
    for(int i = 0; i < 4; ++i){
        targetFloor[i] = -1;
        floor_index[i] = -1;
    }
}


void startUp(){
    int onFloor = elevio_floorSensor();
    if (onFloor == -1){
        driveUp();
    }
    elevio_floorIndicator(elevio_floorSensor());
    previousFloor = elevio_floorSensor();
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
