#include "vote.hpp"

#include <libdevcore/SHA3.h>
#include <libdevcrypto/Common.h>
#include <libethcore/Common.h>

#include "consensus/pbft_manager.hpp"

namespace taraxa {
VrfPbftSortition::VrfPbftSortition(bytes const& b) {
  dev::RLP const rlp(b);
  if (!rlp.isList()) throw std::invalid_argument("VrfPbftSortition RLP must be a list");
  pk = rlp[0].toHash<vrf_pk_t>();
  pbft_msg.blk = rlp[1].toHash<blk_hash_t>();
  pbft_msg.type = PbftVoteTypes(rlp[2].toInt<uint>());
  pbft_msg.round = rlp[3].toInt<uint64_t>();
  pbft_msg.step = rlp[4].toInt<size_t>();
  proof = rlp[5].toHash<vrf_proof_t>();
  verify();
}
bytes VrfPbftSortition::getRlpBytes() const {
  dev::RLPStream s;
  s.appendList(6);
  s << pk;
  s << pbft_msg.blk;
  s << pbft_msg.type;
  s << pbft_msg.round;
  s << pbft_msg.step;
  s << proof;
  return s.out();
}

/*
 * Sortition return true:
 * CREDENTIAL / SIGNATURE_HASH_MAX <= SORTITION THRESHOLD / VALID PLAYERS
 * i.e., CREDENTIAL * VALID PLAYERS <= SORTITION THRESHOLD * SIGNATURE_HASH_MAX
 * otherwise return false
 */
bool VrfPbftSortition::canSpeak(size_t threshold, size_t valid_players) const {
  uint1024_t left = (uint1024_t)((uint512_t)output) * valid_players;
  uint1024_t right = (uint1024_t)max512bits * threshold;
  return left <= right;
}

Vote::Vote(dev::RLP const& rlp) {
  if (!rlp.isList()) throw std::invalid_argument("vote RLP must be a list");
  blockhash_ = rlp[0].toHash<blk_hash_t>();
  vrf_sortition_ = VrfPbftSortition(rlp[1].toBytes());
  vote_signature_ = rlp[2].toHash<sig_t>();
  vote_hash_ = sha3(true);
}

Vote::Vote(bytes const& b) : Vote(dev::RLP(b)) {}

Vote::Vote(secret_t const& node_sk, VrfPbftSortition const& vrf_sortition, blk_hash_t const& blockhash)
    : vrf_sortition_(vrf_sortition), blockhash_(blockhash) {
  vote_signature_ = dev::sign(node_sk, sha3(false));
  vote_hash_ = sha3(true);
}

bytes Vote::rlp(bool inc_sig) const {
  dev::RLPStream s;
  s.appendList(inc_sig ? 3 : 2);

  s << blockhash_;
  s << vrf_sortition_.getRlpBytes();
  if (inc_sig) {
    s << vote_signature_;
  }

  return s.out();
}

void Vote::voter() const {
  if (cached_voter_) return;
  cached_voter_ = dev::recover(vote_signature_, sha3(false));
  assert(cached_voter_);
}

VoteManager::VoteManager(addr_t node_addr, std::shared_ptr<DbStorage> db, std::shared_ptr<FinalChain> final_chain,
                         std::shared_ptr<PbftChain> pbft_chain)
    : db_(db), final_chain_(final_chain), pbft_chain_(pbft_chain) {
  LOG_OBJECTS_CREATE("VOTE_MGR");
  // Retrieve votes from DB
  {
    auto unverified_votes = db_->getUnverifiedVotes();

    uniqueLock_ lock(unverified_votes_access_);
    for (auto const& v : unverified_votes) {
      auto pbft_round = v.getRound();
      auto hash = v.getHash();
      if (unverified_votes_.count(pbft_round)) {
        unverified_votes_[pbft_round][hash] = v;
      } else {
        std::unordered_map<vote_hash_t, Vote> votes{std::make_pair(hash, v)};
        unverified_votes_[pbft_round] = votes;
      }
      LOG(log_dg_) << "Retrieve unverified vote " << v;
    }
  }
  {
    auto verified_votes = db_->getVerifiedVotes();

    uniqueLock_ lock(verified_votes_access_);
    for (auto const& v : verified_votes) {
      auto pbft_round = v.getRound();
      auto hash = v.getHash();
      if (verified_votes_.count(pbft_round)) {
        verified_votes_[pbft_round][hash] = v;
      } else {
        std::unordered_map<vote_hash_t, Vote> votes{std::make_pair(hash, v)};
        verified_votes_[pbft_round] = votes;
      }
      LOG(log_dg_) << "Retrieve verified vote " << v;
    }
  }
}

void VoteManager::addUnverifiedVote(taraxa::Vote const& vote) {
  uint64_t pbft_round = vote.getRound();
  auto hash = vote.getHash();
  {
    upgradableLock_ lock(unverified_votes_access_);
    std::map<uint64_t, std::unordered_map<vote_hash_t, Vote>>::const_iterator found_round =
        unverified_votes_.find(pbft_round);
    if (found_round != unverified_votes_.end()) {
      std::unordered_map<vote_hash_t, Vote>::const_iterator found_vote = found_round->second.find(hash);
      if (found_vote != found_round->second.end()) {
        LOG(log_dg_) << "Vote " << hash << " is in unverified map already";
        return;
      }
      upgradeLock_ locked(lock);
      unverified_votes_[pbft_round][hash] = vote;
    } else {
      std::unordered_map<vote_hash_t, Vote> votes{std::make_pair(hash, vote)};
      upgradeLock_ locked(lock);
      unverified_votes_[pbft_round] = votes;
    }
  }
  LOG(log_dg_) << "Add unverified vote " << vote;
}

void VoteManager::addUnverifiedVotes(std::vector<Vote> const& votes) {
  for (auto const& v : votes) {
    addUnverifiedVote(v);
  }
}

void VoteManager::removeUnverifiedVote(uint64_t const& pbft_round, vote_hash_t const& vote_hash) {
  upgradableLock_ lock(unverified_votes_access_);
  if (unverified_votes_.count(pbft_round)) {
    upgradeLock_ locked(lock);
    unverified_votes_[pbft_round].erase(vote_hash);
  }
}

bool VoteManager::voteInUnverifiedMap(uint64_t const& pbft_round, vote_hash_t const& vote_hash) {
  sharedLock_ lock(unverified_votes_access_);
  if (unverified_votes_.count(pbft_round)) {
    return unverified_votes_[pbft_round].count(vote_hash);
  }

  return false;
}

std::vector<Vote> VoteManager::getUnverifiedVotes() {
  std::vector<Vote> votes;

  sharedLock_ lock(unverified_votes_access_);
  for (auto const& round_votes : unverified_votes_) {
    std::transform(round_votes.second.begin(), round_votes.second.end(), std::back_inserter(votes),
                   [](std::pair<const taraxa::vote_hash_t, taraxa::Vote> const& v) { return v.second; });
  }

  return votes;
}

void VoteManager::clearUnverifiedVotesTable() {
  uniqueLock_ lock(unverified_votes_access_);
  unverified_votes_.clear();
}

uint64_t VoteManager::getUnverifiedVotesSize() const {
  uint64_t size = 0;

  sharedLock_ lock(unverified_votes_access_);
  std::map<uint64_t, std::unordered_map<vote_hash_t, Vote>>::const_iterator it;
  for (it = unverified_votes_.begin(); it != unverified_votes_.end(); ++it) {
    size += it->second.size();
  }

  return size;
}

void VoteManager::addVerifiedVote(Vote const& vote) {
  auto pbft_round = vote.getRound();
  auto hash = vote.getHash();

  {
    upgradableLock_ lock(verified_votes_access_);
    std::map<uint64_t, std::unordered_map<vote_hash_t, Vote>>::const_iterator found_round =
        verified_votes_.find(pbft_round);
    if (found_round != verified_votes_.end()) {
      std::unordered_map<vote_hash_t, Vote>::const_iterator found_vote = found_round->second.find(hash);
      if (found_vote != found_round->second.end()) {
        LOG(log_dg_) << "Vote " << hash << " is in verified map already";
        return;
      }
      upgradeLock_ locked(lock);
      verified_votes_[pbft_round][hash] = vote;
    } else {
      std::unordered_map<vote_hash_t, Vote> votes{std::make_pair(hash, vote)};
      upgradeLock_ locked(lock);
      verified_votes_[pbft_round] = votes;
    }
  }
  LOG(log_dg_) << "Add verified vote " << vote;
}

bool VoteManager::voteInVerifiedMap(uint64_t const& pbft_round, vote_hash_t const& vote_hash) {
  sharedLock_ lock(verified_votes_access_);
  if (verified_votes_.count(pbft_round)) {
    return verified_votes_[pbft_round].count(vote_hash);
  }

  return false;
}

void VoteManager::clearVerifiedVotesTable() {
  uniqueLock_ lock(verified_votes_access_);
  verified_votes_.clear();
}

std::vector<Vote> VoteManager::getVerifiedVotes() {
  std::vector<Vote> votes;

  sharedLock_ lock(verified_votes_access_);
  for (auto const& round_votes : verified_votes_) {
    std::transform(round_votes.second.begin(), round_votes.second.end(), std::back_inserter(votes),
                   [](std::pair<const taraxa::vote_hash_t, taraxa::Vote> const& v) { return v.second; });
  }

  return votes;
}

// Return all verified votes >= pbft_round
std::vector<Vote> VoteManager::getVerifiedVotes(uint64_t const pbft_round, blk_hash_t const& last_pbft_block_hash,
                                                size_t const sortition_threshold, uint64_t eligible_voter_count,
                                                std::function<bool(addr_t const&)> const& is_eligible) {
  // Cleanup votes for previous rounds
  cleanupVotes(pbft_round);

  std::vector<Vote> verified_votes;
  auto votes_to_verify = getUnverifiedVotes();

  for (auto const& v : votes_to_verify) {
    addr_t voter_account_address = dev::toAddress(v.getVoter());
    // Check if the voter account is valid, malicious vote
    if (!is_eligible(voter_account_address)) {
      LOG(log_dg_) << "Account " << voter_account_address << " is not eligible to vote. Vote: " << v;
      continue;
    }

    if (voteValidation(last_pbft_block_hash, v, eligible_voter_count, sortition_threshold)) {
      verified_votes.emplace_back(v);
    }
  }

  auto batch = db_->createWriteBatch();
  for (auto const& v : verified_votes) {
    db_->addVerifiedVoteToBatch(v, batch);
    db_->removeUnverifiedVoteToBatch(v.getHash(), batch);
  }
  db_->commitWriteBatch(batch);

  for (auto const& v : verified_votes) {
    addVerifiedVote(v);
    removeUnverifiedVote(v.getRound(), v.getHash());
  }

  return getVerifiedVotes();
}

// cleanup votes < pbft_round
void VoteManager::cleanupVotes(uint64_t pbft_round) {
  // Remove unverified votes
  vector<vote_hash_t> remove_unverified_votes_hash;
  {
    upgradableLock_ lock(unverified_votes_access_);
    std::map<uint64_t, std::unordered_map<vote_hash_t, Vote>>::iterator it = unverified_votes_.begin();

    upgradeLock_ locked(lock);
    while (it != unverified_votes_.end() && it->first < pbft_round) {
      for (auto const& v : it->second) {
        remove_unverified_votes_hash.emplace_back(v.first);
      }
      it = unverified_votes_.erase(it);
    }
  }
  auto batch = db_->createWriteBatch();
  for (auto const& v_hash : remove_unverified_votes_hash) {
    db_->removeUnverifiedVoteToBatch(v_hash, batch);
  }
  db_->commitWriteBatch(batch);

  // Remove verified votes
  vector<vote_hash_t> remove_verified_votes_hash;
  {
    upgradableLock_ lock(verified_votes_access_);
    std::map<uint64_t, std::unordered_map<vote_hash_t, Vote>>::iterator it = verified_votes_.begin();

    upgradeLock_ locked(lock);
    while (it != verified_votes_.end() && it->first < pbft_round) {
      for (auto const& v : it->second) {
        remove_verified_votes_hash.emplace_back(v.first);
      }
      it = verified_votes_.erase(it);
    }
  }
  batch = db_->createWriteBatch();
  for (auto const& v_hash : remove_verified_votes_hash) {
    db_->removeVerifiedVoteToBatch(v_hash, batch);
  }
  db_->commitWriteBatch(batch);
}

bool VoteManager::voteValidation(taraxa::blk_hash_t const& last_pbft_block_hash, taraxa::Vote const& vote,
                                 size_t const valid_sortition_players, size_t const sortition_threshold) const {
  if (last_pbft_block_hash != vote.getVrfSortition().pbft_msg.blk) {
    LOG(log_tr_) << "Last pbft block hash does not match " << last_pbft_block_hash << ". " << vote;
    return false;
  }

  if (!vote.getVrfSortition().verify()) {
    LOG(log_er_) << "Invalid vrf proof. " << vote;
    return false;
  }

  if (!vote.verifyVote()) {
    LOG(log_er_) << "Invalid vote signature. " << vote;
    return false;
  }

  if (!vote.verifyCanSpeak(sortition_threshold, valid_sortition_players)) {
    LOG(log_er_) << "Vote sortition failed. " << vote;
    return false;
  }

  return true;
}

bool VoteManager::pbftBlockHasEnoughValidCertVotes(PbftBlockCert const& pbft_block_and_votes,
                                                   size_t valid_sortition_players, size_t sortition_threshold,
                                                   size_t pbft_2t_plus_1) const {
  if (pbft_block_and_votes.cert_votes.empty()) {
    LOG(log_er_) << "No any cert votes! The synced PBFT block comes from a "
                    "malicious player.";
    return false;
  }

  blk_hash_t pbft_chain_last_block_hash = pbft_chain_->getLastPbftBlockHash();
  std::vector<Vote> valid_votes;
  auto first_cert_vote_round = pbft_block_and_votes.cert_votes[0].getRound();

  for (auto const& v : pbft_block_and_votes.cert_votes) {
    // Any info is wrong that can determine the synced PBFT block comes from a malicious player
    if (v.getType() != cert_vote_type) {
      LOG(log_er_) << "For PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << ", cert vote "
                   << v.getHash() << " has wrong vote type " << v.getType();
      break;
    } else if (v.getRound() != first_cert_vote_round) {
      LOG(log_er_) << "For PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << ", cert vote "
                   << v.getHash() << " has a different vote round " << v.getRound() << ", compare to first cert vote "
                   << pbft_block_and_votes.cert_votes[0].getHash() << " has vote round " << first_cert_vote_round;
      break;
    } else if (v.getStep() != 3) {
      LOG(log_er_) << "For PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << ", cert vote "
                   << v.getHash() << " has wrong vote step " << v.getStep();
      break;
    } else if (v.getBlockHash() != pbft_block_and_votes.pbft_blk->getBlockHash()) {
      LOG(log_er_) << "For PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << ", cert vote "
                   << v.getHash() << " has wrong vote block hash " << v.getBlockHash();
      break;
    }

    if (voteValidation(pbft_chain_last_block_hash, v, valid_sortition_players, sortition_threshold)) {
      valid_votes.emplace_back(v);
    } else {
      LOG(log_wr_) << "For PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << ", cert vote "
                   << v.getHash() << " failed validation";
    }
  }

  if (valid_votes.size() < pbft_2t_plus_1) {
    LOG(log_er_) << "PBFT block " << pbft_block_and_votes.pbft_blk->getBlockHash() << " with "
                 << pbft_block_and_votes.cert_votes.size() << " cert votes. Has " << valid_votes.size()
                 << " valid cert votes. 2t+1 is " << pbft_2t_plus_1 << ", Valid sortition players "
                 << valid_sortition_players << ", sortition threshold is " << sortition_threshold
                 << ". The last block in pbft chain is " << pbft_chain_last_block_hash;
  }

  return valid_votes.size() >= pbft_2t_plus_1;
}

std::string VoteManager::getJsonStr(std::vector<Vote> const& votes) {
  Json::Value ptroot;
  Json::Value ptvotes(Json::arrayValue);

  for (Vote const& v : votes) {
    Json::Value ptvote;
    ptvote["vote_hash"] = v.getHash().toString();
    ptvote["accounthash"] = v.getVoter().toString();
    ptvote["sortition_proof"] = v.getSortitionProof().toString();
    ptvote["vote_signature"] = v.getVoteSignature().toString();
    ptvote["blockhash"] = v.getBlockHash().toString();
    ptvote["type"] = v.getType();
    ptvote["round"] = Json::Value::UInt64(v.getRound());
    ptvote["step"] = Json::Value::UInt64(v.getStep());
    ptvotes.append(ptvote);
  }
  ptroot["votes"] = ptvotes;

  return ptroot.toStyledString();
}

NextVotesForPreviousRound::NextVotesForPreviousRound(addr_t node_addr, std::shared_ptr<DbStorage> db)
    : db_(db), enough_votes_for_null_block_hash_(false), voted_value_(NULL_BLOCK_HASH), next_votes_size_(0) {
  LOG_OBJECTS_CREATE("NEXT_VOTES");
}

void NextVotesForPreviousRound::clear() {
  uniqueLock_ lock(access_);
  enough_votes_for_null_block_hash_ = false;
  voted_value_ = NULL_BLOCK_HASH;
  next_votes_size_ = 0;
  next_votes_.clear();
  next_votes_set_.clear();
}

bool NextVotesForPreviousRound::find(vote_hash_t next_vote_hash) {
  sharedLock_ lock(access_);
  return next_votes_set_.find(next_vote_hash) != next_votes_set_.end();
}

bool NextVotesForPreviousRound::enoughNextVotes() const {
  sharedLock_ lock(access_);
  return enough_votes_for_null_block_hash_ && (voted_value_ != NULL_BLOCK_HASH);
}

bool NextVotesForPreviousRound::haveEnoughVotesForNullBlockHash() const {
  sharedLock_ lock(access_);
  return enough_votes_for_null_block_hash_;
}

blk_hash_t NextVotesForPreviousRound::getVotedValue() const {
  sharedLock_ lock(access_);
  return voted_value_;
}

std::vector<Vote> NextVotesForPreviousRound::getNextVotes() {
  std::vector<Vote> next_votes_bundle;

  sharedLock_ lock(access_);
  for (auto const& blk_hash_nv : next_votes_) {
    for (auto const& v : blk_hash_nv.second) {
      next_votes_bundle.emplace_back(v);
    }
  }
  assert(next_votes_bundle.size() == next_votes_size_);

  return next_votes_bundle;
}

size_t NextVotesForPreviousRound::getNextVotesSize() const {
  sharedLock_ lock(access_);
  return next_votes_size_;
}

// Assumption is that all votes are validated, in next voting phase, in the same round.
// Votes for same voted value are in the same step
// Voted values have maximum 2 PBFT block hashes, NULL_BLOCK_HASH and a non NULL_BLOCK_HASH
void NextVotesForPreviousRound::addNextVotes(std::vector<Vote> const& next_votes, size_t const pbft_2t_plus_1) {
  if (next_votes.empty()) {
    return;
  }
  if (enoughNextVotes()) {
    LOG(log_dg_) << "Have enough next votes for prevous PBFT round.";
    return;
  }
  auto own_votes = getNextVotes();
  if (!own_votes.empty()) {
    auto own_previous_round = own_votes[0].getRound();
    auto sync_votes_previous_round = next_votes[0].getRound();
    if (own_previous_round != sync_votes_previous_round) {
      LOG(log_dg_) << "Drop it. The previous PBFT round has been at " << own_previous_round
                   << ", syncing next votes voted at round " << sync_votes_previous_round;
      return;
    }
  }
  LOG(log_dg_) << "There are " << next_votes.size() << " new next votes for adding.";

  uniqueLock_ lock(access_);

  // Add all next votes
  std::unordered_set<blk_hash_t> voted_values;
  for (auto const& v : next_votes) {
    LOG(log_dg_) << "Add next vote: " << v;

    auto vote_hash = v.getHash();
    if (!next_votes_set_.count(vote_hash)) {
      next_votes_set_.insert(vote_hash);
      auto voted_block_hash = v.getBlockHash();
      if (next_votes_.count(voted_block_hash)) {
        next_votes_[voted_block_hash].emplace_back(v);
      } else {
        std::vector<Vote> votes{v};
        next_votes_[voted_block_hash] = votes;
      }
      next_votes_size_++;
      voted_values.insert(voted_block_hash);
    }
  }

  LOG(log_dg_) << "PBFT 2t+1 is " << pbft_2t_plus_1 << " in round " << next_votes[0].getRound();
  for (auto const& voted_value : voted_values) {
    auto const& voted_value_next_votes_size = next_votes_.at(voted_value).size();
    if (voted_value_next_votes_size >= pbft_2t_plus_1) {
      LOG(log_dg_) << "Voted PBFT block hash " << voted_value << " has enough " << voted_value_next_votes_size
                   << " next votes";

      if (voted_value == NULL_BLOCK_HASH) {
        enough_votes_for_null_block_hash_ = true;
      } else {
        if (voted_value_ != NULL_BLOCK_HASH) {
          assertError_(next_votes_.at(voted_value), next_votes_.at(voted_value_));
        }

        voted_value_ = voted_value;
      }
    } else {
      // Should not happen here, have checked at updateWithSyncedVotes. For safe
      LOG(log_dg_) << "Shoud not happen here. Voted PBFT block hash " << voted_value << " has "
                   << voted_value_next_votes_size << " next votes. Not enough, removed!";
      for (auto const& v : next_votes_.at(voted_value)) {
        next_votes_set_.erase(v.getHash());
      }
      next_votes_size_ -= voted_value_next_votes_size;
      next_votes_.erase(voted_value);
    }
  }

  if (next_votes_set_.size() != next_votes_size_) {
    LOG(log_er_) << "Size of next votes set is " << next_votes_set_.size() << ", but next votes size is "
                 << next_votes_size_;
    assert(next_votes_set_.size() == next_votes_size_);
  }
  if (next_votes_.size() != 1 && next_votes_.size() != 2) {
    LOG(log_er_) << "There are " << next_votes_.size() << " voted values.";
    for (auto const& voted_value : next_votes_) {
      LOG(log_er_) << "Voted value " << voted_value.first;
    }
    assert(next_votes_.size() == 1 || next_votes_.size() == 2);
  }
}

// Assumption is that all votes are validated, in next voting phase, in the same round and step
void NextVotesForPreviousRound::update(std::vector<Vote> const& next_votes, size_t const pbft_2t_plus_1) {
  LOG(log_nf_) << "There are " << next_votes.size() << " next votes for updating.";
  if (next_votes.empty()) {
    return;
  }

  clear();

  uniqueLock_ lock(access_);

  // Copy all next votes
  for (auto const& v : next_votes) {
    LOG(log_dg_) << "Add next vote: " << v;

    next_votes_set_.insert(v.getHash());
    auto voted_block_hash = v.getBlockHash();
    if (next_votes_.count(voted_block_hash)) {
      next_votes_[voted_block_hash].emplace_back(v);
    } else {
      std::vector<Vote> votes{v};
      next_votes_[voted_block_hash] = votes;
    }
  }

  // Protect for malicious players. If no malicious players, will include either/both NULL BLOCK HASH and a non NULL
  // BLOCK HASH
  LOG(log_nf_) << "PBFT 2t+1 is " << pbft_2t_plus_1 << " in round " << next_votes[0].getRound();
  auto next_votes_size = next_votes.size();

  auto it = next_votes_.begin();
  while (it != next_votes_.end()) {
    if (it->second.size() >= pbft_2t_plus_1) {
      LOG(log_nf_) << "Voted PBFT block hash " << it->first << " has " << it->second.size() << " next votes";

      if (it->first == NULL_BLOCK_HASH) {
        enough_votes_for_null_block_hash_ = true;
      } else {
        if (voted_value_ != NULL_BLOCK_HASH) {
          assertError_(it->second, next_votes_.at(voted_value_));
        }

        voted_value_ = it->first;
      }

      it++;
    } else {
      LOG(log_dg_) << "Voted PBFT block hash " << it->first << " has " << it->second.size()
                   << " next votes. Not enough, removed!";
      for (auto const& v : it->second) {
        next_votes_set_.erase(v.getHash());
      }
      next_votes_size -= it->second.size();
      it = next_votes_.erase(it);
    }
  }

  next_votes_size_ = next_votes_size;

  if (next_votes_set_.size() != next_votes_size_) {
    LOG(log_er_) << "Size of next votes set is " << next_votes_set_.size() << ", but next votes size is "
                 << next_votes_size_;
    assert(next_votes_set_.size() == next_votes_size_);
  }
  if (next_votes_.size() != 1 && next_votes_.size() != 2) {
    LOG(log_er_) << "There are " << next_votes_.size() << " voted values.";
    for (auto const& voted_value : next_votes_) {
      LOG(log_er_) << "Voted value " << voted_value.first;
    }
    assert(next_votes_.size() == 1 || next_votes_.size() == 2);
  }
}

// Assumption is that all synced votes are in next voting phase, in the same round.
// Valid voted values have maximum 2 block hash, NULL_BLOCK_HASH and a non NULL_BLOCK_HASH
void NextVotesForPreviousRound::updateWithSyncedVotes(std::vector<Vote> const& next_votes,
                                                      size_t const pbft_2t_plus_1) {
  // TODO: need do vote verificaton
  if (next_votes.empty()) {
    LOG(log_er_) << "Synced next votes is empty.";
    return;
  }
  if (enoughNextVotes()) {
    LOG(log_dg_) << "Don't need update. Have enough next votes for previous PBFT round already.";
  }

  std::unordered_map<blk_hash_t, std::vector<Vote>> own_votes_map;
  {
    sharedLock_ lock(access_);
    own_votes_map = next_votes_;
  }
  if (own_votes_map.empty()) {
    LOG(log_nf_) << "Own next votes for previous round is empty. Node just start, reject for protecting overwrite own "
                    "next votes.";
    return;
  }

  std::unordered_map<blk_hash_t, std::vector<Vote>> synced_next_votes;
  // All next votes should be in the next voting phase and in the same voted round
  for (auto i = 0; i < next_votes.size(); i++) {
    if (next_votes[i].getType() != next_vote_type) {
      LOG(log_er_) << "Synced next vote is not at next voting phase. Vote " << next_votes[i];
      return;
    } else if (next_votes[i].getRound() != next_votes[0].getRound()) {
      LOG(log_er_) << "Synced next votes have a different voted PBFT round. Vote1 " << next_votes[0] << ", Vote2 "
                   << next_votes[i];
      return;
    }

    auto voted_block_hash = next_votes[i].getBlockHash();
    if (synced_next_votes.count(voted_block_hash)) {
      synced_next_votes[voted_block_hash].emplace_back(next_votes[i]);
    } else {
      std::vector<Vote> votes{next_votes[i]};
      synced_next_votes[voted_block_hash] = votes;
    }
  }

  // Next votes for same voted value should be in the same step
  for (auto const& voted_value_and_votes : synced_next_votes) {
    auto votes = voted_value_and_votes.second;
    auto voted_step = votes[0].getStep();
    for (auto i = 1; i < votes.size(); i++) {
      if (votes[i].getStep() != voted_step) {
        LOG(log_er_) << "Synced next votes have a different voted PBFT step. Vote1 " << votes[0] << ", Vote2 "
                     << votes[i];
        return;
      }
    }
  }

  if (synced_next_votes.size() != 1 && synced_next_votes.size() != 2) {
    LOG(log_er_) << "Synced next votes voted " << synced_next_votes.size() << " values";
    for (auto const& voted_value_and_votes : synced_next_votes) {
      LOG(log_er_) << "Synced next votes voted value " << voted_value_and_votes.first;
    }
    return;
  }

  std::vector<Vote> update_votes;
  // Don't update votes for same valid voted value that >= 2t+1
  for (auto const& voted_value_and_votes : synced_next_votes) {
    if (!own_votes_map.count(voted_value_and_votes.first)) {
      if (voted_value_and_votes.second.size() >= pbft_2t_plus_1) {
        LOG(log_nf_) << "Don't have the voted value " << voted_value_and_votes.first
                     << " for previous round. Add votes";
        for (auto const& v : voted_value_and_votes.second) {
          LOG(log_dg_) << "Add next vote " << v;
          update_votes.emplace_back(v);
        }
      } else {
        LOG(log_dg_) << "Voted value " << voted_value_and_votes.first
                     << " doesn't have enough next votes. Size of syncing next votes "
                     << voted_value_and_votes.second.size() << ", PBFT 2t+1 is " << pbft_2t_plus_1 << " for round "
                     << voted_value_and_votes.second[0].getRound();
      }
    }
  }

  if (!update_votes.empty()) {
    // Add new next votes in DB for the PBFT round
    auto voted_round = update_votes[0].getRound();
    auto next_votes_in_db = db_->getNextVotes(voted_round);
    for (auto const& v : update_votes) {
      next_votes_in_db.emplace_back(v);
    }
    db_->saveNextVotes(voted_round, next_votes_in_db);

    addNextVotes(update_votes, pbft_2t_plus_1);
  }
}

void NextVotesForPreviousRound::assertError_(std::vector<Vote> next_votes_1, std::vector<Vote> next_votes_2) const {
  if (next_votes_1.empty() || next_votes_2.empty()) {
    return;
  }

  LOG(log_er_) << "There are more than one voted values on non NULL_BLOCK_HASH have 2t+1 next votes.";

  LOG(log_er_) << "Voted value " << next_votes_1[0].getBlockHash();
  for (auto const& v : next_votes_1) {
    LOG(log_er_) << v;
  }
  LOG(log_er_) << "Voted value " << next_votes_2[0].getBlockHash();
  for (auto const& v : next_votes_2) {
    LOG(log_er_) << v;
  }

  assert(false);
}

}  // namespace taraxa