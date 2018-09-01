#include "buffer.h"

Buffer::Buffer()
    : buf()
{
  /// Reserve 512 bytes to the buffer to prevent unnecessarily resize.
  buf.reserve(512);
}
