//
// Created by David Lakubu on 08/05/2023.
//

#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H


#include <nlohmann/json.hpp>

class ISerializable {
public:
    virtual nlohmann::json serialize() = 0;
    virtual void deserialize(nlohmann::json serialized) = 0;
};


#endif //ISERIALIZABLE_H
