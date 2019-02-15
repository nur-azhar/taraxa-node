#include "rpc.hpp"
#include "state_block.hpp"
#include "full_node.hpp"
#include "wallet.hpp"
#include "util.hpp"

namespace taraxa{

RpcConfig::RpcConfig (std::string const &json_file):json_file_name(json_file){
	try{
		boost::property_tree::ptree doc = loadJsonFile(json_file);
		port = doc.get<uint16_t>("port");
		address = boost::asio::ip::address::from_string(doc.get<std::string>("address"));
	}
	catch (std::exception &e){
		std::cerr<<e.what()<<std::endl;
	}
}

Rpc::Rpc(boost::asio::io_context & io, std::string conf_rpc, std::shared_ptr<FullNode> node):
		verbose_(false), conf_(RpcConfig(conf_rpc)), io_context_(io), acceptor_(io), node_(node){}

std::shared_ptr<Rpc> Rpc::getShared(){
	try {
		return shared_from_this();
	}
	catch (std::bad_weak_ptr &e) {
		std::cerr<<"Rpc: "<<e.what()<<std::endl;
		assert(false);
		return nullptr;
	}
}

void Rpc::start(){
	boost::asio::ip::tcp::endpoint ep(conf_.address, conf_.port);
	acceptor_.open(ep.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address (true));

	boost::system::error_code ec;
	acceptor_.bind(ep, ec);
	if (ec){
		std::cerr<<"Error! RPC cannot bind ... "<<ec.message()<<"\n";
		throw std::runtime_error(ec.message());
	}
	acceptor_.listen();

	if (verbose_){
		std::cout<<"Rpc is listening on port "<< conf_.port<<std::endl;
	}
	waitForAccept();
}

void Rpc::waitForAccept(){
	std::shared_ptr<RpcConnection> connection (std::make_shared<RpcConnection> (getShared(), node_));
	acceptor_.async_accept(connection->getSocket(), [this, connection](boost::system::error_code const & ec){
		if (!ec){
			if (verbose_){
				std::cout<<"A connection is accepted"<<std::endl;
			}
			connection->read();
		}
		else {
			std::cerr<<"Error! Rpc async_accept error ... "<<ec.message()<<"\n";
			throw std::runtime_error(ec.message());
		}
		waitForAccept();
	});
}

void Rpc::stop(){
	acceptor_.close();
}

std::shared_ptr<RpcConnection> RpcConnection::getShared(){
	try {
		return shared_from_this();
	} 
	catch(std::bad_weak_ptr & e){
		std::cerr<<"RpcConnection: "<<e.what()<<std::endl;
		return nullptr;
	}
}

RpcConnection::RpcConnection(std::shared_ptr<Rpc> rpc, std::shared_ptr<FullNode> node):
	rpc_(rpc), node_(node), socket_(rpc->getIoContext()){
	responded_.clear();
}

void RpcConnection::read(){
	auto this_sp = getShared();
	boost::beast::http::async_read(socket_, buffer_, request_, 
		[this_sp](boost::system::error_code const &ec, size_t byte_transfered){
		if (!ec){
			std::cout<<"POST read ... "<<std::endl;
			
			// define response handler
			auto replier ([this_sp](std::string const & msg){
				// prepare response content
				std::string body = msg;
				this_sp->write_response(msg);
				// async write
				boost::beast::http::async_write(this_sp->socket_,this_sp->response_, 
					[this_sp](boost::system::error_code const &ec, size_t byte_transfered){});
			});
			// pass response handler
			if (this_sp->request_.method() == boost::beast::http::verb::post){
				std::shared_ptr<RpcHandler> rpc_handler ( 
					new RpcHandler(this_sp->rpc_, this_sp->node_, 
					this_sp->request_.body(), replier));
				try{
					rpc_handler->processRequest();
				} catch (...){
					throw;
				}

			}
		}
		else{
			std::cerr<<"Error! RPC conncetion read fail ... "<<ec.message()<<"\n";
		}
		(void) byte_transfered;
	});
}

void RpcConnection::write_response(std::string const & msg){

	if (!responded_.test_and_set()){
		response_.set("Content-Type", "application/json");
		response_.set ("Access-Control-Allow-Origin", "*");
		response_.set ("Access-Control-Allow-Headers", "Accept, Accept-Language, Content-Language, Content-Type");
		response_.set ("Connection", "close");
		response_.result (boost::beast::http::status::ok);
		response_.body() = msg;
		response_.prepare_payload ();
	}
	else{

		assert(false && "RPC already responded ...\n");
	}
}

RpcHandler::RpcHandler(std::shared_ptr<Rpc> rpc, std::shared_ptr<FullNode> node, 
	std::string const &body , 
	std::function<void(std::string const & msg)> const &response_handler):
	rpc_(rpc), node_(node), 
	body_(body), in_doc_(taraxa::strToJson(body_)), replier_(response_handler){}

std::shared_ptr<RpcHandler> RpcHandler::getShared(){
	try {
		return shared_from_this();
	} 
	catch(std::bad_weak_ptr & e){
		std::cerr<<"RpcHandler: "<<e.what()<<std::endl;
		return nullptr;
	}
}

void RpcHandler::processRequest(){
	try{	
		std::string action = in_doc_.get<std::string>("action"); 
		std::string res;
		
		if (action == "insert_dag_block"){
			try{
				blk_hash_t pivot = in_doc_.get<std::string>("pivot");
				vec_tip_t tips = asVector<blk_hash_t, std::string>(in_doc_, "tips");
				sig_t signature = "77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777";
				blk_hash_t hash = in_doc_.get<std::string>("hash"); 
				name_t publisher = in_doc_.get<std::string>("publisher");

				StateBlock blk(pivot, tips, {}, signature, hash, publisher);
				res = blk.getJsonStr(); 
				node_->storeBlock(blk);
			} catch (std::exception &e) {
				res = e.what();
			}
		} 
		else if (action == "insert_stamped_dag_block"){
			try{
				blk_hash_t pivot = in_doc_.get<std::string>("pivot");
				vec_tip_t tips = asVector<blk_hash_t, std::string>(in_doc_, "tips");
				sig_t signature = "77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777";
				blk_hash_t hash = in_doc_.get<std::string>("hash"); 
				name_t publisher = in_doc_.get<std::string>("publisher");
				time_stamp_t stamp= in_doc_.get<time_stamp_t>("stamp");
				StateBlock blk(pivot, tips, {}, signature, hash, publisher);
				res = blk.getJsonStr(); 
				node_->storeBlock(blk);
				node_->setDagBlockTimeStamp(hash, stamp);
				res += ("\n Block stamped at: " + std::to_string(stamp));
			} catch (std::exception &e) {
				res = e.what();
			}
		} 

		else if (action == "get_dag_block"){
			try{
				blk_hash_t hash = in_doc_.get<std::string>("hash");
				StateBlock blk;
				blk = node_->getDagBlock(hash);
				time_stamp_t stamp = node_->getDagBlockTimeStamp(hash);
				res = blk.getJsonStr()+ "\ntime_stamp: "+ std::to_string(stamp);
			} catch (std::exception &e) {
				res = e.what();
			}
		}
		else if (action == "get_dag_block_children"){
			try{
				blk_hash_t hash = in_doc_.get<std::string>("hash");
				time_stamp_t stamp = in_doc_.get<time_stamp_t>("stamp");

				std::vector<std::string> children;
				children = node_->getDagBlockChildren(hash, stamp);
				for (auto const & child: children){
					res+=(child+'\n');
				}
			} catch (std::exception &e) {
				res = e.what();
			}
		}
		else if (action == "get_dag_block_siblings"){
			try{
				blk_hash_t hash = in_doc_.get<std::string>("hash");
				time_stamp_t stamp = in_doc_.get<time_stamp_t>("stamp");
			 
				std::vector<std::string> siblings;
				siblings = node_->getDagBlockSiblings(hash, stamp);
				for (auto const & sibling: siblings){
					res+=(sibling+'\n');
				}
			} catch (std::exception &e) {
				res = e.what();
			}
		}
		else if (action == "get_dag_block_tips"){
			try{
				blk_hash_t hash = in_doc_.get<std::string>("hash");
				time_stamp_t stamp = in_doc_.get<time_stamp_t>("stamp");
			 
				std::vector<std::string> tips;
				tips = node_->getDagBlockTips(hash, stamp);
				for (auto const & tip: tips){
					res+=(tip+'\n');
				}
			} catch (std::exception &e) {
				res = e.what();
			}
		}
		else if (action == "draw_graph"){
			std::string filename = in_doc_.get<std::string>("filename");
			node_->drawGraph(filename);
			res = "Dag is drwan as " + filename + " on the server side ...";
		}
		else {
			res = "Unknown action "+ action;
		}
		res+="\n";
		replier_(res);
	}
	catch(std::exception const & err){
		std::cerr<<err.what()<<"\n";
		replier_(err.what());
	}
	
}
}
