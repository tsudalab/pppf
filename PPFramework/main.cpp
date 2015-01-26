//
//  main.cpp
//  PPFramework
//
//  Created by Dave on 11/28/14.
//  Copyright (c) 2014 Tsudalab. All rights reserved.
//

#include <iostream>

#include "RPCServer.hpp"
#include "RPCClient.hpp"

#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32" // Stupid Boost lib is not yet completely 64bit-friendly
#include "boost/thread/thread.hpp"
#include "boost/function.hpp"
#pragma GCC diagnostic pop


#include "PaillierCryptosystem.hpp"
#include "RLWECryptosystem.hpp"

#include "ProcObliviousMult.hpp"
#include "ProcInnerProduct.hpp"

using namespace ppf;

void testCryptosystem(void);
void runServer();
void runClient();

#define SERVER_PORT 1234
#define PAILLIER_ENCRYPTION_BITS 1024


int main(int argc, const char * argv[]) {
    PaillierCryptosystemPrivateKey cs = PaillierCryptosystemPrivateKey(PAILLIER_ENCRYPTION_BITS);

//    testCryptosystem();

//    boost::thread serverThread(runServer);
//    boost::thread clientThread(runClient);
//    clientThread.join();
    
    return 0;
}

/*
void runServer() {
    try {
        PaillierCryptosystemPrivateKey cs = PaillierCryptosystemPrivateKey(PAILLIER_ENCRYPTION_BITS);
        RPCServer<PaillierCryptosystemPrivateKey> server(&cs, SERVER_PORT);
//        RLWECryptosystemPrivateKey cs = RLWECryptosystemPrivateKey();
//        RPCServer<RLWECryptosystemPrivateKey> server(cs, SERVER_PORT);
        
        server.registerProc(new ProcObliviousMult<PaillierCryptosystemPrivateKey>());
        server.registerProc(new ProcInnerProduct<PaillierCryptosystemPrivateKey>());
        server.run();
    }
    catch(const std::exception & ex) {
        std::cerr << "Error starting Server:" << std::endl << ex.what() << std::endl;
    }
}

void runClient() {
    try {
        boost::this_thread::sleep(boost::posix_time::seconds(1)); // let the server start

        PaillierCryptosystemPrivateKey localCS(2048);
        
        RPCClient<PaillierCryptosystemPrivateKey> client(&localCS, SERVER_PORT);
        
        PaillierCryptosystemPrivateKey::Cipher a(localCS);
        a = 666;
        std::vector<PaillierCryptosystemPrivateKey::Cipher> v;
        v.push_back(a);
        a = 100;
        v.push_back(a);

        auto vOut = client.getLocalCipherVector(ProcInnerProduct<PaillierCryptosystem>::procName(), v);
        
        for(auto e : vOut)
            std::cout << e << " | " << e << std::endl;
        
//        auto c = client.newRemoteCipher();
//        *c = 3;
//        auto c2 = client.newRemoteCipher();
//        *c2 = 5;
        
//        auto z = client.getRemoteCipher(ProcObliviousMult<PaillierCryptosystem>::procName(), c, c2);
        
//        std::cout << z << " == " << (Plain) z << std::endl;

        boost::this_thread::sleep(boost::posix_time::seconds(10)); // let the server finish

    }
    catch(const std::exception & ex) {
        std::cerr << "Error running Client:" << std::endl << ex.what() << std::endl;
    }
}


void testCryptosystem(void) {
    PaillierCryptosystemPrivateKey *aliceCS = new PaillierCryptosystemPrivateKey();

    PaillierCryptosystem *bobCS = new PaillierCryptosystem(aliceCS->getPublicKey());
    PaillierCryptosystem::Cipher a(*bobCS), a_save(*bobCS);
    Plain p = 10;

    a = 3;
    a_save = a;
    a += a;
    a += p;

    std::cout << a_save << " + " <<  a_save << " + " << p << " = " << a << std::endl;

    a_save = a;
    a *= 5;
    a *= p;

    std::cout << a_save << " * 5 * " << p << " = " << a << std::endl;

// Illegal (require decryption):
//    a == a_save; // Illegal (comparison between ciphers)
//    a == b; // Illegal (comparison between cipher and plain)
//    b = a_save; // Illegal (assigns cipher to plain)

    PaillierCryptosystemPrivateKey::Cipher c(*aliceCS), c_save(*aliceCS), d(*aliceCS);
    
    c = 666;
    std::cerr << c;
    std::cerr << " | " << c << std::endl;
    
    c = 3;              // automatically encrypt
    d = 2;
    c_save = c;
    c += p;             // automatically decrypt, add, encrypt
    c += d;             // automatically decrypt both, add, encrypt result
    
    
    std::cout << c_save << " (" << c_save << ") + " << p << " + " << d << " (" << d << ") = " << c << " (" << c << ")" << " = " << c_save + p + d << std::endl;
    
    if(c == c_save + p + d)
        std::cout << "Test successful" << std::endl;
    //
    if(c != c_save + p + d)
        std::cout << "Test failed" << std::endl;

//    
//    RLWECryptosystemPrivateKey *testRLWEPrivate = new RLWECryptosystemPrivateKey(666);
//    RLWECryptosystemPrivateKey::Cipher r(*testRLWEPrivate), s(*testRLWEPrivate);
//    p = 42;
//
//    r = p;
//    s = 96;
//    r *= s;
//    r * s;
//    r += s;
//    p = r;
//    std::cout << "Testing print: " << r << std::endl;
//
//    RLWECryptosystem *testRLWE = new RLWECryptosystem();
//    RLWECryptosystem::Cipher u(*testRLWE), v(*testRLWE);
//    p = 42;
//    
//    u = p;
//    v = 96;
//    u *= v;
//    u += v;
//    std::cout << "Testing print: " << u << std::endl;
    
}
*/