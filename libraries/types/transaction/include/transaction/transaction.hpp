#pragma once

#include <json/json.h>
#include <libdevcore/RLP.h>
#include <libdevcore/SHA3.h>

#include "common/default_construct_copyable_movable.hpp"
#include "common/types.hpp"

namespace taraxa {

struct Transaction {
  struct InvalidSignature : std::runtime_error {
    explicit InvalidSignature(std::string const &msg) : runtime_error("invalid signature:\n" + msg) {}
  };

 private:
  uint64_t nonce_ = 0;
  val_t value_ = 0;
  val_t gas_price_;
  uint64_t gas_ = 0;
  bytes data_;
  std::optional<addr_t> receiver_;
  uint64_t chain_id_ = 0;
  dev::SignatureStruct vrs_;
  mutable trx_hash_t hash_;
  mutable bool hash_initialized_ = false;
  bool is_zero_ = false;
  mutable util::DefaultConstructCopyableMovable<std::mutex> hash_mu_;
  mutable bool sender_initialized_ = false;
  mutable bool sender_valid_ = false;
  mutable addr_t sender_;
  mutable util::DefaultConstructCopyableMovable<std::mutex> sender_mu_;
  mutable std::shared_ptr<bytes> cached_rlp_;
  mutable util::DefaultConstructCopyableMovable<std::mutex> cached_rlp_mu_;

  template <bool for_signature, bool w_sender>
  void streamRLP(dev::RLPStream &s) const;
  trx_hash_t hash_for_signature() const;
  addr_t const &get_sender_() const;

 public:
  // TODO eliminate and use shared_ptr<Transaction> everywhere
  Transaction() : is_zero_(true){};
  Transaction(uint64_t nonce, val_t const &value, val_t const &gas_price, uint64_t gas, bytes data, secret_t const &sk,
              std::optional<addr_t> const &receiver = std::nullopt, uint64_t chain_id = 0);
  explicit Transaction(dev::RLP const &_rlp, bool verify_strict = false, h256 const &hash = {},
                       bool rlp_w_sender = false);
  explicit Transaction(bytes const &_rlp, bool verify_strict = false, h256 const &hash = {})
      : Transaction(dev::RLP(_rlp), verify_strict, hash) {}

  auto isZero() const { return is_zero_; }
  trx_hash_t const &getHash() const;
  addr_t const &getSender() const;
  auto getNonce() const { return nonce_; }
  auto const &getValue() const { return value_; }
  auto const &getGasPrice() const { return gas_price_; }
  auto getGas() const { return gas_; }
  auto const &getData() const { return data_; }
  auto const &getReceiver() const { return receiver_; }
  auto getChainID() const { return chain_id_; }
  auto const &getVRS() const { return vrs_; }

  bool operator==(Transaction const &other) const { return getHash() == other.getHash(); }

  std::shared_ptr<bytes> rlp(bool w_sender = false) const;

  Json::Value toJSON() const;
};

using Transactions = ::std::vector<Transaction>;

}  // namespace taraxa