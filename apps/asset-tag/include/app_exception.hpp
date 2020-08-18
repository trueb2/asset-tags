#ifndef APP_INCLUDE_APP_EXCEPTION_HPP
#define APP_INCLUDE_APP_EXCEPTION_HPP

#include <app_log.hpp>

#include <zephyr.h>

#include <stdexcept>


[[noreturn]] void terminate() noexcept {
	LOG_ERR("Failed with exception");
	while(log_process(false) || true) {
		k_sleep(K_SECONDS(1));
	}
	abort();
}

namespace __cxxabiv1 {
	std::terminate_handler __terminate_handler = terminate;
}


#endif