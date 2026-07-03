#include "plugins/pocketpy_bridge.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {

void bridge_output(const char* text, void*) {
    std::cerr << (text ? text : "");
}

std::string generic_utf8(const fs::path& path) {
#if defined(_WIN32)
    const auto bytes = path.generic_u8string();
    return std::string(bytes.begin(), bytes.end());
#else
    return path.generic_string();
#endif
}

bool write_text(const fs::path& path, const std::string& text) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;
    out.write(text.data(), static_cast<std::streamsize>(text.size()));
    return out.good();
}

} // namespace

int main() {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path root = fs::current_path() / ("plugin_fs_smoke_" + std::to_string(stamp));
    fs::path docs = root / "docs";

    try {
        fs::create_directories(docs);
    } catch (const std::exception& e) {
        std::cerr << "failed creating test dirs: " << e.what() << "\n";
        return 1;
    }

    const fs::path unicode_file = docs / fs::u8path(u8"unicode_测试.txt");
    const fs::path entry = root / "main.py";

    if (!write_text(unicode_file, "hello utf8\n")) {
        std::cerr << "failed writing unicode test file: " << generic_utf8(unicode_file) << "\n";
        return 1;
    }

    std::ostringstream script;
    script
        << "import os\n"
        << "import mcdk_assistant as mcdk\n"
        << "root = __plugin_dir__\n"
        << "docs = root + '/docs'\n"
        << "unicode_name = 'unicode_测试.txt'\n"
        << "unicode_path = docs + '/' + unicode_name\n"
        << "assert mcdk.fs.exists(root), 'mcdk.fs.exists(root) failed'\n"
        << "assert mcdk.fs.isdir(docs), 'mcdk.fs.isdir(docs) failed'\n"
        << "assert mcdk.fs.isfile(unicode_path), 'mcdk.fs.isfile(unicode_path) failed'\n"
        << "assert os.path.exists(unicode_path), 'os.path.exists(unicode_path) failed'\n"
        << "assert os.path.isfile(unicode_path), 'os.path.isfile(unicode_path) failed'\n"
        << "assert os.path.isdir(docs), 'os.path.isdir(docs) failed'\n"
        << "names = os.listdir(docs)\n"
        << "assert unicode_name in names, 'os.listdir missed unicode file: ' + str(names)\n"
        << "entries = mcdk.fs.scandir(docs)\n"
        << "assert len(entries) == 1, 'unexpected scandir size: ' + str(entries)\n"
        << "assert entries[0]['name'] == unicode_name, 'bad scandir name: ' + str(entries)\n"
        << "assert entries[0]['is_file'], 'scandir did not mark file'\n"
        << "walk_rows = list(os.walk(root))\n"
        << "flat_files = []\n"
        << "for _root, _dirs, _files in walk_rows:\n"
        << "    for _name in _files:\n"
        << "        flat_files.append(_name)\n"
        << "assert unicode_name in flat_files, 'os.walk missed unicode file: ' + str(walk_rows)\n"
        << "print('plugin fs smoke ok')\n";

    if (!write_text(entry, script.str())) {
        std::cerr << "failed writing plugin entry: " << generic_utf8(entry) << "\n";
        return 1;
    }

    mcdk_py_config cfg{};
    cfg.stdio_mode = true;
    cfg.output = bridge_output;

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
