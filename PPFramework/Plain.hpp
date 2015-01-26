//
//  Plain.h
//  PPFramework
//
//  Created by Dave on 12/3/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__Plain__
#define __PPFramework__Plain__

#include <stdio.h>

#include "gmpxx.h"

#include "msgpack.hpp"

#include "Cryptosystem.hpp"

#include <iomanip>

namespace ppf {
    
    class mpz_class_packable : public mpz_class {
    public:
        using mpz_class::mpz_class;
        using mpz_class::operator=;
            
        template <typename Packer>
        void msgpack_pack(Packer& pk) const
        {
            char *buf;
            size_t size;
            buf = (char *) mpz_export(NULL, &size, 1, 1, 0, 0, this->get_mpz_t());
            msgpack::type::raw_ref r1((char *) buf, (uint32_t) size);
            msgpack::type::make_define(r1).msgpack_pack(pk);
            free((void *) buf);
        }
        
        void msgpack_unpack(msgpack::object const& o)
        {
            msgpack::type::raw_ref r;
            msgpack::type::make_define(r).msgpack_unpack(o);
            mpz_import(this->get_mpz_t(), r.size, 1, 1, 0, 0, r.ptr);
        }
    };

    typedef mpz_class_packable plain_t;
    
    class Plain {
        
    public:
        Plain() {};
        Plain(const Plain &p) : _p(p._p) {};
        Plain(plain_t p) : _p(p) {};
        Plain(long p) : _p(p) {};
        
//        operator mpz_class() { return _p; }
//        const operator mpz_class() const { return _p; }
        mpz_class& raw() { return _p; }
        const mpz_class raw() const { return _p; }
        
//        operator plain_t() const { return _p; }
        mpz_class& as_mpz_class() { return _p; }
//        operator long() const { return _p.get_si(); }
        
        operator std::string() const { return _p.get_str(); }
        
        friend std::ostream& operator<<(std::ostream& os, const Plain& p) { return os << p._p.get_str(); }
        
        inline bool operator==(const Plain rhs) const { return (_p == rhs._p); };
        inline bool operator!=(const Plain rhs) const { return (_p != rhs._p); };
        inline bool operator< (const Plain rhs) const { return (_p < rhs._p); };
        inline bool operator> (const Plain rhs) const { return (_p > rhs._p); };
        inline bool operator<=(const Plain rhs) const { return (_p <= rhs._p); };
        inline bool operator>=(const Plain rhs) const { return (_p >= rhs._p); };
        
        inline Plain& operator= (const Plain &pSource) { _p = pSource._p; return *this; };
//        inline Plain& operator= (const plain_t &mSource) { _p = mSource; return *this; };
//        inline Plain& operator= (long lSource) { _p = lSource; return *this; };
        
        inline Plain& operator+= (long l) { _p += l; return *this; };
        inline Plain& operator+= (const Plain p) { _p += p._p; return *this; };
        inline Plain& operator-= (long l) { _p -= l; return *this; };
        inline Plain& operator-= (const Plain p) { _p -= p._p; return *this; };
        inline Plain& operator*= (long l) { _p *= l; return *this; };
        inline Plain& operator*= (const Plain p) { _p *= p._p; return *this; };
        inline Plain& operator/= (long l) { _p /= l; return *this; };
        inline Plain& operator/= (const Plain p) { _p /= p._p; return *this; };
        
    private:
        plain_t _p;
    };
    inline Plain operator+(Plain lhs, const Plain& rhs) { lhs += rhs; return lhs; }
    inline Plain operator+(Plain lhs, const long rhs) { lhs += rhs; return lhs; }
    inline Plain operator*(Plain lhs, const Plain& rhs) { lhs *= rhs; return lhs; }
    inline Plain operator*(Plain lhs, const long rhs) { lhs *= rhs; return lhs; }
    inline Plain operator-(Plain lhs, const Plain& rhs) { lhs -= rhs; return lhs; }
    inline Plain operator-(Plain lhs, const long rhs) { lhs -= rhs; return lhs; }
    inline Plain operator/(Plain lhs, const Plain& rhs) { lhs /= rhs; return lhs; }
    inline Plain operator/(Plain lhs, const long rhs) { lhs /= rhs; return lhs; }
    
}


#endif /* defined(__PPFramework__Plain__) */
