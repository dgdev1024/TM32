/**
 * @file    tm32/cpu_execute.cpp
 */

#include <tm32/bus.hpp>
#include <tm32/cpu.hpp>

namespace tm32
{
    #define dest_nibble     ((m_ci >> 4)    & 0xF)
    #define dest_cond       static_cast<condition_type>((m_ci >> 4) & 0xF)
    #define dest_reg        static_cast<register_type>((m_ci >> 4)  & 0xF)
    #define dest_reg_class  static_cast<register_type>((m_ci >> 4)  & 0b1100)
    #define dest_reg_size   ((m_ci >> 4)    & 0b11)
    #define dest_reg_hibit  ((m_da != 0) ? 7 : (dest_reg_size == 0 ? 31 : (dest_reg_size == 1 ? 15 : 7)))
    #define src_nibble      ((m_ci)         & 0xF)
    #define src_reg         static_cast<register_type>((m_ci)       & 0xF)
    #define src_reg_class   static_cast<register_type>((m_ci)       & 0b1100)
    #define src_reg_size    ((m_ci)         & 0b11)
    #define src_reg_hibit  ((m_da != 0) ? 7 : (src_reg_size == 0 ? 31 : (src_reg_size == 1 ? 15 : 7)))

    static bool byte_parity (byte_t p_val)
    {
        byte_t count = 0;
        for (byte_t i = 0; i < 8; ++i)
        {
            if (tm32_checkbit(p_val, i)) { count++; }
        }

        return (count % 2) == 0;
    }

    static bool word_parity (word_t p_val)
    {
        byte_t count = 0;
        for (byte_t i = 0; i < 16; ++i)
        {
            if (tm32_checkbit(p_val, i)) { count++; }
        }

        return (count % 2) == 0;
    }

    static bool dword_parity (dword_t p_val)
    {
        byte_t count = 0;
        for (byte_t i = 0; i < 32; ++i)
        {
            if (tm32_checkbit(p_val, i)) { count++; }
        }

        return (count % 2) == 0;
    }

    void cpu::execute_nop ()
    {

    }
    
    void cpu::execute_stop ()
    {
        change_flag(flag_bit::ft_stop, true);
    }
    
    void cpu::execute_halt ()
    {
        change_flag(flag_bit::ft_halt, true);
    }
    
    void cpu::execute_sec ()
    {
        m_ec = (m_ci & 0xFF);
    }
    
    void cpu::execute_cec ()
    {
        m_ec = 0;
    }
    
    void cpu::execute_di ()
    {
        m_ime = false;
    }
    
    void cpu::execute_ei ()
    {
        m_ei = true;
    }
    
    void cpu::execute_daa ()
    {
        byte_t l_al = read_register(register_type::rt_al);
        byte_t l_adjust = 0;

        if (check_flag(flag_bit::ft_half_carry) || (l_al & 0xF) > 0x9)
        {
            l_adjust += 0x6;
        }
        if (check_flag(flag_bit::ft_carry) || (l_al & 0xF0) > 0x90)
        {
            l_adjust += 0x60;
            change_flag(flag_bit::ft_carry, true);
        }
        else
        {
            change_flag(flag_bit::ft_carry, false);
        }

        l_al = (check_flag(flag_bit::ft_subtraction)) ?
            (l_al - l_adjust) : (l_al + l_adjust);
        
        write_register(register_type::rt_al, l_al);
        change_flag(flag_bit::ft_zero, (l_al == 0));
        change_flag(flag_bit::ft_half_carry, false);
        change_flag(flag_bit::ft_parity, byte_parity(l_al));
    }
    
    void cpu::execute_scf ()
    {
        change_flag(flag_bit::ft_subtraction, false);
        change_flag(flag_bit::ft_half_carry, false);
        change_flag(flag_bit::ft_carry, true);
    }
    
