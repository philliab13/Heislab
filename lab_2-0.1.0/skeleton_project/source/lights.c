#include "lights.h"


void allLightsOff(){
    for (int f = 0; f < N_FLOORS; f++){
        for (int b = 0; b < N_BUTTONS; b++){
            elevio_buttonLamp(f, b, 0);
        }
    }
    elevio_stopLamp(0);
    elevio_doorOpenLamp(0);
}