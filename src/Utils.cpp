//
// Created by David Lakubu on 09/04/2023.
//

#include "Utils.h"
#include "App.h"
#include "Log.h"
#include "RuntimeConfig.h"
#include <csignal>
#include <signal.h>
#include <stdio.h>
#include <string>

int to_int(const std::string &str) {
	int s = str.size();
	int value = 0;
	for (int i = 0; i < s; i++) {
		int n = 0;
		switch (str[i]) {
			case '0':
				n = 0;
				break;
			case '1':
				n = 1;
				break;
			case '2':
				n = 2;
				break;
			case '3':
				n = 3;
				break;
			case '4':
				n = 4;
				break;
			case '5':
				n = 5;
				break;
			case '6':
				n = 6;
				break;
			case '7':
				n = 7;
				break;
			case '8':
				n = 8;
				break;
			case '9':
				n = 9;
				break;
			default:
				break;
		}

		value += n * pow(10, s - i - 1);
	}

	return value;
}

std::string getPath(const std::string &path) {
	auto root = RuntimeConfig::instance()->get("NOON_APP_DIR", ".");
	if (root[root.size() - 1] != '/')
		root = root + "/";
	return root + path;
}
// Connection *openConnection(const std::string &url) {
//     auto info = Connection::getInfo(url);
//     if(info.protocol == PROTOCOL_HTTP) {
//         return new SocketConnection(url);
//     }else if(info.protocol == PROTOCOL_HTTPS){
//         return new SSLConnection(url);
//     }
// }

// template<typename T>
// std::string Result<T>::msg() {
//     return _msg;
// }
// template<typename T>
// T Result<T>::data() {
//     return _data;
// }
//
// template<typename T>
// bool Result<T>::error() {
//     return _error;
// }

void printHelp() {
	std::cout << "Usage: noon --help or noon <property> <value> " << std::endl;
	std::cout << " Properties:" << std::endl;
	std::cout << "\t --appDir DIR    : Set the app directory (the one "
	             "containing the `noon.config.lua` file) "
	          << std::endl;
	std::cout << "\t --port PORT     : Set the port (default: 8080) "
	          << std::endl;
	std::cout
			<< "\t --cert CERT_FILE CERT_KEY_FILE: Set the certificate and the "
			   "certificate key (default : Will use the default server certificate)"
			<< std::endl;
	std::cout
			<< "\t\t For security reason, you should specify your own certificate"
			<< std::endl;
}

float to_float(const std::string &str) {
	/* To convert the string to a float we need :
	 * 1. Identify where is the decimal part and the integer part
	 * 2. Get those different parts
	 * 3. Multiply the decimal part by 10 ^ -(length_of_the_decimal_part)
	 * */

	//

	int pointPos = str.find(".");
	auto intPart_s = str.substr(0, pointPos);
	int intPart = to_int(intPart_s);

	float result;

	if (pointPos != std::string::npos) {
		auto decPart_s = str.substr(pointPos, str.length());
		int decPart = to_int(decPart_s);
		result = intPart + (decPart * pow(10, -pointPos));
	} else {
		result = intPart;
	}
	return result;
}

std::string File::readAllText(const std::string &filename) {
	std::ifstream f = std::ifstream(filename.c_str());
	if (f.is_open()) {
		std::string line;

		auto text = std::stringstream();
		while (std::getline(f, line)) {
			text << line << std::endl;
		}
		f.close();
		return text.str();
	}
	throw;
}

void File::writeAllText(const std::string &filename, const std::string &text) {
	std::ofstream f = std::ofstream(filename.c_str());
	if (f.is_open()) {
		f << text;
		f.close();
	}
	return;
}
