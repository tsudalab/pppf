//
//  PrivateKeyCryptosystem.h
//  PPFramework
//
//  Created by Dave on 11/30/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__PrivateKeyCryptosystem__
#define __PPFramework__PrivateKeyCryptosystem__

#include <stdio.h>
#include "CryptosystemBase.hpp"
#include "Plain.hpp"


namespace ppf {
    template<class CipherBase>
    class PrivateKeyInterface : virtual public CryptosystemBase<CipherBase> {
    public:
        
        class Cipher : public CryptosystemBase<CipherBase>::template Cipher<PrivateKeyInterface> {
            
        public:
            
            inline operator Plain() const { Plain p; this->_cs.dec(p, *this->_cb, false); return p; }
            inline operator long() const { Plain p; this->_cs.dec(p, *this->_cb, true); return p.raw().get_si(); }
            
            mpz_class as_mpz_class() const { Plain p; this->_cs.dec(p, *this->_cb, false); return p.raw(); }
            
            operator std::string() const { Plain p; this->_cs.dec(p, *this->_cb, true); std::ostringstream s; s << '{' << p << '}'; return s.str(); }
            
            friend std::ostream& operator<<(std::ostream& os, const Cipher& b) { return os << (std::string) b; }
            using CryptosystemBase<CipherBase>::template Cipher<PrivateKeyInterface>::Cipher;

            inline Cipher& operator+= (const Plain& p1) {
                PP_DEBUG_MSG("Using PrivateKeyInterface + operator");
                Plain p2;
                this->_cs.dec(p2, *this->_cb);
                this->_cs.enc(*this->_cb, p1+p2);
                return *this;
            };

            inline Cipher& operator+= (const CipherBase& c) {
                PP_DEBUG_MSG("Using PrivateKeyInterface + operator");
                Plain p1;
                this->_cs.dec(p1, c);
                *this += p1;
                return *this;
            };

            
            inline Cipher& operator-= (const Plain& p1) {
                PP_DEBUG_MSG("Using PrivateKeyInterface - operator");
                Plain p2;
                this->_cs.dec(p2, *this->_cb);
                this->_cs.enc(*this->_cb, p2-p1);
                return *this;
            };
            
            inline Cipher& operator-= (const CipherBase& c) {
                PP_DEBUG_MSG("Using PrivateKeyInterface - operator");
                Plain p1;
                this->_cs.dec(p1, c);
                *this -= p1;
                return *this;
            };

            
            inline Cipher& operator*= (const Plain& p1) {
                PP_DEBUG_MSG("DEBUG: Using PrivateKeyInterface * operator");
                Plain p2;
                this->_cs.dec(p2, *this->_cb);
                this->_cs.enc(*this->_cb, p1*p2);
                return *this;
            };
            
            inline Cipher& operator*= (const CipherBase& c) {
                PP_DEBUG_MSG("DEBUG: Using PrivateKeyInterface * operator");
                Plain p1;
                this->_cs.dec(p1, c);
                *this *= p1;
                return *this;
            };
            
//            inline Cipher& operator+ (const Cipher c) { *this += c; return *this; }
//            inline Cipher& operator* (const Cipher c) { *this *= c; return *this; }

//            inline bool operator==(const Plain& rhs) const { Plain p = *this; return (p == rhs); };
//            inline bool operator< (const Plain& rhs) const { return ((Plain) *this < rhs); };
//            inline bool operator> (const Plain& rhs) const { return ((Plain) *this > rhs); };
//            inline bool operator!=(const Plain& rhs) const { return !operator==(rhs);}
//            inline bool operator<=(const Plain& rhs) const { return !operator>(rhs); };
//            inline bool operator>=(const Plain& rhs) const { return !operator<(rhs); };
            

        };

        virtual void dec(Plain&, CipherBase, bool signedValue = false) = 0;
    };
}

#endif /* defined(__PPFramework__PrivateKeyCryptosystem__) */
