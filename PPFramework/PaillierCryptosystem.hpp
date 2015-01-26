//
//  PaillierCryptosystem.h
//  PPFramework
//
//  Created by Dave on 11/28/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#ifndef __PPFramework__PaillierCryptosystem__
#define __PPFramework__PaillierCryptosystem__

#include <stdio.h>

#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#pragma GCC diagnostic pop

#include "Cryptosystem.hpp"
#include "AdditiveInterface.hpp"
#include "PrivateKeyInterface.hpp"

#include "gmp.h"

extern "C" {
#include "paillier.h"
}

#define DEFAULT_KEY_LENGTH 1024
#define USE_FAST_RECRYPT


namespace ppf {

    
    void random_generator(void* buf, int len, void *ptr) {
//        boost::random_device seed_gen;
//        boost::random::mt19937 boostRNG;
//        boostRNG.seed(seed_gen);
//        boostRNG.generate((char*)buf, (char*)buf + len);

        boost::random::mt19937 *boostRNG = (boost::random::mt19937*) ptr;
        boostRNG->generate((char*)buf, (char*)buf + len);
        
//        paillier_get_rand_devrandom(buf, len, ptr);
    }
    
    typedef ppf::Cipher<mpz_class_packable> PaillierCipher; // can also derive, e.g. to override the string cast operator

    class PaillierCryptosystem;
    class PaillierCryptosystemPrivateKey;
    
    template <class... Interfaces>
    class PaillierCryptosystemBase : public Cryptosystem<PaillierCipher, AdditiveInterface<PaillierCipher>, Interfaces...> {

    public:
        typedef PaillierCryptosystem PublicKeyCSType;
        typedef PaillierCryptosystemPrivateKey PrivateKeyCSType;

        
        void enc(PaillierCipher& c, const Plain p) {
            paillier_ciphertext_t pc;
            paillier_plaintext_t pp;

            mpz_init(pc.c);
            mpz_init_set(pp.m, p.raw().get_mpz_t());

            paillier_enc(&pc, this->_pub, &pp, (paillier_get_rand_t) random_generator, &boostRNG);

            mpz_set(c.raw().get_mpz_t(), pc.c);
            mpz_clear(pc.c);
            mpz_clear(pp.m);
        }

        void addTo(PaillierCipher& dest, const PaillierCipher src) {
            paillier_ciphertext_t pdest, psrc;
            
            mpz_init_set(pdest.c, dest.raw().get_mpz_t());
            mpz_init_set(psrc.c, src.raw().get_mpz_t());
            
            paillier_mul(_pub, &pdest, &pdest, &psrc);
            
            mpz_set(dest.raw().get_mpz_t(), pdest.c);
            
            mpz_clear(psrc.c);
            mpz_clear(pdest.c);
            
        }
        
        virtual void subFrom(PaillierCipher& dest, const Plain src) {
            paillier_ciphertext_t cdest;
            paillier_plaintext_t psrc;

            mpz_init_set(cdest.c, dest.raw().get_mpz_t());
            mpz_init_set(psrc.m, src.raw().get_mpz_t());
            mpz_sub(psrc.m, _pub->n, psrc.m);
            
            paillier_ciphertext_t *csrc = paillier_enc(NULL, this->_pub, &psrc, (paillier_get_rand_t) random_generator, &boostRNG);
            paillier_mul(_pub, &cdest, &cdest, csrc);
            
            mpz_set(dest.raw().get_mpz_t(), cdest.c);
            
            paillier_freeciphertext(csrc);
            mpz_clear(cdest.c);
            mpz_clear(psrc.m);
        }
        
        void plainMultiplyTo(PaillierCipher& dest, const Plain src) {
            paillier_ciphertext_t pdest;
            paillier_plaintext_t psrc;

            mpz_init_set(pdest.c, dest.raw().get_mpz_t());
            mpz_init_set(psrc.m, src.raw().get_mpz_t());
            
            paillier_exp(_pub, &pdest, &pdest, &psrc);
            mpz_set(dest.raw().get_mpz_t(), pdest.c);
            
            mpz_clear(psrc.m);
            mpz_clear(pdest.c);
        }
        
