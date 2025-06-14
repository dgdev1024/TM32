/**
 * @file    tm32/cpu_fetch.cpp
 */

#include <tm32/bus.hpp>
#include <tm32/cpu.hpp>

namespace tm32
{
    #define dest_reg        static_cast<register_type>((m_ci >> 4)  & 0xF)
    #define dest_reg_class  static_cast<register_type>((m_ci >> 4)  & 0b1100)
    #define dest_reg_size   ((m_ci >> 4)    & 0b11)
    #define src_reg         static_cast<register_type>((m_ci)       & 0xF)
    #define src_reg_class   static_cast<register_type>((m_ci)       & 0b1100)
    #define src_reg_size    ((m_ci)         & 0b11)

    void cpu::fetch_reg_imm ()
    {
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_pc);  advance(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_pc);   advance(2); break;
            default:    m_sd = m_bus.read_byte(m_pc);   advance(1); break;
        }
    }

    void cpu::fetch_reg_addr32 ()
    {
        m_sa = m_bus.read_dword(m_pc); advance(4);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_reg_regptr32 ()
    {
        if (src_reg_size != 0b00)
        {
            throw error_code::ec_invalid_argument;
        }

        m_sa = read_register(src_reg);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_reg_addr16 ()
    {
        m_sa = 0xFFFF0000 + m_bus.read_word(m_pc); advance(2);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_reg_regptr16 ()
    {
        if (src_reg_size != 0b01)
        {
            throw error_code::ec_invalid_argument;
        }

        m_sa = 0xFFFF0000 + read_register(src_reg);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_reg8_addr8 ()
    {
        if (dest_reg_size < 0b10)
        {
            throw error_code::ec_invalid_argument;
        }

        m_sa = 0xFFFFFF00 + m_bus.read_byte(m_pc); advance(1);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_reg8_regptr8 ()
    {
        if (dest_reg_size < 0b10 || src_reg_size < 0b10)
        {
            throw error_code::ec_invalid_argument;
        }

        m_sa = 0xFFFFFF00 + read_register(src_reg);
        switch (dest_reg_size)
        {
            case 0b00:  m_sd = m_bus.read_dword(m_sa);  m_bus.cycle(4); break;
            case 0b01:  m_sd = m_bus.read_word(m_sa);   m_bus.cycle(2); break;
            default:    m_sd = m_bus.read_byte(m_sa);   m_bus.cycle(1); break;
        }
    }

    void cpu::fetch_addr32_reg ()
    {
        m_da = m_bus.read_dword(m_pc); check_writable(m_da); advance(4);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_regptr32_reg ()
    {
        if (dest_reg_size != 0b00)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = read_register(dest_reg); check_writable(m_da);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_addr16_reg ()
    {
        m_da = 0xFFFF0000 + m_bus.read_word(m_pc); advance(2);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_regptr16_reg ()
    {
        if (dest_reg_size != 0b01)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = 0xFFFF0000 + read_register(dest_reg);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_addr8_reg8 ()
    {
        if (src_reg_size < 0b10)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = 0xFFFFFF00 + m_bus.read_byte(m_pc); advance(1);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_regptr8_reg8 ()
    {
        if (dest_reg_size < 0b10 || src_reg_size < 0b10)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = 0xFFFFFF00 + read_register(dest_reg);
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_reg_reg ()
    {
        m_sd = read_register(src_reg);
    }

    void cpu::fetch_null_imm32 ()
    {
        m_sd = m_bus.read_dword(m_pc); advance(4);
    }

    void cpu::fetch_null_reg32 ()
    {
        if (src_reg_size != 0b00)
        {
            throw error_code::ec_invalid_argument;
        }

        m_sd = read_register(src_reg);
    }

    void cpu::fetch_null_simm16 ()
    {
        m_sd = m_bus.read_word(m_pc); advance(2);
    }

    void cpu::fetch_reg_null ()
    {
        m_sd = read_register(dest_reg);
    }

    void cpu::fetch_regptr32_null ()
    {
        if (src_reg_size != 0b00)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = read_register(src_reg); check_writable(m_da);
        m_sd = m_bus.read_byte(m_da); m_bus.cycle(1);
    }

    void cpu::fetch_imm8_reg ()
    {
        m_sd = read_register(dest_reg);
    }

    void cpu::fetch_imm8_regptr32 ()
    {
        if (dest_reg_size != 0b00)
        {
            throw error_code::ec_invalid_argument;
        }

        m_da = read_register(dest_reg); check_writable(m_da);
        m_sd = m_bus.read_byte(m_da); m_bus.cycle(1);
    }

}
