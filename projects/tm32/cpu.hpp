/**
 * @file    tm32/cpu.hpp
 * @brief   The TM32 CPU class, which implements the TM32 CPU architecture.
 */

#pragma once
#include <tm32/common.hpp>

namespace tm32
{
    class bus;

    class cpu final
    {
    public:
        cpu (bus&);
        ~cpu () = default;

    public:
        bool        step ();        
        dword_t     read_register (register_type) const;
        void        write_register (register_type, const dword_t&);
        bool        check_flag (flag_bit) const;
        void        change_flag (flag_bit, bool);
        std::string report_error ();

    private:
        dword_t     pop_data ();
        void        push_data (const dword_t&);
        void        advance (const byte_t&);
        void        service_interrupt ();
        bool        check_condition (condition_type);
        void        check_writable (const address_t&);
        void        check_executable (const address_t&);

    private:
        void        fetch_reg_imm ();
        void        fetch_reg_addr32 ();
        void        fetch_reg_regptr32 ();
        void        fetch_reg_addr16 ();
        void        fetch_reg_regptr16 ();
        void        fetch_reg8_addr8 ();
        void        fetch_reg8_regptr8 ();
        void        fetch_addr32_reg ();
        void        fetch_regptr32_reg ();
        void        fetch_addr16_reg ();
        void        fetch_regptr16_reg ();
        void        fetch_addr8_reg8 ();
        void        fetch_regptr8_reg8 ();
        void        fetch_reg_reg ();
        void        fetch_null_imm32 ();
        void        fetch_null_reg32 ();
        void        fetch_null_simm16 ();
        void        fetch_reg_null ();
        void        fetch_regptr32_null ();
        void        fetch_imm8_reg ();
        void        fetch_imm8_regptr32 ();

    private:
        void        execute_nop ();
        void        execute_stop ();
        void        execute_halt ();
        void        execute_sec ();
        void        execute_cec ();
        void        execute_di ();
        void        execute_ei ();
        void        execute_daa ();
        void        execute_scf ();
        void        execute_ccf ();
        void        execute_flg ();
        void        execute_stf ();
        void        execute_clf ();
        void        execute_ld ();
        void        execute_st ();
        void        execute_mv ();
        void        execute_pop ();
        void        execute_push ();
        void        execute_jmp ();
        void        execute_jpb ();
        void        execute_call ();
        void        execute_int ();
        void        execute_ret ();
        void        execute_reti ();
        void        execute_inc ();
        void        execute_dec ();
        void        execute_add (bool);
        void        execute_sub (bool);
        void        execute_and ();
        void        execute_or ();
        void        execute_xor ();
        void        execute_not ();
        void        execute_cmp ();
        void        execute_sla ();
        void        execute_sra ();
        void        execute_srl ();
        void        execute_rl ();
        void        execute_rlc ();
        void        execute_rr ();
        void        execute_rrc ();
        void        execute_bit ();
        void        execute_tog ();
        void        execute_set ();
        void        execute_res ();

    private:
        bus&        m_bus;
        dword_t     m_a     = 0;
        dword_t     m_b     = 0;
        dword_t     m_c     = 0;
        dword_t     m_d     = 0;
        address_t   m_pc    = 0x00003000;
        address_t   m_ip    = 0;
        address_t   m_sp    = 0xFFFEFFFF;
        address_t   m_ea    = 0;
        address_t   m_da    = 0;
        address_t   m_sa    = 0;
        dword_t     m_sd    = 0;
        dword_t     m_f     = 0;
        dword_t     m_if    = 0;
        dword_t     m_ie    = 0;
        word_t      m_ci    = 0;
        byte_t      m_ec    = 0;
        bool        m_ime   = false;
        bool        m_ei    = false;
        
    };
}
