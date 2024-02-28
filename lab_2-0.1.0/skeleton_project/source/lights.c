#include "lights.h"

// Lights the stop lamp if the obstruction is pressed. Can have this in the while loop to see if we have to light te stop lamp.
void stopLamp()
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
