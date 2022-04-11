#include "network/tarcap/packets_handlers/transaction_packet_handler.hpp"

#include "dag/dag_block_manager.hpp"
#include "network/tarcap/shared_states/test_state.hpp"
#include "transaction/transaction_manager.hpp"

namespace taraxa::network::tarcap {

TransactionPacketHandler::TransactionPacketHandler(std::shared_ptr<PeersState> peers_state,
                                                   std::shared_ptr<PacketsStats> packets_stats,
                                                   std::shared_ptr<TransactionManager> trx_mgr,
                                                   std::shared_ptr<DagBlockManager> dag_blk_mgr,
                                                   std::shared_ptr<TestState> test_state, const addr_t &node_addr)
    : PacketHandler(std::move(peers_state), std::move(packets_stats), node_addr, "TRANSACTION_PH"),
      trx_mgr_(std::move(trx_mgr)),
      dag_blk_mgr_(std::move(dag_blk_mgr)),
      test_state_(std::move(test_state)) {}

void TransactionPacketHandler::validatePacketRlpFormat([[maybe_unused]] const PacketData &packet_data) const {
  // Number of txs is not fixed, nothing to be checked here
}

inline void TransactionPacketHandler::process(const PacketData &packet_data, const std::shared_ptr<TaraxaPeer> &peer) {
  std::string received_transactions;
  const auto transaction_count = packet_data.rlp_.itemCount();

  SharedTransactions transactions;
  transactions.reserve(transaction_count);

  for (size_t tx_idx = 0; tx_idx < transaction_count; tx_idx++) {
    std::shared_ptr<Transaction> transaction;

    try {
      transaction = std::make_shared<Transaction>(packet_data.rlp_[tx_idx].data().toBytes());
    } catch (const Transaction::InvalidSignature &e) {
      throw MaliciousPeerException("Unable to parse transaction: " + std::string(e.what()));
    }

    if (dag_blk_mgr_) [[likely]] {  // ONLY FOR TESTING
      if (trx_mgr_->isTransactionKnown(transaction->getHash())) {
        continue;
      }
      if (const auto [is_valid, reason] = trx_mgr_->verifyTransaction(transaction); !is_valid) {
        std::ostringstream err_msg;
        err_msg << "Transaction " << transaction->getHash() << " validation failed: " << reason;

        throw MaliciousPeerException(err_msg.str());
      }
    }
    received_transactions += transaction->getHash().abridged() + " ";
    peer->markTransactionAsKnown(transaction->getHash());
    transactions.push_back(std::move(transaction));
  }

  if (transaction_count > 0) {
    LOG(log_tr_) << "Received TransactionPacket with " << packet_data.rlp_.itemCount() << " transactions";
    if (transactions.size() > 0) {
      LOG(log_dg_) << "Received TransactionPacket with " << transactions.size()
                   << " unseen transactions:" << received_transactions << " from: " << peer->getId().abridged();
    }

    onNewTransactions(std::move(transactions));
  }
}

void TransactionPacketHandler::onNewTransactions(SharedTransactions &&transactions) {
  if (dag_blk_mgr_) [[likely]] {
    received_trx_count_ += transactions.size();
    unique_received_trx_count_ += trx_mgr_->insertValidatedTransactions(std::move(transactions));
    return;
  }

  // Only for testing
  for (auto const &trx : transactions) {
    auto trx_hash = trx->getHash();
    if (!test_state_->hasTransaction(trx_hash)) {
      test_state_->insertTransaction(*trx);
      LOG(log_tr_) << "Received New Transaction " << trx_hash;
    } else {
      LOG(log_tr_) << "Received New Transaction" << trx_hash << "that is already known";
    }
  }
}

void TransactionPacketHandler::periodicSendTransactions(SharedTransactions &&transactions) {
  std::unordered_map<dev::p2p::NodeID, std::vector<taraxa::bytes>> transactions_to_send;
  std::unordered_map<dev::p2p::NodeID, std::vector<trx_hash_t>> transactions_hash_to_send;

  auto peers = peers_state_->getAllPeers();
  std::string transactions_to_log;
  std::string peers_to_log;
  for (auto const &trx : transactions) {
    transactions_to_log += trx->getHash().abridged();
  }
  for (const auto &peer : peers) {
    // Confirm that status messages were exchanged otherwise message might be ignored and node would
    // incorrectly markTransactionAsKnown
    if (!peer.second->syncing_) {
      peers_to_log += peer.first.abridged();
      for (auto const &trx : transactions) {
        auto trx_hash = trx->getHash();
        if (peer.second->isTransactionKnown(trx_hash)) {
          continue;
        }

        transactions_to_send[peer.first].push_back(trx->rlp());
        transactions_hash_to_send[peer.first].push_back(trx_hash);
      }
    }
  }

  LOG(log_tr_) << "Sending Transactions " << transactions_to_log << " to " << peers_to_log;

  for (auto &it : transactions_to_send) {
    sendTransactions(it.first, it.second);
  }
  for (auto &it : transactions_hash_to_send) {
    for (auto &it2 : it.second) {
      peers[it.first]->markTransactionAsKnown(it2);
    }
  }
}

void TransactionPacketHandler::sendTransactions(dev::p2p::NodeID const &peer_id,
                                                std::vector<taraxa::bytes> const &transactions) {
  LOG(log_tr_) << "sendTransactions " << transactions.size() << " to " << peer_id;

  uint32_t index = 0;
  while (index < transactions.size()) {
    uint32_t trx_count_to_send = std::min(static_cast<size_t>(kMaxTransactionsInPacket), transactions.size() - index);

    dev::RLPStream s(trx_count_to_send);
    taraxa::bytes trx_bytes;
    for (uint32_t i = index; i < index + trx_count_to_send; i++) {
      const auto &transaction = transactions[i];
      trx_bytes.insert(trx_bytes.end(), std::begin(transaction), std::end(transaction));
    }
    s.appendRaw(trx_bytes, trx_count_to_send);
    sealAndSend(peer_id, TransactionPacket, std::move(s));

    index += trx_count_to_send;
  }
}

}  // namespace taraxa::network::tarcap
