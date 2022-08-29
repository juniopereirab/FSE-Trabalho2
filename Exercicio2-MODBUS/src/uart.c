#include <stdio.h>
#include <string.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include "uart.h"
#include "crc16.h"

int initUart () {
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;
}


void readUart(int uart0_filestream, char *type) {
    // Read up to 255 characters from the port if they are there
    int rx_length;
    int received_int;
    float received_float;
    char received[256];
    
    if(strcmp(type, "integer") == 0){
        rx_length = read(uart0_filestream, &received_int, 4);
        
    }
    else if(strcmp(type, "float") == 0){
        rx_length = read(uart0_filestream, &received_float, 4);
    }
    else if(strcmp(type, "string") == 0){
        rx_length = read(uart0_filestream, received, 256);
        printf("%s\n", received);
        received[rx_length] = '\0';
    }

    if (rx_length < 0)
    {
        printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponível.\n"); //No data waiting
    }
    else
    {
        if(strcmp(type, "integer") == 0){
            printf("Valor Inteiro Recebido: %d\n", received_int);
            
        }
        else if(strcmp(type, "float") == 0){
            printf("Valor Float Recebido: %f\n", received_float);

        }
        else if(strcmp(type, "string") == 0){
            printf("String Recebida: %s\n", received);
        }
    }
}

void writeUart(int uart0_filestream, unsigned char * payload, int length){
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, payload, length);
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);
}

void requestUartInteger(int uart0_filestream){

    unsigned char payload[7] = {0x01, 0x23, 0xA1, 0x01, 0x04, 0x03, 0x08};
    short crc = calcula_CRC(payload, 7);

    unsigned char message[9];
    memcpy(message, &payload, 7);
    memcpy(&message[7], &crc, 2);

    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, message, 9);
    
    readUart(uart0_filestream, "integer");
}

void requestUartFloat(int uart0_filestream){
    
    unsigned char payload[7] = {0x01, 0x23, 0xA2, 0x01, 0x04, 0x03, 0x08};
    short crc = calcula_CRC(payload, 7);

    unsigned char message[9];
    memcpy(message, &payload, 7);
    memcpy(&message[7], &crc, 2);

    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, message, 9);

    readUart(uart0_filestream, "float");
}

void requestUartString(int uart0_filestream){

    unsigned char payload[7] = {0x01, 0x23, 0xA3, 0x01, 0x04, 0x03, 0x08};
    short crc = calcula_CRC(payload, 7);

    unsigned char message[9];
    memcpy(message, &payload, 7);
    memcpy(&message[7], &crc, 2);
    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, payload, 9);

    readUart(uart0_filestream, "string");
}

void sendUartInteger(int uart0_filestream){
    unsigned char payload[7];
    unsigned int dado = 13;

    unsigned char message[9];
    payload[0] = 0x01;
    payload[1] = 0x16;
    payload[2] = 0xB1;
    memcpy(&payload[3], &dado, 4);

    short crc = calcula_CRC(payload, 7);
    memcpy(message, &payload, 7);
    memcpy(&message[7], &crc, 2);
    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, message, 9);
    readUart(uart0_filestream, "integer");
}

void sendUartFloat(int uart0_filestream){
    char payload[9];
    float dado = 75.685;
    char matricula[4] = {1, 4, 3, 8};

    payload[0] = 0xB2;
    memcpy(payload[1], &dado, 4);
    memcpy(payload[5], &matricula, 4);
    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, payload, 9);
    readUart(uart0_filestream, "float");
}

void sendUartString(int uart0_filestream){

    unsigned char payload[16];
    unsigned char dado[13] = {'l', 'u', 'l', 'a', ' ','c', 'o', 'n', 'f', 'i', 'r', 'm', 'a'};
    unsigned char message[18];
    payload[0] = 0x01;
    payload[1] = 0x16;
    payload[2] = 0xB3;
    payload[3] = 13;
    memcpy(&payload[4], &dado, 13);
    short crc = calcula_CRC(payload, 16);
    memcpy(message, &payload, 16);
    memcpy(&message[16], &crc, 2);
    printf("Buffers de memória criados!\n");

    writeUart(uart0_filestream, message, 18);
    printf("1");
    readUart(uart0_filestream, "string");
    printf("2");
}