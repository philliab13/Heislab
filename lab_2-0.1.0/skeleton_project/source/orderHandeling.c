 #include "OrderHandeling.h"

 extern int  totalOrders[10][2];
 extern int targetFloor[4];

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