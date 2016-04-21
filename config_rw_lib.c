#include "config_rw_def.h"
#include "config_rw_lib.h"

/** Internal Functions **/

FILE *CfgFileOpen(SD_UINT8 *cfgfileName)
{
	FILE    *cfgfile = NULL;

	if (cfgfileName != NULL)
	{
		cfgfile = fopen(PLAT_STR(cfgfileName), "r+");
		if (cfgfile == NULL)
		{
			printf("CfgFileOpen: Failed to open preference file (%s).", cfgfileName);
		}
	}
	return (cfgfile);
}

SD_INT32 CfgFileClose(FILE *cfgfile)
{
	int status = SD_STATUS_BAD;
	if (cfgfile != NULL)
	{
		status = ((fclose(cfgfile) == 0) ? SD_STATUS_GOOD : SD_STATUS_BAD);
	}
	cfgfile = NULL;
	return (status);
}

SD_UINT8 *CfgFileGetProperty(FILE *cfgfile, SD_UINT8 *key)
{
	SD_INT8    *seps   = "=\n";
	SD_UINT8   *token  = NULL;
	SD_UINT8   rBuffer[MAX_FILE_RW_BUFFER_SIZE];
	SD_UINT32  isKeyFound = 0;

	fseek(cfgfile, 0, SEEK_SET);

	/* Cycle until end of file reached */
	while(!feof(cfgfile))
	{
		if (fgets(PLAT_STR(rBuffer), MAX_FILE_RW_BUFFER_SIZE, cfgfile) != NULL)
		{
			if(ferror(cfgfile))
			{
				printf("CfgFileGetProperty: Failed to read preference file.");
				break;
			}

			/* Look at the line read in from the file */
			/* Establish string and get the first token */
			token = (SD_UINT8 *)strtok(PLAT_STR(rBuffer), PLAT_STR(seps));
			while (token != NULL)
			{
				if (strcasecmp(PLAT_STR(token), PLAT_STR(key)) == 0)
				{
					isKeyFound = 1;
					token = (SD_UINT8 *)strtok(NULL, PLAT_STR(seps));
					break;
				}
				/* While there are tokens in "string" */
				/* Get next token */
				token = (SD_UINT8 *)strtok(NULL, PLAT_STR(seps));
			}
			/* If the key is found, break from search loop */
			if (isKeyFound)
				break;
		}
	}
	return (token);
}

SD_INT32 CfgFileSetProperty(FILE *cfgfile, SD_UINT8 *key, SD_UINT8 *value)
{
	SD_INT8    *seps   = "=";
	SD_INT8    *token;
	SD_UINT8   buffer[MAX_FILE_RW_BUFFER_SIZE];
	SD_UINT8   tmpBuffer[MAX_FILE_RW_BUFFER_SIZE];
	SD_UINT32  found = 0;
	SD_INT32   error = SD_STATUS_GOOD;
	FILE       *tmpFile = tmpfile();

	fseek(cfgfile, 0, SEEK_SET);

	/* Cycle, reading the preference file, until end of file reached: */
	while(!feof(cfgfile))
	{
		if (fgets(PLAT_STR(buffer), MAX_FILE_RW_BUFFER_SIZE, cfgfile) != NULL)
		{
			if(ferror(cfgfile))
			{
				printf("Failed to read preference file!");
				error = SD_STATUS_BAD;
				break;
			}

			memcpy(PLAT_STR(tmpBuffer), PLAT_STR(buffer), MAX_FILE_RW_BUFFER_SIZE);

			/* Establish string and get the first token: */
			token = (char *)strtok(PLAT_STR(tmpBuffer), PLAT_STR(seps));
			while(token != NULL)
			{
				if (strcmp(PLAT_STR(token), PLAT_STR(key)) == 0)
				{
					found = 1;
					strcpy(PLAT_STR(buffer), PLAT_STR(key));
					strcat(PLAT_STR(buffer), "=");
					token = (char *)strtok(NULL, PLAT_STR(seps));
					strcat(PLAT_STR(buffer), PLAT_STR(value));
					strcat(PLAT_STR(buffer), "\n");
					break;
				}
				/* While there are tokens in "string" */
				/* Get next token: */
				token = (char *)strtok(NULL, PLAT_STR(seps));
			}

			fputs(PLAT_STR(buffer), tmpFile);
			if (ferror(tmpFile))
			{
				printf("Failed to write to temp file!");
				error = SD_STATUS_BAD;
				found = 0;
				break;
			}
		}
	}

	/* Add key/value if not found */
	if (!found)
	{
		strcpy(PLAT_STR(buffer), PLAT_STR(key));
		strcat(PLAT_STR(buffer), "=");
		strcat(PLAT_STR(buffer), PLAT_STR(value));
		strcat(PLAT_STR(buffer), "\n");

		fputs(PLAT_STR(buffer), tmpFile);
		if (ferror(tmpFile))
		{
			printf("Failed to write to temp file!");
			error = SD_STATUS_BAD;
			found = 0;
		}
		else
		{
			found = 1;
		}
	}

	if (found)
	{
		/* we have to write the contents of the tmp file into the properties file*/
		fseek(cfgfile, 0, SEEK_SET);
		fseek(tmpFile, 0, SEEK_SET);

		if ( ftruncate( fileno(cfgfile), 0 ) != 0 )
		{
			CfgFileClose(tmpFile);
			return (SD_STATUS_BAD);
		}

		/* Cycle until end of file reached: */
		while (!feof(tmpFile))
		{
			if (fgets(PLAT_STR(buffer), MAX_FILE_RW_BUFFER_SIZE, tmpFile) != NULL)
			{
				if (ferror(tmpFile))
				{
					printf("Failed to read temp file!");
					error = SD_STATUS_BAD;
					break;
				}
				fputs(PLAT_STR(buffer), cfgfile);

				if (ferror(cfgfile))
				{
					printf("Failed to write to preference file!");
					error = SD_STATUS_BAD;
					break;
				}
			}
		}
	}
	CfgFileClose(tmpFile);

	fflush(cfgfile);

	return (error);
}

/** Library Functions **/

SD_INT32 CfgFileReadProperty(SD_UINT8 *configKey, SD_UINT8 *configValue)
{
	SD_INT32    retValue;
	FILE        *agentPreferenceFile;
	SD_UINT8    *agentProperty;

	retValue = SD_STATUS_BAD;

	agentPreferenceFile = CfgFileOpen(PREFERENCE_FILE_NAME);
	if (agentPreferenceFile != NULL)
	{
		agentProperty = (SD_UINT8*)CfgFileGetProperty(agentPreferenceFile, configKey);
		if (agentProperty != NULL)
		{
			strcpy((char*)configValue, (char*)agentProperty);
			retValue = SD_STATUS_GOOD;
		}

		CfgFileClose(agentPreferenceFile);
	}

	return (retValue);
}

SD_INT32 CfgFileWriteProperty(SD_UINT8 *configKey, SD_UINT8 *configValue )

{
	SD_INT32    retValue;
	FILE        *agentPreferenceFile;

	retValue = SD_STATUS_BAD;

	agentPreferenceFile = CfgFileOpen(PREFERENCE_FILE_NAME);
	if (agentPreferenceFile != NULL)
	{
		retValue = CfgFileSetProperty(agentPreferenceFile, configKey, configValue);
		CfgFileClose(agentPreferenceFile);

	}

	return (retValue);
}
