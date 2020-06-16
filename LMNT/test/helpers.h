#pragma once

#include "lmnt/interpreter.h"
#include "lmnt/opcodes.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>


#define TEST_LOAD_ARCHIVE(ctx, name, def, a) \
    CU_ASSERT_EQUAL_FATAL(lmnt_ictx_load_archive((ctx), (a).buf, (a).size), LMNT_OK);\
    {\
        lmnt_validation_result vr;\
        CU_ASSERT_EQUAL_FATAL(lmnt_ictx_prepare_archive((ctx), &vr), LMNT_OK);\
        CU_ASSERT_EQUAL_FATAL(vr, LMNT_VALIDATION_OK);\
    }\
    CU_ASSERT_EQUAL_FATAL(lmnt_ictx_find_def((ctx), (name), &(def)), LMNT_OK);



static lmnt_ictx* create_interpreter_with_size(size_t bufsize)
{
    lmnt_ictx* ictx = (lmnt_ictx*)malloc(sizeof(lmnt_ictx));
    char* mem = calloc(bufsize, sizeof(char));
    if (lmnt_ictx_init(ictx, mem, bufsize) == LMNT_OK) {
        return ictx;
    } else {
        free(mem);
        free(ictx);
        return NULL;
    }
}

static lmnt_ictx* create_interpreter()
{
    const size_t bufsize = 65536;
    return create_interpreter_with_size(bufsize);
}

static void delete_interpreter(lmnt_ictx* ictx)
{
    free(ictx->memory_area);
    free(ictx);
}

typedef struct
{
    char* buf;
    size_t size;
} archive;

static archive create_archive_array(const char* def_name, uint16_t args_count, uint16_t rvals_count, uint16_t stack_count, uint32_t instr_count, uint32_t consts_count, ...)
{
    const size_t name_len = strlen(def_name);
    assert(name_len <= 0xFE);
    assert(instr_count <= 0x3FFFFFF0);
    assert(consts_count <= 0x3FFFFFFF);

    const size_t header_len = 0x18;
    const size_t strings_len = 0x02 + name_len + 1;
    const size_t defs_len = 0x15;
    uint32_t code_len = 0x04 + instr_count * sizeof(lmnt_instruction);
    const uint32_t padding = (8 - ((header_len + strings_len + defs_len + code_len) % 8)) % 8;
    code_len += padding;
    const uint32_t consts_len = consts_count * sizeof(lmnt_value);

    const size_t total_size = header_len + strings_len + defs_len + code_len + consts_len;
    char* buf = calloc(total_size, sizeof(char));

    size_t idx = 0;
    const char header[] = {
        'L', 'M', 'N', 'T',
        0x00, 0x00, 0x00, 0x00,
        strings_len & 0xFF, (strings_len >> 8) & 0xFF, (strings_len >> 16) & 0xFF, (strings_len >> 24) & 0xFF, // strings length
        defs_len & 0xFF, (defs_len >> 8) & 0xFF, (defs_len >> 16) & 0xFF, (defs_len >> 24) & 0xFF, // defs length
        code_len & 0xFF, (code_len >> 8) & 0xFF, (code_len >> 16) & 0xFF, (code_len >> 24) & 0xFF, // code length
        consts_len & 0xFF, (consts_len >> 8) & 0xFF, (consts_len >> 16) & 0xFF, (consts_len >> 24) & 0xFF // constants_length
    };
    memcpy(buf + idx, header, sizeof(header));
    idx += sizeof(header);

    buf[idx] = (name_len + 1) & 0xFF;
    idx += 2;

    memcpy(buf + idx, def_name, name_len);
    idx += name_len;
    buf[idx++] = '\0';

    const char def[] = {
        0x15, 0x00, // defs[0].length
        0x00, 0x00, // defs[0].name
        0x00, 0x00, // defs[0].flags
        0x00, 0x00, 0x00, 0x00, // defs[0].code
        stack_count & 0xFF, (stack_count >> 8) & 0xFF, // defs[0].stack_count_unaligned
        stack_count & 0xFF, (stack_count >> 8) & 0xFF, // defs[0].stack_count_aligned
        0x00, 0x00, // defs[0].base_args_count
        args_count & 0xFF, (args_count >> 8) & 0xFF, // defs[0].args_count
        rvals_count & 0xFF, (rvals_count >> 8) & 0xFF, // defs[0].rvals_count
        0x00        // defs[0].bases_count
    };
    memcpy(buf + idx, def, sizeof(def));
    idx += sizeof(def);

    memcpy(buf + idx, (const char*)(&instr_count), sizeof(uint32_t));
    idx += sizeof(uint32_t);

    va_list args;
    va_start(args, consts_count);
    for (size_t i = 0; i < instr_count; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            buf[idx++] = va_arg(args, char);
        }
    }

    idx += padding;
    for (size_t i = 0; i < consts_count; ++i) {
        lmnt_value val = va_arg(args, lmnt_value);
        memcpy(buf + idx, (const char*)(&val), sizeof(val));
        idx += sizeof(val);
    }

    assert(idx == total_size);

    archive a = {buf, total_size};
    return a;
}

static void delete_archive_array(archive archive)
{
    free(archive.buf);
}