#pragma once

#include <iostream>

#include "common_internal.hpp"
#include "token_internal.hpp"
#include "ast/ast_internal.hpp"
#include "etree/compiler.hpp"

#include <fmt/format.h>

#include "configuration.hpp"

#define PRINTCASE(a) case a: c = #a; break;
std::string tokens_to_string(const element_tokeniser_ctx* tokeniser, const element_token* nearest_token)
{
    std::string string;

	for(const auto& token : tokeniser->tokens)
	{
        std::string c;
        switch (token.type)
        {
            PRINTCASE(ELEMENT_TOK_NONE);
            PRINTCASE(ELEMENT_TOK_NUMBER);
            PRINTCASE(ELEMENT_TOK_IDENTIFIER);
            PRINTCASE(ELEMENT_TOK_UNDERSCORE);
            PRINTCASE(ELEMENT_TOK_DOT);
            PRINTCASE(ELEMENT_TOK_BRACKETL);
            PRINTCASE(ELEMENT_TOK_BRACKETR);
            PRINTCASE(ELEMENT_TOK_SEMICOLON);
            PRINTCASE(ELEMENT_TOK_COLON);
            PRINTCASE(ELEMENT_TOK_COMMA);
            PRINTCASE(ELEMENT_TOK_BRACEL);
            PRINTCASE(ELEMENT_TOK_BRACER);
            PRINTCASE(ELEMENT_TOK_EQUALS);
			default: c = "";
        }
		
        string += c;

		if(token.tok_pos >= 0 && token.tok_len >= 0)
		{
            //Align the token text using tabs (with tabs = 4 spaces), so that they start at character 24 (4*6)
            auto chunks = c.length() / 4;
            for(auto i = chunks; chunks < 6; chunks++)
                string += "\t";

            auto text = tokeniser->text(&token);
            string += text;

            if (nearest_token == &token)
            {
                string += " <--- HERE";
            }
		}
		
        string += "\n";
	}

	
    return string;
}

std::string ast_to_string(const element_ast* ast, int depth, const element_ast* ast_to_mark)
{
    std::string string;

    for (int i = 0; i < depth; ++i)
        string += "  ";

    if (ast->type == ELEMENT_AST_NODE_LITERAL) {
        string += fmt::format("LITERAL: {}", ast->literal);
    }
    else if (ast->type == ELEMENT_AST_NODE_IDENTIFIER) {
        string += fmt::format("IDENTIFIER: {}", ast->identifier);
    }
    else if (ast->type == ELEMENT_AST_NODE_DECLARATION) {
        const auto intrinsic = ast->has_flag(ELEMENT_AST_FLAG_DECL_INTRINSIC);
        intrinsic
            ? string += fmt::format("INTRINSIC DECLARATION: {}", ast->identifier)
            : string += fmt::format("DECLARATION: {}", ast->identifier);
    }
    else if (ast->type == ELEMENT_AST_NODE_NAMESPACE) {
        string += fmt::format("NAMESPACE: {}", ast->identifier);
    }
    else if (ast->type == ELEMENT_AST_NODE_CALL) {
        string += fmt::format("CALL: {}", ast->identifier);
    }
    else if (ast->type == ELEMENT_AST_NODE_PORT) {
        string += fmt::format("PORT: {}", ast->identifier);
    }
    else if (ast->type == ELEMENT_AST_NODE_UNSPECIFIED_TYPE && ast->parent && ast->parent->type == ELEMENT_AST_NODE_PORT) {
		string += "IMPLICIT TYPE";
    }
    else if (ast->type == ELEMENT_AST_NODE_UNSPECIFIED_TYPE && ast->parent && ast->parent->type == ELEMENT_AST_NODE_DECLARATION) {
        string += "IMPLICIT RETURN";
    }
    else if (ast->type == ELEMENT_AST_NODE_NONE) {
        if (ast->has_flag(ELEMENT_AST_FLAG_DECL_EMPTY_INPUT))
            string += "EMPTY INPUT";
        else if (ast->flags == 0)
            string += "NONE";
        else
            string += "UNKNOWN FLAGS";
    }
    else {
        char* c;
        switch (ast->type) {
            PRINTCASE(ELEMENT_AST_NODE_ROOT);
            PRINTCASE(ELEMENT_AST_NODE_SCOPE);
            PRINTCASE(ELEMENT_AST_NODE_CONSTRAINT);
            PRINTCASE(ELEMENT_AST_NODE_FUNCTION);
            PRINTCASE(ELEMENT_AST_NODE_STRUCT);
            PRINTCASE(ELEMENT_AST_NODE_EXPRESSION);
            PRINTCASE(ELEMENT_AST_NODE_EXPRLIST);
            PRINTCASE(ELEMENT_AST_NODE_PORTLIST);
            PRINTCASE(ELEMENT_AST_NODE_TYPENAME);
            PRINTCASE(ELEMENT_AST_NODE_LAMBDA);
        default: c = "ELEMENT_AST_NODE_<UNKNOWN>"; break;
        }

        //Offset pointer by length of prefix to cutoff prefix
        string += fmt::format("{}", c + strlen("ELEMENT_AST_NODE_"));
    }

    if (ast_to_mark && ast_to_mark == ast)
        string += " <--- Here";

    string += "\n";

    for (const auto& child : ast->children)
        string += ast_to_string(child.get(), depth + 1, ast_to_mark);

    return string;
}

