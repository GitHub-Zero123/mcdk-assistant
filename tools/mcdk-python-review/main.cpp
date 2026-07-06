// mcdk-python-review — 独立 CLI（plan §4.2）。
//
// 用法:
//   mcdk-python-review <behavior_pack_path> [--scope a,b/c] [--format markdown|json]
//                      [--include-third-party] [--output <file>]
//
// 行为包路径为完整 UTF-8 路径；Windows 下用宽字符命令行还原，避免中文路径按 GBK 乱码。
#include "python_review/review_analyzer.hpp"
#include "common/path_utils.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h> // CommandLineToArgvW（WIN32_LEAN_AND_MEAN 下需显式包含）
#endif

namespace {

std::vector<std::string> split_csv(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == ',') { if (!cur.empty()) out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

void print_help() {
    std::cout <<
        "mcdk-python-review — Python AI 代码审查（行为包）\n"
        "用法: mcdk-python-review <behavior_pack_path> [选项]\n"
        "  <behavior_pack_path>       必填，行为包根目录完整 UTF-8 路径\n"
        "  --scope <a,b/c>            选填，缩小到指定包/模块；不填=全部\n"
        "  --format <markdown|json>   输出格式，默认 markdown（json 为闭环主格式）\n"
        "  --include-third-party      连 QuModLibs 等三方库一并审查（默认排除）\n"
        "  --output <file>            写报告到文件（UTF-8）；不传则打印到 stdout\n"
        "  --help                     显示本帮助\n";
}

#ifdef _WIN32
std::vector<std::string> get_utf8_args() {
    std::vector<std::string> args;
    int n = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &n);
    if (!wargv) return args;
    for (int i = 0; i < n; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string s(len > 0 ? len - 1 : 0, '\0');
        if (len > 0) WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, s.data(), len, nullptr, nullptr);
        args.push_back(std::move(s));
    }
    LocalFree(wargv);
    return args;
}
#endif

} // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    std::vector<std::string> args = get_utf8_args();
#else
    std::vector<std::string> args(argv, argv + argc);
#endif

    mcdk::python_review::ReviewOptions options;
    std::string path_text;
    std::string output_file;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& a = args[i];
        auto next = [&](const char* name) -> std::string {
            if (i + 1 < args.size()) return args[++i];
            std::cerr << "缺少 " << name << " 的值\n";
            return "";
        };
        if (a == "--help" || a == "-h") { print_help(); return 0; }
        else if (a == "--scope" || a == "-s") options.scope = split_csv(next("--scope"));
        else if (a == "--format" || a == "-f") options.format = next("--format");
        else if (a == "--include-third-party") options.include_third_party = true;
        else if (a == "--output" || a == "-o") output_file = next("--output");
        else if (!a.empty() && a[0] == '-') { std::cerr << "未知选项: " << a << "\n"; }
        else if (path_text.empty()) path_text = a;
    }

    if (path_text.empty()) {
        std::cerr << "缺少行为包路径。\n\n";
        print_help();
        return 2;
    }
    if (options.format != "markdown" && options.format != "json") {
        std::cerr << "未知 --format: " << options.format << "（应为 markdown|json）\n";
        return 2;
    }

    mcdk::python_review::ReviewAnalyzer analyzer;
    std::string out = analyzer.review_formatted(mcdk::path::from_utf8(path_text), options);

    if (output_file.empty()) {
        std::cout << out << std::endl;
    } else {
        std::ofstream ofs(mcdk::path::from_utf8(output_file), std::ios::binary);
        if (!ofs) { std::cerr << "无法写入: " << output_file << "\n"; return 1; }
        ofs << out;
    }
    return 0;
}
