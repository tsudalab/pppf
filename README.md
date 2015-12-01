# Privacy-Preserving Protocols Framework

This framework lays out a homogenised foundation for homomorphic and semi-homomorphic development, greatly simplifying the implementation and use of new cryptosystems and oblivious protocols over server-client architecture.

Its heavy use of templating allows flexible combinations of cryptosystems and protocols and to build a full algebra on natural numbers (e.g. by combining an additive cryptosystem and a multiplication remote protocol). The resulting implementations let you create and use encrypted values using regular C++ operators.

# Using

The framework files are meant to be used inside your own project and should compile without much effort on any modern compilers (C11).

The 'only' two non-standard requirements are:

- [Boost library](http://www.boost.org) v.1.57 or later
- [Gnu Multiple Precision Arithmetic Library](https://gmplib.org) v.6 or later

Additionally, to use the client-server protocol classes (`RPCClient`/`RPCServer`), you will need the [C++ Msgpack-RPC library](https://github.com/jubatus/jubatus-msgpack-rpc/tree/master/cpp). If you have difficulties compiling this version, you can try [my own fork](https://github.com/david-duverle/msgpack-rpc-cpp), which fixed some issues the lib used to have with C11 support (may have been fixed by the time you read this). Similarly, [this fork](https://github.com/david-duverle/mpio) of MPIO (a requirement of msgpack-rpc) might be helpful.

A few usage examples (some of it commented out) are provided in the `main.cpp` file. Easiest way to compile it is with the Xcode project file (if you are an OS X user), but compiling directly shouldn't be particularly hard (sorry, no Makefile yet).

The code currently provides a fairly fast implementation of Paillier's additive cryptosystem, with a client-server protocol for oblivious multiplication and oblivious vector inner product.

# Implementing your own cryptosystem

Implementing a new cryptosystem is as easy as creating two subclasses (private key and public key versions) of `Cryptosystem`, specifying supported operations (addition, multiplication…) as template arguments. The only required methods to implement are `enc` and `dec` (for the private key class) and key management methods (`getPublicKey` and a public key init method). Depending on the other operators supported (specified as template arguments), additional methods such as `AddTo` or `PlainMultiplyTo` need to be implemented for the class to compile. While a default cipher format is provided (essentially a very long integer, of type `mpz`, as supported by GMP), it is possible to also provide your own cipher format.

The best way to implement a new cryptosystem is to take example on the `PaillierCryptosystem` class, which implements a full Paillier cryptosystem (some of the low-level mathematical code for encryption/decryption is re-used from the [`LibPaillier` library](http://acsc.cs.utexas.edu/libpaillier/)) supporting homomorphic addition and plaintext multiplication.

The base class architecture provides a number of intelligent defaults for all additional operations based on the base operations (such as unary minus operator) and overrides C++ mathematical and stream operators to let you manipulate encrypted variables as if they were clear values (and therefore usable in any template-based libraries that might require such operator support).

# Implementing your own procedures

To use the server/client architecture, look at the simple examples in `main.cpp`: little is needed beside an IP address and port number. Once a server/client is created with the desired cryptosystem, it will automatically handle key exchange and run supported oblivious procedures across the network.

You can subclass `ppf::Procedure` to create your own procedure. Take a look at existing procedures (such as `ppf::ProcObliviousMult`) for examples of how to do it. Each side can simultaneously run its own client and server in order to provide complex oblivious computaiton protocols.
