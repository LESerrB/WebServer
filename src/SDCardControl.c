/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    sdcardcontrol.c

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
 ******************************************************************************/

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
 ******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "sdcardcontrol.h"
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

SDCARDCONTROL_DATA sdcardcontrolData;
BSP_LED_STATE LEDstate = BSP_LED_STATE_OFF;
SYS_FS_FSTAT stat;
uintptr_t  longFileSize;
int32_t initial_position = 0, final_position = 0;                               // Position of the file pointer to create the files names list
BOOL BeginingList;                                                              // Flag to point if a new list is created

char longFileName[300],
     file_name[32],                                                             // Files readed from the directory file
     file_path[60];                                                             // Path to create the new files

int stringLength = 0,                                                           // Length of the file name string
    j,retdate;                                                                  // Returns the value of comparation strings

char buffer[255];

typedef struct Node Node;
struct Node {
      char info[32];                                                            // Data string
      Node* next;                                                               // Used to point to next node
      Node* prev;                                                               // Used to point to previous node 
};
Node *principal_node = NULL;                                                    // Principal node that includes the list of file names
Node *temporal_node = NULL;                                                     // Temporal node used to store a single node to delete/add

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.*/
Node *InsertBefore(Node *principal, char data[30]){
    strcpy(principal->next->info, data);

    return principal->next;
}

Node *InsertAfter(Node *after, char data[30]){
    Node* n;
    n = (Node *)malloc(sizeof(Node));
    strcpy(n->info, data);

    // If header node is not created
    if (!after) {
        n->prev = n;
        n->next = n;
        return n;
    }

    n->prev = after;
    n->next = after->next;

    if (after->prev)                                                            // Just to take care when list is broken
        after->next->prev = n;

    after->next = n;
    return n;
}

Node *DeleteElement(Node *element){
    Node* tmp;
    tmp = element;
    element->next->prev = tmp->prev;

    if (tmp->prev)
        element->prev->next = tmp->next;

    free(tmp);
    return element->prev;
}

int PrintList(Node *h){
    Node* t = h;

    do{
        SYS_FS_FilePrintf(sdcardcontrolData.fileHandle2, "%s\r\n", t->info);
        t = t->next;
    }while(t && (t != h));

    return 0;
}

int PrintWebList(){
    if(principal_node){
        principal_node = principal_node->next;
        strcpy(mensaje, principal_node->info);
        return 0;
    }
    else
        return 1;
}

