//
//  ProcInnerProduct.h
//  PPFramework
//
//  Created by Dave on 12/12/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__ProcInnerProduct__
#define __PPFramework__ProcInnerProduct__

#include <stdio.h>

#include "Procedure.hpp"
#include "Cryptosystem.hpp"
#include "gmpxx.h"

namespace ppf {
    
    template<class CryptosystemType>
    class ProcInnerProduct : public Procedure<CryptosystemType> {
        
    public:
        using LocalRawType = typename CryptosystemType::Cipher::RawType;
        using LocalCipher = typename CryptosystemType::Cipher;
        using RemoteRawType = typename CryptosystemType::PublicKeyCSType::Cipher::RawType;
        using RemoteCipher = typename CryptosystemType::PublicKeyCSType::Cipher;

        static const char* procName() { return "DOT"; };
        
        void run(msgpack::rpc::request req) {
            auto outParams = ProcInnerProduct::template unpack<std::vector<RemoteCipher>>(req);
            
            std::vector<RemoteCipher> v = std::get<0>(outParams);
            
            SERVER_LOG("Received vector of length: " << v.size());

            for(int i = 0; i < v.size(); i++) {
                v[i] += v[i];
            }
            

//            LocalCipher *z0 = std::get<0>(outParams);
//            LocalCipher *z1 = std::get<1>(outParams);
            
//            SERVER_LOG("Received: " << z0 << " | " << *z0 << " | " << (Plain) *z0);
//            SERVER_LOG("Received: " << z1 << " | " << *z1 << " | " << (Plain) *z1);
//
//            *z0 = *z0 * *z1;
//            
//            SERVER_LOG("Returning: " << z0 << " | " << *z0 << " | " << (Plain) *z0);
//            
            ProcInnerProduct::packResult(req, v);
//
//            delete z0;
//            delete z1;
        };
    };
    
}

#endif /* defined(__PPFramework__ProcInnerProduct__) */
