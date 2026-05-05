#include "mqtt.h"
#include <MQTTClient.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define QOS 1
#define TIMEOUT 10000L

static MQTTClient client;
static char last_payload[32] = {0};

static int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  printf("Received on topic %s: %.*s\n", topicName, message->payloadlen, (char *)message->payload);
  snprintf(last_payload, sizeof(last_payload), "%.*s", message->payloadlen, (char *)message->payload);
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

int mqtt_init(const char *address, const char *clientid)
{
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;

  MQTTClient_create(&client, address, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL); // Set the message callback for incoming messages
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

void mqtt_subscribe(const char *topic)
{
  int rc = MQTTClient_subscribe(client, topic, QOS);
  if (rc != MQTTCLIENT_SUCCESS)
  {
    printf("Failed to subscribe to topic %s, return code %d\n", topic, rc);
    exit(1);
  }
}

void mqtt_cleanup()
{
  MQTTClient_disconnect(client, TIMEOUT);
  MQTTClient_destroy(&client);
}

const char *mqtt_get_last_payload(void)
{
  return last_payload;
}