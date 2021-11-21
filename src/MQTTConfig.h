/***
 * MQTTConfig.h
 * Configuration for MQTT framework
 * Jon Durrant
 * 21-Nov-2021
 */

#ifndef MQTTCONFIG_H__
#define MQTTCONFIG_H__

#define LIBRARY_LOG_NAME "MQTT EXP"
#define LIBRARY_LOG_LEVEL LOG_DEBUG
//#define SdkLog( X ) printf X
#include <logging_stack.h>



#define MQTT_RECON_DELAY 3000 //Ticks to wait on reconnect


#endif