std::string expression_to_string(const element_expression& expression, int depth)
{
    std::string string;
	
    for (int i = 0; i < depth; ++i)
        string += "  ";

    if (expression.is<element_expression_constant>())
    {
        const auto& constant = expression.as<element_expression_constant>();
        string += "CONSTANT: " + std::to_string(constant->value());
    }

    if (expression.is<element_expression_input>())
    {
        const auto& input = expression.as<element_expression_input>();
        string += "INPUT: " + std::to_string(input->index());
    }

    if (expression.is<element_expression_structure>())
    {
        const auto& structure = expression.as<element_expression_structure>();
        string += "STRUCTURE: ";
    }
	
    if(expression.is<element_expression_unary>())
    {
        const auto& unary = expression.as<element_expression_unary>();
        string += "UNARY: ";
        char* c = nullptr;
    	switch(unary->operation())
    	{
            PRINTCASE(element_unary_op::sin);
            PRINTCASE(element_unary_op::cos);
            PRINTCASE(element_unary_op::tan);
            PRINTCASE(element_unary_op::asin);
            PRINTCASE(element_unary_op::acos);
            PRINTCASE(element_unary_op::atan);
            PRINTCASE(element_unary_op::ln);
            PRINTCASE(element_unary_op::abs);
            PRINTCASE(element_unary_op::ceil);
            PRINTCASE(element_unary_op::floor);
    	}
        string += c;
    }

    if (expression.is<element_expression_binary>())
    {
        const auto& binary = expression.as<element_expression_binary>();
        string += "BINARY: ";
        char* c = nullptr;
        switch (binary->operation())
        {
            PRINTCASE(element_binary_op::add);
            PRINTCASE(element_binary_op::sub);
            PRINTCASE(element_binary_op::mul);
            PRINTCASE(element_binary_op::div);
            PRINTCASE(element_binary_op::rem);
            PRINTCASE(element_binary_op::pow);
            PRINTCASE(element_binary_op::min);
            PRINTCASE(element_binary_op::max);
            PRINTCASE(element_binary_op::log);
            PRINTCASE(element_binary_op::atan2);
        }
        string += c;
    }
	
    string += "\n";
	
    for (const auto& dependent : expression.dependents())
        string += expression_to_string(*dependent, depth + 1);
    return string;
}

