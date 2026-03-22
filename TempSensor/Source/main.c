#include "PJ_RPI.h"
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "gpiod.h"
#include "logger.h"

int gpio_26_val = -1;
int gpio_27_val = -1;

int main(int argc, char *argv[])
{
	int use_gpiod = (argc > 1 && strcmp(argv[1], "--gpiod") == 0);

	LogSessionStart();

	if (use_gpiod)
	{
		printf("Using libgpiod for GPIO access.\n");

		GPIOD_Init(26, 0); // Set GPIO 26 as input
		GPIOD_Init(27, 0); // Set GPIO 27 as input

		while (1)
		{
			GPIOD_Log(26, &gpio_26_val);
			GPIOD_Log(27, &gpio_27_val);
			sleep(1);
		}
	}
	else
	{
		if (map_peripheral(&gpio) == -1)
		{
			printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
			return -1;
		}

		GPIO_Init(26, 0); // Set GPIO 26 as input
		GPIO_Init(27, 0); // Set GPIO 27 as input

		while (1)
		{
			GPIO_Log(26, &gpio_26_val);
			GPIO_Log(27, &gpio_27_val);
			sleep(1);
		}
	}

	LogSessionEnd();
	return 0;
}