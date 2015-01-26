//
//  RLWECryptosystem.h
//  PPFramework
//
//  Created by Dave on 12/8/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__RLWECryptosystem__
#define __PPFramework__RLWECryptosystem__

#include <stdio.h>

#include "Cryptosystem.hpp"
#include "AdditiveInterface.hpp"
#include "MultiplicativeInterface.hpp"
#include "PrivateKeyInterface.hpp"

#include "gmp.h"

namespace ppf {
    
    typedef ppf::Cipher<std::vector<mpz_class_packable>> RLWECipher; // can also derive, e.g. to override the string cast operator
    
    class RLWECryptosystem;
    class RLWECryptosystemPrivateKey;
    
    template <class... Interfaces>
    class RLWECryptosystemBase : public Cryptosystem<RLWECipher, Interfaces...,
                                                    AdditiveInterface<RLWECipher>,
                                                    MultiplicativeInterface<RLWECipher>> {
        
    public:
        typedef RLWECryptosystem PublicKeyCSType;
        typedef RLWECryptosystemPrivateKey PrivateKeyCSType;

        void enc(RLWECipher& c, Plain p) {
            PP_DEBUG_MSG("RLWE Public Key encryption is not implemented yet");
            
            //Temporary code (storing plain value in first element of cipher vector):
            if(c.raw().size()) {
                c.raw()[0] = p.raw();
            }
            else {
                mpz_class_packable c1 = p.raw();
                c.raw().push_back(c1);
            }
        }
        
        void addTo(RLWECipher& dest, const RLWECipher x) {
            PP_DEBUG_MSG("RLWE Addition is not implemented yet");
            
            //PLACEHOLDER CODE (adding plain values stored in first element of array):
            if(x.raw().size()) {
                if(dest.raw().size() == 0) {
                    mpz_class_packable c1 = x.raw()[0];
                    dest.raw().push_back(c1);
                }
                else
                    dest.raw()[0] += x.raw()[0];
            }
        }
        
        void plainMultiplyTo(RLWECipher& dest, const Plain x) {
            PP_DEBUG_MSG("RLWE Plain multiplication is not implemented yet");
        }
        
        void multiplyTo(RLWECipher& dest, const RLWECipher x) {
            PP_DEBUG_MSG("RLWE Multiplication is not implemented yet");
        }

        const char* getPublicKey() {
            PP_DEBUG_MSG("RLWE getPublicKey is not implemented yet");
            return "RLWE getPublicKey is not implemented yet";
        }

    };
    
    class RLWECryptosystemPrivateKey : public RLWECryptosystemBase<PrivateKeyInterface<RLWECipher>> {
    public:
        RLWECryptosystemPrivateKey(int keyLength = DEFAULT_KEY_LENGTH, int otherParam = 0, int etc = 0) {
            PP_DEBUG_MSG("RLWE Key generation not implemented yet");
        }
        
        void dec(Plain& p, const RLWECipher c) {
            PP_DEBUG_MSG("RLWE Private Key Decryption is not implemented yet");
            
            //PLACEHOLDER CODE (returning plain value of c[0]):
            if(c.raw().size())
                p = c.raw()[0];
            else
                p = 0;
        }
    };
    
    //pure syntactic sugar (avoids having to type '<>'):
    class RLWECryptosystem : public RLWECryptosystemBase<> {
    public:
        
        RLWECryptosystem(const char *pubKeyHex = "") {

        };
    };

}


#endif /* defined(__PPFramework__RLWECryptosystem__) */
