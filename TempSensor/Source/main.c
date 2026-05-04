#include "PJ_RPI.h"
#include <stdio.h>
#include <string.h>
#include "gpio/gpio.h"
#include "gpio/gpiod.h"
#include "i2c/tc74.h"
#include "util/logger.h"
#include "mqtt/mqtt.h"

int gpio_26_val = -1;
int gpio_27_val = -1;

int main(int argc, char *argv[])
{
	int use_gpiod = (argc > 1 && strcmp(argv[1], "--gpiod") == 0);
	int use_temp = (argc > 1 && strcmp(argv[1], "--temp") == 0);

	int interval = 2;
	if (use_temp && argc > 2)
	{
		int val = atoi(argv[2]);
		if (val > 0)
			interval = val;
	}

	log_msg("error.log", "Session started.");

	if (use_gpiod)
	{
		printf("Using libgpiod for GPIO access.\n");

		GPIOD_Init(26, 0); // Set GPIO 26 as input
		GPIOD_Init(27, 0); // Set GPIO 27 as input

		GPIOD_Init(-30, 0); // Test invalid pin to trigger error handling
		log_msg("error.log", GPIOD_GetErrorMessage());

		while (1)
		{
			if (!GPIOD_Read(26, &gpio_26_val))
				log_msg("error.log", GPIOD_GetErrorMessage());

			if (!GPIOD_Read(27, &gpio_27_val))
				log_msg("error.log", GPIOD_GetErrorMessage());

			sleep(1);
		}
	}
	else if (use_temp)
	{
		GPIO_Init(); // Initialize GPIO memory mapping
		TC74_Init(); // Initialize TC74 memory mapping

		GPIO_Alt(2, 0); // Set GPIO 2 (SDA) to ALT0 (I2C1 SDA)
		GPIO_Alt(3, 0); // Set GPIO 3 (SCL) to ALT0 (I2C1 SCL)

		dump_bsc1_status();

		// Initialize MQTT client
		if (mqtt_init("tcp://localhost:1883", "TempSensorClient") != 0)
		{
			printf("MQTT init failed!\n");
			return 1;
		}

		uint8_t temp;
		char payload[32];

		while (1)
		{
			TC74_Read(0x48, &temp); // Read temperature from TC74 (address 0x48)
			snprintf(payload, sizeof(payload), "%d", temp);
			mqtt_publish("sensor/temperature", payload);
			sleep(interval);
		}
	}
	else
	{
		GPIO_Init();			// Initialize GPIO memory mapping
		GPIO_Mode(26, 0); // Set GPIO 26 as input
		GPIO_Mode(27, 0); // Set GPIO 27 as input

		while (1)
		{
			GPIO_Read(26, &gpio_26_val);
			GPIO_Read(27, &gpio_27_val);
			sleep(1);
		}
	}

	log_msg("error.log", "Session ended.");
	return 0;
}