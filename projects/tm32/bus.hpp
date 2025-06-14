/**
 * @file    tm32/bus.hpp
 * @brief   The base interface for the memory bus to which the TM32 CPU is
 *          connected.
 */

#pragma once
#include <tm32/common.hpp>

namespace tm32
{
    class bus
    {
    public:
        bus () = default;
        virtual ~bus () = default;

    public:
        virtual byte_t  read (const address_t&) const = 0;
        virtual void    write (const address_t&, const byte_t&) = 0;
        virtual void    cycle (const byte_t&) = 0;
        
    public:
        byte_t  read_byte (const address_t&) const;
        word_t  read_word (const address_t&) const;
        dword_t read_dword (const address_t&) const;

    public:
        void    write_byte (const address_t&, const byte_t&);
        void    write_word (const address_t&, const word_t&);
        void    write_dword (const address_t&, const dword_t&);

    };
}
