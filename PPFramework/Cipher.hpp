//
//  Cipher.h
//  PPFramework
//
//  Created by Dave on 12/26/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__Cipher__
#define __PPFramework__Cipher__

#include <stdio.h>


namespace ppf {
    
    template<class T>
    class Cipher {
    public:
        using RawType = T;
        
        Cipher() { };

        //        inline operator mpz_class() const& { Plain p; this->_cs.dec(p, *this->_cb); return p.raw(); }
        
        T& raw() { return this->_c; }
        const T raw() const { return this->_c; }
        //        void setRaw(const T r) { this->_c = r; }
        
    protected:
        T _c;
    };
    
}

#endif /* defined(__PPFramework__Cipher__) */
