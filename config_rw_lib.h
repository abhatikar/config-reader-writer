#ifndef __CONFIG_RW_LIB_H
#define __CONFIG_RW_LIB_H

long CfgFileWriteProperty(unsigned char *configKey, unsigned char *configValue);
long CfgFileReadProperty(unsigned char *configKey, unsigned char *configValue);

#endif
