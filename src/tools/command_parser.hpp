#pragma once
// Small shell-like command parser used by merged MCP tools.
//
// It intentionally has no MCP or business dependencies.  The public surface is:
//   tokenize_command_line("wiki \"custom food\" --top 5")
//   parse_command("/assets stair --rp --top 5")
//   flag_str / flag_int / has_flag
#include <cctype>
#include <map>
#include <string>
#include <vector>

namespace mcdk {

namespace command_parser_detail {

inline bool is_ascii_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline bool is_flag_token(const std::string& token) {
    if (token.size() < 2 || token[0] != '-') return false;
    // Negative numbers such as -1 or -.5 are positional values, not flags.
    unsigned char next = static_cast<unsigned char>(token[1]);
    return !(std::isdigit(next) || token[1] == '.');
}

inline std::string to_lower_ascii(std::string s) {
    for (auto& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

inline std::string normalize_flag_key(const std::string& key) {
    if (key == "s") return "scope";
    if (key == "k") return "top";
    if (key == "a") return "assets";
    if (key == "n") return "start";   // reserved for line-start aliases
    return key;
}

inline void append_positional(std::string& positional, const std::string& value) {
    if (!positional.empty()) positional.push_back(' ');
    positional += value;
}

} // namespace command_parser_detail

inline std::vector<std::string> tokenize_command_line(const std::string& line) {
    std::vector<std::string> tokens;
    std::string cur;
    bool in_token = false;
    char quote = 0;

    auto flush = [&]() {
        if (in_token) {
            tokens.push_back(cur);
            cur.clear();
            in_token = false;
        }
    };

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '\\') {
            in_token = true;
            if (i + 1 < line.size()) {
                cur.push_back(line[i + 1]);
                ++i;
            } else {
                cur.push_back('\\');
            }
            continue;
        }

        if (quote) {
            if (c == quote) {
                quote = 0;
            } else {
                cur.push_back(c);
            }
            continue;
        }

        if (c == '"' || c == '\'') {
            quote = c;
            in_token = true;         // Allow empty quoted arguments.
            continue;
        }

        if (command_parser_detail::is_ascii_space(c)) {
            flush();
            continue;
        }

        in_token = true;
        cur.push_back(c);
    }
    flush();
    return tokens;
}

struct ParsedCommand {
    std::string                        sub;        // command, lowercased and without leading '/'
    std::string                        positional; // positional args joined with a single space
    std::map<std::string, std::string> flags;      // normalized long key -> value
};

inline ParsedCommand parse_command(const std::string& line) {
    using namespace command_parser_detail;

    ParsedCommand pc;
    auto tokens = tokenize_command_line(line);
    if (tokens.empty()) return pc;

    std::string sub = tokens[0];
    if (!sub.empty() && sub.front() == '/') sub.erase(sub.begin());
    pc.sub = to_lower_ascii(sub);

    std::string positional;
    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& tok = tokens[i];

        if (is_flag_token(tok)) {
            size_t start = (tok.size() >= 2 && tok[1] == '-') ? 2 : 1;
            std::string body = tok.substr(start);

            std::string key, value;
            auto eq = body.find('=');
            if (eq != std::string::npos) {
                key = body.substr(0, eq);
                value = body.substr(eq + 1);
            } else {
                key = body;
                if (i + 1 < tokens.size() && !is_flag_token(tokens[i + 1])) {
                    value = tokens[i + 1];
                    ++i;
                }
            }
            if (key.empty()) continue;
            pc.flags[normalize_flag_key(to_lower_ascii(key))] = value;
        } else {
            append_positional(positional, tok);
        }
    }
    pc.positional = positional;
    return pc;
}

inline std::string flag_str(const ParsedCommand& pc, const std::string& key,
                            const std::string& def = "") {
    auto it = pc.flags.find(key);
    return it != pc.flags.end() ? it->second : def;
}

inline bool has_flag(const ParsedCommand& pc, const std::string& key) {
    return pc.flags.find(key) != pc.flags.end();
}

inline int flag_int(const ParsedCommand& pc, const std::string& key, int def) {
    auto it = pc.flags.find(key);
    if (it == pc.flags.end() || it->second.empty()) return def;
    try {
        size_t pos = 0;
        int v = std::stoi(it->second, &pos);
        return pos == it->second.size() ? v : def;
    } catch (...) {
        return def;
    }
}

} // namespace mcdk
