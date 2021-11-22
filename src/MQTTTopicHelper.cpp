/*
 * MQTTTopicHelper.cpp
 *
 *  Created on: 21 Nov 2021
 *      Author: jondurrant
 */

#include "MQTTTopicHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>


MQTTTopicHelper::MQTTTopicHelper() {
	// TODO Auto-generated constructor stub

}

MQTTTopicHelper::~MQTTTopicHelper() {
	// TODO Auto-generated destructor stub
}

/***
 * Get length of the lifecycle topic, to allow dynamic creation of string
 * @param id
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 * @return
 */
size_t MQTTTopicHelper::lenLifeCycleTopic(const char *id, const char *name){
	size_t res = 0;
	res = strlen(MQTT_TOPIC_THING_HEADER) +
		  strlen(MQTT_TOPIC_LIFECYCLE) +
		  strlen(id) +
		  strlen(name) +
		  4;
	return res;
}

/***
 * generate the lifecycle topic for thing
 * @param topic - out to write the topic
 * @param id - id of the thing
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 */
void  MQTTTopicHelper::genLifeCycleTopic(char *topic, const char *id, const char *name){
	sprintf(topic, "%s/%s/%s/%s",MQTT_TOPIC_THING_HEADER, id,
			MQTT_TOPIC_LIFECYCLE, name  );

}


/***
 * Get length of the thing topic, under the ID in structure
 * @param id - string id of the thing
 * @param topic - string name of the topic
 * @return
 */
size_t MQTTTopicHelper::lenThingTopic(const char *id, const char *name){
	size_t res = 0;
	res = strlen(MQTT_TOPIC_THING_HEADER) +
		  strlen(id) +
		  strlen(MQTT_TOPIC_HEADER) +
		  strlen(name) +
		  5 ;
	return res;
}

/***
 * Generate the thing topic full name
 * @param topic - output to write topic too
 * @param id - string id of the thing
 * @param name - string name of the topic
 */
void MQTTTopicHelper::genThingTopic(char * topic, const char *id, const char *name){
	sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
			MQTT_TOPIC_HEADER, name
			);
}

/***
 * Get length of the Group topic, under the ID in structure
 * @param grp - string  of the group name
 * @param name - string name of the topic
 * @return
 */
size_t  MQTTTopicHelper::lenGroupTopic(const char *grp, const char *name){
	size_t res = 0;
	res = strlen(MQTT_TOPIC_GROUP_HEADER) +
		  strlen(grp) +
		  strlen(name) +
		  4;
	return res;
}

/***
 * Generate the group topic full name
 * @param topic - output to write topic too
 * @param grp - string of group anme
 * @param name - string name of the topic
 */
void  MQTTTopicHelper::genGroupTopic(char * topic, const char *grp, const char *name){
	sprintf(topic, "%s/%s/%s", MQTT_TOPIC_GROUP_HEADER,
			grp,
			name
			);
}


