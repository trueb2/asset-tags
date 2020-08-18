#include <app_log.hpp>
#include <app_exception.hpp>
#include <app_gpio.hpp>
#include <app_ble.hpp>
#include <app_saadc.hpp>

/**
 * Asset tag life cycle
 * 
 * 1. Advertise
 * 2. Connect
 * 3. Wait for characteristics writes or disconnect 
 * 4. Sleep configured amount of time
 */ 

PREPARE_GPIO(led0);

void main(void) {
	LOG_INF("Beginning main() ...");

	// Prepare hardware components/peripherals
	app_gpio::manager_t gpio_manager(std::make_tuple(led0_gpio, GPIO_OUTPUT_ACTIVE));

	// Prepare ble structures
	app_ble::manager_t ble_manager;

	// Handle lifecycle
	bool led_is_on = true;
	while (1) {
		led0_gpio.set(led_is_on);
		led_is_on = !led_is_on;
		LOG_INF("LED is on: %d", (int) led_is_on);

		app_saadc::measure();

		k_sleep(k_timeout_t{K_SECONDS(1)});
	}
}