Node *SearchList(Node *h, char info[30]){
    Node* after = h;

    do{
        if (strcmp(after->info, info) == 0)
            return after;

        after = after->next;
    }while(after && (after != h));

    return NULL;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SDCARDCONTROL_Initialize ( void )

  Remarks:
    See prototype in sdcardcontrol.h.
 */

void SDCARDCONTROL_Initialize (void){
    /* Place the App state machine in its initial state. */
    sdcardcontrolData.state = SDCARDCONTROL_STATE_INIT;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/*******************************************************************************
  Function:
    void SDCARDCONTROL_Tasks ( void )

  Remarks:
    See prototype in sdcardcontrol.h.
 */

void SDCARDCONTROL_Tasks (void){
    stat.lfname = longFileName;
    stat.lfsize = 300;
    static uint32_t     startTick = 0;
//    SendMessage = false;

    /* Check the application's current state. */
    switch (sdcardcontrolData.state){
        /* Initialization file manager */
        case SDCARDCONTROL_STATE_INIT:
            if (SYS_FS_CurrentDriveGet(buffer) == SYS_FS_RES_SUCCESS){          // Checks if the volume is attached
                strcat(buffer, "/MUESTRAS");
//                sprintf(file_path, "%s", buffer);                      // Copy the file path to the "MUESTRAS" directory
                sdcardcontrolData.dirHandle = SYS_FS_DirOpen(buffer);

                if(sdcardcontrolData.dirHandle != SYS_FS_HANDLE_INVALID){
                // Directory open is successful
                    task = 1;
                    sdcardcontrolData.state = SDCARDCONTROL_STATE_SERVICE_TASKS;
                }
            }
        break;
        
        /*Wait a task*/
        case SDCARDCONTROL_STATE_SERVICE_TASKS:
            switch(task){
                case 1:// Creates a list of files in a txt file
                    sprintf(file_path, "%s/DIRECTORIO.txt", buffer);
                    sdcardcontrolData.fileHandle = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_WRITE));
                    sdcardcontrolData.state = SDCARDCONTROL_READ_FILES;
                break;
                    
                case 2:// Reads the files names in the "DIRECTORIO" file to make a search
                    if(principal_node){
                        principal_node = NULL;
                        BeginingList = false;
                    }
                    
                    list_count = 0;                                             // Resets the list counter in each search
                    
                    if(date_search[0] != NULL){
                        sprintf(file_path, "%s/DIRECTORIO.txt", buffer);
                        sdcardcontrolData.fileHandle = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_READ));
                        sdcardcontrolData.state = SDCARDCONTROL_READ_DIRECTORY_FILE;
                    }
                break;
                
                case 3:// Reads a string to delete from the directory
                    sprintf(file_path, "%s/DIRECTORIO.txt", buffer);
                    sdcardcontrolData.fileHandle = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_READ));
                    sdcardcontrolData.state = SDCARDCONTROL_TEMPORAL_DIRECTORY;
                break;
                
                case 4:// Creates a new file to write
                    if(new_file[0] != '\0')
                        sdcardcontrolData.state = OPEN_FILE;
                break;
                
                default:
                    
                break;
            }
            
        break;
        /* TODO: implement your application state machine.*/
//+++++++++++++++++++++++++++++ READ FILES ROUTINE +++++++++++++++++++++++++++//
/* This section of code is for read the files of the MUESTRAS directory       *
 * in the SD card in use.                                                     */
        case SDCARDCONTROL_READ_FILES:                                          // Reads the files in the "MMUESTRAS" directory to add to the "DIRECTORY" file
            if(SYS_FS_DirRead(sdcardcontrolData.dirHandle, &stat) == SYS_FS_RES_FAILURE)
                sdcardcontrolData.state = SDCARDCONTROL_ERROR;
            else{
                if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0')){       // Reached the end of the directory.
                    SYS_FS_FileClose(sdcardcontrolData.fileHandle);
                    task = 0;
                    sdcardcontrolData.state = SDCARDCONTROL_STATE_SERVICE_TASKS;// Returns to wait a new task
                }
                else if((stat.fname[0] != '.') && (stat.lfname[0] != 'D'))
                    SYS_FS_FilePrintf(sdcardcontrolData.fileHandle, "%s\r\n", stat.lfname);
            }
        break;
