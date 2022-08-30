#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pid.h"
#include "bme280.h"
#include "gpio.h"
#include "uart.h"
#include "display.h"
#include "temperature.h"

int uart_filestream, key_gpio = 1;
struct bme280_dev bme_connection;

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
    do {
        requestToUart(uart_filestream, GET_INTERNAL_TEMP);
        TI = readFromUart(uart_filestream, GET_INTERNAL_TEMP).float_value;
        printf("%f\n", TI);
        double value = pidControl(TI);
        pwmControl(value);
        printf("%lf\n", value);

        requestToUart(uart_filestream, GET_POTENTIOMETER);
        TR = readFromUart(uart_filestream, GET_POTENTIOMETER).float_value;
        printf("%f\n", TR);
        pidUpdateReference(TR);

        TE = getCurrentTemperature(&bme_connection);
        printf("\tTI: %.2f⁰C - TR: %.2f⁰C - TE: %.2f⁰C\n", TI, TR, TE);
        printDisplay(TI, TR, TE);

        if(TR >= TI){
            turnResistanceOn(100);
            turnFanOff();
            value = 100;
        }
        else if(TR <= TI){
            turnFanOn(100);
            turnResistanceOff();
            value = -100;
        }

    } while (1);
}

void startProgram(){
    wiringPiSetup();
    // turnResistanceOff();
    // turnFanOff();
    connectDisplay();
    bme_connection = connectBme();
    uart_filestream = initUart();
}

int main () {

    startProgram();
    signal(SIGINT, exitProgram);


    PID();

    return 0;
}