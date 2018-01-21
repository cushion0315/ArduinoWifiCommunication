#include "Buffer.h"

Buffer::Buffer() {
  pos = 0;
  bufferSize = 64;
}

Buffer::~Buffer() {
}







int Buffer::appendChar(const char c) {
  if(pos < 64)
    buffer[pos++] = c;
  else return 1;
  return 0;
}




void Buffer::clear() {
  token = NULL;
  pos = 0;
  for(int i = 0;i<65;i++){
    buffer[i] = 0;
  }
}

void Buffer::trimEndLine() {
  if (endLine()) {
    buffer[pos - 2] = buffer[pos - 1] = 0;
    pos -= 2;
  }
}

bool Buffer::endLine() {
  if (buffer[pos - 2] == '\r' && buffer[pos - 1] == '\n') return true;
  return false;
}

bool Buffer::empty() {
  if (buffer[0] == 0 && pos == 0) return true;
  return false;
}

int Buffer::size() {
  return pos;
}

bool Buffer::startWith(const char* str, int len) {
  if (pos < len)
    return false;
  for (int i = 0; i < len; i++) {
    if (buffer[i] != str[i]) return false;
  }
  return true;
}

bool Buffer::endWith(const char* str, int len) {
  if (pos < len)
    return false;
  for (int i = 0; i < len; i++) {
    if (buffer[pos - 1 - i] != str[len - 1 - i])return false;
  }
  return true;
}


bool Buffer::equals(const char * str, int len) {
  for (int i = 0; i < len ; i++) {
    if (buffer[i] != 0) {
      if (buffer[i] != str[i])
        return false;
    } else {
      return false;
    }
  }
  return true;
}

const char* Buffer::nextToken(const char* delimiters) {
  prevToken = token;
  if (token == NULL) {
    token = strtok(buffer, delimiters);
  } else {
    token = strtok(NULL, delimiters);
  }
  return token;
}


const char* Buffer::str() {
  return buffer;
}

int Buffer::debug() {
//  Serial.println("Yo");
//  Serial.println(bufferSize);
//  Serial.println(buffer);
  return bufferSize;
}


