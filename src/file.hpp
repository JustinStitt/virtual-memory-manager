#pragma once
#include <string>

#define READ_BUF_SIZ 256

class File{
private:
    FILE* fp;
public:
    File() = default;

    /* constructor reads in file given file name. optional read/write priv. */
    File(const char* fn){
        this->fp = fopen(fn, "r");
    }

    /* destructor called when object exists scope */
    ~File(){
        fclose(this->fp);
    }

    /* overload operator[] to random-access bytes */
    char operator[](int idx) const{
        fseek(this->fp, idx, 0);
        char ret = fgetc(this->fp);
        return ret;
    }

    int getAddress(){
        char line[READ_BUF_SIZ];
        fgets(line, READ_BUF_SIZ, fp);
        return std::stoi(line);
    }

    // RegEx pattern = (?<=Value: )-*\d*
    int parseValue(){
        char line[READ_BUF_SIZ];
        memset(line, 0, sizeof(line));
        fgets(line, READ_BUF_SIZ, fp);

        int idx = READ_BUF_SIZ - 1;
        for(; idx >= 0; --idx){
            if(line[idx] == ':'){
                ++idx; break;
            }
        }
        return std::stoi((char*)&line[idx]); 
    }
};
