#include "plugins/pocketpy_bridge.h"

#include "common/path_utils.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

namespace {

char* dup_for_c(const std::string& text) {
    char* out = static_cast<char*>(std::malloc(text.size() + 1));
    if (!out) return nullptr;
    std::memcpy(out, text.c_str(), text.size() + 1);
    return out;
}

void bridge_output(const char* text, void*) {
    std::cerr << (text ? text : "");
}

std::string generic_utf8(const fs::path& path) {
    return mcdk::path::to_utf8(path);
}

bool write_text(const fs::path& path, const std::string& text) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;
    out.write(text.data(), static_cast<std::streamsize>(text.size()));
    return out.good();
}

char* read_text_file_cb(const char* path, void*) {
    std::ifstream in(mcdk::path::from_utf8(path ? path : ""), std::ios::binary);
    if (!in.is_open()) return nullptr;
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (in.bad()) return nullptr;
    return dup_for_c(text);
}

bool write_text_file_cb(const char* path, const char* text, bool append, void*) {
    auto mode = std::ios::binary | std::ios::out;
    mode |= append ? std::ios::app : std::ios::trunc;
    std::ofstream out(mcdk::path::from_utf8(path ? path : ""), mode);
    if (!out.is_open()) return false;
    const std::string data = text ? text : "";
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return out.good();
}

char* fs_scandir_cb(const char* path, void*) {
    std::error_code ec;
    fs::path root = mcdk::path::from_utf8(path ? path : "");
    if (!fs::is_directory(root, ec) || ec) return dup_for_c("[]");

    nlohmann::json entries = nlohmann::json::array();
    for (const auto& entry : fs::directory_iterator(root, ec)) {
        if (ec) break;
        std::error_code item_ec;
        const bool is_file = entry.is_regular_file(item_ec) && !item_ec;
        item_ec.clear();
        const bool is_dir = entry.is_directory(item_ec) && !item_ec;
        entries.push_back({
            {"name", mcdk::path::filename_to_utf8(entry.path())},
            {"path", mcdk::path::to_utf8(entry.path())},
            {"is_file", is_file},
            {"is_dir", is_dir},
        });
    }
    std::sort(entries.begin(), entries.end(), [](const nlohmann::json& a, const nlohmann::json& b) {
        return a.value("name", std::string()) < b.value("name", std::string());
    });
    return dup_for_c(entries.dump());
}

bool fs_exists_cb(const char* path, void*) {
    std::error_code ec;
    return fs::exists(mcdk::path::from_utf8(path ? path : ""), ec) && !ec;
}

bool fs_is_file_cb(const char* path, void*) {
    std::error_code ec;
    return fs::is_regular_file(mcdk::path::from_utf8(path ? path : ""), ec) && !ec;
}

bool fs_is_dir_cb(const char* path, void*) {
    std::error_code ec;
    return fs::is_directory(mcdk::path::from_utf8(path ? path : ""), ec) && !ec;
}

} // namespace

