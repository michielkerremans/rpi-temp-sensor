#include "mqtt.h"
#include <MQTTClient.h>
#include <stdio.h>
#include <string.h>

#define QOS 1
#define TIMEOUT 10000L

static MQTTClient client;

int mqtt_init(const char *address, const char *clientid)
{
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;

  MQTTClient_create(&client, address, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  rc = MQTTClient_connect(client, &conn_opts);
  if (rc != MQTTCLIENT_SUCCESS)
  {
    printf("Failed to connect to MQTT broker, return code %d\n", rc);
    return rc;
  }
  return 0;
}

int mqtt_publish(const char *topic, const char *payload)
{
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  pubmsg.payload = (void *)payload;
  pubmsg.payloadlen = strlen(payload);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;
  return MQTTClient_publishMessage(client, topic, &pubmsg, NULL);
}

void mqtt_cleanup()
{
  MQTTClient_disconnect(client, TIMEOUT);
  MQTTClient_destroy(&client);
}