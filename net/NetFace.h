/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

// TODO re-generate

#ifndef TARAXA_NODE_NET_NET_FACE_H_
#define TARAXA_NODE_NET_NET_FACE_H_

#include <libweb3jsonrpc/ModularServer.h>

namespace taraxa::net {

class NetFace : public ServerInterface<NetFace> {
 public:
  NetFace() {
    this->bindAndAddMethod(
        jsonrpc::Procedure("net_version", jsonrpc::PARAMS_BY_POSITION,
                           jsonrpc::JSON_STRING, NULL),
        &NetFace::net_versionI);
    this->bindAndAddMethod(
        jsonrpc::Procedure("net_peerCount", jsonrpc::PARAMS_BY_POSITION,
                           jsonrpc::JSON_STRING, NULL),
        &NetFace::net_peerCountI);
    this->bindAndAddMethod(
        jsonrpc::Procedure("net_listening", jsonrpc::PARAMS_BY_POSITION,
                           jsonrpc::JSON_BOOLEAN, NULL),
        &NetFace::net_listeningI);
  }

  inline virtual void net_versionI(const Json::Value &request,
                                   Json::Value &response) {
    (void)request;
    response = this->net_version();
  }
  inline virtual void net_peerCountI(const Json::Value &request,
                                     Json::Value &response) {
    (void)request;
    response = this->net_peerCount();
  }
  inline virtual void net_listeningI(const Json::Value &request,
                                     Json::Value &response) {
    (void)request;
    response = this->net_listening();
  }
  virtual std::string net_version() = 0;
  virtual std::string net_peerCount() = 0;
  virtual bool net_listening() = 0;
};

}  // namespace taraxa::net

#endif  // TARAXA_NODE_NET_NET_FACE_H_