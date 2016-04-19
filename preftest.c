#include "prefs.h"

int main()
{
	unsigned char property[512] = {0};
	unsigned char out[512] = {0};
	sprintf((char*)property, "%s", "wifi.scanning.interface");
	CoreGetNetAgentPropertyV2(property, out);
	printf("out=%s\n",out);
	sprintf((char*) out, "%s", "wlan0");
	CoreSetNetAgentPropertyV2(property, out);
	CoreGetNetAgentPropertyV2(property, out);
	printf("property=%s\n", out);
	return 0;
}
