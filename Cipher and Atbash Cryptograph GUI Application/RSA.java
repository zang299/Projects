// Name: Thian Zhang Hao
// CSC 2000 – Class Project
// Date: 30-11-24
// Description of program: Class for RSA cryptography


import java.io.DataInputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.util.Random;



public class RSA {
    private BigInteger P, Q, N, PHI, e, d;
    private Random R;

    public RSA(int keySize) {
        // initializes the random number generator
        R = new Random();
        // generates a random prime number of half the keysize in bits
        P = BigInteger.probablePrime(keySize / 2, R);
        Q = BigInteger.probablePrime(keySize / 2, R);
        // computes the modulus by multiplying P and Q
        N = P.multiply(Q);
        // generates a random number e
        PHI = P.subtract(BigInteger.ONE).multiply(Q.subtract(BigInteger.ONE));
        e = BigInteger.probablePrime(keySize / 4, R);
        // keeps increasing e by 1 until gcd(PHI,e) equals 1
        while (PHI.gcd(e).compareTo(BigInteger.ONE) > 0 && e.compareTo(PHI) < 0) {
            e = e.add(BigInteger.ONE);
        }
        // computes d as the modular inverse of e 
        d = e.modInverse(PHI);
    }
    
    // encrypts the message by converting the array into a BigInteger and performs modular exponentiation to encrypt the message
    public byte[] encryptMessage(byte[] message) {
        return (new BigInteger(message)).modPow(e, N).toByteArray();
    }
    
    // encrypts the message by converting the array into a BigInteger and performs modular exponentiation to decrypt the message
    public byte[] decryptMessage(byte[] message) {
        return (new BigInteger(message)).modPow(d, N).toByteArray();
    }
    
    
    // returns the public exponent e and N
    public BigInteger getE() {
        return e;
    }

    public BigInteger getN() {
        return N;
    }
}