//-------------------------------- LIST ROUTINE ------------------------------//
/* This section search in the Directory File a list of files that correspond  *
 * to a parameter that is needed (DAY/MONTH/YEAR) and returnas all the files  *
 * in a Double Linked List                                                    */            
        case SDCARDCONTROL_READ_DIRECTORY_FILE:                                 // Read the files names in the "DIRECTORIO" file to create a circle linked list
            if(SYS_FS_FileEOF(sdcardcontrolData.fileHandle) == false){
                SYS_FS_FileStringGet(sdcardcontrolData.fileHandle, file_name, 30);
                ReadyToPrint = true;
                sdcardcontrolData.state = SDCARDCONTROL_ADD_TO_LIST;
            }
            else{
                SYS_FS_FileSeek(sdcardcontrolData.fileHandle, 0, SYS_FS_SEEK_SET);// Returns the pointer of the file to the begin
                SYS_FS_FileClose(sdcardcontrolData.fileHandle);                 // Close the "DIRECTORY" file
                task = 0;                                                       // Reset the task selector
                ReadyToPrint = false;
                sdcardcontrolData.state = SDCARDCONTROL_STATE_SERVICE_TASKS;
            }
        break;
        
        case SDCARDCONTROL_ADD_TO_LIST:{
        /* Add the file if the name mismatch with the DAY/MONTH/YEAR/HOUR     *
         * parameters                                                         */
            retdate = 0;
            stringLength = strlen(file_name);
            
            if(file_name[11] == '_'){
                file_date[0] = file_name[12];
                file_date[1] = file_name[13];
                file_date[2] = file_name[14];
                file_date[3] = file_name[15];
                file_date[4] = file_name[16];
                file_date[5] = file_name[17];
                file_date[6] = file_name[19];
                file_date[7] = file_name[20];
            }
            else
                file_date[0] = NULL;
            
            for(j=0; j<8; j++)
                if(file_date[j] != date_search[j])
                    retdate++;
                
            if (retdate == 0){                
                if(list_count < 50 && file_name[0] != '\r'){
                    principal_node = InsertAfter(principal_node, file_name);
                    list_count++;
                }
                else
                    if(file_name[0] != '\r')
                        principal_node = InsertBefore(principal_node, file_name);
                
                if(BeginingList == false){
                    initial_position = SYS_FS_FileTell(sdcardcontrolData.fileHandle) - stringLength;
                    BeginingList = true;
                }
                
                final_position = SYS_FS_FileTell(sdcardcontrolData.fileHandle);
            }
            
            sdcardcontrolData.state = SDCARDCONTROL_READ_DIRECTORY_FILE;
        break;
        }
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ FILE MANAGER $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$//
        case SDCARDCONTROL_TEMPORAL_DIRECTORY:
        /* Creates a temporal directory file to move the file names to no lost*
         * information.                                                       */
            sprintf(file_path, "%s/Directorio-temp.txt", buffer);
            sdcardcontrolData.fileHandle2 = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_WRITE));
            
            if(sdcardcontrolData.fileHandle2 == SYS_FS_HANDLE_INVALID)
            /* Could not open the file. Error out*/
                sdcardcontrolData.state = SDCARDCONTROL_ERROR;
            else{
                while(SYS_FS_FileTell(sdcardcontrolData.fileHandle) != initial_position){
                    SYS_FS_FileStringGet(sdcardcontrolData.fileHandle, file_name, 30);
                    SYS_FS_FilePrintf(sdcardcontrolData.fileHandle2, "%s", file_name);
                }
                
                SYS_FS_FileClose(sdcardcontrolData.fileHandle2);
                sdcardcontrolData.state = APP_DELETE_FILE;
            }
            
            SYS_FS_FileClose(sdcardcontrolData.fileHandle);
        break;

        case APP_DELETE_FILE:
            if(file2seek[0] != '\0'){
                sprintf(file_path, "%s/", buffer);
                strcat(file_path, file2seek);
                temporal_node = SearchList(principal_node, file2seek);

                if(temporal_node){
                    principal_node = DeleteElement(temporal_node);
                    SYS_FS_FileDirectoryRemove(file_path);
                    file2seek[0] = NULL;
                }
            }
            else
                sdcardcontrolData.state = SDCARDCONTROL_WRITE_REST_OF_FILES;
        break;

        case SDCARDCONTROL_WRITE_REST_OF_FILES:
        /* Writes the rest of the files to the temporal directory */
            sprintf(file_path, "%s/Directorio-temp.txt", buffer);
            sdcardcontrolData.fileHandle2 = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_APPEND));
            
            if(principal_node)
                PrintList(principal_node);
            
            sprintf(file_path, "%s/DIRECTORIO.txt", buffer);
            sdcardcontrolData.fileHandle = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_READ));
            SYS_FS_FileSeek(sdcardcontrolData.fileHandle, final_position, SYS_FS_SEEK_SET);
            
            while(SYS_FS_FileEOF(sdcardcontrolData.fileHandle) == false){
                    SYS_FS_FileStringGet(sdcardcontrolData.fileHandle, file_name, 30);
                    SYS_FS_FilePrintf(sdcardcontrolData.fileHandle2, "%s", file_name);
            }
            
            SYS_FS_FileClose(sdcardcontrolData.fileHandle);
            SYS_FS_FileClose(sdcardcontrolData.fileHandle2);
            
            sdcardcontrolData.state = SDCARDCONTROL_REWRITE_DIRECTORY;
        break;
        
        case SDCARDCONTROL_REWRITE_DIRECTORY:
        /* Delete the directory file and renames the temporal directory to had*
         * only directory file                                                */
            if(SYS_FS_FileDirectoryRemove("/mnt/mchpSite1/MUESTRAS/DIRECTORIO.txt") == SYS_FS_RES_SUCCESS)
                SYS_FS_FileDirectoryRenameMove("/mnt/mchpSite1/MUESTRAS/Directorio-temp.txt", "/mnt/mchpSite1/MUESTRAS/DIRECTORIO.txt");

            task = 0;
            SendMessage = true;
            sdcardcontrolData.state = SDCARDCONTROL_STATE_SERVICE_TASKS;
        break;