int main() {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path root = fs::current_path() / mcdk::path::from_utf8("插件路径_测试_" + std::to_string(stamp));
    fs::path docs = root / mcdk::path::from_utf8("文档");

    try {
        fs::create_directories(docs);
    } catch (const std::exception& e) {
        std::cerr << "failed creating test dirs: " << e.what() << "\n";
        return 1;
    }

    const fs::path unicode_file = docs / mcdk::path::from_utf8("unicode_测试.txt");
    const fs::path entry = root / "main.py";
    const fs::path helper = root / "helper_unicode.py";

    if (!write_text(unicode_file, "hello utf8\n")) {
        std::cerr << "failed writing unicode test file: " << generic_utf8(unicode_file) << "\n";
        return 1;
    }
    if (!write_text(helper, "VALUE = 'helper ok'\n")) {
        std::cerr << "failed writing helper module: " << generic_utf8(helper) << "\n";
        return 1;
    }

    std::ostringstream script;
    script
        << "import os\n"
        << "import helper_unicode\n"
        << "import mcdk_assistant as mcdk\n"
        << "root = __plugin_dir__\n"
        << "docs = root + '/文档'\n"
        << "unicode_name = 'unicode_测试.txt'\n"
        << "unicode_path = docs + '/' + unicode_name\n"
        << "written_path = docs + '/写入_测试.txt'\n"
        << "assert helper_unicode.VALUE == 'helper ok', 'plugin local import failed'\n"
        << "assert 'main.py' in __file__, '__file__ was not populated: ' + str(__file__)\n"
        << "assert mcdk.fs.exists(root), 'mcdk.fs.exists(root) failed'\n"
        << "assert mcdk.fs.isdir(docs), 'mcdk.fs.isdir(docs) failed'\n"
        << "assert mcdk.fs.isfile(unicode_path), 'mcdk.fs.isfile(unicode_path) failed'\n"
        << "assert os.path.exists(unicode_path), 'os.path.exists(unicode_path) failed'\n"
        << "assert os.path.isfile(unicode_path), 'os.path.isfile(unicode_path) failed'\n"
        << "assert os.path.isdir(docs), 'os.path.isdir(docs) failed'\n"
        << "with open(unicode_path, 'r') as f:\n"
        << "    assert f.read() == 'hello utf8\\n', 'open read failed'\n"
        << "with open(written_path, 'w') as f:\n"
        << "    assert f.write('写入 ok') > 0\n"
        << "assert os.path.isfile(written_path), 'open write did not create file'\n"
        << "assert mcdk.fs.read_text(written_path) == '写入 ok', 'fs.read_text failed'\n"
        << "mcdk.fs.write_text(written_path, '+追加', True)\n"
        << "with open(written_path, 'r') as f:\n"
        << "    assert f.read() == '写入 ok+追加', 'append write failed'\n"
        << "names = os.listdir(docs)\n"
        << "assert unicode_name in names, 'os.listdir missed unicode file: ' + str(names)\n"
        << "entries = mcdk.fs.scandir(docs)\n"
        << "entry_names = [item['name'] for item in entries]\n"
        << "assert unicode_name in entry_names, 'bad scandir names: ' + str(entries)\n"
        << "assert '写入_测试.txt' in entry_names, 'scandir missed written file: ' + str(entries)\n"
        << "walk_rows = list(os.walk(root))\n"
        << "flat_files = []\n"
        << "for _root, _dirs, _files in walk_rows:\n"
        << "    for _name in _files:\n"
        << "        flat_files.append(_name)\n"
        << "assert unicode_name in flat_files, 'os.walk missed unicode file: ' + str(walk_rows)\n"
        << "assert '写入_测试.txt' in flat_files, 'os.walk missed written file: ' + str(walk_rows)\n"
        << "print('plugin fs smoke ok')\n";

    if (!write_text(entry, script.str())) {
        std::cerr << "failed writing plugin entry: " << generic_utf8(entry) << "\n";
        return 1;
    }

    mcdk_py_config cfg{};
    cfg.stdio_mode = true;
    cfg.output = bridge_output;
    cfg.read_text_file = read_text_file_cb;
    cfg.write_text_file = write_text_file_cb;
    cfg.fs_scandir = fs_scandir_cb;
    cfg.fs_exists = fs_exists_cb;
    cfg.fs_is_file = fs_is_file_cb;
    cfg.fs_is_dir = fs_is_dir_cb;

    char* error = nullptr;
    if (!mcdk_py_initialize(&cfg, &error)) {
        std::cerr << "mcdk_py_initialize failed: " << (error ? error : "unknown") << "\n";
        mcdk_py_free(error);
        return 1;
    }

    const std::string root_utf8 = generic_utf8(root);
    const std::string entry_utf8 = generic_utf8(entry);
    bool ok = mcdk_py_load_plugin("plugin_fs_smoke", root_utf8.c_str(), entry_utf8.c_str(), &error);
    mcdk_py_finalize();

    try {
        fs::remove_all(root);
    } catch (...) {
    }

    if (!ok) {
        std::cerr << "mcdk_py_load_plugin failed: " << (error ? error : "unknown") << "\n";
        mcdk_py_free(error);
        return 1;
    }

    std::cout << "plugin_fs_smoke_test passed\n";
    return 0;
}
