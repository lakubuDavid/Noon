//
// Created by David Lakubu on 09/04/2023.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cmath>


//template <typename T>
//class Result{
//    std::string _msg = "No error";
//    bool _error = false;
//    T _data;
//public:
//    std::string msg();
//    bool error();
//    T data();
//};
//class Connection;
template <typename T>
typedef struct Result{
    std::string msg = "No error";
    bool error = false;
    T data;

    Result();
    Result(T value);
    Result& operator=(T value);

    static Result Error(std::string msg);
};

template<typename T>
Result<T>::Result() {

}

template<typename T>
Result<T> Result<T>::Error(std::string msg) {
    Result<T> result;
    result.msg = msg ;
    result.error = true;
    return result;
}

template<typename T>
Result<T> &Result<T>::operator=(T value) {
    this->data = value;
    return *this;
}

template<typename T>
Result<T>::Result(T value) {
    data = value;
}
//Result;


int parse_int(const std::string& str);
std::string getPath(const std::string& path);
void printHelp();
//Connection* openConnection(const std::string& url);

#endif //UTILS_H
