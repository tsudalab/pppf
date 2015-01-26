//
//  RPCClient.h
//  PPFramework
//
//  Created by Dave on 12/1/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__RPCClient__
#define __PPFramework__RPCClient__

#define DEFAULT_SERVER_IP "127.0.0.1"

#include <stdio.h>

#define MP_MEMORY_BOOST
#define MP_UNORDERED_MAP_BOOST
#define MP_FUNCTIONAL_BOOST

#include <msgpack/rpc/client.h>

#include "Procedure.hpp"
#include "ProcExchangePublicKeys.hpp"

#include "ProcObliviousMult.hpp"

namespace ppf {
    
    
    // get tuple indices:
    
    template<int...> struct index_tuple{};
    
    template<int I, typename IndexTuple, typename... Types>
    struct make_indexes_impl;
    
    template<int I, int... Indexes, typename T, typename ... Types>
    struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...> {
        typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
    };
    
    template<int I, int... Indexes>
    struct make_indexes_impl<I, index_tuple<Indexes...> > {
        typedef index_tuple<Indexes...> type;
    };
    
    template<typename ... Types>
    struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...> {};


    template<class T1, class T2>
    typename std::enable_if<!std::is_same<T1, T2>::value && std::is_pointer<T2>::value, void>::type
    packConditional(T1& t1, T2 t2) {
        t1 = t2->raw();
    }

    template<class T1, class T2>
    typename std::enable_if<!std::is_same<T1, T2>::value && !std::is_pointer<T2>::value, void>::type
    packConditional(T1& t1, T2 t2) {
        t1 = t2.raw();
    }

    template<class T1, class T2>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    packConditional(T1& t1, T2 t2) {
        t1 = t2;
    }
    
    template<class T1, class T2>
    typename std::enable_if<!std::is_same<T1, T2>::value, void>::type
    packConditional(std::vector<std::vector<T1>>& t1, std::vector<std::vector<T2>> t2) {
        for(auto v : t2) {
            std::vector<T1> vOut;
            for(auto e : v)
                vOut.push_back(e.raw());
            t1.push_back(vOut);
        }
    }
    
    template<class T1, class T2>
    typename std::enable_if<!std::is_same<T1, T2>::value, void>::type
    packConditional(std::vector<T1>& t1, std::vector<T2> t2) {
        for(auto e : t2)
            t1.push_back(e.raw());
    }

    
    // Unpack tuple and apply to function:
    template<class R, class A, class... Args, int... Indexes >
    R apply_helper(const A obj, ppf::index_tuple< Indexes... >, std::tuple<Args...>&& tup)  {
        return obj->msgpack::rpc::client::call(std::forward<Args>( std::get<Indexes>(tup))...);
    }
    
