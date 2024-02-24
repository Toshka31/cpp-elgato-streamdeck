#include <string>
#include <unordered_map>
#include <utility>

// config things

#define COMPONENT_SETTINGS_EMPTY ;
#define COMPONENT_SETTINGS_START = {
#define COMPONENT_SETTINGS_END };

// Macro for adding component settings variables
#define ADD_INT_VARIABLE(name, value, min, max) \
    std::make_pair(name, std::make_shared<ConfigVarInt>(name, value, min, max)),
#define ADD_DOUBLE_VARIABLE(name, value, min, max) \
    std::make_pair(name, std::make_shared<ConfigVarDouble>(name, value, min, max)),
#define ADD_STRING_VARIABLE(name, value, max_size) \
    std::make_pair(name, std::make_shared<ConfigVarString>(name, value, max_size)),
#define ADD_BOOL_VARIABLE(name, value) \
    std::make_pair(name, std::make_shared<ConfigVarBool>(name, value)),
#define ADD_COMBO_VARIABLE(name, value, variants) \
    std::make_pair(name, std::make_shared<ConfigVarCombo>(name, value, std::unordered_map<int, std::string>variants)),

#define COMBO_VARIANT(index, value) std::make_pair(index, value),

// helper macro for get variables values
// return std::any with contained value or empty std::any if variable doesn't exist
#define GET_CONFIG_VALUE(variable_name) getVariableValue(variable_name)

// helper macro for set variable value
// just a wrapper for public setVariable function
// do nothing if variable doesn't exist of invalid type in std::any
#define SET_CONFIG_VALUE(variable_name, value) setVariable(variable_name, value)

// Change min and max restrictions for int variable
#define CHANGE_INT_VARIABLE_RESTRICTIONS(variable_name, min, max) \
    CHANGE_NUMERIC_VARIABLE_RESTRICTIONS(ConfigVarInt, variable_name, min, max)

// Change min and max restrictions for double variable
#define CHANGE_DOUBLE_VARIABLE_RESTRICTIONS(variable_name, min, max) \
    CHANGE_NUMERIC_VARIABLE_RESTRICTIONS(ConfigVarDouble, variable_name, min, max)

// This macro providing possibility to dynamically add new variant to combo variable
#define ADD_COMBO_VARIABLE_VARIANT(variable_name, index, value) {\
    auto it = m_config_variables.find(variable_name); \
    if (it != m_config_variables.end()) { \
        auto downcasted_var = std::dynamic_pointer_cast<ConfigVarCombo>(it->second); \
        if (downcasted_var && !downcasted_var->m_list.contains(index)) {\
            downcasted_var->m_list.emplace(index, value); \
        } \
    } \
}

// private section

// helper for numeric variables restriction changes
#define CHANGE_NUMERIC_VARIABLE_RESTRICTIONS(type, variable_name, min, max) { \
    auto it = m_config_variables.find(#variable_name); \
    if (it != m_config_variables.end()) { \
        auto downcasted_var = std::dynamic_pointer_cast<type>(it->second); \
        if (downcasted_var) { \
            downcasted_var->m_min = min; \
            downcasted_var->m_max = max; \
        } \
    } \
}

#define COMPONENT_SETTINGS_SECTION \
  public: \
  std::vector<std::string> getVariablesList() const final {   \
    std::vector<std::string> result; \
    for (const auto &var : m_config_variables) { \
      result.push_back(var.second->getInfo()); \
    } \
    return result; \
  } \
  void setVariable(const std::string &name, std::any value) final { \
    const auto it = m_config_variables.find(name); \
    if (it != m_config_variables.end()) { \
      it->second->set(std::move(value)); \
    } \
  } \
  private:                         \
  std::any getVariableValue(const std::string &variable_name) { \
    auto it = m_config_variables.find(variable_name); \
    if (it == m_config_variables.end()) return {}; \
    return it->second->getValue(); \
  } \
  const std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> m_config_variables \

struct ConfigVarBase {
    explicit ConfigVarBase(std::string name) : m_name(std::move(name)) {}

    // Return all info about variable in JSON format
    [[nodiscard]] virtual std::string getInfo() const = 0;

    [[nodiscard]] virtual std::any getValue() const = 0;

    virtual void set(std::any value) = 0;

    std::string m_name;
};

struct ConfigVarInt : ConfigVarBase {
    ConfigVarInt(std::string name, int value, int min, int max) :
            ConfigVarBase(std::move(name)),
            m_value(value),
            m_min(min),
            m_max(max) {}

    int m_value{};
    int m_max{};
    int m_min{};