std::string ast_to_code(const element_ast* node, const element_ast* parent, bool skip)
{
    auto has_typed_parent = [](const element_ast* parent, element_ast_node_type type)
    {
        return parent != nullptr && parent->type == type;
    };

    auto has_children = [](const element_ast* node)
    {
        return node != nullptr && !node->children.empty();
    };

    std::stringstream ss;

    if (!skip) {
        switch (node->type)
        {
        case ELEMENT_AST_NODE_PORT:
        case ELEMENT_AST_NODE_IDENTIFIER:
            ss << node->identifier;
            break;

        case ELEMENT_AST_NODE_TYPENAME:
            ss << ":";
            break;

        case ELEMENT_AST_NODE_STRUCT:
            ss << "struct ";
            break;

        case ELEMENT_AST_NODE_NAMESPACE:
            ss << "namespace ";
            break;

        case ELEMENT_AST_NODE_CONSTRAINT:
            ss << "constraint ";
            break;

        case ELEMENT_AST_NODE_LAMBDA:
            ss << "_";
            break;

        case ELEMENT_AST_NODE_LITERAL:
            ss << node->literal;
            if (has_typed_parent(parent, ELEMENT_AST_NODE_CALL))
                ss << ".";
            break;

            //case ELEMENT_AST_NODE_FUNCTION:
            //special case DECLARATION & CALL with terminal and newline

            //all following cases recurse and return early to avoid default child recursion loop at bottom of function
        case ELEMENT_AST_NODE_SCOPE:
            ss << "{\n" << ast_to_code(node, parent, true) << "\n}\n";
            return ss.str();

        case ELEMENT_AST_NODE_PORTLIST:
        case ELEMENT_AST_NODE_EXPRLIST:
        {
            if (has_children(node)) {

                auto fold = [](std::string a, const ast_unique_ptr& ptr) {
                    return std::move(a) + ',' + ast_to_code(ptr.get());
                };

                const auto begin_iter = begin(node->children);
                const auto end_iter = end(node->children);

                ss << std::accumulate(std::next(begin_iter), end_iter, ast_to_code((*begin_iter).get()), fold);

                return ss.str();
            }

            return ss.str();
        }

        case ELEMENT_AST_NODE_DECLARATION:
        {
            ss << node->identifier;
            if (node->children.size() > ast_idx::decl::inputs && has_children(node->children[ast_idx::decl::inputs].get())) {
                ss << "(" + ast_to_code(node->children[ast_idx::decl::inputs].get(), node) << ")";
            }

            if (node->children.size() > ast_idx::decl::outputs) {
                ss << ast_to_code(node->children[ast_idx::decl::outputs].get(), node);
            }

            ss << " = ";
            return ss.str();
        }

        case ELEMENT_AST_NODE_CALL:
        {
            if (node->children.size() > ast_idx::call::parent) {
                ss << ast_to_code(node->children[ast_idx::call::parent].get(), node);
            }

            if (node->children.size() > ast_idx::call::args && has_children(node->children[ast_idx::call::args].get())) {
                ss << node->identifier << "(" + ast_to_code(node->children[ast_idx::call::args].get(), node) << ")";
                if (has_typed_parent(parent, ELEMENT_AST_NODE_CALL))
                    ss << ".";
                return ss.str();
            }

            ss << node->identifier;
            if (has_typed_parent(parent, ELEMENT_AST_NODE_CALL))
                ss << ".";
            return ss.str();
        }

        default:
            break;
        }
    }

    for (const auto& child : node->children) {

        const auto* child_ptr = child.get();
        ss << ast_to_code(child_ptr, node);

    }

    return ss.str();
}

void element_log_ctx::log(const element_tokeniser_ctx& context, element_result code, const std::string& message, int length, element_log_message* related_message) const
{
    element_log_message msg;
    msg.line = context.line;
    msg.character = context.character;
    msg.stage = ELEMENT_STAGE_TOKENISER;
    msg.filename = context.filename.c_str();
    msg.related_log_message = nullptr;

    msg.message_code = code;
    msg.length = length;
    msg.related_log_message = related_message;

    std::string new_log_message = message;
	
    if (flag_set(logging_bitmask, log_flags::debug | log_flags::output_tokens))
        new_log_message += "\n\nTOKENS\n------\n" + tokens_to_string(&context, nullptr);

    msg.message = new_log_message.c_str();

    log(msg);
}

void element_log_ctx::log(const element_interpreter_ctx& context, element_result code, const std::string& message, const std::string& filename) const
{
    auto msg = element_log_message();
    msg.message = message.c_str();
    msg.message_code = code;
    msg.line = -1;
    msg.character = -1;
    msg.length = -1;
    msg.stage = ELEMENT_STAGE_EVALUATOR;
    msg.filename = filename.c_str();
    msg.related_log_message = nullptr;
	
    log(msg);
}

