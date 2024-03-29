/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

#ifndef JSONRPC_CPP_STUB_TARAXA_NET_DEBUGCLIENT_H_
#define JSONRPC_CPP_STUB_TARAXA_NET_DEBUGCLIENT_H_

#include <jsonrpccpp/client.h>

namespace taraxa {
namespace net {
class DebugClient : public jsonrpc::Client {
 public:
  DebugClient(jsonrpc::IClientConnector& conn, jsonrpc::clientVersion_t type = jsonrpc::JSONRPC_CLIENT_V2)
      : jsonrpc::Client(conn, type) {}

  Json::Value debug_traceTransaction(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_traceTransaction", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_traceCall(const Json::Value& param1, const std::string& param2) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    p.append(param2);
    Json::Value result = this->CallMethod("debug_traceCall", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_getPreviousBlockCertVotes(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_getPreviousBlockCertVotes", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_getPeriodTransactionsWithReceipts(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_getPeriodTransactionsWithReceipts", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_getPeriodDagBlocks(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_getPeriodDagBlocks", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value trace_call(const Json::Value& param1, const Json::Value& param2,
                         const std::string& param3) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    p.append(param2);
    p.append(param3);
    Json::Value result = this->CallMethod("trace_call", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value trace_replayTransaction(const std::string& param1,
                                      const Json::Value& param2) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    p.append(param2);
    Json::Value result = this->CallMethod("trace_replayTransaction", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value trace_replayBlockTransactions(const std::string& param1,
                                            const Json::Value& param2) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    p.append(param2);
    Json::Value result = this->CallMethod("trace_replayBlockTransactions", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_dposValidatorTotalStakes(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_dposValidatorTotalStakes", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
  Json::Value debug_dposTotalAmountDelegated(const std::string& param1) throw(jsonrpc::JsonRpcException) {
    Json::Value p;
    p.append(param1);
    Json::Value result = this->CallMethod("debug_dposTotalAmountDelegated", p);
    if (result.isObject())
      return result;
    else
      throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());
  }
};

}  // namespace net
}  // namespace taraxa
#endif  // JSONRPC_CPP_STUB_TARAXA_NET_DEBUGCLIENT_H_
