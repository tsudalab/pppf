//
//  CryptosystemBase.h
//  PPFramework
//
//  Created by Dave on 12/3/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__CryptosystemBase__
#define __PPFramework__CryptosystemBase__

#include <stdio.h>
#include <assert.h>

#include "gmpxx.h"


//#define PP_DEBUG_MSG(str) std::cerr << "DEBUG: " << str << std::endl
#define PP_DEBUG_MSG(str) /* std::cout << "DEBUG: " << str << std::endl */

#include "Plain.hpp"
#include "Cipher.hpp"

namespace ppf {

    template<class CipherBase>
    class CryptosystemBase {
    public:
        
        CryptosystemBase() {};
        
        template<typename Outer>
        class Cipher {
            
        public:
            Cipher(Outer& cs, CipherBase& c) : _cs(cs) {
                this->_cb = &c;
            };
            
            Cipher(const Cipher& c) : _cs(c.getCS()) {
                std::cerr << "Bad copy constructor called" << std::endl;
                assert("Danger: the object returned by this copy constructor points to the same underlying _cb");
            };

//            Cipher(Outer& cs) : _cs(cs) {};
//            Cipher(Outer& cs, long l) : _cs(cs) { *this = l; }
//            Cipher(Outer& cs, Plain p) : _cs(cs) { *this = p; }

            inline Cipher& operator= (const Cipher &cSource) {
                // Need to check that cryptosystem is same (same values)
                // assert(&(cSource._cs) == &(this->_cs));
                this->_cb->raw() = cSource._cb->raw();
                return *this;
            }
            
            inline Cipher& operator= (const CipherBase &cSource) {
                this->raw() = cSource.raw();
                return *this;
            }

            inline Cipher& operator= (const mpz_class &raw) {
                this->setRaw(raw); return *this;
            }
            inline Cipher& operator= (const Plain &pSource) {
                this->_cs.enc(*this->_cb, pSource); return *this;
            }
            inline Cipher& operator= (long l) {
                Plain p(l); *this = p; return *this;
            }

            Outer &getCS() const { return this->_cs; };
            
            
        protected:
            Outer& _cs;
            CipherBase *_cb;
            
        private:
        };
        
        virtual void enc(CipherBase& c, const Plain p) = 0;
        virtual const char* getPublicKey() = 0;

    
    };

}

#endif /* defined(__PPFramework__CryptosystemBase__) */
