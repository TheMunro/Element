#pragma once

#include <map>
#include <string>

#include <element/common.h>
#include <toml.hpp>

namespace libelement::cli
{
    enum class message_level : unsigned int
    {
        Verbose = 0,
        Information,
        Warning,
        Error,
        Fatal,
        Unknown = 0xFFFF
    };

    struct message_code
    {
        typedef const std::map<std::string, message_level>::const_iterator
            message_level_const_iterator;

        element_result type;
        std::string name;
        message_level level;

        message_code(element_result type, std::string name, const std::string& level)
            : type{ type }
            , name{ std::move(name) }
            , level{ get_message_level(level) }
        {}

    private:
        static message_level get_message_level(const std::string& level)
        {
            static std::map<std::string, message_level> map_message_level = {
                { "Fatal", message_level::Fatal },
                { "Warning", message_level::Warning },
                { "Error", message_level::Error },
                { "Information", message_level::Information },
                { "Verbose", message_level::Verbose }
            };

            message_level_const_iterator it = map_message_level.find(level);
            if (it != map_message_level.end())
                return it->second;

            return message_level::Unknown;
        }
    };

    class message_codes
    {
        typedef const std::map<element_result, message_code>::const_iterator
            message_code_const_iterator;

    public:
        message_codes(const std::string& path)
        {
            auto data = toml::parse(path);
            auto table = toml::get<toml::table>(data);

            message_code_const_iterator it = code_map.begin();
            for (const auto& item : table)
            {
                auto name = toml::find<std::string>(item.second, "name");
                auto level = toml::find<std::string>(item.second, "level");
                auto index = static_cast<element_result>(std::stoi(item.first.substr(3)));
                code_map.insert(it, std::pair<element_result, message_code>(
                                        index, message_code(index, name, level)));
            }
        }

        const message_code* get_code(element_result type) const
        {
            message_code_const_iterator it = code_map.find(type);
            if (it != code_map.end())
                return &it->second;

            return nullptr;
        }

        message_level get_level(element_result type) const
        {
            const message_code* message_code = get_code(type);
            if (message_code == nullptr)
                return message_level::Unknown;

            return message_code->level;
        }

    private:
        std::map<element_result, message_code> code_map;
    };

    inline element_result error_conversion(element_result result)
    {
        if (result > 0)
            return result;

        if (result == ELEMENT_ERROR_RESERVED_IDENTIFIER)
            return ELEMENT_ERROR_INVALID_IDENTIFIER;

        return ELEMENT_ERROR_UNKNOWN;
    };
} // namespace libelement::cli