        void recrypt(PaillierCipher& c) {
            
            
#ifdef USE_FAST_RECRYPT
            //Fast:
#define RECRYPT_POOL_SIZE 4096
#define RECRYPT_CHOOSE_SIZE 20
            
            if(randomVals.size() < RECRYPT_POOL_SIZE) {
                //TODO: add mutex lock:
                
                mpz_class expR;
                for(int i = 0; i < RECRYPT_POOL_SIZE; i++) {
                    
                    do {
                        mpz_urandomb(expR.get_mpz_t(), gmpRandState, _pub->bits);
                    }
                    while( mpz_cmp(expR.get_mpz_t(), _pub->n) >= 0 );
                    mpz_powm(expR.get_mpz_t(), expR.get_mpz_t(), _pub->n, _pub->n_squared);
                    
                    randomVals.push_back(expR);
                }
            }

            for(int i = 0; i < RECRYPT_CHOOSE_SIZE && i < randomVals.size(); i++) {
                int randIdx = rand() % randomVals.size();
                mpz_mul(c.raw().get_mpz_t(), c.raw().get_mpz_t(), randomVals[randIdx].get_mpz_t());
                mpz_mod(c.raw().get_mpz_t(), c.raw().get_mpz_t(), _pub->n_squared);
            }

#else
            //Slow (new random each time):
            mpz_t r;
            mpz_init(r);
            do {
                mpz_urandomb(r, gmpRandState, _pub->bits);
            }
            while( mpz_cmp(r, _pub->n) >= 0 );
            
            mpz_powm(r, r, _pub->n, _pub->n_squared);
            mpz_mul(c.raw().get_mpz_t(), c.raw().get_mpz_t(), r);
            mpz_mod(c.raw().get_mpz_t(), c.raw().get_mpz_t(), _pub->n_squared);

#endif
        }

        const char* getPublicKey() {
            return paillier_pubkey_to_hex(_pub);
        }
        
        const int getBitLength() {
            return _pub->bits;
        }
        
    protected:
        paillier_pubkey_t *_pub;
        mpz_t _halfN;
        mpz_t _minusOne;
        std::vector<mpz_class> randomVals;
        boost::random::mt19937 boostRNG;
        gmp_randstate_t gmpRandState;
        
        void initRand()
        {
            boost::random_device seed_gen;
            boostRNG.seed(seed_gen);

            void* buf;
            mpz_t s;
            int bytes = _pub->bits / 8 + 1;
            
            buf = malloc(bytes);
            random_generator(buf, bytes, &boostRNG);
            
            gmp_randinit_default(gmpRandState);
            mpz_init(s);
            mpz_import(s, bytes, 1, 1, 0, 0, buf);
            gmp_randseed(gmpRandState, s);
            mpz_clear(s);
            free(buf);
            
            srand((int) time(NULL));
        }

        void initConstants() {
            mpz_init(_halfN);
            mpz_tdiv_q_2exp(_halfN, _pub->n, 1);
            
            mpz_init(_minusOne);
            mpz_sub_ui(_minusOne, _pub->n, 1);
            
            initRand();
        }
        
        void destroyConstants() {
            mpz_clear(_halfN);
            mpz_clear(_minusOne);
        }
    };
    
    class PaillierCryptosystemPrivateKey : public PaillierCryptosystemBase<PrivateKeyInterface<PaillierCipher>> {
    public:
        PaillierCryptosystemPrivateKey(int keyLength = DEFAULT_KEY_LENGTH) {
            paillier_keygen(keyLength, &_pub, &_prv, paillier_get_rand_devrandom, NULL);
            initConstants();
        }

        ~PaillierCryptosystemPrivateKey() {
            paillier_freepubkey(_pub);
            paillier_freeprvkey(_prv);
            destroyConstants();
        }

        void dec(Plain& p, const PaillierCipher c, bool signedValue) {
            paillier_ciphertext_t pc;
            paillier_plaintext_t pp;

//            PP_DEBUG_MSG("Using public key:" << paillier_pubkey_to_hex(_pub));

            mpz_init(pp.m);
            mpz_init_set(pc.c, c.raw().get_mpz_t());
            
            paillier_dec(&pp, this->_pub, this->_prv, &pc);
            
            // Return negative values if > N/2
            if(signedValue && mpz_sizeinbase(pp.m, 2) >= mpz_sizeinbase(_halfN, 2)-1 && mpz_cmp(pp.m, _halfN))
                mpz_sub(pp.m, pp.m, _pub->n);
            
            mpz_set(p.raw().get_mpz_t(), pp.m);
            mpz_clear(pc.c);
            mpz_clear(pp.m);
            
        }
        
    private:
        paillier_prvkey_t *_prv;
    };

    
    class PaillierCryptosystem : public PaillierCryptosystemBase<> {
    public:

        PaillierCryptosystem(const char *pubKeyHex) {
            _pub = paillier_pubkey_from_hex(pubKeyHex);
            initConstants();
        }
        
        ~PaillierCryptosystem() {
            paillier_freepubkey(_pub);
            destroyConstants();
        }

    };

    template <class T, class... I>
    inline typename Cryptosystem<ppf::Cipher<T>, I...>::Cipher operator+(typename Cryptosystem<ppf::Cipher<T>, I...>::Cipher lhs, const Plain rhs) { lhs += rhs; return lhs; }
    
}

#endif /* defined(__PPFramework__PaillierCryptosystem__) */
