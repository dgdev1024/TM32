/**
 * @file    tm32/cpu.cpp
 */

#include <tm32/bus.hpp>
#include <tm32/cpu.hpp>

namespace tm32
{

    /* Constructors and Destructor ********************************************/

    cpu::cpu (bus& p_bus) :
        m_bus   { p_bus }
    {

    }

    /* Public Methods *********************************************************/

    bool cpu::step ()
    {
        if (check_flag(flag_bit::ft_stop) == true)
        {
            return false;
        }

        try
        {
            // Clear whatever was fetched from the last frame.
            m_sa = 0;
            m_da = 0;
            m_sd = 0;
            m_ci = 0;
            
            if (check_flag(flag_bit::ft_halt) == false)
            {
                // Get the next instruction.
                m_ip = m_pc;
                m_ci = m_bus.read_word(m_ip);
                check_executable(m_ip);
                advance(2);
    
                // Determine which fetch and instruction needs to be run.
                switch ((m_ci >> 8) & 0xFF)
                {
                    case 0x00: execute_nop(); break;
                    case 0x01: execute_stop(); break;
                    case 0x02: execute_halt(); break;
                    case 0x03: execute_sec(); break;
                    case 0x04: execute_cec(); break;
                    case 0x05: execute_di(); break;
                    case 0x06: execute_ei(); break;
                    case 0x07: execute_daa(); break;
                    case 0x08: execute_scf(); break;
                    case 0x09: execute_ccf(); break;
                    case 0x0A: execute_flg(); break;
                    case 0x0B: execute_stf(); break;
                    case 0x0C: execute_clf(); break;
    
                    case 0x10: fetch_reg_imm(); execute_ld(); break;
                    case 0x11: fetch_reg_addr32(); execute_ld(); break;
                    case 0x12: fetch_reg_regptr32(); execute_ld(); break;
                    case 0x13: fetch_reg_addr16(); execute_ld(); break;
                    case 0x14: fetch_reg_regptr16(); execute_ld(); break;
                    case 0x15: fetch_reg8_addr8(); execute_ld(); break;
                    case 0x16: fetch_reg8_regptr8(); execute_ld(); break;
                    case 0x17: fetch_addr32_reg(); execute_st(); break;
                    case 0x18: fetch_regptr32_reg(); execute_st(); break;
                    case 0x19: fetch_addr16_reg(); execute_st(); break;
                    case 0x1A: fetch_regptr16_reg(); execute_st(); break;
                    case 0x1B: fetch_addr8_reg8(); execute_st(); break;
                    case 0x1C: fetch_regptr8_reg8(); execute_st(); break;
                    case 0x1D: fetch_reg_reg(); execute_mv(); break;
                    case 0x1E: execute_pop(); break;
                    case 0x1F: execute_push(); break;
    
                    case 0x20: fetch_null_imm32(); execute_jmp(); break;
                    case 0x21: fetch_null_reg32(); execute_jmp(); break;
                    case 0x22: fetch_null_simm16(); execute_jpb(); break;
                    case 0x23: fetch_null_imm32(); execute_call(); break;
                    case 0x24: execute_int(); break;
                    case 0x25: execute_ret(); break;
                    case 0x26: execute_reti(); break;
    
                    case 0x30: fetch_reg_null(); execute_inc(); break;
                    case 0x31: fetch_regptr32_null(); execute_inc(); break;
                    case 0x32: fetch_reg_null(); execute_dec(); break;
                    case 0x33: fetch_regptr32_null(); execute_dec(); break;
                    case 0x34: fetch_reg_imm(); execute_add(false); break;
                    case 0x35: fetch_reg_addr32(); execute_add(false); break;
                    case 0x36: fetch_reg_regptr32(); execute_add(false); break;
                    case 0x37: fetch_reg_imm(); execute_add(true); break;
                    case 0x38: fetch_reg_addr32(); execute_add(true); break;
                    case 0x39: fetch_reg_regptr32(); execute_add(true); break;
                    case 0x3A: fetch_reg_imm(); execute_sub(false); break;
                    case 0x3B: fetch_reg_addr32(); execute_sub(false); break;
                    case 0x3C: fetch_reg_regptr32(); execute_sub(false); break;
                    case 0x3D: fetch_reg_imm(); execute_sub(true); break;
                    case 0x3E: fetch_reg_addr32(); execute_sub(true); break;
                    case 0x3F: fetch_reg_regptr32(); execute_sub(true); break;
    
                    case 0x40: fetch_reg_imm(); execute_and(); break;
                    case 0x41: fetch_reg_addr32(); execute_and(); break;
                    case 0x42: fetch_reg_regptr32(); execute_and(); break;
                    case 0x43: fetch_reg_imm(); execute_or(); break;
                    case 0x44: fetch_reg_addr32(); execute_or(); break;
                    case 0x45: fetch_reg_regptr32(); execute_or(); break;
                    case 0x46: fetch_reg_imm(); execute_xor(); break;
                    case 0x47: fetch_reg_addr32(); execute_xor(); break;
                    case 0x48: fetch_reg_regptr32(); execute_xor(); break;
                    case 0x49: fetch_reg_null(); execute_not(); break;
                    case 0x4A: fetch_regptr32_null(); execute_not(); break;
    
                    case 0x50: fetch_reg_imm(); execute_cmp(); break;
                    case 0x51: fetch_reg_addr32(); execute_cmp(); break;
                    case 0x52: fetch_reg_regptr32(); execute_cmp(); break;
    
                    case 0x60: fetch_reg_null(); execute_sla(); break;
                    case 0x61: fetch_regptr32_null(); execute_sla(); break;
                    case 0x62: fetch_reg_null(); execute_sra(); break;
                    case 0x63: fetch_regptr32_null(); execute_sra(); break;
                    case 0x64: fetch_reg_null(); execute_srl(); break;
                    case 0x65: fetch_regptr32_null(); execute_srl(); break;
                    case 0x66: fetch_reg_null(); execute_rl(); break;
                    case 0x67: fetch_regptr32_null(); execute_rl(); break;
                    case 0x68: fetch_reg_null(); execute_rlc(); break;
                    case 0x69: fetch_regptr32_null(); execute_rlc(); break;
                    case 0x6A: fetch_reg_null(); execute_rr(); break;
                    case 0x6B: fetch_regptr32_null(); execute_rr(); break;
                    case 0x6C: fetch_reg_null(); execute_rrc(); break;
                    case 0x6D: fetch_regptr32_null(); execute_rrc(); break;
    
                    case 0x70: fetch_imm8_reg(); execute_bit(); break;
                    case 0x71: fetch_imm8_regptr32(); execute_bit(); break;
                    case 0x72: fetch_imm8_reg(); execute_tog(); break;
                    case 0x73: fetch_imm8_regptr32(); execute_tog(); break;
                    case 0x74: fetch_imm8_reg(); execute_set(); break;
                    case 0x75: fetch_imm8_regptr32(); execute_set(); break;
                    case 0x76: fetch_imm8_reg(); execute_res(); break;
                    case 0x77: fetch_imm8_regptr32(); execute_res(); break;
    
                    default:
                        throw error_code::ec_invalid_opcode;
                }
            }
            else
            {
                m_bus.cycle(1);
                if (m_if != 0)
                {
                    change_flag(flag_bit::ft_halt, false);
                }
            }

            if (m_ime == true)
            {
                service_interrupt();
                m_ei = false;
            }

            if (m_ei == true)
            {
                m_ime = true;
            }

            return true;
        }
        catch (const error_code& p_code)
        {
            m_ec = p_code;
            change_flag(flag_bit::ft_stop, true);
            return false;
        }
        catch (const std::exception& p_ex)
        {
            tm32_errorf("[TM32] Caught C++ exception: '{}'!", p_ex.what());
            change_flag(flag_bit::ft_stop, true);
            return false;
        }
    }