    void cpu::execute_ccf ()
    {
        bool l_carry = check_flag(flag_bit::ft_carry);

        change_flag(flag_bit::ft_subtraction, false);
        change_flag(flag_bit::ft_half_carry, false);
        change_flag(flag_bit::ft_carry, !l_carry);
    }
    
    void cpu::execute_flg ()
    {
        change_flag(flag_bit::ft_zero, tm32_checkbit(m_f, (16 + dest_nibble)));
    }
    
    void cpu::execute_stf ()
    {
        change_flag(static_cast<flag_bit>(16 + dest_nibble), true);
    }
    
    void cpu::execute_clf ()
    {
        change_flag(static_cast<flag_bit>(16 + dest_nibble), false);
    }
    
    void cpu::execute_ld ()
    {
        write_register(dest_reg, m_sd);
    }
    
    void cpu::execute_st ()
    {
        switch (src_reg_size)
        {
            case 0b00:  m_bus.write_dword(m_da, m_sd);  m_bus.cycle(4); break;
            case 0b01:  m_bus.write_word(m_da, m_sd);   m_bus.cycle(2); break;
            default:    m_bus.write_byte(m_da, m_sd);   m_bus.cycle(1); break;
        }
    }
    
    void cpu::execute_mv ()
    {
        write_register(dest_reg, m_sd);
    }
    
    void cpu::execute_pop ()
    {
        m_sd = pop_data();
        write_register(dest_reg, m_sd);
    }
    
    void cpu::execute_push ()
    {
        m_sd = read_register(src_reg);
        push_data(m_sd);
    }
    
    void cpu::execute_jmp ()
    {
        if (check_condition(dest_cond) == true)
        {
            m_pc = m_sd;
            m_bus.cycle(1);
        }
    }
    
    void cpu::execute_jpb ()
    {
        if (check_condition(dest_cond) == true)
        {
            m_pc += static_cast<s_word_t>(m_sd & 0xFFFF);
            m_bus.cycle(1);
        }
    }
    
    void cpu::execute_call ()
    {
        if (check_condition(dest_cond) == true)
        {
            push_data(m_pc);
            m_pc = m_sd;
            m_bus.cycle(1);
        }
    }
    
    void cpu::execute_int ()
    {
        byte_t l_index = (m_ci & 0xFF);
        if (l_index > 0x1F)
        {
            throw error_code::ec_invalid_argument;
        }

        m_ime = false;
        push_data(m_pc);
        m_pc = 0x1000 + (0x100 * l_index);
    }
    
    void cpu::execute_ret ()
    {
        if (check_condition(dest_cond) == true)
        {
            m_pc = pop_data();
            m_bus.cycle(1);
        }
    }
    
    void cpu::execute_reti ()
    {
        m_ime = true;
        m_pc = pop_data();
        m_bus.cycle(1);
    }
    
    void cpu::execute_inc ()
    {
        dword_t l_result = m_sd + 1;
        if (m_da == 0)
        {
            m_bus.write_byte(m_da, (l_result & 0xFF));
            m_bus.cycle(1);

            change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_half_carry,    (l_result & 0xF) == 0);
            change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
        }
        else
        {
            write_register(dest_reg, l_result);
            switch (dest_reg_size)
            {
                case 0b00:
                    change_flag(flag_bit::ft_zero,          (l_result) == 0);
                    change_flag(flag_bit::ft_parity,        dword_parity(l_result));
                    break;
                case 0b01:
                    change_flag(flag_bit::ft_zero,          (l_result & 0xFFFF) == 0);
                    change_flag(flag_bit::ft_half_carry,    (l_result & 0xFFF) == 0);
                    change_flag(flag_bit::ft_parity,        word_parity(l_result & 0xFFFF));
                    break;
                default:
                    change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
                    change_flag(flag_bit::ft_half_carry,    (l_result & 0xF) == 0);
                    change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
                    break;
            }
        }

