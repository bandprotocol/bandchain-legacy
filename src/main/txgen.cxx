#include <iostream>

#include "tx/message.h"

int main()
{
  char buffer[1 << 16];
  TxMsg& msg = *(new (buffer) TxMsg());

  int int_temp;
  std::string temp;

  msg.msg_type = TxMsg::MsgID;
  msg.msg_ts = 1528512683000;

  Log::info("How many input tx?");
  std::cin >> int_temp;
  msg.input_cnt = int_temp;

  for (int idx = 0; idx < msg.input_cnt; ++idx) {
    Log::info("Input #{} ident hex:", idx);
    std::cin >> temp;
    msg.get_input(idx).ident = Bytes<32>::from_hex(temp);
    Log::info("Input #{} addr_suffix hex:", idx);
    std::cin >> temp;
    msg.get_input(idx).addr_suffix = Bytes<12>::from_hex(temp);
    Log::info("Input #{} sig hex:", idx);
    std::cin >> temp;
    msg.get_input(idx).sig = Bytes<64>::from_hex(temp);
  }

  Log::info("How many output tx?");
  std::cin >> int_temp;
  msg.output_cnt = int_temp;

  for (int idx = 0; idx < msg.output_cnt; ++idx) {
    Log::info("Output #{} addr hex:", idx);
    std::cin >> temp;
    msg.get_output(idx).addr = Bytes<20>::from_hex(temp);
    Log::info("Output #{} amount:", idx);
    std::cin >> msg.get_output(idx).amount;
  }

  Log::info("{}", msg_as_hex(msg));
  return 0;
}
