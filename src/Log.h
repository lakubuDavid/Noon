//
// Created by David Lakubu on 22/04/2023.
//

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

enum LogLevel{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ALWAYS
};
class Log {
    Log();
    bool _useConsoleOutput = false;
    FILE* _outputStream = NULL;
    static Log* _instance;
public:
    static void init();

    static void print(const std::string &message, LogLevel logLevel=LOG_LEVEL_DEBUG);

    static void useConsoleOutput(bool value);
    static void setOutputStream(FILE* output);

    static Log* instance(){
        if(!_instance)
            _instance = new Log();
        return _instance;
    }

    static void println(const std::string &message, LogLevel logLevel=LOG_LEVEL_DEBUG);
};


#endif //LOG_H
