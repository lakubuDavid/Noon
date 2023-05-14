//
// Created by David Lakubu on 22/04/2023.
//

#include "Log.h"
#include "RuntimeConfig.h"

Log *Log::_instance = NULL;

Log::Log() {

}

void Log::print(const std::string &message, LogLevel logLevel) {
	if (logLevel == LogLevel::LOG_LEVEL_DEBUG) {
		if (!instance()->_useConsoleOutput)
			return;
		if (instance()->_outputStream) {
			fprintf(instance()->_outputStream, "%s", message.c_str());
		}
	} else if (logLevel == LogLevel::LOG_LEVEL_INFO) {
		if (instance()->_outputStream) {
			fprintf(instance()->_outputStream, "%s", message.c_str());
		}
	} else if (logLevel == LogLevel::LOG_LEVEL_ALWAYS) {
		printf("%s", message.c_str());
	}

}

void Log::init() {
	instance();
	if (RuntimeConfig::instance()->get("NOON_DEBUG", "0") == "1") {
		useConsoleOutput(true);
		setOutputStream(stdout);
	}
}

void Log::useConsoleOutput(bool value) {
	instance()->_useConsoleOutput = value;
}

void Log::setOutputStream(FILE *output) {
	instance()->_outputStream = output;
}

void Log::println(const std::string &message, LogLevel logLevel) {
	print(message + "\n", logLevel);
}
