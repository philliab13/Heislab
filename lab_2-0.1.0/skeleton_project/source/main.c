#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include <stdbool.h>
#include <unistd.h>

/*Global variables*/
bool isDoorOpen = false;
int totalOrders[10][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{1,1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};


/*Declaring functions so the layout does not matter*/
bool safeToDrive();
int elevatorReady();
void driveToFloor(int);
void allFloorLightsOff();
void executeOrder();

/*end of declerations*/




 
typedef struct{
    int location;
    int direction;
}elevatorOrder;

int elevatorReady(){
    while(true){
       
        executeOrder();
    }
    return 0;
}

void searchOrders(){
     /*f iterates the different floors 0-3, b iterates the different buttontypes on that floor*/
        for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                int btnPressed = elevio_callButton(f, b);
                elevio_buttonLamp(f, b, btnPressed);
                /*add order*/
                if(btnPressed){
                    addOrder(f,b);
                }
            }
        }
}

/*Need failsafe for identical orders*/
int addOrder(floor, button){
    /*Looking for the first empty spot in the array to insert the new order*/
    for (int i = 0; i < 10; i++) {
        if(totalOrders[i][0] = -1){
            totalOrders[i][0] = floor;
            totalOrders[i][1] = button;
            break;
        }
    }
}
/*Denne skal slette elementet på index i arrayen og flytte alt bak et hakk mot venstre */
void deleteOrder(int indexInArray){
    for(int i=0; i<10; i++){
        printf("ordre før: %d, %d\n", totalOrders[i][0], totalOrders[i][1]);
    }
    for( int i =0; i<2; i++){
        
        totalOrders[indexInArray][i]=-1;
    }

    for(int i = indexInArray; i<9; i++)
    {
        totalOrders[i][0]=totalOrders[i+1][0];
        totalOrders[i][1]=totalOrders[i+1][1];
    }
    totalOrders[9][0]=-1;
    totalOrders[9][1]=-1;
    printf("-------------------------\n");
    for(int i=0; i<10; i++){
        printf("ordre før: %d, %d\n", totalOrders[i][0], totalOrders[i][1]);
    }
    
    
}

void executeOrder(){
    int targetFloor;
    int button;
    int index;
    int currentFloor = elevio_floorSensor();

    /*Iterates through the order array and picks an order to execute*/
    for (int i = 0; i < 10; i++) {
        if(totalOrders[i][0] != -1){
            targetFloor = totalOrders[i][0];
            button = totalOrders[i][1];
            index = i;
        }
    }
    /*this part is for the logic og the up and down buttons outside the elevator*/
        /*Checking if the order is from the same floor as the elevator, if it is we can open the door*/
        if(currentFloor == targetFloor){
            openDoor();
            deleteOrder(index);
        }
        /*Finding out if the floor difference is more than 1, if it is we need to check if there are any floors we should stop by*/
        if(abs(currentFloor - targetFloor) > 1){

        }





    /*check if the order is coming from the floor the elevator is*/
    /*Logic to find out if there are any orders on the way the elevator should pick up*/

    
}





int main(){
    elevio_init();

   /*  startUp(); */

   deleteOrder(2);

    


    while (true) {}


    
    return 0;
} 

void driveUp(int onFloor){
    MotorDirection direction = 1;
    elevio_motorDirection(direction);
    while(onFloor == -1){
        onFloor = elevio_floorSensor();
    }
    elevio_motorDirection(0);
}

void startUp(){
    int onFloor = elevio_floorSensor();
    if(onFloor == -1){
        driveUp(onFloor);
    }
    elevatorReady();
}

void driveToFloor(int destinationFloor){
    /*Safety part*/
    elevio_floorIndicator(2);
    /*the driving part*/
    int currentFloor = elevio_floorSensor();
    int difference = destinationFloor - currentFloor;
    MotorDirection direction;
    if(difference > 0){
        direction = 1;
    }else if(difference < 0){
        direction = -1;
    }else{
        direction = 0;
    }
    bool safe = safeToDrive();
    elevio_motorDirection(direction);
    while(difference != 0 && safe){
        
        currentFloor = elevio_floorSensor();
        if(currentFloor > -1){
            difference = destinationFloor - currentFloor;
        }
        safe = safeToDrive();

        /*Updating the lights*/
    }
    elevio_motorDirection(0);
    elevio_motorDirection(0);
}

void allFloorLightsOff(){
    for(int i = 0; i < N_FLOORS; ++i){
        elevio_floorIndicator(i);
    }
}

/*returning 1 if sucsessfully opened door, 0 otherwise*/
int openDoor(){
    int floor = elevio_floorSensor();
    /*checking if it safe to open door*/
    if(floor > 0){
        /*Update global variable to door is open*/
        isDoorOpen = true;
        /*set open door light on*/
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
        sleep(3);
        return 1;
    }else{
        return 0;
    }
}
/*returning 1 if sucsessfully opened door, 0 otherwise*/
int closeDoor(){
    /*check if there is an obstruction*/
    bool obstruction = elevio_obstruction();
    if(obstruction){
        return 0;
    }else{
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