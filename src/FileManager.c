/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    filemanager.c

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

#include "filemanager.h"

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

SYS_FS_FSTAT stat;

char buffer[32],                                                                // Buffer that stores the mount name of uSD Card
     DirectoryFiles_path[50],                                                   // Path to directory files list
     DirectoryFile[100],                                                        // Directory's files names
     longFileName[300];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
/* READS "DIRECTORIO" TO LIST IN WEB PAGE THE DIRECTORY FILES                   */
void ReciveName(void){
    stat.lfname = longFileName;
    stat.lfsize = 300;
    
    switch(filemanagerData.substate){
        case FILEMANAGER_OPEN_DIRECTORIO:{
            filemanagerData.dirHandle = SYS_FS_DirOpen(DirectoryFiles_path);

            if(filemanagerData.dirHandle != SYS_FS_HANDLE_INVALID){             // Directory open is successful
                filemanagerData.substate = FILEMANAGER_READ_DIRECTORIO;
            }
        }break;
        
        case FILEMANAGER_READ_DIRECTORIO:{
            if(SYS_FS_DirRead(filemanagerData.dirHandle, &stat) == SYS_FS_RES_FAILURE)
                filemanagerData.substate = FILEMANAGER_ERROR;
            else{
                // Directory read succeeded.
                if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0')){
                    filemanagerData.EOD = true;
                    memset(filemanagerData.DirectoryName, 0, strlen(filemanagerData.DirectoryName));
                    SYS_FS_DirClose(filemanagerData.dirHandle);
                    filemanagerData.substate = FILEMANAGER_OPEN_DIRECTORIO;
                }
                else{
                    strcpy(filemanagerData.DirectoryName, stat.lfname);
                    SYS_CONSOLE_PRINT("Archivo: %s\r\n", filemanagerData.DirectoryName);
                }
            }
        }break;
    }
}

void AddNewFile(char newfilename[50], char dirlocation[50]){
    sprintf(DirectoryFile, "/Directorio/%s_Dir.txt", dirlocation);
    filemanagerData.fileHandle = SYS_FS_FileOpen(DirectoryFile, (SYS_FS_FILE_OPEN_APPEND));

    if(filemanagerData.fileHandle != SYS_FS_HANDLE_INVALID){
        SYS_FS_FilePrintf(filemanagerData.fileHandle, "%s\r\n", newfilename);
        SYS_FS_DirClose(filemanagerData.fileHandle);
    }
}

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
    void FILEMANAGER_Initialize ( void )

  Remarks:
    See prototype in filemanager.h.
 */

