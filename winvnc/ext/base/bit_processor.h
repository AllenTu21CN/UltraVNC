#pragma once
/*
    Tag : 20170811_0.0.1_beta
    Tag : 20170814_0.0.2_beta
*/

#include <stdint.h>

namespace media {

class BitExtractor 
{
public:
    BitExtractor();
    ~BitExtractor();

    /*
        Initialize GetBitContext.
    */
    int initExtractor(const uint8_t *buffer, int bit_size);
    
    /*
        Skip bits , when you read bits, n could be negative
    */
    void skipBits(int n);

    /*
       how many bits you have read
    */
    int bitsReaded();

    /*
        Get bits remain in buffer
    */
    int bitsRemain();

    /*
        Read bits in big endian, Read up at most 32 bit once
    */
    int readBitsBE(int n, uint32_t &v);
    
    /*
        Read bits in little endian, Read up at most 32 bit once
    */
    int readBitsLE(int n, uint32_t &v);

    /*
        Peek bits in big endian, Read up at most 32 bit once
    */
    int peekBitsBE(int n, uint32_t &v);

    /*
    Peek bits in little endian, Read up at most 32 bit once
    */
    int peekBitsLE(int n, uint32_t &v);

private:
    void *m_priv;

}; // End of BitExtractor

class BitAssembler 
{
public:
    BitAssembler();
    ~BitAssembler();
    /*
        Init BitAssembler
        @buffer : bit stream buffer
        @size   : bit stream buffer size, in bytes
    */
    int initBitAssembler(uint8_t *buffer, int size);

    /*
        Return the total number of bits written to the bit stream.
    */
    int bitsWrited();

    /*
        Return the number of bits available in the bit stream.
    */
    int bitsRemain();

    /*
    Align bit stream by set 0 to bit left
    */
    void paddingBits();

    /*
        Write up at most 32 bit once
        @n      bit   to write
        @value  value to write
    */
    int writeBitBE(int n, uint32_t value);
    void flushBE();

    int writeBitLE(int n, uint32_t value);
    void flushLE();

private:
    void* m_priv;
}; // End of BitAssembler
}  //End of namespace base
