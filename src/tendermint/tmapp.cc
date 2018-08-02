#include "tmapp.h"

bool TendermintApplication::read_integer(Buffer& read_buffer, int& value)
{
  value = 0;
  for (size_t i = 0; i < read_buffer.size(); ++i) {
    int byte = std::to_integer<int>(read_buffer[i]);
    value |= (byte & 0x7F) << (7 * i);
    if (!(byte & 0x80)) {
      return read_buffer.size() >= 1 + i + value;
    }
  }
  return false;
}

void TendermintApplication::write_integer(Buffer& write_buffer, int value)
{
  while (true) {
    std::byte towrite = static_cast<std::byte>(value & 0x7F);
    value >>= 7;
    if (value != 0) {
      write_buffer.append(towrite | std::byte{0x80});
    } else {
      write_buffer.append(towrite);
      break;
    }
  }
}
