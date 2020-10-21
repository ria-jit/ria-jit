//
// Created by Flo Schmidt on 18.10.20.
//

#include <stdbool.h>

bool flag_log_general = false;
bool flag_log_syscall = false;
bool flag_log_asm_in = false;
bool flag_log_asm_out = false;
bool flag_verbose_disassembly = false;
bool flag_log_reg_dump = false;
bool flag_log_cache = false;
bool flag_log_cache_contents = false;
bool flag_log_context = false;
bool flag_fail_silently = false;
bool flag_single_step = false;
bool flag_translate_opt_ras = true;
bool flag_translate_opt_chain = true;
bool flag_translate_opt_jump = true;
bool flag_translate_opt_fusion = true;
bool flag_do_benchmark = false;
bool flag_do_analyze_mnem = false;
bool flag_do_analyze_reg = false;
bool flag_do_analyze_pattern = false;
bool flag_do_profile = false;
