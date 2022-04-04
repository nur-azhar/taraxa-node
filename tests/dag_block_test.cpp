#include "dag/dag_block.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "common/static_init.hpp"
#include "common/types.hpp"
#include "common/util.hpp"
#include "dag/dag.hpp"
#include "logger/logger.hpp"
#include "node/node.hpp"
#include "util_test/samples.hpp"
#include "util_test/util.hpp"
#include "vdf/sortition.hpp"

namespace taraxa::core_tests {
const unsigned NUM_BLK = 4;
const unsigned BLK_TRX_LEN = 4;
const unsigned BLK_TRX_OVERLAP = 1;
using namespace vdf_sortition;

struct DagBlockTest : BaseTest {};
struct DagBlockMgrTest : BaseTest {};

auto g_blk_samples = samples::createMockDagBlkSamples(0, NUM_BLK, 0, BLK_TRX_LEN, BLK_TRX_OVERLAP);

auto g_secret = dev::Secret("3800b2875669d9b2053c1aff9224ecfdc411423aac5b5a73d7a45ced1c3b9dcd",
                            dev::Secret::ConstructFromStringType::FromHex);
auto g_key_pair = dev::KeyPair(g_secret);

TEST_F(DagBlockTest, clear) {
  std::string str("8f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
  ASSERT_EQ(str.size(), 64);
  uint256_hash_t test(str);
  ASSERT_EQ(test.toString(), "8f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
  test.clear();
  ASSERT_EQ(test.toString(), "0000000000000000000000000000000000000000000000000000000000000000");
}

TEST_F(DagBlockTest, serialize_deserialize) {
  SortitionParams sortition_params(0xFFFF, 0xe665, 0, 5, 5, 1500);
  vrf_sk_t sk(
      "0b6627a6680e01cea3d9f36fa797f7f34e8869c3a526d9ed63ed8170e35542aad05dc12c"
      "1df1edc9f3367fba550b7971fc2de6c5998d8784051c5be69abc9644");
  level_t level = 3;
  VdfSortition vdf(sortition_params, sk, getRlpBytes(level));
  blk_hash_t vdf_input(200);
  vdf.computeVdfSolution(sortition_params, vdf_input.asBytes(), false);
  DagBlock blk1(blk_hash_t(1), 2, {}, {}, {}, vdf, secret_t::random());
  auto b = blk1.rlp(true);
  DagBlock blk2(b);
  EXPECT_EQ(blk1, blk2);

  DagBlock blk3(blk1.getJsonStr());
  EXPECT_EQ(blk1, blk3);
}

TEST_F(DagBlockTest, send_receive_one) {
  uint256_hash_t send("8f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");

  std::vector<uint8_t> data;
  {
    vectorstream strm(data);
    write(strm, send);
  }

  ASSERT_EQ(data.size(), 32);
  bufferstream strm2(data.data(), data.size());
  uint256_hash_t recv;
  read(strm2, recv);

  ASSERT_EQ(send, recv);
}

TEST_F(DagBlockTest, send_receive_two) {
  using std::string;
  using std::vector;
  vector<uint256_hash_t> outgoings{blk_hash_t("8f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"),
                                   blk_hash_t("7f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f")};

  vector<uint8_t> data;
  {
    vectorstream strm(data);
    for (auto const& i : outgoings) {
      write(strm, i);
    }
  }
  ASSERT_EQ(data.size(), 32 * 2);

  vector<uint256_hash_t> receivings(2);
  bufferstream strm2(data.data(), data.size());
  for (auto i = 0; i < 2; ++i) {
    uint256_hash_t& recv = receivings[i];
    read(strm2, recv);
  }
  ASSERT_EQ(outgoings, receivings);
}

TEST_F(DagBlockTest, send_receive_three) {
  using std::string;
  using std::vector;
  vector<uint256_hash_t> outgoings{blk_hash_t("8f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"),
                                   blk_hash_t("7f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f"),
                                   blk_hash_t("6f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f")};

  vector<uint8_t> data;
  {
    vectorstream strm(data);
    for (auto const& i : outgoings) {
      write(strm, i);
    }
  }
  ASSERT_EQ(data.size(), 32 * 3);

  vector<uint256_hash_t> receivings(3);
  bufferstream strm2(data.data(), data.size());
  for (auto i = 0; i < 3; ++i) {
    uint256_hash_t& recv = receivings[i];
    read(strm2, recv);
  }
  ASSERT_EQ(outgoings, receivings);
}

TEST_F(DagBlockTest, sender_and_hash_verify) {
  DagBlock blk1(blk_hash_t(111),   // pivot
                0,                 // level
                {blk_hash_t(222),  // tips
                 blk_hash_t(333), blk_hash_t(444)},
                {trx_hash_t(555),  // trxs
                 trx_hash_t(666)},
                g_secret);
  EXPECT_EQ(g_key_pair.address(), blk1.getSender());
  EXPECT_TRUE(blk1.verifySig());

  DagBlock blk_from_rlp(blk1.rlp(true));
  EXPECT_EQ(blk_from_rlp.getSender(), blk1.getSender());
  EXPECT_EQ(blk_from_rlp.getHash(), blk1.getHash());
}

TEST_F(DagBlockTest, sign_verify) {
  DagBlock blk1(blk_hash_t(111),   // pivot
                0,                 // level
                {blk_hash_t(222),  // tips
                 blk_hash_t(333), blk_hash_t(444)},
                {trx_hash_t(555),  // trxs
                 trx_hash_t(666)},
                g_secret);
  DagBlock blk1c(blk_hash_t(111),   // pivot
                 0,                 // level
                 {blk_hash_t(222),  // tips
                  blk_hash_t(333), blk_hash_t(444)},
                 {trx_hash_t(555),  // trxs
                  trx_hash_t(666)},
                 g_secret);
  EXPECT_EQ(blk1.getSig(), blk1c.getSig()) << blk1 << std::endl << blk1c;
  EXPECT_EQ(blk1.getSender(), blk1c.getSender());
  EXPECT_EQ(blk1.getHash(), blk1c.getHash());

  EXPECT_TRUE(blk1.verifySig());

  DagBlock blk2(blk_hash_t(9999),  // pivot
                0,                 // level
                {},                // tips,
                {}, g_secret);     // trxs

  EXPECT_NE(blk1.getSig(), blk2.getSig());
  EXPECT_NE(blk1.getHash(), blk2.getHash());
  EXPECT_EQ(blk2.getSender(), blk1.getSender());

  EXPECT_TRUE(blk2.verifySig());
}

TEST_F(DagBlockMgrTest, proposal_period) {
  auto node = create_nodes(1).front();
  auto db = node->getDB();
  auto dag_blk_mgr = node->getDagBlockManager();

  // Proposal period 0 has in DB already at DAG block manager constructor
  auto proposal_period = db->getProposalPeriodForDagLevel(10);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 0);
  proposal_period = db->getProposalPeriodForDagLevel(100);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 0);

  db->saveProposalPeriodDagLevelsMap(110, *proposal_period + 1);
  proposal_period = db->getProposalPeriodForDagLevel(101);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 1);
  proposal_period = db->getProposalPeriodForDagLevel(110);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 1);

  db->saveProposalPeriodDagLevelsMap(130, *proposal_period + 1);
  proposal_period = db->getProposalPeriodForDagLevel(111);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 2);
  proposal_period = db->getProposalPeriodForDagLevel(130);
  EXPECT_TRUE(proposal_period);
  EXPECT_EQ(*proposal_period, 2);

  // Proposal period not exsit
  proposal_period = db->getProposalPeriodForDagLevel(131);
  EXPECT_FALSE(proposal_period);
}

