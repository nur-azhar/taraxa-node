#pragma once

#include "network/tarcap/packets_handlers/common/ext_syncing_packet_handler.hpp"

namespace taraxa::network::tarcap {

class DagSyncPacketHandler : public ExtSyncingPacketHandler {
 public:
  DagSyncPacketHandler(std::shared_ptr<PeersState> peers_state, std::shared_ptr<PacketsStats> packets_stats,
                       std::shared_ptr<SyncingState> syncing_state, std::shared_ptr<PbftChain> pbft_chain,
                       std::shared_ptr<PbftManager> pbft_mgr, std::shared_ptr<DagManager> dag_mgr,
                       std::shared_ptr<DagBlockManager> dag_blk_mgr, const addr_t& node_addr);

  DagSyncPacketHandler(const DagSyncPacketHandler&) = default;
  DagSyncPacketHandler& operator=(const DagSyncPacketHandler&) = default;
  DagSyncPacketHandler(DagSyncPacketHandler&&) = default;
  DagSyncPacketHandler& operator=(DagSyncPacketHandler&&) = default;
  virtual ~DagSyncPacketHandler() = default;

 private:
  void process(const PacketData& packet_data, const std::shared_ptr<TaraxaPeer>& peer) override;
};

}  // namespace taraxa::network::tarcap
