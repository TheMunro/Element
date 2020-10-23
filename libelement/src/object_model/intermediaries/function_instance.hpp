#pragma once

//SELF
#include "../object_internal.hpp"
#include "../capture_stack.hpp"

namespace element
{
    class function_instance final : public object, public std::enable_shared_from_this<function_instance>
    {
    public:
        explicit function_instance(const function_declaration* declarer, capture_stack captures, source_information source_info);
        explicit function_instance(const function_declaration* declarer, capture_stack captures, source_information source_info, std::vector<object_const_shared_ptr> args);

        [[nodiscard]] std::string typeof_info() const override;
        [[nodiscard]] std::string to_string() const override;

        [[nodiscard]] bool matches_constraint(const compilation_context& context, const constraint* constraint) const override;
        [[nodiscard]] const constraint* get_constraint() const override;

        [[nodiscard]] object_const_shared_ptr call(const compilation_context& context,
                                                   std::vector<object_const_shared_ptr> compiled_args,
                                                   const source_information& source_info) const override;

        [[nodiscard]] object_const_shared_ptr compile(const compilation_context& context,
                                                      const source_information& source_info) const override;

        [[nodiscard]] bool is_constant() const override;
        [[nodiscard]] bool valid_at_boundary(const compilation_context& context) const;

        const function_declaration* const declarer;

    private:
        mutable capture_stack captures;
        std::vector<object_const_shared_ptr> provided_arguments;
    };
} // namespace element