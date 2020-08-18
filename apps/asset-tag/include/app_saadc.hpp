#ifndef APP_INCLUDE_APP_SAADC_HPP
#define APP_INCLUDE_APP_SAADC_HPP

#include <app_battery.hpp>
#include <app_log.hpp>

#include <zephyr.h>
#include <device.h>
#include <drivers/adc.h>
#include <drivers/sensor.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_saadc.h>

#include <cstring>
#include <stdexcept>

namespace app_saadc {

	static constexpr const uint16_t ADC_ACQUISITION_TIME = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10);

	static constexpr uint32_t SAMPLES_IN_BUFFER = 1;
	static int16_t sample_buffer[SAMPLES_IN_BUFFER];

	void measure() {
		static device* adc_device = device_get_binding(DT_LABEL(DT_INST(0, nordic_nrf_saadc)));

		const adc_channel_cfg vdd_channel_cfg = {
			.gain             = ADC_GAIN_1_6,
			.reference        = ADC_REF_INTERNAL,
			.acquisition_time = ADC_ACQUISITION_TIME,
			.channel_id       = 0,
			.input_positive   = NRF_SAADC_INPUT_VDD
		};

		const adc_channel_cfg* cfgs[] = {
			&vdd_channel_cfg, 
		};
		
		const int num_channels = sizeof(cfgs) / sizeof(struct adc_channel_cfg*);
		uint8_t channels = 0;
		int ret;
		for(int i = 0; i < num_channels; i++) {
			// LOG_DBG("Channel: %d on PinP: %d PinN: %d", (int32_t) cfgs[i]->channel_id, (int32_t) cfgs[i]->input_positive, (int32_t) cfgs[i]->input_negative);
			channels |= BIT(cfgs[i]->channel_id);
			ret = adc_channel_setup(adc_device, cfgs[i]);
			if(ret) {
				LOG_ERR("Failed to register channel config for %d", i);
				throw std::runtime_error("Failed to register channel");
			}
		}

		// Initiate the saadc sequence
		std::memset(sample_buffer, 0, sizeof(sample_buffer));
		const struct adc_sequence sequence = {
			.channels    = channels,
			.buffer      = sample_buffer,
			.buffer_size = sizeof(sample_buffer),
			.resolution  = 12,
		};
		ret = adc_read(adc_device, &sequence);
		if(ret) {
			LOG_ERR("Failed to do an adc_read: %d", ret);
			throw std::runtime_error("Failed to do adc_read");
		}

		// Convert the samples to meaningful scale (mV)
		for(int i = 0; i < num_channels; i++) {
			int32_t value = sample_buffer[i];
			ret = adc_raw_to_millivolts(adc_ref_internal(adc_device),
								cfgs[i]->gain,
								sequence.resolution,
								&value);
			// LOG_DBG("Channel: %" PRId32" Before: %" PRId32 " After: %" PRId32, cfgs[i]->channel_id, before_value, value);
			if (ret) {
				printk("Failed to convert adc raw to millivolts: ret %d", ret);
				throw std::runtime_error("Failed to convert adc");
			}

			if(i == 0) {
				uint8_t pct = battery_level_pct(value);
				LOG_DBG("VDD: %d mV (%d pct)", value, (int32_t) pct);
				bt_gatt_bas_set_battery_level(pct);
			} else {
				throw std::runtime_error("Unexpected channel sample! Programming error");
			}
		}
	}
}

#endif