void FILEMANAGER_Initialize(void){
    /* Place the App state machine in its initial state. */
    filemanagerData.state = FILEMANAGER_STATE_INIT;
    filemanagerData.substate = FILEMANAGER_OPEN_DIRECTORIO;
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

/******************************************************************************
  Function:
    void FILEMANAGER_Tasks ( void )

  Remarks:
    See prototype in filemanager.h.
 */

void FILEMANAGER_Tasks(void){
    stat.lfname = longFileName;
    stat.lfsize = 300;
    /* Check the application's current state. */
    switch (filemanagerData.state){
        /* OPENS AND CREATES THE "DIRECTORIO" DIRECTORY WITH THE FILES WITH THE
         * NAMES IN EACH DIRECTORY FOUND IN ROOT                                */
        case FILEMANAGER_STATE_INIT:{
            if (SYS_FS_CurrentDriveGet(buffer) == SYS_FS_RES_SUCCESS){          // Checks if the volume is attached
                sprintf(DirectoryFiles_path, "%s/Directorio", buffer);
                filemanagerData.dirHandle = SYS_FS_DirOpen(DirectoryFiles_path);
            
                if(filemanagerData.dirHandle != SYS_FS_HANDLE_INVALID){         // Directory open is successful
                    SYS_FS_DirClose(filemanagerData.dirHandle);                 // Close Handle for "DIRECTORIO"
                }
                else 
                    SYS_FS_DirectoryMake("Directorio");                         // Creates "Directorio" directory to save the list of files names
                
                filemanagerData.dirHandle = SYS_FS_DirOpen(buffer);             // Reads the root to found "MUESTRAS" directories
                filemanagerData.state = FILEMANAGER_STATE_READ_ROOT;
            }
        }break;

        case FILEMANAGER_STATE_SERVICE_TASKS:{
            switch(filemanagerData.task){
                case 1:{
                    filemanagerData.DelStatus = false;
                    
                    if(filemanagerData.DelFile[0] != '\0'){
                        //sprintf(DirectoryFiles_path, "%s/%s", buffer, DelFile);
                        filemanagerData.state = FILEMANAGER_DELETE_FILE;
                    }
                }break;
                
                default:
                break;
            }
        }break;
        
        /* CREATES THE FILE THAT CONTAINS ALL THE NAMES IN THE DIRECTORY WIITH 
         * THE SAME NAME                                                        */
        case FILEMANAGER_STATE_READ_ROOT:{
            if(filemanagerData.dirHandle != SYS_FS_HANDLE_INVALID){             // Reading the root
                // Directory open is successful
                if(SYS_FS_DirRead(filemanagerData.dirHandle, &stat) != SYS_FS_RES_FAILURE){
                // Directory read succeeded.
                    if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0')){   // Reached the end of the root
                        if(SYS_FS_DirClose(filemanagerData.dirHandle) != SYS_FS_RES_FAILURE){// Close Handle for root
                            memset(filemanagerData.DirectoryName, 0, strlen(filemanagerData.DirectoryName));
                            memset(DirectoryFile, 0, strlen(DirectoryFile));
                            filemanagerData.state = FILEMANAGER_STATE_SERVICE_TASKS;
                        }
                    }
                    else if (stat.lfname[0] == 'M'){                            // Search for a "MUESTRAS" directory in root
                        strcpy(filemanagerData.DirectoryName, stat.lfname);
                        sprintf(DirectoryFile, "%s/%s_Dir.txt", DirectoryFiles_path, filemanagerData.DirectoryName);
                        filemanagerData.fileHandle = SYS_FS_FileOpen(DirectoryFile, (SYS_FS_FILE_OPEN_WRITE));
                        filemanagerData.dirHandle1 = SYS_FS_DirOpen(filemanagerData.DirectoryName);
                        
                        if(SYS_FS_DirRead(filemanagerData.dirHandle1, &stat) != SYS_FS_RES_FAILURE)
                            filemanagerData.state = FILEMANAGER_READ_FILES;
                    }
                }
            }
        }break;

        /* READS THE FILE´S NAME IN THE SELECTED DIRECTORY AND WRITES IN THE FILE 
         * WITH THE SAME NAME                                                   */
        case FILEMANAGER_READ_FILES:{
            // Reads the files in the selected directory to add to the "DIRECTORY" file
            if(SYS_FS_DirRead(filemanagerData.dirHandle1, &stat) != SYS_FS_RES_FAILURE){
                if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0')){       // Reached the end of the directory.
                    SYS_FS_FileClose(filemanagerData.fileHandle);               // Close the *_Dir.txt file 
                    SYS_FS_DirClose(filemanagerData.dirHandle1);                // And the Directory
                    filemanagerData.state = FILEMANAGER_STATE_READ_ROOT;
                }
                else if(stat.fname[0] != '.'){
                    SYS_FS_FilePrintf(filemanagerData.fileHandle, "%s\r\n", stat.lfname);
                }
            }
        }break;
        
        /* DELETES A FILE SELECTED FROM THE WEBPAGE */
        case FILEMANAGER_DELETE_FILE:{
            if(SYS_FS_FileDirectoryRemove(filemanagerData.DelFile) == SYS_FS_RES_FAILURE){
                filemanagerData.DelStatus = false;
            }
            else{
                strcpy(filemanagerData.DirectoryName, strtok(filemanagerData.DelFile, "/"));
                sprintf(DirectoryFile, "/mnt/mchpSite1/Directorio/%s_Dir.txt", filemanagerData.DirectoryName);
                filemanagerData.task = 0;
                filemanagerData.DelStatus = true;
                
                filemanagerData.fileHandle = SYS_FS_FileOpen(DirectoryFile, (SYS_FS_FILE_OPEN_WRITE));
                filemanagerData.dirHandle1 = SYS_FS_DirOpen(filemanagerData.DirectoryName);
                
                if(SYS_FS_DirRead(filemanagerData.dirHandle1, &stat) != SYS_FS_RES_FAILURE)
                            filemanagerData.state = FILEMANAGER_REWRITE_DIRECTORY;
            }
        }break;
        
        case FILEMANAGER_REWRITE_DIRECTORY:{
            if(SYS_FS_DirRead(filemanagerData.dirHandle1, &stat) != SYS_FS_RES_FAILURE){
                if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0')){       // Reached the end of the directory.
                    SYS_FS_FileClose(filemanagerData.fileHandle);               // Close the *_Dir.txt file 
                    SYS_FS_DirClose(filemanagerData.dirHandle1);                // Close the Directory
                    memset(filemanagerData.DelFile, 0, strlen(filemanagerData.DelFile));
                    filemanagerData.state = FILEMANAGER_STATE_SERVICE_TASKS;
                }
                else if(stat.fname[0] != '.'){
                    SYS_FS_FilePrintf(filemanagerData.fileHandle, "%s\r\n", stat.lfname);
                }
            }            
        }break;
        
        /* The default state should never be executed. */
        case FILEMANAGER_ERROR:
        default:
            /* TODO: Handle error in application's state machine. */
        break;
    }
}

/*******************************************************************************
 End of File
 */
