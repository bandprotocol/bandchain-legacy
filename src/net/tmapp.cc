#include "tmapp.h"

void TendermintApplication::do_info(const RequestInfo& req, ResponseInfo& res)
{
  if (req.version() != get_tm_version()) {
    throw std::runtime_error("Invalid Version");
  }
  res.set_data(get_name());
  res.set_version(get_version());
  if (last_block_height != 0) {
    res.set_last_block_height(last_block_height);
    res.set_last_block_app_hash(get_current_app_hash());
  }
}

void TendermintApplication::do_init_chain(const RequestInitChain& req)
{
  init(req.app_state_bytes());
}

void TendermintApplication::do_query(const RequestQuery& req,
                                     ResponseQuery& res)
{
  res.set_code(0);
  res.set_height(last_block_height);
  res.set_value(query(req.path(), req.data()));
}

void TendermintApplication::do_begin_block(const RequestBeginBlock&)
{
  // TODO: Penalize missing validators
}

void TendermintApplication::do_check_tx(const RequestCheckTx& req,
                                        ResponseCheckTx& res)
{
  res.set_code(check(req.tx(), false) ? 0 : 1);
}

void TendermintApplication::do_deliver_tx(const RequestDeliverTx& req,
                                          ResponseDeliverTx& res)
{
  res.set_code(check(req.tx(), true) ? 0 : 1);
}

void TendermintApplication::do_end_block(const RequestEndBlock&,
                                         ResponseEndBlock&)
{
  // TODO: Update the set of validators
}

void TendermintApplication::do_commit(ResponseCommit& res)
{
  // TODO: Notify the application to flush the blockchain state
  ++last_block_height;
  res.set_data(get_current_app_hash());
}

bool TendermintApplication::process(Buffer& read_buffer, Buffer& write_buffer)
{
  int size = 0;
  if (!read_integer(read_buffer, size))
    return false;

  Request req;
  Response res;

  req.ParseFromArray(read_buffer.begin(), size);
  read_buffer.consume(size);

  switch (req.value_case()) {
    case abci::Request::ValueCase::kEcho:
      res.mutable_echo()->set_message(req.echo().message());
      break;
    case abci::Request::ValueCase::kFlush:
      res.mutable_flush();
      break;
    case abci::Request::ValueCase::kInfo:
      do_info(req.info(), *res.mutable_info());
      break;
    case abci::Request::ValueCase::kSetOption:
      throw std::runtime_error("set_option is not supported");
      break;
    case abci::Request::ValueCase::kInitChain:
      res.mutable_init_chain();
      do_init_chain(req.init_chain());
      break;
    case abci::Request::ValueCase::kQuery:
      do_query(req.query(), *res.mutable_query());
      break;
    case abci::Request::ValueCase::kBeginBlock:
      res.mutable_begin_block();
      do_begin_block(req.begin_block());
      break;
    case abci::Request::ValueCase::kCheckTx:
      do_check_tx(req.check_tx(), *res.mutable_check_tx());
      break;
    case abci::Request::ValueCase::kDeliverTx:
      do_deliver_tx(req.deliver_tx(), *res.mutable_deliver_tx());
      break;
    case abci::Request::ValueCase::kEndBlock:
      do_end_block(req.end_block(), *res.mutable_end_block());
      break;
    case abci::Request::ValueCase::kCommit:
      do_commit(*res.mutable_commit());
      break;
    default:
      throw std::runtime_error("Unexpected request type");
  }

  std::cout << req.DebugString().c_str() << std::endl;
  std::cout << res.DebugString().c_str() << std::endl;

  size_t write_size = res.ByteSizeLong();
  write_integer(write_buffer, write_size);
  std::byte* write_location = write_buffer.reserve(write_size);
  res.SerializeToArray(write_location, write_size);

  return true;
}

bool TendermintApplication::read_integer(Buffer& read_buffer, int& value)
{
  value = 0;
  for (size_t i = 0; i < read_buffer.size(); ++i) {
    int byte = std::to_integer<int>(read_buffer[i]);
    value |= (byte & 0x7F) << (7 * i);
    if (!(byte & 0x80)) {
      value >>= 1;
      if (read_buffer.size() > i + value) {
        read_buffer.consume(1 + i);
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

void TendermintApplication::write_integer(Buffer& write_buffer, int value)
{
  value <<= 1;
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
