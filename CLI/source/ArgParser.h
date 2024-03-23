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

    explicit Command(std::string name)
        : m_name(std::move(name))
    {}

    ~Command() {
        for (auto cmd : children)
            delete cmd;
    }

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

    void printInfo(int tab) {
        for (int i = 0; i < tab; ++i)
            std::cout << "  ";
        std:: cout << m_name;
        for (const auto &param : parameters)
            std::cout << " " << "[" << param << "]";
        for (const auto &cmd : children) {
            std::cout << std::endl;
            cmd->printInfo(tab+1);
        }
    }
};

class ArgParser : public Command {
public:
    explicit ArgParser(const std::string &name) : Command(name) {};
    ~ArgParser() = default;

    bool parse(int argc, char *argv[]) {
        std::map<std::string, std::string> collected_args;

        int cur = 1;
        Command *command = this;

        while (cur < argc) {
            auto list_params = command->needParameterList();
            for (const auto &param : list_params) {
                if (cur < argc) {
                    collected_args.insert({ param, argv[cur] });
                    cur++;
                } else {
                    return false;
                }
            }

            if (command->callback) {
                break;
            }

            command = command->lookForSubCommand(argv[cur]);
            cur++;

            if (!command)
                return false;
        }
        if (command && command->callback) {
            command->callback(collected_args);
        }
        delete command;
        return true;
    }

    void printHelp() {
        printInfo(0);
    }
};