TEST_F(DagBlockMgrTest, incorrect_tx_estimation) {
  auto node = create_nodes(1).front();
  auto db = node->getDB();
  auto dag_blk_mgr = node->getDagBlockManager();

  auto trx = samples::createSignedTrxSamples(0, 1, g_secret).front();
  node->getTransactionManager()->insertTransaction(trx);
  // Generate DAG blocks
  auto dag_genesis = node->getConfig().chain.dag_genesis_block.getHash();
  SortitionConfig vdf_config(node->getConfig().chain.sortition);
  auto propose_level = 1;
  const auto period_block_hash = node->getDB()->getPeriodBlockHash(propose_level);
  vdf_sortition::VdfSortition vdf1(vdf_config, node->getVrfSecretKey(),
                                   VrfSortitionBase::makeVrfInput(propose_level, period_block_hash));
  vdf1.computeVdfSolution(vdf_config, dag_genesis.asBytes(), false);

  // transactions.size and estimations size is not equal
  {
    DagBlock blk(dag_genesis, propose_level, {}, {trx->getHash()}, {}, vdf1, node->getSecretKey());
    EXPECT_EQ(node->getDagBlockManager()->insertAndVerifyBlock(std::move(blk)),
              DagBlockManager::InsertAndVerifyBlockReturnType::IncorrectTransactionsEstimation);
  }

  // wrong estimated tx
  {
    DagBlock blk(dag_genesis, propose_level, {}, {trx->getHash()}, {100}, vdf1, node->getSecretKey());
    EXPECT_EQ(node->getDagBlockManager()->insertAndVerifyBlock(std::move(blk)),
              DagBlockManager::InsertAndVerifyBlockReturnType::IncorrectTransactionsEstimation);
  }
}

