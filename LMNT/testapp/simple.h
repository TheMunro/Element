#pragma once

#include "lmnt/opcodes.h"

static const char filedata_simple[] = {
    'L', 'M', 'N', 'T',
    0x00, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, // strings length
    0x15, 0x00, 0x00, 0x00, // defs length
    0x0E, 0x00, 0x00, 0x00, // code length
    0x04, 0x00, 0x00, 0x00, // data length
    0x00, 0x00, 0x00, 0x00, // constants_length
    // constants
    0x07, 0x00, 'P', 'o', 't', 'a', 't', 'o', '\0', // strings[0]
    0x15, 0x00, // defs[0].length
    0x00, 0x00, // defs[0].name
    0x00, 0x00, // defs[0].flags
    0x00, 0x00, 0x00, 0x00, // defs[0].code
    0x03, 0x00, // defs[0].stack_count_unaligned
    0x03, 0x00, // defs[0].stack_count_aligned
    0x00, 0x00, // defs[0].base_args_count
    0x02, 0x00, // defs[0].args_count
    0x01, 0x00, // defs[0].rvals_count
    0x00,       // defs[0].bases_count
    // code
    0x01, 0x00, 0x00, 0x00, // ops_count
    LMNT_OP_BYTES(LMNT_OP_ADDSS, 0x00, 0x01, 0x02),
    // pad to multiple of 4
    0x00, 0x00,
    // data
    0x00, 0x00, 0x00, 0x00
};