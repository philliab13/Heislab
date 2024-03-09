#include "HandleOrders.h"


extern bool isDoorOpen;

extern int  totalOrders[10][2];
extern int targetFloor[4];
extern int floor_index[4];
extern bool foundOrder;
extern int direction;
//extern int counter;
extern int placement;
extern int previousFloor;

 
/*Check's if there are any orders the elevator should stop by, if yes add to targetFloor and floor_index*/
bool checkPassingFloors(int currentFloor){
    bool changes= false;
    if(direction == 1){
        for (int i = 0; i < 10; ++i){
            //if the elevator is moving up an order need's to be either an hall_up or cab button. At the same time the floor in question needs
            //to be over the floor we are currently at. The function ignores orders from floors that already have an order in targetFloor
            if(  ((totalOrders[i][1] == 0 || totalOrders[i][1]==2) && ((totalOrders[i][0] > currentFloor)) && (!Duplicate(totalOrders[i][0])))   ){
                for(int k = 0; k < 4; ++k){
                    if(targetFloor[k] == -1 && floor_index[k] == -1){
                        targetFloor[k] = totalOrders[i][0];
                        floor_index[k] = i;
                        changes=true;
                        break;
                    }
                }
            }
        }
        /*Sort the targetFloor and floor_index from smallest to largest number*/
        bubbleSort(4,true);
    }
    else if (direction == -1){
        for (int i = 0; i < 10; ++i){            
            if( ((totalOrders[i][1] == 1 || totalOrders[i][1]==2) && (totalOrders[i][0] < currentFloor) && (!Duplicate(totalOrders[i][0]))) ){
                for(int k = 0; k < 4; ++k){
                    if(targetFloor[k] == -1 && floor_index[k] == -1){
                        targetFloor[k] = totalOrders[i][0];
                        floor_index[k] = i;
                        changes=true;
                        break;
                    }
                }
            }
        }
        /*sort largest floor first*/
        bubbleSort(4,false);
    }
    if(changes){
        return true;
    }else{
        return false;}
}


void executeOrder(){
    /*reset's variables*/
    placement=0;
    foundOrder = false;
    direction = 0;
    for(int i = 0; i < 4; ++i){
        targetFloor[i] = -1;
        floor_index[i] = -1;
    }
    int currentFloor = elevio_floorSensor();

    /*pick the oldest order*/
    updateOrders();

    if(foundOrder){
        /*check if the elevator is on the same floor as the destination floor*/
        if(currentFloor == targetFloor[0]){
            openDoor();
            closeDoor();
            deleteOrder(floor_index[0]);
            // targetFloor[0] = -1;
            // floor_index[0] = -1;
            elevatorRunning();
        }
        /*while runs as long as we have orders left to execute in this direction*/
        while(targetFloor[0] != -1 || targetFloor[1] != -1 || targetFloor[2] != -1 || targetFloor[3] != -1){
            if(targetFloor[0] >= 0){
                closeDoor();
                /*Make sure the direction is correct before checking for orders we should stop by*/
                updateDirection();
                /*Check for orders worth stopping by*/
                checkPassingFloors(previousFloor);
                updateDirection();
                /*elevator start driving first target floor*/
                bool madeItToDestination = driveToFloor(targetFloor[0]);
                /*True if new order is added to targetFloor*/
                while(!madeItToDestination){
                    madeItToDestination = driveToFloor(targetFloor[0]);
                }

                if(madeItToDestination){
                    currentFloor = elevio_floorSensor();
                    openDoor();
                    closeDoor();
                    deleteOrder(floor_index[0]);
                    targetFloor[0] = -1;
                    floor_index[0] = -1;
                    if(direction==1){
                        bubbleSort(4,true);
                    }else if(direction==-1){
                        bubbleSort(4,false);
                    }
                }
                checkPassingFloors(previousFloor);
            }
        }
    }
}


/*Looks for an order to execute, pick the oldest one if there are any*/
/*sets the global variable foundOrder = true if found*/
void updateOrders(){
    for (int i = 0; i < 10; ++i) {
        if(totalOrders[i][0] != -1){
            targetFloor[0] = totalOrders[i][0];
            floor_index[0] = i;
            foundOrder = true;
            break;
        }
    }
    if(foundOrder){
        if(elevio_floorSensor() - targetFloor[0] < 0){
            direction = 1;
        }else if(elevio_floorSensor() - targetFloor[0] > 0){
            direction = -1;
        }
    }
}
