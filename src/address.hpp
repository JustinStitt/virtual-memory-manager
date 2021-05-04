#pragma once

using BYTE = int8_t;

class LogicalAddress{
private:
    int16_t addy;
public:

    LogicalAddress(int _address){
        this->addy = static_cast<decltype(this->addy)>(_address);
    }

    BYTE getPage(){
        return (0xFF00 & this->addy) >> 8;
    }

    BYTE getOffset(){
        return (0xFF & this->addy);
    }

};