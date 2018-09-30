#include "voting.h"

#include "contract/token.h"
#include "crypto/sha256.h"
#include "store/global.h"

Voting::Voting(const Address& voting_id, const Address& _token_id)
    : Contract(voting_id)
    , token_id(_token_id)
{
}

void Voting::request_voting_power(uint256_t value)
{
  auto& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(m_addr, value);
  voting_power[get_sender()] += value;
}

void Voting::withdraw_voting_power(uint256_t value)
{
  uint256_t available_token =
      voting_power[get_sender()] - get_maximum_vote(get_sender());
  if (value > available_token)
    throw Error("Not enough available token to withdraw, reveal or rescue your "
                "vote first.");

  auto& token = Global::get().m_ctx->get<Token>(token_id);
  voting_power[get_sender()] -= value;

  // Remove from map
  if (voting_power[get_sender()] == 0) {
    voting_power.erase(get_sender());
  }

  Address receiver = get_sender();
  set_sender();
  token.transfer(receiver, value);
}

void Voting::rescue_token(uint256_t poll_id)
{
  assert_poll_exists(poll_id);

  assert_vote_exist(get_sender(), poll_id);

  assert_con(is_poll_ended(poll_id),
             "You cannot rescue token in commit/reveal period.");

  // Remove in votes and poll_list
  auto it = m_poll.at(poll_id).votes.find(get_sender());
  PollReceipt poll_receipt = it->second;
  poll_list[get_sender()].erase({poll_receipt.num_tokens, poll_id});
  m_poll.at(poll_id).votes.erase(it);
}

void Voting::commit_vote(uint256_t poll_id, Hash secret_hash,
                         uint256_t num_tokens)
{
  assert_con(is_commit_period(poll_id),
             "Now you cannot commit vote to this poll.");
  if (num_tokens > voting_power[get_sender()]) {
    throw Error("Your voting power in this contract less than your request.");
  }

  // If already commit delete old vote
  Address sender = get_sender();
  if (did_commit(sender, poll_id)) {
    uint256_t old_num_tokens = get_num_tokens(sender, poll_id);
    m_poll.at(poll_id).votes.erase(sender);
    poll_list[sender].erase({old_num_tokens, poll_id});
  }

  PollReceipt poll_receipt(num_tokens, secret_hash);
  m_poll.at(poll_id).votes.emplace(sender, poll_receipt);
  poll_list[sender].insert({num_tokens, poll_id});
}

void Voting::reveal_vote(uint256_t poll_id, bool vote_option, uint256_t salt)
{
  assert_con(is_reveal_period(poll_id),
             "Now you cannot reveal vote in this poll.");
  assert_con(did_commit(get_sender(), poll_id),
             "You didn't committed vote in this poll.");
  assert_con(!did_reveal(get_sender(), poll_id),
             "You already revealed this vote.");

  assert_con(get_hash(vote_option, salt) ==
                 get_commit_hash(get_sender(), poll_id),
             "Your vote doesn't match your commmit hash.");

  uint256_t num_tokens = get_num_tokens(get_sender(), poll_id);
  if (vote_option) {
    m_poll.at(poll_id).votes_for += num_tokens;
  } else {
    m_poll.at(poll_id).votes_against += num_tokens;
  }

  // After reveal change status in poll receipt and remove vote in address poll
  // list
  m_poll.at(poll_id).votes.at(get_sender()).status = VotingStatus::Revealed;
  poll_list[get_sender()].erase({num_tokens, poll_id});
}

uint256_t Voting::get_number_pass_token(const Address& address,
                                        const uint256_t& poll_id,
                                        const uint256_t& salt) const
{
  assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");
  assert_con(did_reveal(address, poll_id),
             "This address didn't revealed vote.");

  assert_con(get_hash(is_passed(poll_id), salt) ==
                 get_commit_hash(address, poll_id),
             "Your vote is invalid or losing vote.");

  return get_num_tokens(address, poll_id);
}

uint256_t Voting::start_poll(uint8_t vote_quorum, uint64_t commit_duration,
                             uint64_t reveal_duration)
{
  poll_nonce++;
  uint64_t commit_end_time = Global::get().block_time + commit_duration;
  uint64_t reveal_end_time = commit_end_time + reveal_duration;

  Poll poll(commit_end_time, reveal_end_time, vote_quorum);
  m_poll.insert({poll_nonce, poll});
  return poll_nonce;
}

