//
//  AdditiveCryptosystem.h
//  PPFramework
//
//  Created by Dave on 11/28/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__AdditiveCryptosystem__
#define __PPFramework__AdditiveCryptosystem__

#include <stdio.h>
#include <iostream>
#include "gmp.h"

#include "CryptosystemBase.hpp"
#include "Plain.hpp"

#include "PrivateKeyInterface.hpp"

namespace ppf {
    template<class CipherBase>
    class AdditiveInterface : virtual public CryptosystemBase<CipherBase> {
    public:

        class Cipher : public CryptosystemBase<CipherBase>::template Cipher<AdditiveInterface> {
        public:
            using CryptosystemBase<CipherBase>::template Cipher<AdditiveInterface>::Cipher;
            using CryptosystemBase<CipherBase>::template Cipher<AdditiveInterface>::operator=;
                        
            Cipher& operator+= (const Cipher& c) {
                PP_DEBUG_MSG("Using AdditiveInterface += operator");
                this->_cs.addTo(*this->_cb, *c._cb);
                return *this;
            };
            Cipher& operator+= (const Plain& p) {
                PP_DEBUG_MSG("Using AdditiveInterface plain += operator");
                CipherBase cb;
                this->_cs.enc(cb, p);
                this->_cs.addTo(*this->_cb, cb);
                return *this;
            };

            Cipher& operator-= (const Cipher& c) {
                PP_DEBUG_MSG("Using AdditiveInterface -= operator");
                this->_cs.subFrom(*this->_cb, *c._cb);
                return *this;
            };
            Cipher& operator-= (const Plain& p) {
                PP_DEBUG_MSG("Using AdditiveInterface plain -= operator");
                this->_cs.subFrom(*this->_cb, p);
                return *this;
            }

            Cipher& operator*= (const Plain& p) {
                PP_DEBUG_MSG("Using AdditiveInterface plain * operator"); this->_cs.plainMultiplyTo(*this->_cb, p); return *this; };
            
            void recrypt() {
                this->_cs.recrypt(*this->_cb);
            }

        };
        
        virtual void addTo(CipherBase&, const CipherBase) = 0;
        virtual void plainMultiplyTo(CipherBase&, const Plain) = 0;
        
        virtual void subFrom(CipherBase& c, const CipherBase d) {
            PP_DEBUG_MSG("Using AdditiveInterface default subFrom");
            Plain p = -1;
            CipherBase md = d;
            plainMultiplyTo(md, p);
            addTo(c, md);
        }

        virtual void subFrom(CipherBase& c, const Plain p) {
            PP_DEBUG_MSG("Using AdditiveInterface default plain subFrom");
            CipherBase d;
            this->enc(d, p);
            subFrom(c, d);
        }
  
        virtual void recrypt(CipherBase& c) {
            CipherBase z; this->enc(z, 0); this->addTo(c, z);
        }

//        virtual Cipher* add(const Cipher x, const Cipher y) {
//            Cipher *dest = new Cipher(*this);
//            *dest = x;
//            addTo(*dest, y);
//            return dest;
//        };
//
//        virtual Cipher* plainMultiply(const Cipher x, const Plain a) {
//            Cipher *dest = new Cipher(*this);
//            *dest = x;
//            plainMultiplyTo(*dest, a);
//            return dest;
//        };
        

        
    };
}

#endif /* defined(__PPFramework__AdditiveCryptosystem__) */
