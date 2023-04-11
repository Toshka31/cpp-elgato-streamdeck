#pragma once

#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>

#include <3rdparty/nlohmann/json.hpp>

struct ComponentParameters
{
    typedef std::variant<int, double, std::string, std::vector<std::string>> Value;
    typedef std::vector<std::pair<std::string, Value>> Variables;

    void createParameter(const std::string &name, int value) {
        params.insert({name, std::make_shared<Parameter>(value, Parameter::INT)});
    }

    void createParameter(const std::string &name, double value) {
        params.insert({name, std::make_shared<Parameter>(value, Parameter::DOUBLE)});
    }

    void createParameter(const std::string &name, std::string &&value) {
        params.insert({name,  std::make_shared<Parameter>(value, Parameter::STRING)});
    }

    template<typename T>
    T getParameterValue(const std::string &name)
    {
        const auto it = params.find(name);
        if (it == params.end())
            throw std::runtime_error("No such variable");
        return std::get<T>(it->second->value);
    }

    void setParameter(const std::string &name, Value val)
    {
        const auto it = params.find(name);
        if (it == params.end())
            throw std::runtime_error("No such variable");
        if (it->second->value.index() != val.index())
            throw std::runtime_error("Incorrect variable type");
        it->second->value = val;
    }

    static std::string serialiseParameters(const ComponentParameters &params)
    {
        nlohmann::json json_result;

        for (auto &param : params.params)
        {
            nlohmann::json json_var;
            json_var["name"] = param.first;
            json_var["type"] = param.second->type;
            switch (param.second->type) {
                case Parameter::INT:
                    json_var["value"] = std::get<int>(param.second->value);
                    break;
                case Parameter::DOUBLE:
                    json_var["value"] = std::get<double>(param.second->value);
                    break;
                case Parameter::STRING:
                    json_var["value"] = std::get<std::string>(param.second->value);
                    break;
                case Parameter::LIST:
                    break;
            }

            json_result.push_back(json_var);
        }

        return json_result.dump();
    }

    static Variables deserialiseVariables(const std::string &from)
    {
        Variables result;

        nlohmann::json json_vars = nlohmann::json::parse(from);
        for (const auto& el : json_vars)
        {
            std::string name = el["name"].get<std::string>();
            Parameter::Type t = el["type"].get<Parameter::Type>();
            Value val;
            switch (t) {
                case Parameter::INT:
                    val = el["value"].get<int>();
                    break;
                case Parameter::DOUBLE:
                    val = el["value"].get<double>();
                    break;
                case Parameter::STRING:
                    val = el["value"].get<std::string>();
                    break;
                case Parameter::LIST:
                    break;
            }
            result.emplace_back(name, val);
        }

        return result;
    }
private:
    struct Parameter {
        enum Type {
            INT, DOUBLE, STRING, LIST
        };

        Parameter(Value val, Type t) : value(std::move(val)), type(t) {}

        Value value;
        Type type;
    };

    std::map<std::string, std::shared_ptr<Parameter>> params;
};