void element_log_ctx::log(const std::string& message, const element_stage stage) const
{
    auto msg = element_log_message();
    msg.message = message.c_str();
    msg.message_code = ELEMENT_OK;
    msg.line = -1;
    msg.character = -1;
    msg.length = -1;
    msg.stage = stage;
    msg.related_log_message = nullptr;
	
    log(msg);
}

void element_log_ctx::log(const element_parser_ctx& context, element_result code, const std::string& message, const element_ast* nearest_ast) const
{
    assert(context.tokeniser);

    const bool starts_with_prelude = context.tokeniser->filename.rfind("Prelude\\", 0) == 0;
	
    auto msg = element_log_message();
    msg.message = message.c_str();
    msg.message_code = code;
    msg.line = -1;
    msg.character = -1;
    msg.length = -1;
    msg.stage = ELEMENT_STAGE_PARSER;
    msg.filename = context.tokeniser->filename.c_str();
    msg.related_log_message = nullptr;

    std::string new_log_message;
    if (nearest_ast && nearest_ast->nearest_token) {
        msg.line = nearest_ast->nearest_token->line;
        msg.character = nearest_ast->nearest_token->character;
        msg.length = nearest_ast->nearest_token->tok_len;

        if (msg.line > 0) {
            std::string source_line = context.tokeniser->text_on_line(msg.line) + "\n";

            //todo: doesn't handle UTF8 I'm guessing
            if (msg.character >= 0) {
                for (int i = 0; i < msg.character - 1; ++i)
                    source_line += " ";

                source_line += "^";

                for (int i = 0; i < msg.length - 1; ++i)
                    source_line += "^";
            }

            new_log_message = "\n\n" + source_line + " " + message;
        }
    } else {
        new_log_message = message;
    }

    //Only print ast/token prelude info if it's forced on or if a non-zero code is being logged
    if (code != ELEMENT_OK)
    {
        if (flag_set(logging_bitmask, log_flags::debug | log_flags::output_tokens))
            new_log_message += "\n\nTOKENS\n------\n" + tokens_to_string(context.tokeniser, nearest_ast ? nearest_ast->nearest_token : nullptr);
    	
        if (flag_set(logging_bitmask, log_flags::debug | log_flags::output_ast))
			new_log_message += "\n\nAST\n---\n" + ast_to_string(context.root, 0, nearest_ast ? nearest_ast : nullptr);
    }
	
    if (starts_with_prelude && !flag_set(logging_bitmask, log_flags::debug | log_flags::output_prelude)) {
        return; //early out if prelude logging disabled
    }
	
    msg.message = new_log_message.c_str();
	
    log(msg);
}

void element_log_ctx::log(const element_compiler_ctx& context, element_result code, const std::string& message,
    const element_ast* nearest_ast) const
{
    auto msg = element_log_message();
    msg.message = message.c_str();
    msg.message_code = code;
    msg.line = -1;
    msg.character = -1;
    msg.length = -1;
    msg.stage = ELEMENT_STAGE_COMPILER;
    msg.filename = "<unknown>"; //todo: get from scope/ast?
    msg.related_log_message = nullptr;

    std::string new_log_message;
    if (nearest_ast && nearest_ast->nearest_token) {
        msg.line = nearest_ast->nearest_token->line;
        msg.character = nearest_ast->nearest_token->character;
        msg.length = nearest_ast->nearest_token->tok_len;

        //todo: print source code/use james debug recreation
        new_log_message = message;
    }
    else {
        new_log_message = message;
    }

    if (code != ELEMENT_OK) {
        if (flag_set(logging_bitmask, log_flags::debug | log_flags::output_ast))
			new_log_message += "\n\nAST\n---\n" + ast_to_string(get_root_from_ast(nearest_ast), 0, nearest_ast);
    }

    msg.message = new_log_message.c_str();

    log(msg);
}

void element_log_ctx::log(const element_log_message& log) const
{
    if (callback == nullptr)
        return;

    callback(&log);
}
