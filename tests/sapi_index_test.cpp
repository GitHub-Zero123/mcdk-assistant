#include "sapi/sapi_index.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace {

const mcdk::sapi::SapiSymbol* find_symbol(const mcdk::sapi::SapiIndex& index,
                                          const std::string& fqname) {
    for (const auto& symbol : index.symbols) {
        if (symbol.fqname == fqname) return &symbol;
    }
    return nullptr;
}

const mcdk::sapi::SapiMember* find_member(const mcdk::sapi::SapiSymbol& symbol,
                                          const std::string& name) {
    for (const auto& member : symbol.members) {
        if (member.name == name) return &member;
    }
    return nullptr;
}

bool require(bool ok, const std::string& message) {
    if (!ok) std::cerr << "[sapi_index_test] FAIL: " << message << std::endl;
    return ok;
}

} // namespace

int main() {
    fs::path sapi_root = fs::path(MCDK_WORKSPACE_DIR) / "knowledge" / "SAPI";
    std::cerr << "[sapi_index_test] SAPI root: " << sapi_root.string() << std::endl;

    auto index = mcdk::sapi::build_sapi_index(sapi_root);
    std::cerr << "[sapi_index_test] files: " << index.files_scanned << std::endl;
    std::cerr << "[sapi_index_test] modules: " << index.modules.size() << std::endl;
    std::cerr << "[sapi_index_test] symbols: " << index.symbols.size() << std::endl;
    std::cerr << "[sapi_index_test] parse error files: " << index.parse_error_files
              << ", error nodes: " << index.error_nodes
              << ", missing nodes: " << index.missing_nodes << std::endl;

    bool ok = true;
    ok &= require(index.files_scanned >= 10, "expected SAPI .d.ts files");
    ok &= require(index.modules.size() >= 10, "expected SAPI modules");
    ok &= require(index.symbols.size() >= 100, "expected many SAPI symbols");

    const auto* secret = find_symbol(index, "@minecraft/server-admin.SecretString");
    ok &= require(secret != nullptr, "missing @minecraft/server-admin.SecretString");
    if (secret) {
        ok &= require(secret->kind == "class", "SecretString should be class");
        ok &= require(secret->doc.find("secret string") != std::string::npos ||
                      secret->doc.find("Secret") != std::string::npos,
                      "SecretString should keep JSDoc");
    }

    const auto* dimension = find_symbol(index, "@minecraft/server.Dimension");
    ok &= require(dimension != nullptr, "missing @minecraft/server.Dimension");
    if (dimension) {
        const auto* spawn = find_member(*dimension, "spawnEntity");
        ok &= require(spawn != nullptr, "missing Dimension.spawnEntity");
        if (spawn) {
            ok &= require(spawn->signature.find("spawnEntity") != std::string::npos,
                          "spawnEntity signature missing name");
            ok &= require(spawn->doc.find("@remarks") != std::string::npos ||
                          spawn->doc.find("Creates") != std::string::npos ||
                          spawn->doc.find("entity") != std::string::npos,
                          "spawnEntity should keep JSDoc");
        }
    }

    const auto* blocks = find_symbol(index, "@minecraft/vanilla-data/mojang-block.MinecraftBlockTypes");
    ok &= require(blocks != nullptr, "missing vanilla-data MinecraftBlockTypes");
    if (blocks) {
        ok &= require(blocks->kind == "enum", "MinecraftBlockTypes should be enum");
    }

    auto fuzzy_spawn = mcdk::sapi::search_sapi_symbols(index, "spawn entity", 8, 1);
    bool found_spawn = false;
    for (const auto& result : fuzzy_spawn) {
        const auto& symbol = index.symbols[result.symbol_id];
        if (result.member_index >= 0 &&
            symbol.name == "Dimension" &&
            symbol.members[result.member_index].name == "spawnEntity") {
            found_spawn = true;
            ok &= require(!result.referenced_symbols.empty(), "spawnEntity search should carry refs");
            break;
        }
    }
    ok &= require(found_spawn, "fuzzy search should find Dimension.spawnEntity for 'spawn entity'");

    auto fuzzy_secret = mcdk::sapi::search_sapi_symbols(index, "secret string", 8, 1);
    bool found_secret = false;
    for (const auto& result : fuzzy_secret) {
        const auto& symbol = index.symbols[result.symbol_id];
        if (symbol.fqname == "@minecraft/server-admin.SecretString") {
            found_secret = true;
            break;
        }
    }
    ok &= require(found_secret, "fuzzy search should find SecretString for 'secret string'");

    auto lookup_player = mcdk::sapi::lookup_sapi_symbol(index, "Player", 1);
    ok &= require(!lookup_player.empty(), "symbol lookup should find Player");
    bool lookup_has_player = false;
    for (const auto& result : lookup_player) {
        if (index.symbols[result.symbol_id].fqname == "@minecraft/server.Player") {
            lookup_has_player = true;
            break;
        }
    }
    ok &= require(lookup_has_player, "symbol lookup should include @minecraft/server.Player");

    fs::path cache_path = fs::path(MCDK_WORKSPACE_DIR) / ".codex_tmp" / "sapi_index_test.bin";
    fs::create_directories(cache_path.parent_path());
    std::string error;
    ok &= require(mcdk::sapi::save_sapi_cache(cache_path, index, &error),
                  "failed to save SAPI cache: " + error);

    mcdk::sapi::SapiIndex loaded;
    ok &= require(mcdk::sapi::load_sapi_cache(cache_path, loaded, &error),
                  "failed to load SAPI cache: " + error);
    ok &= require(loaded.modules.size() == index.modules.size(), "cache module count mismatch");
    ok &= require(loaded.symbols.size() == index.symbols.size(), "cache symbol count mismatch");
    ok &= require(find_symbol(loaded, "@minecraft/server-admin.SecretString") != nullptr,
                  "cache missing SecretString");

    if (!ok) return 1;
    std::cerr << "[sapi_index_test] PASS" << std::endl;
    return 0;
}
