#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "element/interpreter.h"
#include "ast/ast_internal.hpp"
#include "ast/fwd.hpp"
#include "construct.hpp"
#include "ast/scope.hpp"
#include "typeutil.hpp"



// TODO
// HACK
void update_scopes(const element_scope* names);


struct element_constraint : public element_construct, public rtti_type<element_constraint>
{
public:
    static const constraint_const_shared_ptr any;
    static const constraint_const_shared_ptr function;
    static const constraint_const_shared_ptr serializable;

    // TODO: separate into "shape matches" and "type matches"
    virtual bool is_satisfied_by(const constraint_const_shared_ptr& other) const
    {
        return other.get() == this || other == element_constraint::any;
    }

    bool is_same_shape_as(const constraint_const_shared_ptr& other) const
    {
        if (other.get() == this) return true;

        const auto& i1 = inputs();
        const auto& o1 = outputs();
        const auto& i2 = other->inputs();
        const auto& o2 = other->outputs();
        if (i1.size() != i2.size() || o1.size() != o2.size())
            return false;
        for (size_t i = 0; i < i1.size(); ++i) {
            if (!i1[i].type->is_same_shape_as(i2[i].type))
                return false;
        }
        for (size_t i = 0; i < o1.size(); ++i) {
            if (!o1[i].type->is_same_shape_as(o2[i].type))
                return false;
        }
        return true;
    }

protected:
    element_constraint(element_type_id id)
        : rtti_type(id)
    {
    }
};


struct element_type : public element_constraint
{
public:
    DECLARE_TYPE_ID();

    static const type_const_shared_ptr num;  // the absolute unit
    static const type_const_shared_ptr unary;
    static const type_const_shared_ptr binary;

    // TODO: make this accurate?
    virtual bool is_serializable() const { return get_size() != 0; }
    virtual bool is_variadic() const { return false; }

    std::string name() const { return m_name; }

protected:
    element_type(element_type_id id, std::string name)
        : element_constraint(id | type_id)
        , m_name(std::move(name))
    {
    }

    std::string m_name;
};

struct element_type_named : public element_type
{
    DECLARE_TYPE_ID();

    element_type_named(const element_scope* scope, std::string name)
        : element_type(type_id, std::move(name))
        , m_scope(scope)
    {
    }

    element_type_named(const element_scope* scope)
        : element_type_named(scope, scope ? scope->name : "")
    {
    }

    virtual const element_scope* scope() const { return m_scope; }

protected:
    void generate_ports_cache() const override;

    const element_scope* m_scope;
};

struct element_type_anonymous : public element_type
{
    DECLARE_TYPE_ID();

    static type_shared_ptr get(std::vector<port_info> inputs, std::vector<port_info> outputs)
    {
        // check cache first
        const size_t inputs_size = inputs.size();
        const size_t outputs_size = outputs.size();
        auto it_pair = m_cache.equal_range(std::make_pair(inputs_size, outputs_size));
        for (auto it = it_pair.first; it != it_pair.second; ++it) {
            if (it->second->inputs() == inputs && it->second->outputs() == outputs)
                return it->second;
        }

        auto t = std::shared_ptr<element_type_anonymous>(new element_type_anonymous(std::move(inputs), std::move(outputs)));
        m_cache.emplace(std::make_pair(inputs_size, outputs_size), t);
        return t;
    }

    bool is_satisfied_by(const constraint_const_shared_ptr& other) const override
    {
        // for anonymous types, also allow shape matches
        return element_constraint::is_satisfied_by(other) || is_same_shape_as(other);
    }

private:
    element_type_anonymous(std::vector<port_info> inputs, std::vector<port_info> outputs)
        : element_type(type_id, "<anonymous>")
    {
        m_inputs = std::move(inputs);
        m_outputs = std::move(outputs);
        m_ports_cached = true;
    }

    // Keeping a cache of anonymous types (matching on signature) allows us to use pointer-comparison for
    // type matching while supporting disparate anonymous types
    static std::unordered_multimap<std::pair<size_t, size_t>, type_shared_ptr, pair_hash> m_cache;
};
