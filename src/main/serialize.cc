class Buffer
{
public:
};

Buffer& operator<<(Buffer& buf, uint16_t x)
{
  log::info("{}", x + 1);
  return buf;
}

Buffer& operator<<(Buffer& buf, uint32_t x)
{
  log::info("{}", x + 2);
  return buf;
}

int main()
{
  uint32_t x = 2;
  Buffer buf;
  buf << x;
  return 0;
}