    dword_t cpu::read_register (register_type p_type) const
    {
        switch (p_type)
        {
            case register_type::rt_a:  return  m_a;
            case register_type::rt_aw: return (m_a & 0xFFFF);
            case register_type::rt_ah: return (m_a & 0xFF00) >> 8;
            case register_type::rt_al: return (m_a & 0xFF);
            case register_type::rt_b:  return  m_b;
            case register_type::rt_bw: return (m_b & 0xFFFF);
            case register_type::rt_bh: return (m_b & 0xFF00) >> 8;
            case register_type::rt_bl: return (m_b & 0xFF);
            case register_type::rt_c:  return  m_c;
            case register_type::rt_cw: return (m_c & 0xFFFF);
            case register_type::rt_ch: return (m_c & 0xFF00) >> 8;
            case register_type::rt_cl: return (m_c & 0xFF);
            case register_type::rt_d:  return  m_d;
            case register_type::rt_dw: return (m_d & 0xFFFF);
            case register_type::rt_dh: return (m_d & 0xFF00) >> 8;
            case register_type::rt_dl: return (m_d & 0xFF);
            default: return 0;
        }
    }

    void cpu::write_register (register_type p_type, const dword_t& p_value)
    {
        switch (p_type)
        {
            case register_type::rt_a:  m_a = p_value; break;
            case register_type::rt_aw: m_a = (m_a & 0xFFFF0000) | (p_value & 0xFFFF);
            case register_type::rt_ah: m_a = (m_a & 0xFFFF00FF) | ((p_value & 0xFF) << 8);
            case register_type::rt_al: m_a = (m_a & 0xFFFFFF00) | (p_value & 0xFF);
            case register_type::rt_b:  m_b = p_value; break;
            case register_type::rt_bw: m_b = (m_b & 0xFFFF0000) | (p_value & 0xFFFF);
            case register_type::rt_bh: m_b = (m_b & 0xFFFF00FF) | ((p_value & 0xFF) << 8);
            case register_type::rt_bl: m_b = (m_b & 0xFFFFFF00) | (p_value & 0xFF);
            case register_type::rt_c:  m_c = p_value; break;
            case register_type::rt_cw: m_c = (m_c & 0xFFFF0000) | (p_value & 0xFFFF);
            case register_type::rt_ch: m_c = (m_c & 0xFFFF00FF) | ((p_value & 0xFF) << 8);
            case register_type::rt_cl: m_c = (m_c & 0xFFFFFF00) | (p_value & 0xFF);
            case register_type::rt_d:  m_d = p_value; break;
            case register_type::rt_dw: m_d = (m_d & 0xFFFF0000) | (p_value & 0xFFFF);
            case register_type::rt_dh: m_d = (m_d & 0xFFFF00FF) | ((p_value & 0xFF) << 8);
            case register_type::rt_dl: m_d = (m_d & 0xFFFFFF00) | (p_value & 0xFF);
        }
    }

