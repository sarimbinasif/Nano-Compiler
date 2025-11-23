#pragma once
#include <map>
#include <vector>
#include <string>
#include "../data/token-type.h"

// Struct to hold info about a declared variable
struct SymbolInfo {
    TokenType type; // TYPE_INT, TYPE_FLOAT, etc.
    bool initialized;
};

class SymbolTable {
private:
    // A stack of scopes. Each scope is a map of Name -> Info
    std::vector<std::map<std::string, SymbolInfo>> scopes;

public:
    SymbolTable() {
        // Start with a global scope
        beginScope();
    }

    void beginScope() {
        scopes.push_back(std::map<std::string, SymbolInfo>());
    }

    void endScope() {
        if (!scopes.empty()) scopes.pop_back();
    }

    bool declare(std::string name, TokenType type) {
        if (scopes.empty()) return false;
        // Check if already declared in *current* scope
        if (scopes.back().find(name) != scopes.back().end()) {
            return false; // Already exists
        }
        scopes.back()[name] = {type, true};
        return true;
    }

    // Look up a variable in any scope, starting from innermost
    bool get(std::string name, SymbolInfo& outInfo) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                outInfo = found->second;
                return true;
            }
        }
        return false;
    }
};