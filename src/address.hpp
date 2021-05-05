#pragma once

using BYTE = uint8_t;
using SBYTE = int8_t;

class LogicalAddress{
private:
    uint16_t addy; // WORD (2 BYTES)
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