TEST_F(DagBlockMgrTest, too_big_dag_block) {
  // make config
  auto node_cfgs = make_node_cfgs<20>(1);
  node_cfgs.front().chain.dag.gas_limit = 250000;

  auto node = create_nodes(node_cfgs).front();
  auto db = node->getDB();

  std::vector<trx_hash_t> hashes;
  std::vector<u256> estimations;
  for (uint32_t i = 0; i < 5; ++i) {
    Transaction create_trx(i, 100, 0, 0, dev::fromHex(samples::greeter_contract_code), node->getSecretKey());
    auto [ok, err_msg] = node->getTransactionManager()->insertTransaction(create_trx);
    EXPECT_EQ(ok, true);
    hashes.emplace_back(create_trx.getHash());
    const auto& e = node->getTransactionManager()->estimateTransactionByHash(create_trx.getHash(), std::nullopt);
    std::cout << "estimation: " << e << std::endl;
    estimations.emplace_back(e);
  }

  for (uint32_t i = 0; i < hashes.size(); ++i) {
    std::cout << hashes[i] << ": " << estimations[i] << std::endl;
  }

  // Generate DAG block
  auto dag_genesis = node->getConfig().chain.dag_genesis_block.getHash();
  SortitionConfig vdf_config(node->getConfig().chain.sortition);
  auto propose_level = 1;
  const auto period_block_hash = node->getDB()->getPeriodBlockHash(propose_level);
  vdf_sortition::VdfSortition vdf1(vdf_config, node->getVrfSecretKey(),
                                   VrfSortitionBase::makeVrfInput(propose_level, period_block_hash));
  vdf1.computeVdfSolution(vdf_config, dag_genesis.asBytes(), false);

  {
    DagBlock blk(dag_genesis, propose_level, {}, hashes, estimations, vdf1, node->getSecretKey());
    EXPECT_EQ(node->getDagBlockManager()->insertAndVerifyBlock(std::move(blk)),
              DagBlockManager::InsertAndVerifyBlockReturnType::BlockTooBig);
  }
}

