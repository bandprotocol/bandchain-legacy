#include "tcr.h"

#include "contract/token.h"
#include "contract/voting.h"
#include "store/global.h"

Registry::Registry(const Address& registry_id, const Address& _token_id,
                   const Address& _voting_id)
    : Contract(registry_id)
    , token_id(_token_id)
    , voting_id(_voting_id)
{
  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  if (vote.token_id != token_id) {
    throw Error("Token type doesn't match with voting contract");
  }
}

// Callabel function
uint256_t Registry::apply(std::string data, uint256_t token_deposit)
{
  assert_con(token_deposit >= params.min_deposit,
             "You need deposit not less than min_deposit ({})."_format(
                 params.min_deposit));

  listing_nonce++;
  Listing listing(Global::get().block_time + params.apply_duration,
                  get_sender(), token_deposit, data);

  auto& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(m_addr, token_deposit);

  m_listings.emplace(listing_nonce, listing);
  return listing_nonce;
}

void Registry::deposit(uint256_t list_id, uint256_t token_deposit)
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  assert_con(get_sender() == m_listings.at(list_id).owner,
             "You aren't owner's item.");

  m_listings.at(list_id).unstake_deposit += token_deposit;

  auto& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(m_addr, token_deposit);
}

void Registry::withdraw(uint256_t list_id, uint256_t token_withdraw)
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  assert_con(get_sender() == m_listings.at(list_id).owner,
             "You aren't owner's item.");
  assert_con(token_withdraw <= m_listings.at(list_id).unstake_deposit,
             "Not enough unstake deposit to withdraw.");
  assert_con(m_listings.at(list_id).unstake_deposit - token_withdraw >=
                 params.min_deposit,
             "Not enough remained unstake deposit to stake on next challenge.");

  m_listings.at(list_id).unstake_deposit -= token_withdraw;

  auto& token = Global::get().m_ctx->get<Token>(token_id);
  Address sender = get_sender();
  set_sender();
  token.transfer(sender, token_withdraw);
}

void Registry::exit(uint256_t list_id)
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  assert_con(get_sender() == m_listings.at(list_id).owner,
             "You aren't owner's item.");
  assert_con(is_listed(list_id), "This item isn't in list.");

  assert_con(!has_been_challenge(list_id), "This item have been challenged");
  reset_listing(list_id);
}

uint256_t Registry::challenge(uint256_t list_id, std::string data)
{
  assert_con(app_was_made(list_id), "Item isn't created.");
  Listing& listing = m_listings.at(list_id);
  uint256_t min_deposit = params.min_deposit;

  assert_con(!has_been_challenge(list_id), "This item have been challenged");

  if (listing.unstake_deposit < min_deposit) {
    reset_listing(list_id);
    return 0;
  }

  challenge_nonce++;

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);

  uint256_t poll_id = vote.start_poll(
      params.vote_quorum, params.commit_duration, params.reveal_duration);

  uint256_t reward =
      ((100 - params.dispensation_percentage) * min_deposit) / 100;
  Challenge cha(poll_id, get_sender(), data, reward, min_deposit);
  m_challenges.emplace(challenge_nonce, cha);

  listing.challenge_id = challenge_nonce;
  listing.unstake_deposit -= min_deposit;

  auto& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(m_addr, min_deposit);
  return challenge_nonce;
}

void Registry::update_status(uint256_t list_id)
{
  if (can_in_list(list_id)) {
    set_to_list(list_id);
  } else if (challenge_can_be_resolved(list_id)) {
    resolve_challenge(list_id);
  } else {
    throw Error("Nothing happened.");
  }
}

void Registry::claim_reward(uint256_t challenge_id, uint256_t salt)
{
  Address sender = get_sender();
  assert_con(!token_claimed(challenge_id, sender),
             "Tokens were already claimed.");
  assert_con(m_challenges.at(challenge_id).is_resolved,
             "This challenge hasn't resolved yet.");

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  uint256_t voter_tokens = vote.get_number_pass_token(
      sender, m_challenges.at(challenge_id).poll_id, salt);
  uint256_t reward = voter_reward(sender, challenge_id, salt);

  m_challenges.at(challenge_id).remaining_winning_token -= voter_tokens;
  m_challenges.at(challenge_id).remaining_reward_pool -= reward;

  m_challenges.at(challenge_id).token_claimed.emplace(sender, true);

  set_sender();
  auto& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(sender, reward);
}

// Function for get information
uint256_t Registry::voter_reward(const Address& voter,
                                 const uint256_t challenge_id,
                                 const uint256_t& salt) const
{
  assert_con(challenge_exist(challenge_id), "Challenge doesn't exist.");

  const Challenge& cha = m_challenges.at(challenge_id);
  uint256_t remain_total_token = cha.remaining_winning_token;
  uint256_t remain_reward = cha.remaining_reward_pool;
  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);

  uint256_t voter_tokens = vote.get_number_pass_token(voter, cha.poll_id, salt);
  return (voter_tokens * remain_reward) / remain_total_token;
}

bool Registry::can_in_list(const uint256_t& list_id) const
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  return (m_listings.at(list_id).app_expire_time < Global::get().block_time &&
          !is_listed(list_id) && !has_been_challenge(list_id));
}

