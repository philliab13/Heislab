#include "Door.h"

extern bool isDoorOpen;
extern int  totalOrders[10][2];


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
    for(int i=0; i<10;i++){
        if(elevio_floorSensor()==totalOrders[i][0]){
            deleteOrder(i);
        }
    }
    elevio_doorOpenLamp(0);
    isDoorOpen = false;
    return 1;
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