    bool cpu::check_flag (flag_bit p_bit) const
    {
        return (m_f & (p_bit));
    }

    void cpu::change_flag (flag_bit p_bit, bool p_state)
    {
        m_f = 
            (p_state == true) ?
                (m_f | (p_bit)) :
                (m_f & ~(p_bit));
    }

    std::string cpu::report_error ()
    {
        switch (m_ec)
        {
            case error_code::ec_ok:
                return "No error occured.";
            default:
                return std::format("A user-defined error '0x{:X}' occured.",
                    m_ec);
        }
    }

    /* Private Methods ********************************************************/

    dword_t cpu::pop_data ()
    {
        dword_t l_popped = m_bus.read_dword(m_sp); m_bus.cycle(4);
        m_sp += 4; m_bus.cycle(1);
        return l_popped;
    }

    void cpu::push_data (const dword_t& p_pushed)
    {
        m_sp -= 4; m_bus.cycle(1);
        m_bus.write_dword(m_sp, p_pushed); m_bus.cycle(4);
    }

    void cpu::advance (const byte_t& p_cycle_count)
    {
        m_bus.cycle(p_cycle_count);
        m_pc += p_cycle_count;
    }

    void cpu::service_interrupt ()
    {
        for (int i = 0; i < 32; ++i)
        {
            if (    
                tm32_checkbit(m_if, i) &&
                tm32_checkbit(m_ie, i)
            )
            {
                tm32_clearbit(m_if, i);
                change_flag(flag_bit::ft_halt, false);
                m_ime = false;
                push_data(m_pc);
                m_pc = 0x00001000 + (0x100 * i);
                break;
            }
        }
    }

    bool cpu::check_condition (condition_type p_cond)
    {
        switch (p_cond)
        {
            case condition_type::ct_none:           return true;
            case condition_type::ct_zero_clear:     return check_flag(flag_bit::ft_zero) == false;
            case condition_type::ct_zero_set:       return check_flag(flag_bit::ft_zero) == true;
            case condition_type::ct_carry_clear:    return check_flag(flag_bit::ft_carry) == false;
            case condition_type::ct_carry_set:      return check_flag(flag_bit::ft_carry) == true;
            case condition_type::ct_parity_clear:   return check_flag(flag_bit::ft_parity) == false;
            case condition_type::ct_parity_set:     return check_flag(flag_bit::ft_parity) == true;
            case condition_type::ct_sign_clear:     return check_flag(flag_bit::ft_sign) == false;
            case condition_type::ct_sign_set:       return check_flag(flag_bit::ft_sign) == true;
            default:                                return false;
        }
    }

    void cpu::check_writable (const address_t& p_addr)
    {
        if (p_addr < 0x80000000)
        {
            m_ea = p_addr;
            throw error_code::ec_invalid_write;
        }
    }

    void cpu::check_executable (const address_t& p_addr)
    {
        if (p_addr < 0x1000 || p_addr > 0x7FFFFFFF)
        {
            m_ea = p_addr;
            throw error_code::ec_invalid_execute;
        }
    }

}
