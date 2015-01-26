//
//  RPCServer.h
//  PPFramework
//
//  Created by Dave on 12/1/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__RPCServer__
#define __PPFramework__RPCServer__

#define DEFAULT_PORT_NUMBER 9090

#include <stdio.h>

#define MP_MEMORY_BOOST
#define MP_UNORDERED_MAP_BOOST
#define MP_FUNCTIONAL_BOOST

#include <msgpack/rpc/server.h>

#include "Procedure.hpp"
#include "ProcExchangePublicKeys.hpp"


namespace ppf {

template <class CryptosystemType>
    class RPCServer : public msgpack::rpc::dispatcher  {
public:
//    typedef void (*procRunFn_ptr)(CryptosystemType&, typename CryptosystemType::PublicKeyCSType**, msgpack::rpc::request);


    RPCServer(CryptosystemType *cs = NULL, int port = DEFAULT_PORT_NUMBER, std::string ip = "0.0.0.0")
    : _localCS(cs) {
        _server.serve(this);
        _server.listen(ip, port);
        SERVER_LOG("listening on port " << port);

        registerProc(new ProcExchangePublicKeys<CryptosystemType>());
    }
    
    ~RPCServer() {
        _server.close();
        std::cout << "Destroying server instance" << std::endl;
    }
    
    template <class T>
    void registerProc(T *proc) {
        proc->setCSes(_localCS, &_remoteCS);
        _procs.insert(std::make_pair(T::procName(), proc));
    }

    void run() {
        _server.run(2);  // run at least enough threads for concurrent execution
    }
    

    void dispatch(msgpack::rpc::request req)
    try {
        std::string procName;
        req.method().convert(&procName);
        
        SERVER_LOG("called with proc name: " << procName);
        
        Procedure<CryptosystemType> *proc = _procs.at(procName);
        
        SERVER_LOG("Found procedure '" << proc << "'. Executing.");
                
        proc->run(req);
        
    } catch (std::out_of_range& e) {
        req.error(msgpack::rpc::NO_METHOD_ERROR);
        return;
    } catch (msgpack::type_error& e) {
        req.error(msgpack::rpc::ARGUMENT_ERROR);
        return;
    } catch (std::exception& e) {
        req.error(std::string(e.what()));
        return;
    }
    
private:
    msgpack::rpc::server _server;

    CryptosystemType *_localCS = NULL;
    typename CryptosystemType::PublicKeyCSType *_remoteCS = NULL;
    std::unordered_map<std::string, Procedure<CryptosystemType>*> _procs;
};

}

#endif /* defined(__PPFramework__RPCServer__) */
