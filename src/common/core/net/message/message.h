#pragma once

#include <string>

#include "core/net/yojimboimport.h"

template<typename T>
class Message : public yojimbo::Message {
protected:
    T data;
public:
    Message(const T& data) :
        data(data)
    { }

    virtual std::string toString(void) = 0;
    T getData(void) const {
        return data;
    }
};
