#include <iostream>
#include "buffer.h"
int main(int argc, const char *argv[])
{
  Buffer buffer;
  char * buf = buffer.GetBuffer();
  uint64 size = buffer.GetBufferSize();
  std::cout << "Init Size = " << size << std::endl;
  // fill data
  int i = 0;
  for (; i < 6; i++) {
    buf[i] = 'a' + i;
  }
  buffer.SetBufferUsed(i);
  std::cout << "after 100 Size = " << buffer.GetBufferSize() << std::endl;
  buf = buffer.GetBuffer();
  size = buffer.GetBufferSize();
  for (i = 0; i < 10; i++) {
    buf[i] = '1' + i;
  }
  buffer.SetBufferUsed(i);
  std::cout << "after 100 Size = " << buffer.GetBufferSize() << std::endl;
  
  buf = buffer.GetData();
  size = buffer.GetDataSize();
  std::string ss(buf, buf + size);
  std::cout << ss << std::endl;
  
  //
  buffer.TakeData(3);
  buf = buffer.GetData();
  size = buffer.GetDataSize();
  std::string ss1(buf, buf + size);
  std::cout << ss1 << std::endl;
  // 
  buffer.Append("peitaoaaaa");
  std::string data;
  buffer.GetDataAsString(&data);
  std::cout << data << std::endl;
  buffer.TakeData(4);
  buffer.GetDataAsString(&data);
  std::cout << data << std::endl;
  return 0;
}
