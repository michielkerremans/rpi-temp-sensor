#ifndef MQTT_H
#define MQTT_H

int mqtt_init(const char *address, const char *clientid);
int mqtt_publish(const char *topic, const char *payload);
void mqtt_cleanup();

#endif