        change_flag(flag_bit::ft_subtraction, false);
    }
    
    void cpu::execute_dec ()
    {
        dword_t l_result = m_sd - 1;
        if (m_da == 0)
        {
            m_bus.write_byte(m_da, (l_result & 0xFF));
            m_bus.cycle(1);

            change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_half_carry,    (l_result & 0xF) == 0xF);
            change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
        }
        else
        {
            write_register(dest_reg, l_result);
            switch (dest_reg_size)
            {
                case 0b00:
                    change_flag(flag_bit::ft_zero,          (l_result) == 0);
                    change_flag(flag_bit::ft_parity,        dword_parity(l_result));
                    break;
                case 0b01:
                    change_flag(flag_bit::ft_zero,          (l_result & 0xFFFF) == 0);
                    change_flag(flag_bit::ft_half_carry,    (l_result & 0xFFF) == 0xFFF);
                    change_flag(flag_bit::ft_parity,        word_parity(l_result & 0xFFFF));
                    break;
                default:
                    change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
                    change_flag(flag_bit::ft_half_carry,    (l_result & 0xF) == 0xF);
                    change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
                    break;
            }
        }

        change_flag(flag_bit::ft_subtraction, true);
    }
    
    void cpu::execute_add (bool p_with_carry)
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        bool        l_carry     =   (p_with_carry == true) ? 
                                        check_flag(flag_bit::ft_carry) : false;
        dword_t     l_lefthand  =   read_register(dest_reg);
        qword_t     l_sum       =   l_lefthand + m_sd + l_carry;
        dword_t     l_halfsum   =   0;

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_sign,          false);
        
        write_register(dest_reg, l_sum);
        switch (dest_reg_size)
        {
            case 0b00:
                l_halfsum = (l_lefthand & 0xFFFFFFF) + (m_sd & 0xFFFFFFF) + l_carry;
                change_flag(flag_bit::ft_zero,          (l_sum & 0xFFFFFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfsum > 0xFFFFFFF);
                change_flag(flag_bit::ft_carry,         l_sum > 0xFFFFFFFF);
                change_flag(flag_bit::ft_parity,        dword_parity(l_sum & 0xFFFFFFFF));
                break;
            case 0b01:
                l_halfsum = (l_lefthand & 0xFFF) + (m_sd & 0xFFF) + l_carry;
                change_flag(flag_bit::ft_zero,          (l_sum & 0xFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfsum > 0xFFF);
                change_flag(flag_bit::ft_carry,         l_sum > 0xFFFF);
                change_flag(flag_bit::ft_parity,        word_parity(l_sum & 0xFFFF));
                break;
            default:
                l_halfsum = (l_lefthand & 0xF) + (m_sd & 0xF) + l_carry;
                change_flag(flag_bit::ft_zero,          (l_sum & 0xFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfsum > 0xF);
                change_flag(flag_bit::ft_carry,         l_sum > 0xFF);
                change_flag(flag_bit::ft_parity,        byte_parity(l_sum & 0xFF));
                break;
        }
    }
    
    void cpu::execute_sub (bool p_with_carry)
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        bool        l_carry     =   (p_with_carry == true) ? 
                                        check_flag(flag_bit::ft_carry) : false;
        dword_t     l_lefthand  =   read_register(dest_reg);
        s_qword_t   l_diff      =   l_lefthand - m_sd - l_carry;
        s_dword_t   l_halfdiff  =   0;

        change_flag(flag_bit::ft_subtraction,   true);
        change_flag(flag_bit::ft_carry,         l_diff < 0);
        change_flag(flag_bit::ft_sign,          l_diff < 0);
        
        write_register(dest_reg, l_diff);
        switch (dest_reg_size)
        {
            case 0b00:
                l_halfdiff = (l_lefthand & 0xFFFFFFF) - (m_sd & 0xFFFFFFF) - l_carry;
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFFFFFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                change_flag(flag_bit::ft_parity,        dword_parity(l_diff & 0xFFFFFFFF));
                break;
            case 0b01:
                l_halfdiff = (l_lefthand & 0xFFF) - (m_sd & 0xFFF) - l_carry;
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                change_flag(flag_bit::ft_parity,        word_parity(l_diff & 0xFFFF));
                break;
            default:
                l_halfdiff = (l_lefthand & 0xF) - (m_sd & 0xF) - l_carry;
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                change_flag(flag_bit::ft_parity,        byte_parity(l_diff & 0xFF));
                break;
        }
    }
    
    void cpu::execute_and ()
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        dword_t     l_lefthand  =   read_register(dest_reg);
        dword_t     l_result    =   l_lefthand & m_sd;

        change_flag(flag_bit::ft_subtraction,           false);
        change_flag(flag_bit::ft_half_carry,            true);
        change_flag(flag_bit::ft_carry,                 false);
        change_flag(flag_bit::ft_sign,                  false);
        
        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,          (l_result) == 0);
                change_flag(flag_bit::ft_parity,        dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,        word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_or ()
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        dword_t     l_lefthand  =   read_register(dest_reg);
        dword_t     l_result    =   l_lefthand | m_sd;

        change_flag(flag_bit::ft_subtraction,           false);
        change_flag(flag_bit::ft_half_carry,            false);
        change_flag(flag_bit::ft_carry,                 false);
        change_flag(flag_bit::ft_sign,                  false);
        
        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,          (l_result) == 0);
                change_flag(flag_bit::ft_parity,        dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,        word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_xor ()
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        dword_t     l_lefthand  =   read_register(dest_reg);
        dword_t     l_result    =   l_lefthand ^ m_sd;

        change_flag(flag_bit::ft_subtraction,           false);
        change_flag(flag_bit::ft_half_carry,            false);
        change_flag(flag_bit::ft_carry,                 false);
        change_flag(flag_bit::ft_sign,                  false);
        
        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,          (l_result) == 0);
                change_flag(flag_bit::ft_parity,        dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,        word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,          (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,        byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_not ()
    {
        if (m_da != 0)
        {
            m_bus.write_byte(m_da, ~(m_sd & 0xFF));
            m_bus.cycle(1);
        }
        else
        {
            write_register(dest_reg, ~m_sd);
        }

        change_flag(flag_bit::ft_subtraction,   true);
        change_flag(flag_bit::ft_half_carry,    true);
    }
    
    void cpu::execute_cmp ()
    {
        if (dest_reg_class != register_type::rt_a)
        {
            throw error_code::ec_invalid_argument;
        }

        dword_t     l_lefthand  =   read_register(dest_reg);
        s_qword_t   l_diff      =   l_lefthand - m_sd;
        s_dword_t   l_halfdiff  =   0;

        change_flag(flag_bit::ft_subtraction,   true);
        change_flag(flag_bit::ft_carry,         l_diff < 0);
        change_flag(flag_bit::ft_sign,          l_diff < 0);
        
        switch (dest_reg_size)
        {
            case 0b00:
                l_halfdiff = (l_lefthand & 0xFFFFFFF) - (m_sd & 0xFFFFFFF);
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFFFFFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                break;
            case 0b01:
                l_halfdiff = (l_lefthand & 0xFFF) - (m_sd & 0xFFF);
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFFFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                break;
            default:
                l_halfdiff = (l_lefthand & 0xF) - (m_sd & 0xF);
                change_flag(flag_bit::ft_zero,          (l_diff & 0xFF) == 0);
                change_flag(flag_bit::ft_half_carry,    l_halfdiff < 0);
                break;
        }
    }
    
    void cpu::execute_sla ()
    {
        dword_t l_result = (m_sd << 1) | 0b0;

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
            change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 31));
                change_flag(flag_bit::ft_parity,    dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 15));
                change_flag(flag_bit::ft_parity,    word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
                change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_sra ()
    {
        dword_t l_result = (m_sd >> 1) | tm32_checkbit(m_sd, dest_reg_hibit);

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);
        change_flag(flag_bit::ft_carry,         tm32_checkbit(m_sd, 0));

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_parity,    dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,    word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_srl ()
    {
        dword_t l_result = (m_sd >> 1);
        tm32_clearbit(l_result, dest_reg_hibit);

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);
        change_flag(flag_bit::ft_carry,         tm32_checkbit(m_sd, 0));

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_parity,    dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,    word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_rl ()
    {
        bool l_carry = check_flag(flag_bit::ft_carry);
        dword_t l_result = (m_sd << 1) | l_carry;

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
            change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 31));
                change_flag(flag_bit::ft_parity,    dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 15));
                change_flag(flag_bit::ft_parity,    word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
                change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_rlc ()
    {
        bool l_leftmost = tm32_checkbit(m_sd, dest_reg_hibit);
        dword_t l_result = (m_sd << 1) | l_leftmost;

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 31));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 15));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_carry,     tm32_checkbit(m_sd, 7));
                break;
        }
    }
    
    void cpu::execute_rr ()
    {
        bool l_carry = check_flag(flag_bit::ft_carry);
        dword_t l_result = (m_sd >> 1) | (l_carry << dest_reg_hibit);

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);
        change_flag(flag_bit::ft_carry,         tm32_checkbit(m_sd, 0));

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                change_flag(flag_bit::ft_parity,    dword_parity(l_result));
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                change_flag(flag_bit::ft_parity,    word_parity(l_result & 0xFFFF));
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                change_flag(flag_bit::ft_parity,    byte_parity(l_result & 0xFF));
                break;
        }
    }
    
    void cpu::execute_rrc ()
    {
        bool l_rightmost = tm32_checkbit(m_sd, 0);
        dword_t l_result = (m_sd >> 1) | (l_rightmost << dest_reg_hibit);

        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    false);
        change_flag(flag_bit::ft_carry,         tm32_checkbit(m_sd, 0));

        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
            return;
        }

        write_register(dest_reg, l_result);
        switch (dest_reg_size)
        {
            case 0b00:
                change_flag(flag_bit::ft_zero,      (l_result) == 0);
                break;
            case 0b01:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFFFF) == 0);
                break;
            default:
                change_flag(flag_bit::ft_zero,      (l_result & 0xFF) == 0);
                break;
        }
    }
    
    void cpu::execute_bit ()
    {
        byte_t l_bit = m_bus.read_byte(m_pc);
        if (l_bit > dest_reg_hibit)
        {
            throw error_code::ec_invalid_argument;
        } else advance(1);
        
        change_flag(flag_bit::ft_zero,          tm32_checkbit(m_sd, l_bit) == 0);
        change_flag(flag_bit::ft_subtraction,   false);
        change_flag(flag_bit::ft_half_carry,    true);
    }
    
    void cpu::execute_tog ()
    {
        byte_t l_bit = m_bus.read_byte(m_pc);
        if (l_bit > dest_reg_hibit)
        {
            throw error_code::ec_invalid_argument;
        } else advance(1);

        dword_t l_result = m_sd ^ (1 << l_bit);
        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            return;
        }

        write_register(dest_reg, l_result);
    }
    
    void cpu::execute_set ()
    {
        byte_t l_bit = m_bus.read_byte(m_pc);
        if (l_bit > dest_reg_hibit)
        {
            throw error_code::ec_invalid_argument;
        } else advance(1);

        dword_t l_result = m_sd | (1 << l_bit);
        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            return;
        }

        write_register(dest_reg, l_result);
    }
    
    void cpu::execute_res ()
    {
        byte_t l_bit = m_bus.read_byte(m_pc);
        if (l_bit > dest_reg_hibit)
        {
            throw error_code::ec_invalid_argument;
        } else advance(1);

        dword_t l_result = m_sd & ~(1 << l_bit);
        if (m_da != 0)
        {
            m_bus.write_byte(m_da, l_result & 0xFF);
            m_bus.cycle(1);
            return;
        }

        write_register(dest_reg, l_result);
    }
}
