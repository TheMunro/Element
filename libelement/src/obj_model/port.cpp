#include "port.hpp"

//SELF
#include "declarations.hpp"
#include "scope.hpp"

namespace element
{
    port::port(const declaration* declarer, identifier name, std::unique_ptr<type_annotation> annotation)
        : declarer(declarer)
        , name{ std::move(name) }
        , annotation{ std::move(annotation) }
    {
    }

    const declaration* port::resolve_annotation(const compilation_context& context) const
    {
        if (!annotation)
            return context.get_global_scope()->find(identifier{ "Any" }, false);

        return declarer->get_scope()->find(annotation->to_string(), true);
    }

    object_const_shared_ptr port::generate_placeholder(const compilation_context& context, int& placeholder_index) const
    {
        const auto type = resolve_annotation(context);
        if (!type)
            return nullptr;

        return type->generate_placeholder(context, placeholder_index);
    }
}