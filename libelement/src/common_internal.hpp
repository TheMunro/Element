#pragma once

#include <string>

#include "element/token.h"
#include "element/common.h"
#include "etree/fwd.hpp"

struct element_expression;
struct element_tokeniser_ctx;
struct element_interpreter_ctx;
struct element_parser_ctx;
struct element_ast;

#define ENSURE_NOT_NULL(t) if (t == nullptr) { return ELEMENT_ERROR_INVALID_PTR; }

using LogCallback = void (*)(const element_log_message* const);

struct element_log_ctx
{
    LogCallback callback;
	
    void log(const std::string& message, const element_stage stage) const;
    void log(const element_tokeniser_ctx& context, element_result code, const std::string& message, int length = -1, element_log_message* related_message = nullptr) const;
    void log(const element_interpreter_ctx& context, element_result code, const std::string& message, const std::string& filename) const;
    void log(const element_parser_ctx& context, element_result code, const std::string& message, const element_ast* nearest_ast = nullptr) const;
    void log(const element_compiler_ctx& context, element_result code, const std::string& message, const element_ast* nearest_ast = nullptr) const;
    void log(const element_log_message& log) const;
};

std::string tokens_to_string(const element_tokeniser_ctx* tokeniser, const element_token* nearest_token = nullptr);
std::string ast_to_string(const element_ast* ast, int depth = 0, const element_ast* ast_to_mark = nullptr);
std::string expression_to_string(const element_expression& expression, int depth = 0);
std::string ast_to_code(const element_ast* node, const element_ast* parent = nullptr, bool skip = false);