//============================ FILE WRITE ROUTINE ==============================
/* This section of code opens a file to write or update a file */
        case OPEN_FILE:
            sprintf(file_path, "%s/", buffer);
            strcat(file_path, new_file);
            sdcardcontrolData.fileHandle1 = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_READ)); // Checks if the file exist
            
            if(sdcardcontrolData.fileHandle1 == SYS_FS_HANDLE_INVALID){
            /* Could not open the file. File doesn´t exist, new file is created and add to Directory File */
                sdcardcontrolData.fileHandle = SYS_FS_FileOpen("/mnt/mchpSite1/MUESTRAS/DIRECTORIO.txt", (SYS_FS_FILE_OPEN_APPEND));
                
                if(sdcardcontrolData.fileHandle != SYS_FS_HANDLE_INVALID)
                    SYS_FS_FilePrintf(sdcardcontrolData.fileHandle, "%s\r\n", new_file);
                
                SYS_FS_FileClose(sdcardcontrolData.fileHandle);
            }
            else{
            /* File exist. Close the file and re-opens in write mode */
                SYS_FS_FileClose(sdcardcontrolData.fileHandle1);
                sdcardcontrolData.state = WRITE_TO_FILE;
            }
        break;
     
        case WRITE_TO_FILE:
            sdcardcontrolData.fileHandle1 = SYS_FS_FileOpen(file_path, (SYS_FS_FILE_OPEN_APPEND)); // Re-opens the file to write
            
            if(sdcardcontrolData.fileHandle1 != SYS_FS_HANDLE_INVALID){
                /* Writes to the requested file */
                if(SYS_FS_FilePrintf(sdcardcontrolData.fileHandle1, "%s\r\n", string2write) != SYS_FS_RES_SUCCESS)
                    sdcardcontrolData.state = SDCARDCONTROL_ERROR;
                else{
                    SYS_FS_FileClose(sdcardcontrolData.fileHandle1);
                    task = 0;
                    sdcardcontrolData.state = SDCARDCONTROL_STATE_SERVICE_TASKS;
                }
            }            
        break;
//############################### ERROR ROUTINE ################################
        case SDCARDCONTROL_ERROR:
        /*HEARTBEAT LED ROUTINE*/
            if(SYS_TMR_TickCountGet() - startTick >= SYS_TMR_TickCounterFrequencyGet()/16ul){
                startTick = SYS_TMR_TickCountGet();
                LEDstate ^= BSP_LED_STATE_ON;
                BSP_LEDStateSet(APP_LED_0, LEDstate);
            }
        break;
            
        /* The default state should never be executed. */
        default:
            
        break;
    }
}

/*******************************************************************************
 End of File
 */