    template<class R, class A, class ... Args>
    R apply(const A obj, const std::tuple<Args...>&  tup) {
        return apply_helper<R>(obj, typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
    }
    
    template<class R, class A, class ... Args>
    R apply(const A obj, std::tuple<Args...>&&  tup) {
        return apply_helper<R>(obj, typename make_indexes<Args...>::type(), std::forward<std::tuple<Args...>>(tup));
    }
    
    struct CipherPackerTransform {
        template <typename T1, typename T2>
        void operator () (T1& x1, T2 x2) {
            packConditional(x1, x2);
        }
    };
    
    template <class PrivateCryptosystemType>
    class RPCClient : public msgpack::rpc::client {
    public:
        
        RPCClient(int port = DEFAULT_PORT_NUMBER, std::string ip = DEFAULT_SERVER_IP)
        : msgpack::rpc::client(ip, port) {
//            localCS = new PrivateCryptosystemType();
            localCS = NULL;
//            auto req = this->call(ProcExchangePublicKeys<PrivateCryptosystemType>::procName(), localCS->getPublicKey());
            auto req = this->call(ProcExchangePublicKeys<PrivateCryptosystemType>::procName(), "");
            std::string publicKey = req.template get<std::string>();
            if(publicKey.length() > 0)
                remoteCS = new typename PrivateCryptosystemType::PublicKeyCSType(publicKey.c_str());
            
            this->set_timeout(3600*24);
        }


        RPCClient(PrivateCryptosystemType *cs, int port = DEFAULT_PORT_NUMBER, std::string ip = DEFAULT_SERVER_IP)
            : msgpack::rpc::client(ip, port)
            , localCS(cs) {
            auto req = this->call(ProcExchangePublicKeys<PrivateCryptosystemType>::procName(), cs->getPublicKey());
            std::string publicKey = req.template get<std::string>();
            if(publicKey.length() > 0)
                remoteCS = new typename PrivateCryptosystemType::PublicKeyCSType(publicKey.c_str());

            this->set_timeout(3600*24);

        }
        

        template <class... Args>
        auto call(Args&&... args) -> decltype(this->msgpack::rpc::client::call(args...)) {
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            return call(preArgs);
        }

        template <class... Args>
        msgpack::rpc::future call(std::tuple<Args...> preArgs) {
            
            std::tuple<
                typename ppf::replace_type<typename PrivateCryptosystemType::Cipher*, typename PrivateCryptosystemType::Cipher::RawType,
                typename ppf::replace_type<typename PrivateCryptosystemType::PublicKeyCSType::Cipher*, typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType,
                typename ppf::replace_type<typename PrivateCryptosystemType::Cipher, typename PrivateCryptosystemType::Cipher::RawType,
                typename ppf::replace_type<typename PrivateCryptosystemType::PublicKeyCSType::Cipher, typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType,
                typename ppf::replace_type<std::vector<typename PrivateCryptosystemType::Cipher>, std::vector<typename PrivateCryptosystemType::Cipher::RawType>,
                typename ppf::replace_type<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher>, std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType>,
                typename ppf::replace_type<std::vector<std::vector<typename PrivateCryptosystemType::Cipher>>, std::vector<std::vector<typename PrivateCryptosystemType::Cipher::RawType>>,
                typename ppf::replace_type<std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher>>, std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType>>,
            Args>::type>::type>::type>::type>::type>::type>::type>::type...> postArgs;
            
            transform_tuple(postArgs, preArgs, CipherPackerTransform {});
            
            try {
                return apply<msgpack::rpc::future>(this, postArgs);
            }
            catch(msgpack::rpc::remote_error& e) {
                std::cerr << "RPC Remote Exception: " << e.what() << std::endl;
                std::cerr << "error object: " << e.error() << std::endl;
                std::cerr << "result object: " << e.result() << std::endl;
                throw e;
            }
        }

        
        typename PrivateCryptosystemType::PublicKeyCSType::Cipher getRemoteCipher(msgpack::rpc::future m) {
            auto r = m.get<typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType>();
            typename PrivateCryptosystemType::PublicKeyCSType::Cipher c(*remoteCS);
            c.raw() = r;
            return c;
        }

        
        std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher> getRemoteCipherVector(msgpack::rpc::future m) {
            auto v = m.get<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType>>();
            
            std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher> v2;
            
            for(auto r : v) {
                typename PrivateCryptosystemType::PublicKeyCSType::Cipher c(*remoteCS);
                c.raw() = r;
                v2.push_back(c);
            }
            
            return v2;
        }
        
        std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher>> getRemoteCipherVectorVector(msgpack::rpc::future m) {
            auto vv = m.get<std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher::RawType>>>();
            
            std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher>> vvOut;
            
            typename PrivateCryptosystemType::PublicKeyCSType::Cipher c(*remoteCS);

            for(auto v : vv) {
                std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher> vOut;

                for(auto r : v) {
                    c.raw() = r;
                    vOut.push_back(c);
                }
                vvOut.push_back(vOut);
            }
            
            return vvOut;
        }
        

        typename PrivateCryptosystemType::Cipher getLocalCipher(msgpack::rpc::future m) {
            auto r = m.get<typename PrivateCryptosystemType::Cipher::RawType>();
            typename PrivateCryptosystemType::Cipher c(*localCS);
            c.raw() = r;
            return c;
        }
        
        
        std::vector<typename PrivateCryptosystemType::Cipher> getLocalCipherVector(msgpack::rpc::future m) {
            auto v = m.get<std::vector<typename PrivateCryptosystemType::Cipher::RawType>>();
            
            std::vector<typename PrivateCryptosystemType::Cipher> v2;
            
            for(auto r : v) {
                typename PrivateCryptosystemType::Cipher c(*localCS);
                c.raw() = r;
                v2.push_back(c);
            }
            
            return v2;
        }
        
        std::vector<std::vector<typename PrivateCryptosystemType::Cipher>> getLocalCipherVectorVector(msgpack::rpc::future m) {
            auto vv = m.get<std::vector<std::vector<typename PrivateCryptosystemType::Cipher::RawType>>>();
            
            std::vector<std::vector<typename PrivateCryptosystemType::Cipher>> vvOut;
            
            typename PrivateCryptosystemType::Cipher c(*localCS);
            
            for(auto v : vv) {
                std::vector<typename PrivateCryptosystemType::Cipher> vOut;

                for(auto r : v) {
                    c.raw() = r;
                    vOut.push_back(c);
                }
                vvOut.push_back(vOut);
            }
            
            return vvOut;
        }
        

        template <class... Args>
        typename PrivateCryptosystemType::PublicKeyCSType::Cipher getRemoteCipher(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getRemoteCipher(req);
        }
        
        template <class... Args>
        std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher> getRemoteCipherVector(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getRemoteCipherVector(req);
        }
        
        
        
        template <class... Args>
        std::vector<std::vector<typename PrivateCryptosystemType::PublicKeyCSType::Cipher>> getRemoteCipherVectorVector(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getRemoteCipherVectorVector(req);
        }
        
        template <class... Args>
        typename PrivateCryptosystemType::Cipher getLocalCipher(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getLocalCipher(req);
        }
        
        template <class... Args>
        std::vector<typename PrivateCryptosystemType::Cipher> getLocalCipherVector(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getLocalCipherVector(req);
        }
        
        template <class... Args>
        std::vector<std::vector<typename PrivateCryptosystemType::Cipher>> getLocalCipherVectorVector(Args&&... args) {
            
            auto preArgs = std::make_tuple(std::forward<Args>(args)...);
            msgpack::rpc::future req = call(preArgs);
            
            return getLocalCipherVectorVector(req);
        }

        
        typename PrivateCryptosystemType::Cipher *newLocalCipher(void) {
            if(localCS == NULL)
                assert("This client does not have a local CS");
            return new typename PrivateCryptosystemType::Cipher(*localCS);
        }
        
        typename PrivateCryptosystemType::PublicKeyCSType::Cipher *newRemoteCipher(void) {
            return new typename PrivateCryptosystemType::PublicKeyCSType::Cipher(*remoteCS);
        }
        
        
    
        PrivateCryptosystemType *localCS = NULL;
        typename PrivateCryptosystemType::PublicKeyCSType *remoteCS = NULL;
    };
}

#endif /* defined(__PPFramework__RPCClient__) */
