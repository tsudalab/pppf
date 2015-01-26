//
//  MultiplicativeCryptosystem.h
//  PPFramework
//
//  Created by Dave on 11/28/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__MultiplicativeCryptosystem__
#define __PPFramework__MultiplicativeCryptosystem__

#include <stdio.h>
#include <iostream>
#include "gmp.h"

#include "CryptosystemBase.hpp"
#include "Plain.hpp"


namespace ppf {

    template<class CipherBase>
    class MultiplicativeInterface : virtual public CryptosystemBase<CipherBase> {
    public:
        
        class Cipher : public CryptosystemBase<CipherBase>::template Cipher<MultiplicativeInterface> {
        public:
            using CryptosystemBase<CipherBase>::template Cipher<MultiplicativeInterface>::Cipher;
            using CryptosystemBase<CipherBase>::template Cipher<MultiplicativeInterface>::operator=;
            
            Cipher& operator*= (const Cipher& c) { PP_DEBUG_MSG("Using MultiplicativeInterface operator"); this->_cs.multiplyTo(*this->_cb, *c._cb); return *this; };
        };
        
        virtual void multiplyTo(CipherBase&, const CipherBase) = 0;
        
//        virtual Cipher* multiply(const Cipher x, const Cipher y) {
//            Cipher *dest = new Cipher(*this);
//            *dest = x;
//            multiplyTo(*dest, y);
//            return dest;
//        };
        
    };

}

#endif /* defined(__PPFramework__MultiplicativeCryptosystem__) */
