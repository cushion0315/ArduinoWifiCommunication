
#ifndef BUFFER_H
#define BUFFER_H

#include "Arduino.h"


class Buffer{
  public:
    /**
     * create a new buffer
     */
    Buffer();

    /**
     * distory a buffer
     */
    ~Buffer();

    /**
     * put a char after the sequence and pos +1
     * if append success, return 0
     * if buffer size over flow, return 1
     * if it is freezed, return 2
     */
    int appendChar(const char);

    /**
     * clear buffer content
     */
    void clear();
    /**
     * remove the endline at the end
     */
    void trimEndLine();
    bool equals(const char *, int len);
    bool endLine();
    /**
     * check if buffer is empty
     */
    bool empty();
    const char* str();
    int debug();
    bool startWith(const char*,int);
    bool endWith(const char*,int);
    int size();
    const char* nextToken(const char*);
  private:
    unsigned char buffer[65] = {0};
    char * iterator;
    char * prevToken;
    char * token;
    int pos;
    int bufferSize;
};

#endif

