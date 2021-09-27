#pragma once

#include <libp2p/Common.h>

#include <atomic>
#include <boost/noncopyable.hpp>

#include "util/util.hpp"

namespace taraxa::network::tarcap {

class TaraxaPeer : public boost::noncopyable {
 public:
  TaraxaPeer()
      : known_dag_blocks_(10000, 1000),
        known_transactions_(100000, 10000),
        known_pbft_blocks_(10000, 1000),
        known_votes_(10000, 1000) {}
  explicit TaraxaPeer(dev::p2p::NodeID id)
      : m_id(id),
        known_dag_blocks_(10000, 1000),
        known_transactions_(100000, 10000),
        known_pbft_blocks_(10000, 1000),
        known_votes_(100000, 1000) {}

  /**
   * @brief Mark dag block as known
   *
   * @param _hash
   * @return true in case dag block was actually marked as known(was not known before), otherwise false (was already
   * known)
   */
  bool markDagBlockAsKnown(blk_hash_t const &_hash) { return known_dag_blocks_.insert(_hash); }
  bool isDagBlockKnown(blk_hash_t const &_hash) const { return known_dag_blocks_.count(_hash); }

  /**
   * @brief Mark transaction as known
   *
   * @param _hash
   * @return true in case transaction was actually marked as known(was not known before), otherwise false (was already
   * known)
   */
  bool markTransactionAsKnown(trx_hash_t const &_hash) { return known_transactions_.insert(_hash); }
  bool isTransactionKnown(trx_hash_t const &_hash) const { return known_transactions_.count(_hash); }

  // PBFT
  /**
   * @brief Mark vote as known
   *
   * @param _hash
   * @return true in case vote was actually marked as known(was not known before), otherwise false (was already known)
   */
  bool markVoteAsKnown(vote_hash_t const &_hash) { return known_votes_.insert(_hash); }
  bool isVoteKnown(vote_hash_t const &_hash) const { return known_votes_.count(_hash); }

  /**
   * @brief Mark pbft block as known
   *
   * @param _hash
   * @return true in case pbft block was actually marked as known(was not known before), otherwise false (was already
   * known)
   */
  bool markPbftBlockAsKnown(blk_hash_t const &_hash) { return known_pbft_blocks_.insert(_hash); }
  bool isPbftBlockKnown(blk_hash_t const &_hash) const { return known_pbft_blocks_.count(_hash); }

  const dev::p2p::NodeID &getId() const { return m_id; }

  std::atomic<bool> syncing_ = false;
  std::atomic<uint64_t> dag_level_ = 0;
  std::atomic<uint64_t> pbft_chain_size_ = 0;
  std::atomic<uint64_t> pbft_round_ = 1;
  std::atomic<size_t> pbft_previous_round_next_votes_size_ = 0;

 private:
  dev::p2p::NodeID m_id;

  ExpirationCache<blk_hash_t> known_dag_blocks_;
  ExpirationCache<trx_hash_t> known_transactions_;
  // PBFT
  ExpirationCache<blk_hash_t> known_pbft_blocks_;
  ExpirationCache<vote_hash_t> known_votes_;  // for peers
};

}  // namespace taraxa::network::tarcap