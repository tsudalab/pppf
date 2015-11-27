Privacy-Preserving Protocols Framework

This framework lays out the foundation to homomorphic and semi-homomorphic computation and server-client protocols using such cryptosystems.

The framework makes heavy use of templating to allow flexible combination of cryptosystems and protocols and allow building a full algebra on natural numbers (e.g. by combining an additive cryptosystem and a multiplication protocol).

Implementing a new cryptosystem is as easy as creating two subclasses (private key and public key versions) of `Cryptosystem`, specifying supported operations (addition, multiplication…) as template arguments. The only required methods to implement are `enc` and `dec` (for the private key class) and key management methods (`getPublicKey` and a public key init method). Depending on the other operators supported (specified as template arguments), additional methods such as `AddTo` or `PlainMultiplyTo` need to be implemented for the class to compile. While a default cipher format is provided (essentially a very long integer, of type `mpz`, as supported by GMP), it is possible to also provide your own cipher format.

The best way to implement a new cryptosystem is to take example on the `PaillierCryptosystem` class, which implements a full Paillier cryptosystem (some of the low-level mathematical code for encryption/decryption is re-used from the [`LibPaillier` library](http://acsc.cs.utexas.edu/libpaillier/)) supporting homomorphic addition and plaintext multiplication.

The base class architecture provides a number of intelligent defaults for all additional operations based on the base operations (such as unary minus operator) and overrides C++ mathematical and stream operators that lets you manipulate encrypted variables as if they were clear text (and therefore can be used in any template-based libraries that might require such operator support).
