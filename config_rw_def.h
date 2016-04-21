#ifndef __CONFIG_RW_DEF_H
#define __CONFIG_RW_DEF_H

/* ANSI Includes */
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/** Default buffer manipulation size used during file read/write operations */
#define MAX_FILE_RW_BUFFER_SIZE     1024

/** Default preference file name */
#define PREFERENCE_FILE_NAME (unsigned char*)"wifiagent.properties"

/** Polling interval key in preference file */
#define PREFERENCE_WIFI_INTERFACE "wifi.scanning.interface"

#define PLAT_STR(x)     ((char *)x)
#define SD_STATUS_GOOD 0
#define SD_STATUS_BAD -1

#ifndef SD_INT8
#define SD_INT8 char
#endif

#ifndef SD_INT16
#define SD_INT16 short
#endif

#ifndef SD_INT32
#define SD_INT32 long
#endif

#ifndef SD_UINT8
#define SD_UINT8 unsigned char
#endif

#ifndef SD_UINT16
#define SD_UINT16 unsigned short
#endif

#ifndef SD_UINT32
#define SD_UINT32 unsigned long
#endif

/** Open a preference file for manipulation */
FILE        *CfgFileOpen(SD_UINT8 *preferenceFileName);
/** Close a previously opened preference file */
SD_INT32   CfgFileClose(FILE *preferenceFile);
/** Retrieve a property value from a preference file */
SD_UINT8   *CfgFileGetProperty(FILE *preferenceFile, SD_UINT8 *key);
/** Assign a property value to a preference file */
SD_INT32 CfgFileSetProperty(FILE *preferenceFile, SD_UINT8 *key, SD_UINT8 *value);

#endif