TEST_F(DagBlockMgrTest, single_node_create_execute_transaction) {
  auto node_cfgs = make_node_cfgs<5, true>(1);
  node_cfgs.front().chain.dag.gas_limit = 250000;
  auto node = create_nodes(node_cfgs, true /*start*/).front();

  Transaction trx(0, 100, 0, 0, dev::fromHex(samples::greeter_contract_code), node->getSecretKey());
  auto [ok, err_msg] = node->getTransactionManager()->insertTransaction(trx);
  EXPECT_EQ(ok, true);

  EXPECT_HAPPENS({60s, 1s}, [&](auto& ctx) {
    WAIT_EXPECT_EQ(ctx, node->getDB()->getNumTransactionExecuted(), 1)
    WAIT_EXPECT_EQ(ctx, node->getTransactionManager()->getTransactionCount(), 1)
    WAIT_EXPECT_EQ(ctx, node->getDagManager()->getNumVerticesInDag().first, 2)
  });

  auto res = node->getFinalChain()->transaction_receipt(trx.getHash());
  auto contract_addr = res->new_contract_address;

  std::cout << "First trx executed ..." << std::endl;
  std::cout << "Send second trx ..." << std::endl;

  auto greet = [&] {
    auto ret = node->getFinalChain()->call({
        node->getAddress(),
        0,
        contract_addr,
        0,
        0,
        0,
        // greet()
        dev::fromHex("0xcfae3217"),
    });
    return dev::toHexPrefixed(ret.code_retval);
  };
  ASSERT_EQ(greet(),
            // "Hello"
            "0x0000000000000000000000000000000000000000000000000000000000000020"
            "000000000000000000000000000000000000000000000000000000000000000548"
            "656c6c6f000000000000000000000000000000000000000000000000000000");
  auto executed_count = node->getDB()->getNumTransactionExecuted();
  {
    for (int i = 1; i < 102; ++i) {
      auto [ok, err_msg] = node->getTransactionManager()->insertTransaction(
          Transaction(i, 11, 0, 0,
                      // setGreeting("Hola")
                      dev::fromHex("0xa4136862000000000000000000000000000000000000000000000000"
                                   "00000000000000200000000000000000000000000000000000000000000"
                                   "000000000000000000004486f6c61000000000000000000000000000000"
                                   "00000000000000000000000000"),
                      node->getSecretKey(), contract_addr));
      executed_count += 1;
      EXPECT_HAPPENS({60s, 100ms}, [&](auto& ctx) {
        WAIT_EXPECT_EQ(ctx, node->getDB()->getNumTransactionExecuted(), executed_count)
      });
    }
  }
  {
    for (int i = 102; i < 127; ++i) {
      auto [ok, err_msg] = node->getTransactionManager()->insertTransaction(
          Transaction(i, 11, 0, 0,
                      // setGreeting("Hola")
                      dev::fromHex("0xa4136862000000000000000000000000000000000000000000000000"
                                   "00000000000000200000000000000000000000000000000000000000000"
                                   "000000000000000000004486f6c61000000000000000000000000000000"
                                   "00000000000000000000000000"),
                      node->getSecretKey(), contract_addr));
    }
    ASSERT_EQ(ok, true);
  }
  EXPECT_HAPPENS({60s, 1s}, [&](auto& ctx) {
    WAIT_EXPECT_EQ(ctx, node->getDB()->getNumTransactionExecuted(), 124)
    WAIT_EXPECT_EQ(ctx, node->getTransactionManager()->getTransactionCount(), 127)
    WAIT_EXPECT_EQ(ctx, node->getDagManager()->getNumVerticesInDag().first, 104)
  });
  std::cout << " TESTING CHANGED GREET" << std::endl;
  ASSERT_EQ(greet(),
            // "Hola"
            "0x000000000000000000000000000000000000000000000000000000000000002000"
            "00000000000000000000000000000000000000000000000000000000000004486f"
            "6c6100000000000000000000000000000000000000000000000000000000");
}

}  // namespace taraxa::core_tests

using namespace taraxa;
int main(int argc, char** argv) {
  static_init();
  auto logging = logger::createDefaultLoggingConfig();
  logging.verbosity = logger::Verbosity::Error;

  addr_t node_addr;
  logger::InitLogging(logging, node_addr);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}