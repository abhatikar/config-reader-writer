#include "prefs.h"

FILE *PrefOpenPreferenceFile(SD_UINT8 *preferenceFileName)
{
	FILE    *preferenceFile = NULL;

	if (preferenceFileName != NULL)
	{
		printf("Opening file:  %s.\n", preferenceFileName);
		preferenceFile = fopen(PLAT_STR(preferenceFileName), "r+");
		if (preferenceFile == NULL)
		{
			printf("PrefOpenPreferenceFile: Failed to open preference file (%s).", preferenceFileName);
		}
	}
	return (preferenceFile);
}

int PrefClosePreferenceFile(FILE *preferenceFile)
{
	int status = SD_STATUS_BAD;
	if (preferenceFile != NULL)
	{
		status = ((fclose(preferenceFile) == 0) ? SD_STATUS_GOOD : SD_STATUS_BAD);
	}
	preferenceFile = NULL;
	return (status);
}

char *PrefGetProperty(FILE *preferenceFile, char *key)
{
	char    *seps   = "=\n";
	char    *token  = NULL;
	char    rBuffer[MAX_FILE_RW_BUFFER_SIZE];
	int     isKeyFound = 0;
	int openLocal = 0;

	fseek(preferenceFile, 0, SEEK_SET);

	/* Cycle until end of file reached */
	while(!feof(preferenceFile))
	{
		if (fgets(PLAT_STR(rBuffer), MAX_FILE_RW_BUFFER_SIZE, preferenceFile) != NULL)
		{
			if(ferror(preferenceFile))
			{
				printf("PrefGetProperty: Failed to read preference file.");
				break;
			}

			/* Look at the line read in from the file */
			/* Establish string and get the first token */
			token = (char *)strtok(PLAT_STR(rBuffer), PLAT_STR(seps));
			while (token != NULL)
			{
				if (strcasecmp(PLAT_STR(token), PLAT_STR(key)) == 0)
				{
					isKeyFound = 1;
					token = (char *)strtok(NULL, PLAT_STR(seps));
					break;
				}
				/* While there are tokens in "string" */
				/* Get next token */
				token = (char *)strtok(NULL, PLAT_STR(seps));
			}
			/* If the key is found, break from search loop */
			if (isKeyFound)
				break;
		}
	}
	return (token);
}

int    PrefSetProperty(FILE *preferenceFile, char *key, char *value)
{
	char    *seps   = "=";
	char    *token;
	char    buffer[MAX_FILE_RW_BUFFER_SIZE];
	char    tmpBuffer[MAX_FILE_RW_BUFFER_SIZE];
	int     found = 0;
	int     error = SD_STATUS_GOOD;
	FILE    *tmpFile = tmpfile();

	fseek(preferenceFile, 0, SEEK_SET);

	/* Cycle, reading the preference file, until end of file reached: */
	while(!feof(preferenceFile))
	{
		if (fgets(PLAT_STR(buffer), MAX_FILE_RW_BUFFER_SIZE, preferenceFile) != NULL)
		{
			if(ferror(preferenceFile))
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
		fseek(preferenceFile, 0, SEEK_SET);
		fseek(tmpFile, 0, SEEK_SET);

		if ( ftruncate( fileno(preferenceFile), 0 ) != 0 )
		{
		#if 0
			if(openLocal == 1)
			{
				PrefClosePreferenceFile(preferenceFile);
				openLocal = 0;
			}
		#endif
			PrefClosePreferenceFile(tmpFile);
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
				fputs(PLAT_STR(buffer), preferenceFile);

				if (ferror(preferenceFile))
				{
					printf("Failed to write to preference file!");
					error = SD_STATUS_BAD;
					break;
				}
			}
		}
	}
	PrefClosePreferenceFile(tmpFile);

	fflush(preferenceFile);

	return (error);
}

SD_INT32 CoreGetNetAgentPropertyV2(SD_UINT8 *configKey, SD_UINT8 *configValue)
{
	SD_INT32    retValue;
	FILE        *agentPreferenceFile;
	SD_UINT8    *agentProperty;

	retValue = SD_STATUS_BAD;

	agentPreferenceFile = PrefOpenPreferenceFile(PREFERENCE_FILE_NAME);
	if (agentPreferenceFile != NULL)
	{
		agentProperty = (SD_UINT8*)PrefGetProperty(agentPreferenceFile,(char*) configKey);
		if (agentProperty != NULL)
		{
			strcpy((char*)configValue, (char*)agentProperty);
			retValue = SD_STATUS_GOOD;
		}

		PrefClosePreferenceFile(agentPreferenceFile);
	}

	return (retValue);
}

SD_INT32 CoreSetNetAgentPropertyV2(SD_UINT8 *configKey, SD_UINT8 *configValue )

{
	SD_INT32    retValue;
	FILE        *agentPreferenceFile;

	retValue = SD_STATUS_BAD;

	agentPreferenceFile = PrefOpenPreferenceFile(PREFERENCE_FILE_NAME);
	if (agentPreferenceFile != NULL)
	{
		retValue = PrefSetProperty(agentPreferenceFile,(char*) configKey,(char*) configValue);
		PrefClosePreferenceFile(agentPreferenceFile);

	}

	return (retValue);
}

int main()
{
	char test[512] = {0};
	CoreGetNetAgentPropertyV2("wifi.scanning.interface", test);
	printf("test=%s\n",test);
	strcpy(test, "wlan0");
	CoreSetNetAgentPropertyV2("wifi.scanning.interface", test );
	CoreGetNetAgentPropertyV2("wifi.scanning.interface", test);
	printf("test=%s\n",test);
	return 0;
}
