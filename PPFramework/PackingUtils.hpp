//
//  PackingUtils.h
//  PPFramework
//
//  Created by Dave on 1/4/15.
//  Copyright (c) 2015 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__PackingUtils__
#define __PPFramework__PackingUtils__

#include <stdio.h>

namespace ppf {
    
    template <typename A, typename B, typename T>
    struct replace_type { using type = T; };
    
    template <typename A, typename B>
    struct replace_type<A, B, A> { using type = B; };
    
    // This templated functions conditionally executes if T1 is a pointer to T2:
//    template<class T1, class T2, class T3, class T4>
//    typename std::enable_if<std::is_same<T1*, T2>::value, void>::type
//    unpackConditional(T2& t1, T3 t2, T4& cs) {
//        T1 *c = new T1(cs);
//        c->raw() = t2;
//        t1 = c;
//        PP_DEBUG_MSG("[1a]");
//    }
//    
//    template<class T1, class T2, class T3, class T4>
//    typename std::enable_if<std::is_same<T1*, T2>::value, void>::type
//    unpackConditional(T2& t1, T3 t2, T4* cs) {
//        T1 *c = new T1(*cs);
//        c->raw() = t2;
//        t1 = c;
//        PP_DEBUG_MSG("[1b]");
//    }

    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(T2& t1, T3 t2, T4& cs) {
        t1.raw() = t2;
        PP_DEBUG_MSG("[1a]");
    }
    
    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(T2& t1, T3 t2, T4* cs) {
        t1.raw() = t2;
        PP_DEBUG_MSG("[1b]");
    }


    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<! std::is_same<T1*, T2>::value, void>::type
    unpackConditional(T2& t1, T3 t2, T4& cs) {
        PP_DEBUG_MSG("[2a]");
    }
    
    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<! std::is_same<T1*, T2>::value, void>::type
    unpackConditional(T2& t1, T3 t2, T4 *cs) {
        PP_DEBUG_MSG("[2b]");
    }
    
    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(std::vector<T2>& t1, std::vector<T3> t2, T4 *cs) {
        for(auto r : t2) {
            T1 c(*cs);
            c.raw() = r;
            t1.push_back(c);
        }
        PP_DEBUG_MSG("vector: " << t1.size() << " | " << t2.size());
        PP_DEBUG_MSG("[3a]");
    }
    
    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(std::vector<T2>& t1, std::vector<T3> t2, T4& cs) {
        for(auto r : t2) {
            T1 c(cs);
            c.raw() = r;
            t1.push_back(c);
        }
        PP_DEBUG_MSG("vector: " << t1.size() << " | " << t2.size());
        PP_DEBUG_MSG("[3b]");
    }
    
    
    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(std::vector<std::vector<T2>>& t1, std::vector<std::vector<T3>> t2, T4 *cs) {
        for(auto v : t2) {
            std::vector<T2> vOut;
            for(auto r : v) {
                T1 c(*cs);
                c.raw() = r;
                vOut.push_back(c);
            }
            t1.push_back(vOut);
        }
        PP_DEBUG_MSG("vectors of vectors: " << t1.size() << " | " << t2.size());
        PP_DEBUG_MSG("[4a]");
    }

    template<class T1, class T2, class T3, class T4>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(std::vector<std::vector<T2>>& t1, std::vector<std::vector<T3>> t2, T4& cs) {
        for(auto v : t2) {
            std::vector<T2> vOut;
            for(auto r : v) {
                T1 c(cs);
                c.raw() = r;
                vOut.push_back(c);
            }
            t1.push_back(vOut);
        }
        PP_DEBUG_MSG("vectors of vectors: " << t1.size() << " | " << t2.size());
        PP_DEBUG_MSG("[4b]");
    }

    template<class T1, class T2>
    typename std::enable_if<std::is_same<T1, T2>::value, void>::type
    unpackConditional(T1& t1, T2 t2) {
        t1 = t2;
        PP_DEBUG_MSG("[5]");
    }
    
    template<class T1, class T2>
    typename std::enable_if<! std::is_same<T1, T2>::value, void>::type
    unpackConditional(T1& t1, T2 t2) {
        PP_DEBUG_MSG("[6]");
    }
    
    // transform_tuple (and helper) runs all elements of tuple t2 through f.transform() and returns the result in t1:
    
    template <std::size_t I, std::size_t N, typename F, typename T1, typename T2>
    struct transform_tuple_helper {
        static void do_(F f, T1& t1, T2 t2) {
            f(std::get<I>(t1), std::get<I>(t2));
            transform_tuple_helper<I + 1, N, F, T1, T2>::do_(f, t1, t2);
        }
    };
    
    template <std::size_t N, typename F, typename T1, typename T2>
    struct transform_tuple_helper<N, N, F, T1, T2> {
        static void do_(F f, T1& t1, T2 t2) { }
    };
    
    template <typename F, typename T1, typename T2>
    void transform_tuple(T1& t1, T2 t2, F f) {
        transform_tuple_helper<0, std::tuple_size<T1>::value, F, T1, T2>::do_(f, t1, t2);
    }
    
    
    template <typename LocalCS, typename RemoteCS>
    struct CipherUnpackerTransform {
        RemoteCS *remoteCS;
        LocalCS *localCS;
        
        CipherUnpackerTransform(LocalCS *localCS, RemoteCS **remoteCS)
        : localCS(localCS), remoteCS(*remoteCS) {}
        
        template <typename T1, typename T2>
        void operator () (T1& x1, T2 x2) {
            if(remoteCS)
                unpackConditional<typename RemoteCS::Cipher>(x1, x2, remoteCS);
            if(localCS)
                unpackConditional<typename LocalCS::Cipher>(x1, x2, localCS);
            
            unpackConditional(x1, x2);
        }
    };
    // end unpacking tuples
    
    
    
    template<class T1a, class T1b, class T2>
    typename std::enable_if<std::is_same<T1a, T2>::value || std::is_same<T1b, T2>::value, void>::type
    packResult(msgpack::rpc::request& req, T2 t) {
        req.result(t.raw());
    }
    
    template<class T1a, class T1b, class T2>
    typename std::enable_if<std::is_same<T1a, T2>::value || std::is_same<T1b, T2>::value, void>::type
    packResult(msgpack::rpc::request& req, T2 *t) {
        req.result(t->raw());
    }
    
    template<class T1a, class T1b, class T2>
    typename std::enable_if<std::is_same<T1a, T2>::value || std::is_same<T1b, T2>::value, void>::type
    packResult(msgpack::rpc::request& req, std::vector<T2> t) {
        std::vector<typename T2::RawType> v;
        for (auto e : t)
            v.push_back(e.raw());
        
        req.result(v);
    }

    template<class T1a, class T1b, class T2>
    typename std::enable_if<std::is_same<T1a, T2>::value || std::is_same<T1b, T2>::value, void>::type
    packResult(msgpack::rpc::request& req, std::vector<std::vector<T2>> t) {
        std::vector<std::vector<typename T2::RawType>> mOut;
        for (auto v : t) {
            std::vector<typename T2::RawType> vOut;
            for (auto e : v)
                vOut.push_back(e.raw());
            mOut.push_back(vOut);
        }
        req.result(mOut);
    }

    template<class T1a, class T1b, class T2>
    typename std::enable_if<!std::is_same<T1a, T2>::value && !std::is_same<T1b, T2>::value, void>::type
    packResult(msgpack::rpc::request& req, T2 t) {
        req.result(t);
    }

}

#endif /* defined(__PPFramework__PackingUtils__) */
