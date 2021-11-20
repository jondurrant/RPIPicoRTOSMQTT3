/*
 * MQTTAgent.h
 *
 *  Created on: 15 Nov 2021
 *      Author: jondurrant
 */

#ifndef MQTTAGENT_H_
#define MQTTAGENT_H_

#include "FreeRTOS.h"
#include "lwesp/lwesp.h"
#include "lwesp/apps/lwesp_mqtt_client_api.h"

#include "MQTTInterface.h"
#include "MQTTRouter.h"


#define MQTT_AGENT_NETWORK_BUFFER_SIZE 512
#define TCP_BUFFER_SIZE 512


enum MQTTState {  Offline, MQTTConn, MQTTRecon, MQTTConned, Online};

class MQTTAgent: public MQTTInterface {
public:
	MQTTAgent(size_t rxBufSize, size_t txBufSize);
	virtual ~MQTTAgent();

	/***
	 * Set credentials
	 * @param user - string pointer. Not copied so pointer must remain valid
	 * @param passwd - string pointer. Not copied so pointer must remain valid
	 * @param id - string pointer. Not copied so pointer must remain valid. I
	 * f not provide ID will be user
	 * @return lwespOK if succeeds
	 */
	void credentials(const char * user, const char * passwd, const char * id = NULL );

	/***
	 * Connect to mqtt server
	 * @param target - hostname or ip address, Not copied so pointer must remain valid
	 * @param port - port number
	 * @param ssl - unused
	 * @return
	 */
	 bool connect(char * target, lwesp_port_t  port, bool ssl=false);

	 /***
	 *  create the vtask, will get picked up by scheduler
	 *
	 *  */
	void start(UBaseType_t priority = tskIDLE_PRIORITY);

	/***
	 * Returns the id of the client
	 * @return
	 */
	virtual const char * getId();

	/***
	 * Publish message to topic
	 * @param topic - zero terminated string. Copied by function
	 * @param payload - payload as pointer to memory block
	 * @param payloadLen - length of memory block
	 */
	virtual bool pubToTopic(const char * topic, const void * payload, size_t payloadLen);

	/***
	 * Close connection
	 */
	virtual void close();

	/***
	 * Route a message to the router object
	 * @param topic - non zero terminated string
	 * @param topicLen - topic length
	 * @param payload - raw memory
	 * @param payloadLen - payload length
	 */
	virtual void route(const char * topic, size_t topicLen, const void * payload, size_t payloadLen);

	 MQTTRouter* getRouter() ;

	void setRouter( MQTTRouter *pRouter = NULL);


	virtual bool subToTopic(const char * topic, const uint8_t QoS=0);


private:
	static void vTask( void * pvParameters );
	bool init();
	void run();
	bool mqttConn();
	bool mqttSub();
	bool mqttRec();

	const char * user;
	const char * passwd;
	const char * id;
	const char * target = NULL;
	lwesp_port_t port = 1883 ;
	bool ssl = false;
	MQTTRouter * pRouter = NULL;

	TaskHandle_t xHandle = NULL;

	MQTTState connState = Offline;

	static const char * WILLTOPICFORMAT;
	char willTopic[20];
	static const char * WILLPAYLOAD;
	static const char * ONLINEPAYLOAD;

	lwesp_mqtt_client_api_p pMQTTClient;
	lwesp_mqtt_client_info_t xMqttClientInfo;
	size_t xRxBufSize;
	size_t xTxBufSize;



};

#endif /* MQTTAGENT_H_ */
