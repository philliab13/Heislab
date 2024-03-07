#include "Supportfunctions.h"


/*Global variables*/
bool isDoorOpen = false;

int totalOrders[10][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
int targetFloor[4]={-1,-1,-1, -1};
int floor_index[4] = {-1, -1, -1, -1};
bool foundOrder = false;
int direction = 0;
int counter = 0;


/*Declaring functions so the layout does not matter*/
int elevatorReady();
void executeOrder();
void addOrder(int, int);
//void searchOrders();
void deleteOrder(int indexInArray);
void checkPassingFloors(int, int, int);
//int findOrder(int,int);
void updateOrdersCab();
void updateOrders();



/*end of declerations*/

/* typedef struct
{
    int location;
    int direction;
} elevatorOrder;
 */
// int elevatorReady(){
//     while (true)    {
//         executeOrder();
//     }
//     return 0;
// }

/*Scans all the buttons to see if any of them are pressed in that instance, if yes call addOrder*/
void searchOrders(){
    /*f iterates the different floors 0-3, b iterates the different buttontypes on that floor*/
    for (int f = 0; f < N_FLOORS; f++){
        for (int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            /*add order*/
            if (btnPressed){
                addOrder(f, b);
            }
            if(elevio_stopButton() == 1){
                stopProcedure();
            }
        }
    }
}


/*Add the order detected from searchOrders to the global array totalOrders*/
/*Need failsafe for identical orders*/

void addOrder(floor, button){
    /*First the function checks if the order already exist and ignores it if it does*/
    for(int i = 0; i < 10; ++i){
        if(totalOrders[i][0] == floor && totalOrders[i][1] == button){
            return;
        }
    }
    /*Looking for the first empty spot in the array to insert the new order*/
    for (int i = 0; i < 10; ++i) {
        if(totalOrders[i][0] == -1){
            totalOrders[i][0] = floor;
            totalOrders[i][1] = button;
        /*Sets the button lamp on for the order*/
            elevio_buttonLamp(floor, button, 1);
            break;
        }
    }
}

/*Denne skal slette elementet på index i arrayen og flytte alt bak et hakk mot venstre  */
/*this function should also delete all other orders for the same floor*/
void deleteOrder(int indexInArray){
    int floor = totalOrders[indexInArray][0];
    /*
    for (int i = 0; i < 2; i++)    {

        totalOrders[indexInArray][i] = -1;
        elevio_floorIndicator(indexInArray);
    }
    */
   if(totalOrders[indexInArray][0] != -1){
    elevio_buttonLamp(totalOrders[indexInArray][0], totalOrders[indexInArray][1], 0);
   }
    for (int i = indexInArray; i < 9; i++){
        totalOrders[i][0] = totalOrders[i + 1][0];
        totalOrders[i][1] = totalOrders[i + 1][1];
    }
    totalOrders[9][0] = -1;
    totalOrders[9][1] = -1;

    for (int i = 0; i < 10; ++i){
        if(totalOrders[i][0] == floor && floor != -1){
            deleteOrder(i);
        }
    }
}

/*This function checks the totalOrder array and checks if there are any orders going the same direction on passing floors*/
/*If yes, updates the arrays targetFloor with what floor the order is on and index for location in totalOrder*/
/*defyning that going up equals a positive number for direction*/

void checkPassingFloors(int currentFloor, int typeOfButton, int counter){
    //int direction = targetFloor[0] - currentFloor;
    //printf("direction is: %d \n", direction);
    if(direction == 1){
        for (int i = 0; i < 10; ++i){
            //I have changed the logic for middle statement from == to 
            if((totalOrders[i][1] == 0) && ((totalOrders[i][0] >= currentFloor) && (totalOrders[i][0] < targetFloor[0]))){
                printf("counter: %d \n", counter);
                targetFloor[counter] = totalOrders[i][0];
                ++counter;
                floor_index[counter] = i;
            }
        }
    }
    else if (direction == -1){
        for (int i = 0; i < 10; ++i){
            /*Need to check the logic for this if-test*/
            if((totalOrders[i][1] == 1) && ((totalOrders[i][0] <= currentFloor) || (totalOrders[i][0] > targetFloor[i]))){
                targetFloor[counter] = totalOrders[i][0];
                ++counter;
                floor_index[counter] = i;
            }
        }
    }
}



/*Find if there happens to be an order for the floor we are stopping at*/
int findOrderOnFloor(int floor){
    for (int i = 0; i < 10; ++i){
        if (totalOrders[i][0] == floor){
            return i;
        }
    }
    return -1;
}


void executeOrder(){
    counter = 0;
    int placement;
    foundOrder = false;
    direction = 0;
    for(int i = 0; i < 4; ++i){
        targetFloor[i] = -1;
        floor_index[i] = -1;
    }

    //printArray(totalOrders);
    updateOrdersCab();
    /*This section is for handling orders when they come from inside the cab*/
    int currentFloor = elevio_floorSensor();
    //printf("foundOrder: %d \n", foundOrder);
    if(foundOrder){
        if (currentFloor == targetFloor[0]){
            //printf("111 \n");
            //printf("targetfloor: %d  %d  %d \n",totalOrders[0][0],targetFloor[1], targetFloor[2]);
            openDoor();
            deleteOrder(floor_index[0]);
            closeDoor();
            elevatorRunning();
        }
        /*We should now have an main order to execute and all the floors worth stopping by in the array targetFloor*/
        /*The elevator can now drive to the target floors, when it has stopped by all of them the order is completed*/
        while((targetFloor[0] != -1) || (targetFloor[1] != -1) || (targetFloor[2] != -1)){;
            //printf("er i denne lokken \n");
            printf("targetfloor: %d  %d  %d \n",targetFloor[0],targetFloor[1], targetFloor[2]);

            closeDoor();
            checkPassingFloors(currentFloor, typeOfButton, counter);
            driveToFloor(findNearestFloor(currentFloor, &placement));
            currentFloor = elevio_floorSensor();
            openDoor();
            //deleteOrder(floor_index[placement]);
            int passBy = findOrderOnFloor(currentFloor);
            if (passBy != -1){
                printf("Hei\n");
                deleteOrder(passBy);
               
            }
            //targetFloor[placement] = -1;
            closeDoor();
            checkPassingFloors(currentFloor, typeOfButton, counter);
        }
    }
        /* for (int i = 0; i < 4; ++i){
            printf("er i denne lokken \n");
            if (targetFloor[i] != -1){
                closeDoor();
                driveToFloor(findNearestFloor(currentFloor, &placement));
                openDoor();
                //deleteOrder(index[i]);
                int passBy = findOrderOnFloor(currentFloor);
                if (passBy != -1){
                    deleteOrder(passBy);
                    targetFloor[placement] = -1;
                }
                closeDoor();
                checkPassingFloors(currentFloor, typeOfButton, counter);
            }
        } */

    /*this section is for handling orders from outside the cab*/
    else{
        /*Iterates through the order array and picks an order to execute.*/
        /*This loop should only be able to pick orders from outside the cab*/
        updateOrders();

        if(foundOrder){
            if (currentFloor == targetFloor[0]){
                openDoor();
                deleteOrder(floor_index[0]);
                closeDoor();
                elevatorRunning();
            }

        /*We should now have an main order to execute and all the floors worth stopping by in the array targetFloor*/
        /*The elevator can now drive to the target floors, when it has stopped by all of them the order is completed*/
        while((targetFloor[0] != -1) || (targetFloor[1] != -1) || (targetFloor[2] != -1)){;
            printf("er i denne lokken nr 2 \n");
            closeDoor();
            checkPassingFloors(currentFloor, typeOfButton, counter);
            driveToFloor(findNearestFloor(currentFloor, &placement));
            currentFloor=elevio_floorSensor();
            openDoor();
                //deleteOrder(index[i]);
            int passBy = findOrderOnFloor(currentFloor);
            if (passBy != -1){
                printf("Hei\n");
                deleteOrder(passBy);
               
            }
            closeDoor();
            checkPassingFloors(currentFloor, typeOfButton, counter);
        }
            /* for (int i = 0; i < 4; ++i){
                printf("er i denne lokke nr2 \n");
                if (targetFloor[i] != -1){
                    closeDoor();
                    driveToFloor(findNearestFloor(currentFloor, &placement));
                    openDoor();
                    //deleteOrder(index[i]);
                    int passBy = findOrderOnFloor(currentFloor);
                    if (passBy != -1){
                        deleteOrder(passBy);
                        targetFloor[placement] = -1;
                    }
                    closeDoor();
                    checkPassingFloors(currentFloor, typeOfButton, counter);
                }
            } */
        }
    }

}
void updateOrdersCab(){
        /*Iterates through the order array and picks an order to execute.*/
    /*In this loop we are looking for orders from inside the elevator, these are prioritized*/
    /*Sets foundOder = true so that we don't look for more orders once one is found*/
    for (int i = 0; i < 10; ++i) {
        if((totalOrders[i][0] != -1) && (totalOrders[i][1] == 2)){
            targetFloor[0] = totalOrders[i][0];
            typeOfButton = totalOrders[i][1];
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
void updateOrders(){
    /*Iterates through the order array and picks an order to execute.*/
    /*In this loop we are looking for orders from inside the elevator, these are prioritized*/
    /*Sets foundOder = true so that we don't look for more orders once one is found*/
    for (int i = 0; i < 10; ++i) {
        if(totalOrders[i][0] != -1){
            targetFloor[0] = totalOrders[i][0];
            typeOfButton = totalOrders[i][1];
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

int main(){
    elevio_init();
    startUp();
    allLightsOff();
    while (true) {
        searchOrders();
        executeOrder();
        /* if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        } */
    }

    return 0;
}

void elevatorRunning(){
    while (true) {
        searchOrders();
        executeOrder();
    }
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