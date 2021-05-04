#pragma once

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
};
