#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>

#define COMPORT "\\\\.\\COM19"
#define BAUDRATE CBR_9600

//--------------------------------------------------------------
// Function: initSio
// Description: intializes the parameters as Baudrate, Bytesize,
//           Stopbits, Parity and Timeoutparameters of
//           the COM port
//--------------------------------------------------------------



void initSio(HANDLE hSerial){

    COMMTIMEOUTS timeouts ={0};
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        //error getting state
        printf("error getting state \n");
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams)){
        //error setting serial port state
        printf("error setting state \n");
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts)){
    //error occureed. Inform user
        printf("error setting timeout state \n");
    }
}

//--------------------------------------------------------------
// Function: readByte
// Description: reads a single byte from the COM port into
//              buffer buffRead
//--------------------------------------------------------------
DWORD readByte(HANDLE hSerial, char *buffRead, DWORD* buffSize) {

    DWORD dwBytesRead = 0;

    if (!ReadFile(hSerial, buffRead + *buffSize, 10, &dwBytesRead, NULL))
    {
        printf("error reading byte from input buffer \n");
    }
    *buffSize += dwBytesRead;

    return dwBytesRead;
}

//--------------------------------------------------------------
// Function: writeByte
// Description: writes a single byte stored in buffRead to
//              the COM port
//--------------------------------------------------------------
DWORD writeByte(HANDLE hSerial, char *buffWrite, DWORD bytesToWrite){

    DWORD dwBytesWritten = 0;

    if (!WriteFile(hSerial, buffWrite, bytesToWrite, &dwBytesWritten, NULL))
    {
        printf("error writing byte to output buffer \n");
    }

    return(0);
}

int main()
{
    HANDLE hSerial;


    char readBuffer[BUFSIZ+1];
    DWORD readBufferSize = 0;
    char writeBuffer[BUFSIZ+1];

    int i;

    //----------------------------------------------------------
    // Open COMPORT for reading and writing
    //----------------------------------------------------------
    hSerial = CreateFile(COMPORT,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if(hSerial == INVALID_HANDLE_VALUE){
        if(GetLastError()== ERROR_FILE_NOT_FOUND){
            //serial port does not exist. Inform user.
            printf(" serial port does not exist \n");
        }
        //some other error occurred. Inform user.
        printf(" some other error occured. Inform user.\n");
    }

    //----------------------------------------------------------
    // Initialize the parameters of the COM port
    //----------------------------------------------------------

    initSio(hSerial);

    while ( 1 ) {
        gets(writeBuffer);
        strcpy(writeBuffer, "Jemoeder");
        writeByte(hSerial, writeBuffer, 9);

        readByte(hSerial, readBuffer, &readBufferSize);
        for(i = 0; i < readBufferSize; i++){
            printf("%c", readBuffer[i]);
        }
        readBufferSize = 0;
    }

    printf("ZIGBEE IO DONE!\n");
    return 0;

    CloseHandle(hSerial);
}
