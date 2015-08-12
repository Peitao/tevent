#pragma once
#include <vector>
#include <string>
#include <string.h>
#include "types.h"
class Buffer {
 public:
  Buffer(uint64 size = 1024):used_size_(0) {
    data_.resize(size);
  }
  // 把一个 string 加入到缓存中
  void Append(const std::string & input);
  // 获得一个可用的输入 buffer 最后需要用 SetBufferUsed
  // 来设置使用了多少 buffer
  char * GetBuffer();
  uint64 GetBufferSize();
  void SetBufferUsed(uint64 s);
  // 可以用来分配一个不会少于 size 的 buffer 
  void MakeBufferSize(uint64 size);
  // 读取 buffer 中数据的 0-copy 的数组
  char * GetData();
  uint64 GetDataSize();
  // 读取 buffer 中的数据 copy 到 string 中 
  // 但是不会把数据从 buffer 中移出
  void GetDataAsString(std::string * data);
  // 把一定大小的数据从 buffer 中移出
  void TakeData(uint64 size);
 private:
  uint64 used_size_;
  std::vector<char> data_;
};
