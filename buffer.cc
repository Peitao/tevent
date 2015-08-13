#include "buffer.h"

void Buffer::Append(const std::string & input) {
  uint64 size = GetBufferSize();
  char * buf = GetBuffer();
  if (size < input.size()) {
    MakeBufferSize(input.size());
  }
  ::memcpy(buf, input.data(), input.size());
  SetBufferUsed(input.size());
}
char * Buffer::GetBuffer() {
  return data_.data() + used_size_;
}
uint64 Buffer::GetBufferSize() { 
  return data_.size() - used_size_;
}
void Buffer::SetBufferUsed(uint64 s) {
  used_size_ += s;
}
void Buffer::MakeBufferSize(uint64 size) {
  if (GetBufferSize() > size) return;
  data_.resize(used_size_ + size * 2);
}
char * Buffer::GetData() {
  return data_.data();
}
uint64 Buffer::GetDataSize() {
  return used_size_;
}
void Buffer::GetDataAsString(std::string * data) {
  data->clear();
  data->append(std::string(GetData(), GetData() + GetDataSize()));
}
void Buffer::TakeData(uint64 size) {
  uint64 real_size = size;
  real_size = real_size < used_size_ ? real_size : used_size_;
  uint64 remain_data = used_size_ - real_size;
  if (remain_data)
    ::memcpy(data_.data(), data_.data() + real_size, remain_data);
  used_size_ = remain_data;
}
