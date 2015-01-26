//
//  ProcExchangePublicKeys.h
//  PPFramework
//
//  Created by Dave dV on 12/11/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__ProcExchangePublicKeys__
#define __PPFramework__ProcExchangePublicKeys__

#include <stdio.h>

#include "Procedure.hpp"
#include "Cryptosystem.hpp"

namespace ppf {
    
    template<class CryptosystemType>
    class ProcExchangePublicKeys : public Procedure<CryptosystemType> {
        
    public:
        static const char* procName() { return "XCHANGE_KEYS"; };

        //TODO: Ensure that both local and remote CS are compatible...
        void run(msgpack::rpc::request req) {
            try {
                msgpack::type::tuple<std::string> params;
                req.params().convert(&params);
                std::string remotePublicKey = params.get<0>();
                
                if(remotePublicKey.length() > 0) {
                    SERVER_LOG("setting client public key: " << remotePublicKey.substr(0,5) << "[...]");
                    
                    if(*this->remoteCS != NULL)
                        delete *this->remoteCS;
                    *this->remoteCS = new typename CryptosystemType::PublicKeyCSType(remotePublicKey.c_str());
                }
            } catch (msgpack::type_error& e) {
                SERVER_LOG("need a client public key parameter (can be empty)");
            }
        
            std::string localPublicKey;
            if(this->localCS) {
                localPublicKey = this->localCS->getPublicKey();
                SERVER_LOG("returning server public key: " << localPublicKey.substr(0,5) << "[...]");
            }
            else {
                localPublicKey = "";
                SERVER_LOG("returning empty public key (no server key set)");
            }
         
            req.result(localPublicKey);
        };
    };

}

#endif /* defined(__PPFramework__ProcExchangePublicKeys__) */
