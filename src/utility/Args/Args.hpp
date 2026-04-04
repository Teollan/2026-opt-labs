#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Args {
private:
    struct StringArg {
        std::string shortName;
        bool required;
        std::string value;
        bool provided = false;
    };

    struct FlagArg {
        std::string shortName;
        bool value = false;
    };

    std::string _program;
    std::vector<std::string> _argv;

    std::unordered_map<std::string, StringArg> _strings;
    std::unordered_map<std::string, FlagArg> _flags;
    std::unordered_map<std::string, std::string> _aliases;

    void printUsage() const;

public:
    Args(int argc, char* argv[]);

    Args& expectString(
        const std::string& name,
        const std::string& shortName,
        bool required = false
    );
    Args& expectFlag(
        const std::string& name,
        const std::string& shortName
    );

    Args& parse();

    [[nodiscard]] const std::string& getString(const std::string& name) const;
    [[nodiscard]] bool getFlag(const std::string& name) const;
};
