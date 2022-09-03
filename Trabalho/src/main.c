#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "pid.h"
#include "bme280.h"
#include "gpio.h"
#include "uart.h"
#include "display.h"
#include "temperature.h"

int uart_filestream, key_gpio = 1;
struct bme280_dev bme_connection;
pthread_t frying;

int seconds = 0;

// Modo Desligado = 0
// Modo Ligado >= 1
// Modo Funcionando = 2
// Modo Cancelado = 3
int mode = 0;

void exitProgram(){
    system("clear");
    printf("Programa encerrado!\n");
    turnResistanceOff();
    turnFanOff();
    sendToUartByte(uart_filestream, SEND_SYSTEM_STATE, 0);
    sendToUartByte(uart_filestream, SEND_FUNC_STATE, 0);
    sendToUart(uart_filestream, SEND_TIME, 0);
    closeUart(uart_filestream);
    exit(0);
}

void *PID(void *arg) {
    system("clear");
    float TI, TR, TE;
    pidSetupConstants(30, 0.2, 400);
    int timerStarted = 0;
    do {
        requestToUart(uart_filestream, GET_INTERNAL_TEMP);
        TI = readFromUart(uart_filestream, GET_INTERNAL_TEMP).float_value;
        double value = pidControl(TI);
        pwmControl(value);

        requestToUart(uart_filestream, GET_POTENTIOMETER);
        TR = readFromUart(uart_filestream, GET_POTENTIOMETER).float_value;
        pidUpdateReference(TR);

        TE = getCurrentTemperature(&bme_connection);
        printf("\tTI: %.2f⁰C - TR: %.2f⁰C - TE: %.2f⁰C\n", TI, TR, TE);

        if(timerStarted) {
            int min = seconds / 60;
            int sec = seconds % 60;
            printDisplay(TI, min, sec);
        } else {
            printHeating();
        }

        if(TR > TI){
            turnResistanceOn(100);
            turnFanOff();
            value = 100;
            sendToUart(uart_filestream, SEND_SIGNAL, value);
        } else if(TR <= TI) {
            turnResistanceOff();
            turnFanOn(100);
            value = -100;
            sendToUart(uart_filestream, SEND_SIGNAL, value);
            if(!timerStarted) {
                timerStarted = 1;
            }
        }

        if(timerStarted) {
            seconds--;
            
            delay(1000);
        }

        if(seconds == 0) {
            mode = 1;
            turnFanOn(100);
            pthread_exit(0);
        }
    } while (mode == 2);
}

void startProgram(){
    wiringPiSetup();
    turnResistanceOff();
    turnFanOff();
    connectDisplay();
    bme_connection = connectBme();
    uart_filestream = initUart();
}

void menu() {
    int command;
    do {
        requestToUart(uart_filestream, GET_KEY_VALUE);
        command = readFromUart(uart_filestream, GET_KEY_VALUE).int_value;
        switchMode(command);
        delay(500);
    } while (1);
}

void switchMode(int command) {
    switch(command) {
        case 1:
            if (mode == 0) {
                printf("Ligou!");
                sendToUartByte(uart_filestream, SEND_SYSTEM_STATE, 1);
                mode = 1;
            }
            break;
        case 2:
            if (mode != 0) {
                printf("Desligou!");
                sendToUartByte(uart_filestream, SEND_SYSTEM_STATE, 0);
                mode = 0;
            }
            break;
        case 3:
            if(seconds > 0) {
                printf("Iniciou!");
                sendToUartByte(uart_filestream, SEND_FUNC_STATE, 1);
                mode = 2;
                pthread_create(&frying, NULL, PID, NULL);
            }
            break;
        case 4:
            if (mode == 2) {
                printf("Cancelou!");
                int b = 0;
                sendToUartByte(uart_filestream, SEND_FUNC_STATE, 0);
                mode = 3;
            }
            break;
        case 5:
            printf("Mais um minuto!");
            seconds = seconds + 60;
            int minutes = seconds / 60;
            sendToUart(uart_filestream, SEND_TIME, minutes);
            break;
        case 6:
            if(seconds > 0) {
                printf("Menos um minuto!");
                seconds = seconds - 60;
                int minutes = seconds / 60;
                sendToUart(uart_filestream, SEND_TIME, minutes);
            }
            break;
        case 7:
            printf("Menu pre configurado!");
            // Chama menu pre configurado.
            break;
        default:
            break;
    }
}

int main () {
    signal(SIGINT, exitProgram);
    startProgram();
    menu();
    return 0;
}