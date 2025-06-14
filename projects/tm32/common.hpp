/**
 * @file    tm32/common.hpp
 * @brief   Commonly-used includes, typedefs, macros and constants used by the
 *          TM32 CPU library and its client applications.
 */

#pragma once

/* Include Files **************************************************************/

#include <format>
#include <iostream>
#include <string>
#include <cstdint>

/* Macros *********************************************************************/

#define tm32_printf(...) std::cout << std::format(__VA_ARGS__) << '\n'
#define tm32_errorf(...) std::cerr << std::format(__VA_ARGS__) << std::endl
#define tm32_checkbit(v, b) (v & (1 << b))
#define tm32_setbit(v, b) v = (v | (1 << b))
#define tm32_clearbit(v, b) v = (v & ~(1 << b))
#define tm32_changebit(v, b, o) \
    if (o) { v = (v | (1 << b)); } \
    else { v = (v & ~(1 << b)); }

/* Typedefs *******************************************************************/

namespace tm32
{
    using byte_t    = std::uint8_t;
    using word_t    = std::uint16_t;
    using dword_t   = std::uint32_t;
    using qword_t   = std::uint64_t;
    using s_byte_t  = std::int8_t;
    using s_word_t  = std::int16_t;
    using s_dword_t = std::int32_t;
    using s_qword_t = std::int64_t;
    using address_t = std::uint32_t;
}

/* Enumerations ***************************************************************/

namespace tm32
{
    enum register_type : byte_t
    {
        rt_a   = 0b0000,   rt_aw  = 0b0001,   rt_ah = 0b0010,    rt_al = 0b0011,
        rt_b   = 0b0100,   rt_bw  = 0b0101,   rt_bh = 0b0110,    rt_bl = 0b0111,
        rt_c   = 0b1000,   rt_cw  = 0b1001,   rt_ch = 0b1010,    rt_cl = 0b1011,
        rt_d   = 0b1100,   rt_dw  = 0b1101,   rt_dh = 0b1110,    rt_dl = 0b1111,
    };

    enum flag_bit : byte_t
    {
        ft_stop        = (1 << 0),
        ft_halt        = (1 << 1),
        ft_parity      = (1 << 2),
        ft_sign        = (1 << 3),
        ft_carry       = (1 << 4),
        ft_half_carry  = (1 << 5),
        ft_subtraction = (1 << 6),
        ft_zero        = (1 << 7)
    };

    enum condition_type : byte_t
    {
        ct_none            = 0b0000,
        ct_zero_clear      = 0b0001,
        ct_zero_set        = 0b1001,
        ct_carry_clear     = 0b0010,
        ct_carry_set       = 0b1010,
        ct_parity_clear    = 0b0011,
        ct_parity_set      = 0b1011,
        ct_sign_clear      = 0b0100,
        ct_sign_set        = 0b1100
    };

    enum error_code : byte_t
    {
        ec_ok,
        ec_invalid_opcode,
        ec_invalid_argument,
        ec_invalid_read,
        ec_invalid_write,
        ec_invalid_execute,
        ec_hardware_error
    };
}