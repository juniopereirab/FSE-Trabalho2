#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pid.h"
#include "gpio.h"

void PID() {
    system("clear");
    float hys, TI, TR, TE;
    printf("\n██████████████████ Método PID ██████████████████\n");
    pidSetupConstants(30, 0.2, 400);
    
    

}

int main () {
    if(wiringPiSetup() == -1) return 1;
    PID();

    return 0;
}