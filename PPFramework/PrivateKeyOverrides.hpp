//
//  PrivateKeyOverrides.h
//  PPFramework
//
//  Created by Dave on 12/8/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__PrivateKeyOverrides__
#define __PPFramework__PrivateKeyOverrides__

#include <stdio.h>

namespace ppf {
    
    
    //Following code ensures that Private Key decryption is used when available, instead of oblivious computation, for operator overloading:
    
    template<typename...>struct is_among: std::false_type {}; template<typename T, typename... Ts>struct is_among<T, T, Ts...>: std::true_type {}; template<typename T, typename U, typename... Ts> struct is_among<T, U, Ts...>: is_among<T, Ts...> {};
    
    template <class CipherBase, bool T, class... Interfaces>
    class PrivateKeyOverrides;
    
    template <class CipherBase, class... Interfaces>
    class Cryptosystem;
    
    template <class CipherBase, class... Interfaces>
    class PrivateKeyOverrides<CipherBase, true, Interfaces...>
    : virtual public CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>, virtual public Interfaces::Cipher... {
    public:
        using PrivateKeyInterface<CipherBase>::Cipher::operator+=;
        using PrivateKeyInterface<CipherBase>::Cipher::operator*=;
        using PrivateKeyInterface<CipherBase>::Cipher::operator-=;
        
        using PrivateKeyInterface<CipherBase>::Cipher::operator std::string;
        
        PrivateKeyOverrides(Cryptosystem<CipherBase, Interfaces...>& cs, CipherBase& c)
        : Interfaces::Cipher(cs, c)...
        , CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>(cs, c) {
        };
    };
    
    template <class CipherBase, class... Interfaces>
    class PrivateKeyOverrides<CipherBase, false, Interfaces...> : virtual public CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>, virtual public Interfaces::Cipher... {
    public:
        PrivateKeyOverrides(Cryptosystem<CipherBase, Interfaces...>& cs, CipherBase& c)
        : Interfaces::Cipher(cs, c)...
        , CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>(cs, c) {
        };
    };
    
}


#endif /* defined(__PPFramework__PrivateKeyOverrides__) */
