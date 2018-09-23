#include "tmapp.h"

#include "store/contract.h"
#include "store/global.h"
#include "util/buffer.h"
#include "util/endian.h"

namespace
{
bool read_ahead(Buffer& buf, int& value)
{
  value = 0;

  int idx = 0;
  for (const auto raw_byte : buf.as_span()) {
    int byte = std::to_integer<int>(raw_byte);
    value |= (byte & 0x7F) << (7 * idx);
    if (!(byte & 0x80)) {
      value >>= 1;
      if (buf.size_bytes() > idx + value) {
        buf.consume(1 + idx);
        return true;
      } else {
        return false;
      }
    }

    ++idx;
  }
  return false;
}
} // namespace

void TendermintApplication::do_info(const RequestInfo& req, ResponseInfo& res)
{
  if (req.version() != get_tm_version()) {
    throw Failure("Invalid Version");
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
  res.set_height(last_block_height);
  try {
    res.set_value(query(req.path(), req.data()));
    res.set_code(0);
  } catch (const std::exception& err) {

    DEBUG(log, "DO_QUERY");
    DEBUG(log, "  RequestPath: {}", req.path());
    DEBUG(log, "  RequestData: {}", req.data());
    DEBUG(log, "  Error: {}", err.what());

    res.set_info(err.what());
    res.set_code(1);
  }
}

void TendermintApplication::do_begin_block(const RequestBeginBlock& req)
{
  // TODO: Penalize missing validators
  Global::get().block_time = req.header().time();
  // Global::get().block_proposer =
  //     Address::from_raw(req.header().proposer().address());

  // auto addr = Address::from_raw(req.header().validators_hash());
  // // COMMIT_LOG("{}", req.header().validators_hash());
  //   // NOCOMMIT_LOG("{}", addr);
  //   NOCOMMIT_LOG("{}", req.header().proposer().pub_key().data());
  //   NOCOMMIT_LOG("Size: {}",
  //   req.header().proposer().pub_key().data().size());
}

void TendermintApplication::do_check_tx(const RequestCheckTx& req,
                                        ResponseCheckTx& res)
{
  try {
    check(req.tx());
    res.set_code(0);
  } catch (const Error& err) {
    res.set_info(err.what());
    res.set_code(1);
  } catch (const std::range_error& e) {
    res.set_info("uint256 integer range error");
    res.set_code(1);
  } catch (const std::overflow_error& e) {
    res.set_info("uint256 integer overflow error");
    res.set_code(1);
  }
}

void TendermintApplication::do_deliver_tx(const RequestDeliverTx& req,
                                          ResponseDeliverTx& res)
{
  try {
    apply(req.tx());
    res.set_code(0);
  } catch (const Error& err) {
    res.set_info(err.what());
    res.set_code(1);
  } catch (const std::range_error& e) {
    res.set_info("uint256 integer range error");
    res.set_code(1);
  } catch (const std::overflow_error& e) {
    res.set_info("uint256 integer overflow error");
    res.set_code(1);
  }
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
  if (!read_ahead(read_buffer, size))
    return false;

  Request req;
  Response res;

  req.ParseFromArray(read_buffer.as_span().data(), size);
  read_buffer.consume(size);

  switch (req.value_case()) {
    case Request::ValueCase::kEcho:
      res.mutable_echo()->set_message(req.echo().message());
      break;
    case Request::ValueCase::kFlush:
      res.mutable_flush();
      break;
    case Request::ValueCase::kInfo:
      do_info(req.info(), *res.mutable_info());
      break;
    case Request::ValueCase::kSetOption:
      throw Failure("set_option is not supported");
      break;
    case Request::ValueCase::kInitChain:
      res.mutable_init_chain();
      do_init_chain(req.init_chain());
      break;
    case Request::ValueCase::kQuery:
      do_query(req.query(), *res.mutable_query());
      break;
    case Request::ValueCase::kBeginBlock:
      res.mutable_begin_block();
      do_begin_block(req.begin_block());
      break;
    case Request::ValueCase::kCheckTx:
      do_check_tx(req.check_tx(), *res.mutable_check_tx());
      break;
    case Request::ValueCase::kDeliverTx:
      do_deliver_tx(req.deliver_tx(), *res.mutable_deliver_tx());
      break;
    case Request::ValueCase::kEndBlock:
      do_end_block(req.end_block(), *res.mutable_end_block());
      break;
    case Request::ValueCase::kCommit:
      do_commit(*res.mutable_commit());
      break;
    default:
      throw Failure("Unexpected request type");
  }

  uint64_t write_size = res.ByteSize();
  write_buffer << (write_size << 1);

  std::string write_data;
  res.SerializeToString(&write_data);
  write_buffer << gsl::make_span(write_data);
  return true;
}
