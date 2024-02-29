#include "lights.h"

// Lights the stop lamp if the obstruction is pressed. Can have this in the while loop to see if we have to light te stop lamp.
void obstructionActiveLamp()
{
    if (elevio_obstruction())
    {
        elevio_stopLamp(1);
    }
    else
    {
        elevio_stopLamp(0);
    }
}

// Stop light is lit when it is pressed FAT: L6
void stopLamp()
{
    while (elevio_stopButton())
    {
        elevio_stopLamp(1);
    }
}
