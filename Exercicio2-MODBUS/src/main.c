#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include "uart.h"

int main(int argc, const char * argv[]) {

    int uart0_filestream = initUart();
 
    //requestUartInteger(uart0_filestream);
    //sleep(1);
    //requestUartFloat(uart0_filestream);
    //sleep(1);
    requestUartString(uart0_filestream);
    //sleep(1);
    // sendUartInteger(uart0_filestream);
    // sendUartFloat(uart0_filestream);
    // sendUartString(uart0_filestream);
    
    close(uart0_filestream);
   return 0;
}