#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pid.h"
#include "gpio.h"
#include "uart.h"
#include "display.h"

int uart_filestream, key_gpio = 1;
// struct bme280_dev bme_connection;

void exitProgram(){
    system("clear");
    printf("Programa encerrado!\n");
    turnResistanceOff();
    turnFanOff();
    closeUart(uart_filestream);
    exit(0);
}

void PID() {
    system("clear");
    float TI, TR, TE;
    printf("\n██████████████████ Método PID ██████████████████\n");
    pidSetupConstants(30, 0.2, 400);
    
    requestToUart(uart_filestream, GET_INTERNAL_TEMP);
    TI = readFromUart(uart_filestream, GET_INTERNAL_TEMP).int_value;
    printf("%f\n", TI);
    double value = pidControl(TI);
    //pwmControl(value);
    printf("%lf\n", value);

    requestToUart(uart_filestream, GET_POTENTIOMETER);
    TR = readFromUart(uart_filestream, GET_POTENTIOMETER).float_value;
    printf("%f\n", TR);
    pidUpdateReference(TR);

    TE = 0;
    printDisplay(TI, TR, TE);
}

void startProgram(){
    wiringPiSetup();
    // turnResistanceOff();
    // turnFanOff();
    connectDisplay();
    // bme_connection = connectBme();
    uart_filestream = initUart();
}

int main () {

    signal(SIGINT, exitProgram);

    startProgram();

    PID();

    return 0;
}