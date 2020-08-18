#ifndef APP_INCLUDE_GPIO_HPP
#define APP_INCLUDE_GPIO_HPP

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <string_view>
#include <cstring>
#include <stdexcept>


struct gpio_t {
    std::string_view label;
    uint32_t pin;
    uint32_t flags;
    device*  device_binding;

    explicit constexpr gpio_t(bool status_okay, std::string_view label, uint32_t pin, uint32_t flags) 
    : label(label), pin(pin), flags(flags), device_binding(nullptr) {
        if(!status_okay) {
            std::string_view message_begin = "Invalid label: ";
            char message[message_begin.size() + label.size()] = {};
            std::memcpy(message, message_begin.begin(), message_begin.size());
            std::memcpy(message + message_begin.size(), label.begin(), label.size());
            throw std::logic_error(message);
        }

    }

    void configure(uint32_t extra_flags) {
        device_binding = device_get_binding(label.data());
        if(gpio_pin_configure(device_binding, pin, extra_flags | flags)) {
            throw std::runtime_error("Failed to configure gpio");
        }
    }

    void set(int value) {
        gpio_pin_set(device_binding, pin, value);
    }
};

using namespace std::literals;

#define PREPARE_GPIO(label) struct label ## _binding_t { \
    static constexpr bool             status_okay = DT_NODE_HAS_STATUS(DT_ALIAS(label), okay); \
    static constexpr std::string_view label       = DT_GPIO_LABEL(DT_ALIAS(label), gpios) "\0"; \
    static constexpr const uint32_t   pin         = DT_GPIO_PIN(DT_ALIAS(label), gpios); \
    static constexpr const uint32_t   flags       = DT_GPIO_FLAGS(DT_ALIAS(label), gpios); \
}; \
gpio_t label ## _gpio( \
    label ## _binding_t::status_okay, \
    label ## _binding_t::label, \
    label ## _binding_t::pin, \
    label ## _binding_t::flags);


#endif