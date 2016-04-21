#include <stdio.h>
#include "config_rw_lib.h"

int main()
{
	unsigned char property[512] = {0};
	unsigned char out[512] = {0};
	sprintf((char*)property, "%s", "wifi.scanning.interface");
	CfgFileReadProperty(property, out);
	printf("out=%s\n",out);
	sprintf((char*) out, "%s", "wlan0");
	CfgFileWriteProperty(property, out);
	CfgFileReadProperty(property, out);
	printf("property=%s\n", out);
	return 0;
}
