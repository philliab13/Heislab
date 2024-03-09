#include "Supportfunctions.h"


/*Global variables*/
 bool isDoorOpen = false;

 int  totalOrders[10][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
// int targetFloor[4]={-1,-1,-1, -1};
// int floor_index[4] = {-1, -1, -1, -1};
int targetFloor[4]={2,-1,1,-1};
int floor_index[4]={20,-1,10,-1};

 bool foundOrder = false;
 int direction = 0;
 //int counter = 0;
 int placement = 0;
 int typeOfButton = 0;
 int previousFloor=0;


/*Declaring functions so the layout does not matter*/
int elevatorReady();
void executeOrder();
void addOrder(int, int);
//void searchOrders();
void deleteOrder(int indexInArray);
bool checkPassingFloors(int);
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

/*Delete the order at index indexInArray in totalorders, also delete all orders at same floor*/
void deleteOrder(int indexInArray){
    assert(indexInArray >= 0 && "Index passed to delete order is <0");
    int floor = totalOrders[indexInArray][0];
    /*Turn of light for the order to be deleted*/
    if(totalOrders[indexInArray][0] != -1){
        elevio_buttonLamp(totalOrders[indexInArray][0], totalOrders[indexInArray][1], 0);
    }
    /*Scans through and shift's every element to the left from indexInArray*/
    for (int i = indexInArray; i < 9; i++){
        totalOrders[i][0] = totalOrders[i + 1][0];
        totalOrders[i][1] = totalOrders[i + 1][1];
    }
    totalOrders[9][0] = -1;
    totalOrders[9][1] = -1;
    /*deletes other orders from same floor*/
    for (int i = 0; i < 10; ++i){
        if(totalOrders[i][0] == floor && floor != -1){
            deleteOrder(i);
        }
    }
}

/*TODO: switch function*/
/*Function check's if there are any other orders for the same floor in targetFloor*/
bool Duplicate(int floor){
    for(int i=0; i<4; i++){
        if(floor==targetFloor[i]){
            return true;
        }
    }
    return false;
}
bool targetFloorContains(int floor) {
    for (int i = 0; i < 4; i++) {
        if (targetFloor[i] == floor) {
            return true; // Floor found in the array
        }
    }
    return false; // Floor not found in the array
}

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
void updateDirection(){
    if(previousFloor - targetFloor[0] < 0){
        direction = 1;
    }else if(previousFloor - targetFloor[0] > 0){
        direction = -1;
    }else{
        direction = 0;
    }
}

int main(){
    elevio_init();
    startUp();
    allLightsOff();
    elevatorRunning();
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