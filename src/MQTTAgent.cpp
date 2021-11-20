/*
 * MQTTAgent.cpp
 *
 *  Created on: 15 Nov 2021
 *      Author: jondurrant
 */

#include "MQTTAgent.h"
#include "MQTTDebug.h"
#include <stdlib.h>


const char * MQTTAgent::WILLTOPICFORMAT = "TNG/%s/LC";
const char * MQTTAgent::WILLPAYLOAD = "{'online':0}";
const char * MQTTAgent::ONLINEPAYLOAD = "{'online':1}";



MQTTAgent::MQTTAgent(size_t rxBufSize, size_t txBufSize) {
	xRxBufSize = rxBufSize;
	xTxBufSize = txBufSize;
}

MQTTAgent::~MQTTAgent() {
	// TODO Auto-generated destructor stub
}


bool MQTTAgent::init(){
	pMQTTClient = lwesp_mqtt_client_api_new(xRxBufSize, xTxBufSize);
	if (pMQTTClient == NULL) {
		dbg("MQTTAgent::init mqtt  failed\n");
		return false;
	} else {
		dbg("MQTTAgent::init complete\n");
	}
	return true;
}


/***
 * Set credentials
 * @param user - string pointer. Not copied so pointer must remain valid
 * @param passwd - string pointer. Not copied so pointer must remain valid
 * @param id - string pointer. Not copied so pointer must remain valid. I
 * f not provide ID will be user
 * @return lwespOK if succeeds
 */
void MQTTAgent::credentials(const char * user, const char * passwd, const char * id){
	this->user = user;
	this->passwd = passwd;
	if (id != NULL){
		this->id = id;
	} else {
		this->id = user;
	}
	dbg("MQTT Credentials Id=%s, usr=%s, pwd=%s\n", this->id, this->user, this->passwd);
}

/***
 * Connect to mqtt server
 * @param target - hostname or ip address, Not copied so pointer must remain valid
 * @param port - port number
 * @param ssl - unused
 * @return
 */
bool MQTTAgent::connect(char * target, lwesp_port_t  port, bool ssl){
	this->target = target;
	this->port = port;
	connState = MQTTConn;
	return true;
}



/***
*  create the vtask, will get picked up by scheduler
*
*  */
void MQTTAgent::start(UBaseType_t priority){
	if (init() ){
		xTaskCreate(
			MQTTAgent::vTask,
			"MQTTAgent",
			512,
			( void * ) this,
			priority,
			&xHandle
		);
	}
}

/***
 * Internal function used by FreeRTOS to run the task
 * @param pvParameters
 */
 void MQTTAgent::vTask( void * pvParameters ){
	 MQTTAgent *task = (MQTTAgent *) pvParameters;
	 if (task->init()){
		 task->run();
	 }
 }


 void MQTTAgent::run(){
	 dbg("MQTTAgent run\n");


	 for(;;){

		 switch(connState){
		 case Offline: {
			 break;
		 }
		 case MQTTConn: {
			 mqttConn();
			 break;
		 }
		 case MQTTConned: {
			 pubToTopic(willTopic, ONLINEPAYLOAD, strlen(ONLINEPAYLOAD));
			 mqttSub();
			 connState = Online;
			 break;
		 }
		 case MQTTRecon: {

			 break;
		 }
		 case Online: {
			 mqttRec();

			 break;
		 }
		 default:{

		 }

		 };

		 taskYIELD();
	 }



 }




 /***
* Returns the id of the client
* @return
*/
const char * MQTTAgent::getId(){
	return id;
}

/***
* Publish message to topic
* @param topic - zero terminated string. Copied by function
* @param payload - payload as pointer to memory block
* @param payloadLen - length of memory block
*/
bool MQTTAgent::pubToTopic(const char * topic, const void * payload, size_t payloadLen){

	lwespr_t status;
	dbg("Publish: %s:%.*s\n", topic, payloadLen, payload);
	status = lwesp_mqtt_client_api_publish(pMQTTClient, topic,
			payload, payloadLen, LWESP_MQTT_QOS_AT_LEAST_ONCE, false);
	return (status == lwespOK);
}

