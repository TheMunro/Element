#pragma once

#include <element/common.h>

#include "element/interpreter.h"

#include "command.hpp"
#include "compiler_message.hpp"

namespace libelement::cli
{
    struct evaluate_command_arguments
    {
        std::string expression;
        std::string arguments;

        [[nodiscard]] std::string as_string() const
        {
            std::stringstream ss;
            ss << "evaluate --expression \"" << expression << "\"";
            if (!arguments.empty())
                ss << "--arguments \"" << arguments << "\"";
            return ss.str();
        }
    };

    class evaluate_command final : public command
    {
        evaluate_command_arguments custom_arguments;

    public:
        evaluate_command(common_command_arguments common_arguments,
                         evaluate_command_arguments custom_arguments)
            : command(std::move(common_arguments))
            , custom_arguments{ std::move(custom_arguments) }
        {}

        [[nodiscard]] compiler_message execute(const compilation_input& compilation_input) const override
        {
            const auto result = setup(compilation_input);
            if (result != ELEMENT_OK)
                return compiler_message(error_conversion(result),
                                        "Failed to setup context",
                                        compilation_input.get_log_json()); // todo

            if (compilation_input.get_compiletime())
                return execute_compiletime(compilation_input);

            return execute_runtime(compilation_input);
        }

        [[nodiscard]] compiler_message execute_compiletime(const compilation_input& compilation_input) const
        {
            auto expression = custom_arguments.expression;
            if (!custom_arguments.arguments.empty())
                expression += custom_arguments.arguments;

            constexpr auto max_output_size = 512;
            element_value outputs_buffer[max_output_size];
            element_outputs output;
            output.values = outputs_buffer;
            output.count = max_output_size;

            const auto result = element_interpreter_evaluate_expression(
                context, nullptr, expression.c_str(), &output);

            if (result != ELEMENT_OK)
            {
                return compiler_message(error_conversion(result),
                                        "Failed to evaluate: " + expression + " with element_result " + std::to_string(result),
                                        compilation_input.get_log_json());
            }

            return generate_response(result, output, compilation_input.get_log_json());
        }

        [[nodiscard]] compiler_message execute_runtime(const compilation_input& compilation_input) const
        {
            return compiler_message(ELEMENT_ERROR_UNKNOWN,
                                    "Runtime evaluation is not supported",
                                    compilation_input.get_log_json()); // todo
        }

        [[nodiscard]] std::string as_string() const override
        {
            std::stringstream ss;
            ss << custom_arguments.as_string() << " " << common_arguments.as_string();
            return ss.str();
        }

        static void configure(CLI::App& app,
            const std::shared_ptr<common_command_arguments>& common_arguments,
            callback callback)
        {
            const auto arguments = std::make_shared<evaluate_command_arguments>();

            auto* command = app.add_subcommand("evaluate")->fallthrough();

            command->add_option("-e,--expression", arguments->expression,
                             "Expression to evaluate.")
                ->required();

            command->add_option("-a,--arguments", arguments->arguments,
                                "Arguments for performing call expression to given expression.");

            command->callback([callback, common_arguments, arguments]() {
                evaluate_command cmd(*common_arguments, *arguments);
                callback(cmd);
            });
        }
    };
} // namespace libelement::cli