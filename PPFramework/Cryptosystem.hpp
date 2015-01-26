//
//  Cryptosystem.h
//  PPFramework
//
//  Created by Dave on 11/28/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__Cryptosystem__
#define __PPFramework__Cryptosystem__

#include <stdio.h>
#include "PrivateKeyInterface.hpp"
#include "CryptosystemBase.hpp"
#include "Plain.hpp"
#include "PrivateKeyOverrides.hpp"

namespace ppf {
    // Main Cryptosystem class (inherited by any custom cryptosystem: merges all interfaces and cipher interfaces):
    
    template <class CipherBase, class... Interfaces>
    class Cryptosystem : virtual public Interfaces... {
        
    public:
        
        class Cipher
                : virtual public Interfaces::Cipher...
                , virtual public CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>
                , virtual public PrivateKeyOverrides<CipherBase, is_among<PrivateKeyInterface<CipherBase>, Interfaces...>::value, Interfaces...>
        {
                        
        public:
            using RawType = typename CipherBase::RawType;

            Cipher(Cryptosystem& e)
                : CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>(e, this->__c)
                , Interfaces::Cipher(e, this->__c)...
                , PrivateKeyOverrides<CipherBase, is_among<PrivateKeyInterface<CipherBase>, Interfaces...>::value, Interfaces...>(e, this->__c) { }
            
            Cipher(const Cipher& c)
            : CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>(c.getCS(), this->__c)
            , Interfaces::Cipher(c.getCS(), this->__c)...
            , PrivateKeyOverrides<CipherBase, is_among<PrivateKeyInterface<CipherBase>, Interfaces...>::value, Interfaces...>(c.getCS(), this->__c) {
                //Todo: ensure this is a deep copy:
                this->__c.raw() = c.__c.raw();
            }



            using CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>::operator=;

            using CryptosystemBase<CipherBase>::template Cipher<Cryptosystem<CipherBase, Interfaces...>>::getCS;
            
//            operator std::string() const { std::ostringstream s; s << "{#" << this << '}'; return s.str(); }

            friend std::ostream& operator<<(std::ostream& os, const Cipher& b) {
                std::string s = (std::string) b;
                os << s;
                return os;
            }

            typename CipherBase::RawType& raw() { return this->__c.raw(); }
            const typename CipherBase::RawType raw() const { return this->_c.raw(); }

        protected:
            CipherBase __c;
        };
    };
    

    // Cipher binary operators need to be defined as external members:

    template <class C, class = typename C::RawType>
    C operator+ (C lhs, const C& rhs) { lhs += rhs; return lhs; }
    
    template <class C, class = typename C::RawType>
    inline C operator+ (C lhs, const Plain& rhs) { lhs += rhs; return lhs; }

    template <class C, class = typename C::RawType>
    inline C operator*(C lhs, const C& rhs) { lhs *= rhs; return lhs; }
    
    template <class C, class = typename C::RawType>
    inline C operator* (C lhs, const Plain& rhs) { lhs *= rhs; return lhs; }
    
    template <class C, class = typename C::RawType>
    inline C operator-(C lhs, const C& rhs) { lhs -= rhs; return lhs; }
    
    template <class C, class = typename C::RawType>
    inline C operator- (C lhs, const Plain& rhs) { lhs -= rhs; return lhs; }
    

}



#endif /* defined(__PPFramework__Cryptosystem__) */