    void set(std::any value) final {
        try {
            auto val = std::any_cast<int>(value);
            if (val >= m_min && val <= m_max)
                m_value = val;
        }
        catch (const std::bad_any_cast& e) {}
    }

    [[nodiscard]] std::string getInfo() const final {
        std::string result = "{";
        result += R"("type": "String",)";
        result += R"("name": ")" + m_name + R"(",)";
        result += R"("value": ")" + std::to_string(m_value) + R"(",)";
        result += R"("min": ")" + std::to_string(m_min) + R"(",)";
        result += R"("max": ")" + std::to_string(m_max) + R"(")";
        result += "}";
        return result;
    }

    [[nodiscard]] std::any getValue() const final {
        return m_value;
    }
};

struct ConfigVarDouble : ConfigVarBase {
    ConfigVarDouble(std::string name, double value, double min, double max) :
            ConfigVarBase(std::move(name)),
            m_value(value),
            m_min(min),
            m_max(max) {}

    double m_value{};
    double m_max{};
    double m_min{};

    void set(std::any value) final {
        try {
            auto val = std::any_cast<double>(value);
            if (val >= m_min && val <= m_max)
                m_value = val;
        }
        catch (const std::bad_any_cast& e) {}
    }

    [[nodiscard]] std::string getInfo() const final {
        std::string result = "{";
        result += R"("type": "String",)";
        result += R"("name": ")" + m_name + R"(",)";
        result += R"("value": ")" + std::to_string(m_value) + R"(",)";
        result += R"("min": ")" + std::to_string(m_min) + R"(",)";
        result += R"("max": ")" + std::to_string(m_max) + R"(")";
        result += "}";
        return result;
    }

    [[nodiscard]] std::any getValue() const final {
        return m_value;
    }
};

struct ConfigVarString : ConfigVarBase {
    ConfigVarString(std::string name, std::string  value, std::string::size_type max_size) :
            ConfigVarBase(std::move(name)),
            m_value(std::move(value)),
            m_max_size(max_size) {}


    std::string m_value;
    std::string::size_type m_max_size;

    void set(std::any value) final {
        try {
            auto val = std::any_cast<std::string>(value);
            if (m_max_size == 0 || val.size() <= m_max_size)
                m_value = val;
        }
        catch (const std::bad_any_cast& e) {}
    }

    [[nodiscard]] std::string getInfo() const final {
        std::string result = "{";
        result += R"("type": "String",)";
        result += R"("name": ")" + m_name + R"(",)";
        result += R"("value": ")" + m_value + R"(",)";
        result += R"("max_size": ")" + std::to_string(m_max_size) + R"(")";
        result += "}";
        return result;
    }

    [[nodiscard]] std::any getValue() const final {
        return m_value;
    }
};

struct ConfigVarBool : ConfigVarBase {
    explicit ConfigVarBool(std::string name, bool value) :
            ConfigVarBase(std::move(name)),
            m_value(value) {}

    bool m_value{};

    void set(std::any value) final {
        try {
            auto val = std::any_cast<bool>(value);
            m_value = val;
        }
        catch (const std::bad_any_cast& e) {}
    }

    [[nodiscard]] std::string getInfo() const final {
        std::string result = "{";
        result += R"("type": "Bool",)";
        result += R"("name": ")" + m_name + R"(",)";
        result += R"("value": ")" + std::to_string(m_value) + R"(")";
        result += "}";
        return result;
    }

    [[nodiscard]] std::any getValue() const final {
        return m_value;
    }
};

struct ConfigVarCombo : ConfigVarBase {
    ConfigVarCombo(std::string name, int value, std::unordered_map<int, std::string> list) :
            ConfigVarBase(std::move(name)),
            m_value(value),
            m_list(std::move(list)) {}

    int m_value;
    std::unordered_map<int, std::string> m_list;

    void set(std::any value) final {
        try {
            auto val = std::any_cast<int>(value);
            if (m_list.contains(val))
                m_value = val;
        }
        catch (const std::bad_any_cast& e) {}
    }

    [[nodiscard]] std::string getInfo() const final {
        std::string result = "{";
        result += R"("type": "Combo",)";
        result += R"("name": ")" + m_name + R"(",)";
        result += R"("value": ")" + std::to_string(m_value) + R"(",)";
        result += R"("variants": [)";
        for (const auto &variant : m_list) {
            result += R"({")" + std::to_string(variant.first) + R"(": )";
            result += R"(")" + variant.second + R"("},)";
        }
        if (!m_list.empty()) result.pop_back();
        result += "]";
        result += "}";
        return result;
    }

    [[nodiscard]] std::any getValue() const final {
        return m_value;
    }
};