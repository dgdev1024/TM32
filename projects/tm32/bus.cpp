/**
 * @file    tm32/bus.cpp
 */

#include <tm32/bus.hpp>

namespace tm32
{

    /* Public Methods *********************************************************/

    byte_t bus::read_byte (const address_t& p_address) const
    {
        return read(p_address);
    }

    word_t bus::read_word (const address_t& p_address) const
    {
        byte_t l_byte0 = read(p_address);
        byte_t l_byte1 = read(p_address + 1);

        return (
            (static_cast<word_t>(l_byte0) <<  0) |
            (static_cast<word_t>(l_byte1) <<  8)
        );
    }

    dword_t bus::read_dword (const address_t& p_address) const
    {
        byte_t l_byte0 = read(p_address);
        byte_t l_byte1 = read(p_address + 1);
        byte_t l_byte2 = read(p_address + 2);
        byte_t l_byte3 = read(p_address + 3);

        return (
            (static_cast<dword_t>(l_byte0) <<  0) |
            (static_cast<dword_t>(l_byte1) <<  8) |
            (static_cast<dword_t>(l_byte2) << 16) |
            (static_cast<dword_t>(l_byte3) << 24)
        );
    }

    void bus::write_byte (const address_t& p_address, const byte_t& p_value)
    {
        write(p_address, p_value);
    }

    void bus::write_word (const address_t& p_address, const word_t& p_value)
    {
        write(p_address,     (p_value >>  0) & 0xFF);
        write(p_address + 1, (p_value >>  8) & 0xFF);
    }

    void bus::write_dword (const address_t& p_address, const dword_t& p_value)
    {
        write(p_address,     (p_value >>  0) & 0xFF);
        write(p_address + 1, (p_value >>  8) & 0xFF);
        write(p_address + 2, (p_value >> 16) & 0xFF);
        write(p_address + 3, (p_value >> 24) & 0xFF);
    }

}