bool Registry::is_listed(const uint256_t& list_id) const
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  return m_listings.at(list_id).is_listed;
}

bool Registry::app_was_made(const uint256_t& list_id) const
{
  return m_listings.count(list_id) != 0;
}

bool Registry::has_been_challenge(const uint256_t& list_id) const
{
  assert_con(app_was_made(list_id), "Item isn't created.");

  uint256_t challenge_id = m_listings.at(list_id).challenge_id;
  return (challenge_id > 0 && !m_challenges.at(challenge_id).is_resolved);
}

bool Registry::challenge_can_be_resolved(const uint256_t& list_id) const
{
  assert_con(has_been_challenge(list_id),
             "Doesn't have challenge on this listee.");

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  uint256_t challenge_id = m_listings.at(list_id).challenge_id;
  return vote.is_poll_ended(m_challenges.at(challenge_id).poll_id);
}

uint256_t Registry::determine_reward(const uint256_t& challenge_id) const
{
  assert_con(challenge_exist(challenge_id), "Challenge doesn't exist.");

  assert_con(!m_challenges.at(challenge_id).is_resolved,
             "This challenge has already resolved.");

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  uint256_t poll_id = m_challenges.at(challenge_id).poll_id;
  assert_con(vote.is_poll_ended(poll_id), "Vote havn't ended yet.");

  // No people vote
  if (vote.get_total_winning_token(poll_id) == 0) {
    return 2 * m_challenges.at(challenge_id).stake;
  }

  return 2 * m_challenges.at(challenge_id).stake -
         m_challenges.at(challenge_id).remaining_reward_pool;
}

bool Registry::token_claimed(const uint256_t& challenge_id,
                             const Address& voter) const
{
  assert_con(challenge_exist(challenge_id), "Challenge doesn't exist.");

  const Challenge& cha = m_challenges.at(challenge_id);
  auto it = cha.token_claimed.find(voter);
  if (it == cha.token_claimed.end())
    return false;
  return true;
}

// Private function
void Registry::resolve_challenge(const uint256_t& list_id)
{
  uint256_t challenge_id = m_listings.at(list_id).challenge_id;

  uint256_t reward = determine_reward(challenge_id);

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);

  Challenge& cha = m_challenges.at(challenge_id);
  cha.is_resolved = true;
  cha.remaining_winning_token = vote.get_total_winning_token(cha.poll_id);

  // Case challenge failed
  if (vote.is_passed(cha.poll_id)) {
    m_listings.at(list_id).unstake_deposit += reward;
  } else {
    reset_listing(list_id);

    set_sender();
    auto& token = Global::get().m_ctx->get<Token>(token_id);
    token.transfer(cha.challenger, reward);
  }

  if (cha.remaining_winning_token == 0) {
    cha.remaining_reward_pool = 0;
  }
}

void Registry::set_to_list(const uint256_t& list_id)
{
  m_listings.at(list_id).is_listed = true;
}

void Registry::reset_listing(const uint256_t& list_id)
{
  Listing& listing = m_listings.at(list_id);
  Address owner = listing.owner;
  uint256_t left_deposit = listing.unstake_deposit;

  m_listings.erase(list_id);
  if (left_deposit > 0) {
    set_sender();
    auto& token = Global::get().m_ctx->get<Token>(token_id);
    token.transfer(owner, left_deposit);
  }
}

bool Registry::challenge_exist(const uint256_t& challenge_id) const
{
  return m_challenges.count(challenge_id) != 0;
}

void Registry::debug_create() const
{
  DEBUG(log, "TRC contract created at {}", m_addr);
}

void Registry::debug_save() const
{
  DEBUG(log, "Total Items: {}", m_listings.size());

  for (auto& [list_id, listing] : m_listings) {
    DEBUG(log, "  ListingID {}", list_id);
    DEBUG(log, "    app_expire_time: {}", listing.app_expire_time);
    DEBUG(log, "    is_listed: {}", listing.is_listed);
    DEBUG(log, "    owner: {}", listing.owner);
    DEBUG(log, "    unstake_deposit: {}", listing.unstake_deposit);
    DEBUG(log, "    last_challenge: {}", listing.challenge_id);
    DEBUG(log, "    data: {}", listing.data);
  }
  DEBUG(log, "------------------------------------------------------");

  DEBUG(log, "Total Challenges: {}", m_challenges.size());
  for (auto& [challenge_id, cha] : m_challenges) {
    DEBUG(log, "  ChallengeID {}", challenge_id);
    DEBUG(log, "    poll_id: {}", cha.poll_id);
    DEBUG(log, "    challenger: {}", cha.challenger);
    DEBUG(log, "    is_resolved: {}", cha.is_resolved);
    DEBUG(log, "    reward_left: {}", cha.remaining_reward_pool);
    DEBUG(log, "    stake: {}", cha.stake);
    DEBUG(log, "    winning_token_left: {}", cha.remaining_winning_token);
    DEBUG(log, "    data: {}", cha.data);
    DEBUG(log, "  Claimed voter:");
    for (auto& addr : cha.token_claimed) {
      DEBUG(log, "    {}", addr.first);
    }
  }
  DEBUG(log, "======================================================");
}

std::unique_ptr<Contract> Registry::clone() const
{
  return std::make_unique<Registry>(*this);
}
