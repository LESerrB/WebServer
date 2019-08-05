/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    hr_sync.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "hr_sync.h"
#include "GenericTypeDefs.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

HR_SYNC_DATA hr_syncData;
uint32_t actual_year, actual_month, today, actual_hour, actual_min, 
             month_counter, actual_secs;
BOOL     leap_year;
uint32_t UNIX_seg;
TCPIP_SNTP_RESULT status;
char buffer[255],
        recado;
int u = 0;
rtccTime Time;
rtccDate Date;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void HR_SYNC_Initialize ( void )

  Remarks:
    See prototype in hr_sync.h.
 */

void HR_SYNC_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    hr_syncData.state = HR_SYNC_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void HR_SYNC_Tasks ( void )

  Remarks:
    See prototype in hr_sync.h.
 */

void HR_SYNC_Tasks (void){
    /* Check the application's current state. */
    switch (hr_syncData.state){
        /* Application's initial state. */
        case HR_SYNC_STATE_INIT:
            if (SYS_FS_CurrentDriveGet(buffer) == SYS_FS_RES_SUCCESS){          // Checks if the volume is attached
                task = 4;
                strcpy(new_file, "Prueba.txt");
                hr_syncData.state = HR_SYNC_STATE_SERVICE_TASKS;
            }
        break;
        /* TODO: implement your application state machine.*/
        case HR_SYNC_STATE_SERVICE_TASKS:
            LED0On();
            u++;
            if(u % 100 == 0)
                string2write = "Hola s";
            else if (u>30000)
                u = 0;
        break;

        /* The default state should never be executed. */
        default:
            /* TODO: Handle error in application's state machine. */
        break;
        
    }
}

 

/*******************************************************************************
 End of File
 */
