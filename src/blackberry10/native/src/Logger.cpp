/*
 * Copyright (c) 2013 BlackBerry Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Logger.hpp"
#include "quantogastei_js.hpp"
#include <slog2.h>
#include <stdarg.h>

const unsigned short LOGCODE = 10680;

Logger::Logger(const char* name, TCPSocketLib_JS *parent): m_pParent(parent) {
	buffer_config.buffer_set_name = name;
	buffer_config.num_buffers = 2;
	buffer_config.verbosity_level = SLOG2_DEBUG1;

	/* Configure the first buffer, using 7 x 4KB pages.  This larger buffer will be used for
	   very chatty logging.  Our goal is to have 30-60 seconds of history at any given time,
	   so we will want to log at a rate of around one log line with a string of 16 bytes
	   long every 150 milliseconds.
	*/

	buffer_config.buffer_config[0].buffer_name = "low_priority";
	buffer_config.buffer_config[0].num_pages = 7;

	/* Configure the second buffer, which we will use for high level info logging that is very
	   infrequent, but we want a longer history (hours or maybe even over a day or two).  This
	   buffer uses 1 x 4KB.
	*/

	buffer_config.buffer_config[1].buffer_name = "high_priority";
	buffer_config.buffer_config[1].num_pages = 1;

	/* Register the buffer set. */

	if( -1 == slog2_register( &buffer_config, buffer_handle, 0 ) ) {
		fprintf( stderr, "Error registering slogger2 buffer!\n" );
	} else {
		info("Created slogger2 buffers");
	}

}

Logger::~Logger() {
	critical("slogger2 buffers reset");
	slog2_reset();
}

int Logger::log(slog2_buffer_t buffer, _Uint8t severity, const char* message, std::va_list arglist) {
    return vslog2f(buffer, LOGCODE, severity, message, arglist);
}

int Logger::debug(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
	int result = log(lowPriorityBuffer(), SLOG2_DEBUG1, message, arglist);
	va_end(arglist);
	return result;
}

int Logger::info(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
    int result = log(lowPriorityBuffer(), SLOG2_INFO, message, arglist);
	va_end(arglist);
	return result;
}

int Logger::notice(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
    int result = log(lowPriorityBuffer(), SLOG2_NOTICE, message, arglist);
    va_end(arglist);
    return result;
}

int Logger::warn(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
    int result = log(lowPriorityBuffer(), SLOG2_WARNING, message, arglist);
    va_end(arglist);
    return result;
}

int Logger::error(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
    int result = log(hiPriorityBuffer(), SLOG2_ERROR, message, arglist);
    va_end(arglist);
    return result;
}

int Logger::critical(const char* message, ...) {
    va_list arglist;
    va_start(arglist, message);
    int result = log(hiPriorityBuffer(), SLOG2_CRITICAL, message, arglist);
    va_end(arglist);
    return result;
}

int Logger::setVerbosity(_Uint8t verbosity) {
	return slog2_set_verbosity(buffer_handle[0], verbosity);
}

_Uint8t Logger::getVerbosity() {
	return slog2_get_verbosity(buffer_handle[0]);
}

slog2_buffer_t Logger::hiPriorityBuffer() {
	return buffer_handle[1];
}

slog2_buffer_t Logger::lowPriorityBuffer() {
	return buffer_handle[0];
}
