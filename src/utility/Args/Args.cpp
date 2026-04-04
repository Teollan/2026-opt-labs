#include "Args.hpp"

#include <format>
#include <iostream>
#include <stdexcept>

Args::Args(int argc, char* argv[]) : _program(argv[0]) {
    for (int i = 1; i < argc; i++) {
        _argv.emplace_back(argv[i]);
    }
}

Args& Args::expectString(
    const std::string& name,
    const std::string& shortName,
    bool required
) {
    _strings[name] = {shortName, required, "", false};
    _aliases["--" + name] = name;
    _aliases["-" + shortName] = name;
    return *this;
}

Args& Args::expectFlag(
    const std::string& name,
    const std::string& shortName
) {
    _flags[name] = {shortName, false};
    _aliases["--" + name] = name;
    _aliases["-" + shortName] = name;
    return *this;
}

Args& Args::parse() {
    for (size_t i = 0; i < _argv.size(); i++) {
        const auto& arg = _argv[i];

        auto alias = _aliases.find(arg);

        if (alias == _aliases.end()) {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage();
            exit(1);
        }

        const auto& name = alias->second;

        auto stringArg = _strings.find(name);
        if (stringArg != _strings.end()) {
            if (i + 1 >= _argv.size()) {
                std::cerr << "Missing value for: " << arg << "\n";
                printUsage();
                exit(1);
            }

            stringArg->second.value = _argv[++i];
            stringArg->second.provided = true;
            continue;
        }

        auto flagArg = _flags.find(name);
        if (flagArg != _flags.end()) {
            flagArg->second.value = true;
        }
    }

    for (const auto& [name, arg] : _strings) {
        if (arg.required && !arg.provided) {
            std::cerr << "Missing required argument: --" << name << "\n";
            printUsage();
            exit(1);
        }
    }

    return *this;
}

void Args::printUsage() const {
    std::cerr << "\nUsage: " << _program;

    for (const auto& [name, arg] : _strings) {
        if (arg.required) {
            std::cerr << " --" << name << " <value>";
        }
    }

    std::cerr << " [options]\n\nOptions:\n";

    for (const auto& [name, arg] : _strings) {
        std::cerr << std::format(
            "  --{}, -{} <value>{}\n", name, arg.shortName,
            arg.required ? " (required)" : ""
        );
    }

    for (const auto& [name, arg] : _flags) {
        std::cerr << std::format("  --{}, -{}\n", name, arg.shortName);
    }
}

const std::string& Args::getString(const std::string& name) const {
    auto it = _strings.find(name);

    if (it == _strings.end()) {
        throw std::runtime_error("Unknown string argument: " + name);
    }

    return it->second.value;
}

bool Args::getFlag(const std::string& name) const {
    auto it = _flags.find(name);

    if (it == _flags.end()) {
        throw std::runtime_error("Unknown flag argument: " + name);
    }

    return it->second.value;
}
