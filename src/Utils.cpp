//
// Created by David Lakubu on 09/04/2023.
//

#include "Utils.h"

int parse_int(const std::string &str) {
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

//template<typename T>
//std::string Result<T>::msg() {
//    return _msg;
//}
//template<typename T>
//T Result<T>::data() {
//    return _data;
//}
//
//template<typename T>
//bool Result<T>::error() {
//    return _error;
//}

