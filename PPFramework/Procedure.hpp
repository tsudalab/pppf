//
//  Procedure.h
//  PPFramework
//
//  Created by Dave on 12/1/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__Procedure__
#define __PPFramework__Procedure__

#include <stdio.h>

#include "Cryptosystem.hpp"
#include "PrivateKeyInterface.hpp"

#include "PackingUtils.hpp"


#define SERVER_LOG(str) std::cout << "SERVER: " << str << std::endl;
#define PROC_ERROR(_str) { std::cerr << _str << std::endl; throw(_str); }

namespace ppf {
    
    
    template<class CryptosystemType>
    class Procedure {
        
    public:
        static const char* procName() { assert("Trying to use abstract procedure."); return "Unknown"; };
        
        using LocalRawType = typename CryptosystemType::Cipher::RawType;
        using LocalCipher = typename CryptosystemType::Cipher;
        using RemoteRawType = typename CryptosystemType::PublicKeyCSType::Cipher::RawType;
        using RemoteCipher = typename CryptosystemType::PublicKeyCSType::Cipher;
        
        
        template <typename... T>
        std::tuple<T...> unpack(msgpack::rpc::request req) {

            msgpack::type::tuple<typename ppf::replace_type<RemoteCipher, RemoteRawType,
                                typename ppf::replace_type<LocalCipher, LocalRawType,
                                typename ppf::replace_type<std::vector<LocalCipher>, std::vector<LocalRawType>,
                                typename ppf::replace_type<std::vector<RemoteCipher>, std::vector<RemoteRawType>,
                                typename ppf::replace_type<std::vector<std::vector<LocalCipher>>, std::vector<std::vector<LocalRawType>>,
                                typename ppf::replace_type<std::vector<std::vector<RemoteCipher>>, std::vector<std::vector<RemoteRawType>>,
                                T>::type>::type>::type>::type>::type>::type...> rawParams;
            req.params().convert(&rawParams);

            std::tuple<T...> outParams;
            
            transform_tuple(outParams, rawParams, CipherUnpackerTransform<CryptosystemType, typename CryptosystemType::PublicKeyCSType>(localCS, remoteCS));

            return outParams;
        }
        
        void setCSes(CryptosystemType *_localCS, typename CryptosystemType::PublicKeyCSType **_remoteCS) {
            localCS = _localCS;
            remoteCS = _remoteCS;
        }
        
        virtual void run(msgpack::rpc::request req) {
            std::cerr << "Procedure " << typeid(*this).name() << " does not implement a run() method." << std::endl;
        };
        
        template <class T>
        void packResult(msgpack::rpc::request& req, T t) {
            ppf::packResult<typename CryptosystemType::Cipher, typename CryptosystemType::PublicKeyCSType::Cipher>(req, t);
        }

        
    protected:
        CryptosystemType *localCS;
        typename CryptosystemType::PublicKeyCSType **remoteCS;
    };
}

#endif /* defined(__PPFramework__Procedure__) */
