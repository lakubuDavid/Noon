//
// Created by David Lakubu on 09/04/2023.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cmath>
#include <boost/algorithm/string.hpp>

template<typename T>
typedef
struct Result {
	std::string msg = "No error";
	bool error = false;
	T data;

	Result();

	explicit Result(T value);

	Result &operator=(T value);

	static Result Error(std::string msg);
};
//typedef struct Result Result;

template<typename T>
Result<T>::Result() = default;

template<typename T>
Result<T> Result<T>::Error(std::string msg) {
	Result<T> result;
	result.msg = msg;
	result.error = true;
	return result;
}

template<typename T>
Result<T> &Result<T>::operator=(T value) {
	msg = "No error";
	error = false;
	data = value;
	return *this;
}

template<typename T>
Result<T>::Result(T value) {
	msg = "No error";
	error = false;
	data = value;
}
//Result;

std::string upperCase(const std::string &str) {
	std::string result = boost::algorithm::to_upper_copy(str);
//	std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
	return result;
}

std::string lowerCase(const std::string &str) {
	std::string result = boost::algorithm::to_lower_copy(str);
//	std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}


int to_int(const std::string &str);

float to_float(const std::string &str);

std::string getPath(const std::string &path);

void printHelp();
//Connection* openConnection(const std::string& url);

struct File {

	static std::string readAllText(const std::string &filename);

	static void writeAllText(const std::string &filename, const std::string &text);
};

#endif //UTILS_H
