#pragma once

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <functional>

struct Command {
    std::string m_name;
    std::vector<std::string> parameters;
    std::function<void(const std::map<std::string, std::string> &)> callback;
    std::vector<Command *> children;

    Command *createSubCommand(const std::string &cmd) {
        auto command = new Command(cmd);
        children.push_back(command);
        return command;
    }

    Command *addParameter(const std::string &param) {
        parameters.push_back(param);
        return this;
    }

    Command *addAction(std::function<void(const std::map<std::string, std::string> &)> action) {
        callback = std::move(action);
        return this;
    }

    Command *lookForSubCommand(const std::string &name) {
        for (const auto &child : children) {
            if (child->m_name == name) {
                return child;
            }
        }
        return nullptr;
    }

    [[nodiscard]] const std::vector<std::string> &needParameterList() const {
        return parameters;
    }
};

class ArgParser : public Command {
public:
    ArgParser() = default;
    ~ArgParser() = default;

    void parse(int argc, char *argv[]) {
        m_name = argv[0];

        std::map<std::string, std::string> collected_args;

        int cur = 1;
        Command *command = this;

        while (cur < argc) {
            auto list_params = command->needParameterList();
            for (const auto &param : list_params) {
                if (cur < argc) {
                    collected_args.insert({param, argv[cur]});
                    cur++;
                }
                else {
                    // TODO error
                }
            }

            if (command->callback) {
                command->callback(collected_args);
                break;
            }

            command = command->lookForSubCommand(argv[cur]);
            cur++;

            if (!command)
                break; // TODO error
        }
        if (command && command->callback) {
            command->callback(collected_args);
        }
    }
};
