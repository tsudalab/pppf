//
//  ProcObliviousMult.h
//  PPFramework
//
//  Created by Dave on 12/12/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__ProcObliviousMult__
#define __PPFramework__ProcObliviousMult__

#include <stdio.h>

#include "Procedure.hpp"
#include "Cryptosystem.hpp"
#include "gmpxx.h"

namespace ppf {
    
    
    
    template<class CryptosystemType>
    class ProcObliviousMult : public Procedure<CryptosystemType> {
        
    public:
        using LocalCipher = typename CryptosystemType::Cipher;

        static const char* procName() { return "*"; };
        
        void run(msgpack::rpc::request req) {
            auto outParams = ProcObliviousMult::template unpack<LocalCipher*, LocalCipher*>(req);

            LocalCipher *z0 = std::get<0>(outParams), *z1 = std::get<1>(outParams);
            
            SERVER_LOG("Received: " << z0 << " | " << *z0 << " | " << (Plain) *z0);
            SERVER_LOG("Received: " << z1 << " | " << *z1 << " | " << (Plain) *z1);

            *z0 = *z0 * *z1;
            
            SERVER_LOG("Returning: " << z0 << " | " << *z0 << " | " << (Plain) *z0);
            
            ProcObliviousMult::packResult(req, *z0);
            
            delete z0;
            delete z1;
        };
    };
}

#endif /* defined(__PPFramework__ProcObliviousMult__) */
