#include <app_log.hpp>
#include <app_exception.hpp>
#include <app_led.hpp>

/**
 * Asset tag life cycle
 * 
 * 1. Advertise
 * 2. Connect
 * 3. Wait for characteristics writes or disconnect 
 * 4. Sleep configured amount of time
 */ 

PREPARE_GPIO(led0);

void main(void)
{
	LOG_INF("Beginning main() ...");
	led0_gpio.configure(GPIO_OUTPUT_ACTIVE);

	bool led_is_on = true;
	while (1) {
		led0_gpio.set(led_is_on);
		led_is_on = !led_is_on;

		LOG_INF("LED is on: %d", (int) led_is_on);
		log_process(false);
		k_sleep(k_timeout_t{K_SECONDS(1)});
	}
}