bool Voting::is_passed(const uint256_t& poll_id) const
{
  assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");
  Poll poll = m_poll.at(poll_id);
  return (100 * poll.votes_for) >
         (poll.vote_quorum * (poll.votes_for + poll.votes_against));
}

uint256_t Voting::get_total_winning_token(const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);

  assert_con(is_poll_ended(poll_id), "Poll hasn't ended yet.");

  if (is_passed(poll_id))
    return m_poll.at(poll_id).votes_for;
  else
    return m_poll.at(poll_id).votes_against;
}

bool Voting::is_poll_ended(const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  return is_expired(m_poll.at(poll_id).reveal_end_time);
}

bool Voting::is_commit_period(const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  return !is_expired(m_poll.at(poll_id).commit_end_time);
}

bool Voting::is_reveal_period(const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  return !is_expired(m_poll.at(poll_id).reveal_end_time) &&
         !is_commit_period(poll_id);
}

bool Voting::did_commit(const Address& address, const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  auto it = m_poll.at(poll_id).votes.find(address);
  if (it == m_poll.at(poll_id).votes.end()) {
    return false;
  }
  return true;
}

bool Voting::did_reveal(const Address& address, const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  assert_vote_exist(address, poll_id);
  return m_poll.at(poll_id).votes.at(address).status == VotingStatus::Revealed;
}

void Voting::assert_poll_exists(const uint256_t& poll_id) const
{
  if (auto it = m_poll.find(poll_id); it == m_poll.end())
    throw Error("This poll doesn't exist.");
}

void Voting::assert_vote_exist(const Address& address,
                               const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  auto it = m_poll.at(poll_id).votes.find(address);
  if (it == m_poll.at(poll_id).votes.end()) {
    throw Error("Not found your vote in poll_id {}.", poll_id);
  }
}
Hash Voting::get_commit_hash(const Address& address,
                             const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  assert_vote_exist(address, poll_id);

  return m_poll.at(poll_id).votes.at(address).commit_hash;
}

uint256_t Voting::get_num_tokens(const Address& address,
                                 const uint256_t& poll_id) const
{
  assert_poll_exists(poll_id);
  assert_vote_exist(address, poll_id);

  return m_poll.at(poll_id).votes.at(address).num_tokens;
}

uint256_t Voting::get_maximum_vote(const Address& address) const
{
  // Check address in poll_list
  if (auto it = poll_list.find(address); it == poll_list.end())
    return 0;

  // Check set is empty
  if (poll_list.at(address).empty())
    return 0;

  return poll_list.at(address).rbegin()->first;
}

bool Voting::is_expired(uint64_t terminate_time) const
{
  return Global::get().block_time > terminate_time;
}

Hash Voting::get_hash(bool vote, const uint256_t& salt) const
{
  Buffer buf;
  buf << vote << salt;
  return sha256(gsl::make_span(buf.to_raw_string()));
}

void Voting::debug_create() const
{
  DEBUG(log, "Voting contract created at {}", m_addr);
}

void Voting::debug_save() const
{
  DEBUG(log, "Total Poll: {}", poll_nonce);

  for (auto& [poll_id, poll] : m_poll) {
    DEBUG(log, "  Result in Poll {}", poll_id);
    DEBUG(log, "    commit_end_time: {}", poll.commit_end_time);
    DEBUG(log, "    reveal_end_time: {}", poll.reveal_end_time);
    DEBUG(log, "    vote quorum: {}", poll.vote_quorum);
    DEBUG(log, "    votes for: {}", poll.votes_for);
    DEBUG(log, "    votes against: {}", poll.votes_against);
    DEBUG(log, "  List of voters");
    for (auto& [addr, poll_receipt] : poll.votes) {
      DEBUG(log, "    {}: {} voted {}", poll_receipt.status._to_string(), addr,
            poll_receipt.num_tokens);
    }
    DEBUG(log, "------------------------------------------------------");
  }

  DEBUG(log, "Total Voter: {}", voting_power.size());
  for (auto& [addr, vp] : voting_power) {
    DEBUG(log, "  {} has {}", addr, vp);
  }
  DEBUG(log, "======================================================");
}

std::unique_ptr<Contract> Voting::clone() const
{
  return std::make_unique<Voting>(*this);
}