/***
* Close connection
*/
void MQTTAgent::close(){

}

/***
* Route a message to the router object
* @param topic - non zero terminated string
* @param topicLen - topic length
* @param payload - raw memory
* @param payloadLen - payload length
*/
void MQTTAgent::route(const char * topic, size_t topicLen, const void * payload, size_t payloadLen){
	if (pRouter != NULL){
		pRouter->route(topic, topicLen, payload, payloadLen, this);
	}
}





 MQTTRouter* MQTTAgent::getRouter()  {
	return pRouter;
}

void MQTTAgent::setRouter( MQTTRouter *pRouter) {
	this->pRouter = pRouter;
}

bool MQTTAgent::mqttConn(){
	lwesp_mqtt_conn_status_t conn_status;

	sprintf(willTopic, WILLTOPICFORMAT, id);

	xMqttClientInfo.id = id;
	xMqttClientInfo.user = user;
	xMqttClientInfo.pass = passwd;
	xMqttClientInfo.keep_alive = 10;
	xMqttClientInfo.will_topic = willTopic;
	xMqttClientInfo.will_message = WILLPAYLOAD;
	xMqttClientInfo.will_qos = LWESP_MQTT_QOS_AT_LEAST_ONCE;

	conn_status = lwesp_mqtt_client_api_connect(pMQTTClient,
			target, port, &xMqttClientInfo);
	if (conn_status == LWESP_MQTT_CONN_STATUS_ACCEPTED) {
		connState = MQTTConned;
		dbg("Connected and accepted!\r\n");
	} else {
		connState = Offline;
		dbg("Connect API response: %d\r\n", (int)conn_status);
		return false;
	}
	return true;
}

bool MQTTAgent::subToTopic(const char * topic, const uint8_t QoS){
	dbg("Subscribe tp %s\n", topic);

	lwespr_t status;
	lwesp_mqtt_qos_t q;
	switch (QoS){
	case 0:{
		q = LWESP_MQTT_QOS_AT_MOST_ONCE;
		break;
	}
	case 1:{
		q = LWESP_MQTT_QOS_AT_LEAST_ONCE;
		break;
	}
	case 2:{
		q = LWESP_MQTT_QOS_EXACTLY_ONCE;
		break;
	}
	default:{
		q = LWESP_MQTT_QOS_AT_MOST_ONCE;
		break;
	}
	}
	status = lwesp_mqtt_client_api_subscribe( pMQTTClient,
			topic, q);
	return (status == lwespOK);
}


bool MQTTAgent::mqttSub(){

	if (pRouter != NULL){
		pRouter->subscribe(this);
		return true;
	}
	return false;
}

bool MQTTAgent::mqttRec(){
	lwespr_t res;
	lwesp_mqtt_client_api_buf_p buf;
	res = lwesp_mqtt_client_api_receive(pMQTTClient, &buf, 5000);
	if (res == lwespOK) {
		if (buf != NULL) {
			printf("Publish received!\r\n");
			printf("Topic: %s, payload: %s\r\n", buf->topic, buf->payload);
			pRouter->route(buf->topic, buf->topic_len,
					buf->payload, buf->payload_len, this );
			lwesp_mqtt_client_api_buf_free(buf);
			buf = NULL;
		}
	} else if (res == lwespCLOSED) {
		printf("MQTT connection closed!\r\n");
		connState = Offline;
		return false;
	} else if (res == lwespTIMEOUT) {
		printf("Timeout on MQTT receive function. Manually publishing.\r\n");
		pubToTopic(willTopic, ONLINEPAYLOAD, strlen(ONLINEPAYLOAD));
	}
	return true;
}


