#include "governance.h"

#include "contract/token.h"
#include "contract/voting.h"
#include "store/global.h"

Governance::Governance(const Address& governance_id, const Address& _token_id,
                       const Address& _voting_id, uint8_t _losing_threshold,
                       uint8_t _winning_threshold,
                       const uint256_t& _min_deposit, uint64_t _commit_duration,
                       uint64_t _reveal_duration)
    : Contract(governance_id)
    , UpgradableImpl<GovernanceParameters>({_losing_threshold,
                                            _winning_threshold, _min_deposit,
                                            _commit_duration, _reveal_duration},
                                           governance_id)
    , token_id(_token_id)
    , voting_id(_voting_id)

{
  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  if (vote.token_id != token_id) {
    throw Error("Token type doesn't match with voting contract");
  }
}

uint256_t Governance::propose_new_parameter(Address contract_id, Buffer buffer)
{
  assert_con(in_progress.count(contract_id) == 0,
             "This registry 's parameter is in progress.");

  auto& upgradable_object = Global::get().m_ctx->get<Upgradable>(contract_id);

  assert_con(upgradable_object.get_governance_id() == m_addr,
             "Governance id of TCR contract doesn't match this address.");
  // Transfer min_deposit to contract
  Token& token = Global::get().m_ctx->get<Token>(token_id);
  token.transfer(m_addr, params.min_deposit);

  // Create poll
  Voting& vote = Global::get().m_ctx->get<Voting>(voting_id);
  uint256_t poll_id =
      vote.start_poll(params.losing_threshold, params.winning_threshold,
                      params.commit_duration, params.reveal_duration);

  // We need to read buffer to params first?
  Buffer buf = buffer;
  // Create Proposal
  nonce_proposal++;
  Proposal proposal{poll_id,
                    contract_id,
                    get_sender(),
                    buffer,
                    upgradable_object.to_string(buf),
                    false};
  m_proposals.emplace(nonce_proposal, proposal);

  in_progress.emplace(contract_id, true);
  return nonce_proposal;
}

void Governance::resolve_proposal(uint256_t proposal_id)
{
  assert_con(m_proposals.count(proposal_id) == 1, "Proposal not found");

  Proposal& proposal = m_proposals.at(proposal_id);
  assert_con(!proposal.is_resolved, "Proposal was resolved already");

  auto& vote = Global::get().m_ctx->get<Voting>(voting_id);
  uint8_t result = vote.get_result(proposal.poll_id);
  switch (result) {
    case VoteResult::Approve: {
      // Update parameter and send money back
      // set token back

      set_sender();
      Token& token = Global::get().m_ctx->get<Token>(token_id);
      token.transfer(proposal.proposer, params.min_deposit);

      Global::get()
          .m_ctx->get<Upgradable>(proposal.contract_id)
          .upgrade(proposal.data);
      break;
    }
    case VoteResult::NoVote: {
      // Don't update parameter and send money back
      set_sender();
      Token& token = Global::get().m_ctx->get<Token>(token_id);
      token.transfer(proposal.proposer, params.min_deposit);
      break;
    }
    case VoteResult::Reject:
      // Don't update and don't send money back

      break;
    default:
      throw Failure("No matching vote option.");
  }

  proposal.is_resolved = true;
  in_progress.erase(proposal.contract_id);
}

void Governance::debug_create() const
{
  DEBUG(log, "Governance contract created at {}", m_addr);
}

void Governance::debug_save() const
{
  DEBUG(log, "Total Proposals: {}", m_proposals.size());

  for (auto& [proposal_id, proposal] : m_proposals) {
    DEBUG(log, "  ProposalID {}", proposal_id);
    DEBUG(log, "    poll_id: {}", proposal.poll_id);
    DEBUG(log, "    contract_id: {}", proposal.contract_id);
    DEBUG(log, "    proposer: {}", proposal.proposer);
    DEBUG(log, "    is_resolved: {}", proposal.is_resolved);
    DEBUG(log, "    parameters {}:", proposal.detail);
  }

  DEBUG(log, "------------------------------------------------------");

  DEBUG(log, "Total In progress: {}", in_progress.size());
  for (auto& addr : in_progress) {
    DEBUG(log, "  Address: ", addr.first);
  }
  DEBUG(log, "======================================================");
}

std::unique_ptr<Contract> Governance::clone() const
{
  return std::make_unique<Governance>(*this);
}
