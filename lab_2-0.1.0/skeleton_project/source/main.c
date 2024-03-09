#include "Supportfunctions.h"

/*Global variables*/
bool isDoorOpen = false;
bool foundOrder = false;

int  totalOrders[10][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};
int targetFloor[4]={-1,-1,-1, -1};
int floor_index[4] = {-1, -1, -1, -1};

int direction = 0;
int placement = 0;
int typeOfButton = 0;
int previousFloor=0;


/*end of declerations*/

int main(){
    elevio_init();
    startUp();
    allLightsOff();
    elevatorRunning();
    return 0;
}