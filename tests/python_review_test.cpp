#include "python_review/review_analyzer.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

namespace {

bool write_file(const fs::path& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary);
    out << content;
    return out.good();
}

} // namespace

int main() {
    const fs::path root = fs::temp_directory_path() / "mcdk_python_review_unicode_test";
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root, ec);
    if (ec || !write_file(root / "sample.py",
                          "a = unicode(value)\n"
                          "b = unicode(value, 'utf-8')\n"
                          "c = unicode()\n"
                          "d = codec.unicode(value)\n"
                          "import os, json\n"
                          "import sys as runtime_sys\n"
                          "from os.path import join\n"
                          "from sys import version\n"
                          "import safe.os\n"
                          "from . import os\n"
                          "import importlib\n"
                          "from imp import load_module\n"
                          "import subprocess\n"
                          "import json\n"
                          "loaded = __import__(module_name)\n"
                          "result = eval(expression)\n"
                          "execfile(path)\n"
                          "exec code in globals_dict\n"
                          "loader = getattr(handler, '__globals__')['__builtins__']['__import__'](module_name)\n"
                          "os_module = handler.func_globals['__builtins__']['__import__']('os')\n"
                          "value = getattr(handler, '__globals__')['__builtins__']['eval'](expression)\n"
                          "internal = getattr(handler, '__globals__')['internal_api']()\n"
                          "cached_import = getattr(handler, '__globals__')['__builtins__']['__import__']\n"
                          "reloaded = getattr(handler, '__globals__')['__builtins__']['reload'](module)\n"
                          "markers = ('__globals__', '__builtins__', '__import__')\n"
                          "metadata = getattr(handler, '__globals__')['__builtins__']['len']\n"
                          "nested = getattr(getattr(handler, '__globals__')['__builtins__'], '__import__')(module_name)\n"
                          "attribute_loader = getattr(handler, '__globals__')['__builtins__'].__import__(module_name)\n"
                          "obfuscated = getattr(handler, '__globals__')['__builtins__']['__im' + 'port__'](module_name)\n"
                          "missing_builtins = getattr(handler, '__globals__')['__import__'](module_name)\n"
                          "missing_globals = __builtins__['__import__'](module_name)\n"
                          "def split_import(module_name):\n"
                          "    scope = getattr(handler, '__globals__')\n"
                          "    noise = harmless_call()\n"
                          "    builtins_map = scope['__builtins__']\n"
                          "    alias = builtins_map\n"
                          "    loader = alias['__import__']\n"
                          "    return loader(module_name)\n"
                          "def split_call(expression):\n"
                          "    scope = handler.func_globals\n"
                          "    if condition:\n"
                          "        harmless_call()\n"
                          "    builtins_map = scope['__builtins__']\n"
                          "    return builtins_map['eval'](expression)\n"
                          "def reset_chain(module_name):\n"
                          "    scope = getattr(handler, '__globals__')\n"
                          "    scope = {}\n"
                          "    builtins_map = scope['__builtins__']\n"
                          "    return builtins_map['__import__'](module_name)\n"
                          "module_scope = getattr(handler, '__globals__')\n"
                          "module_noise = 1\n"
                          "module_builtins = module_scope['__builtins__']\n"
                          "module_loader = module_builtins['__import__']\n"
                          "multiline_loader = (\n"
                          "    getattr(\n"
                          "        handler,\n"
                          "        '__globals__',\n"
                          "    )\n"
                          "    ['__builtins__']\n"
                          "    # unrelated formatting noise\n"
                          "    ['__import__']\n"
                          ")(module_name)\n") ||
        !write_file(root / "legal.py",
                    "def QConstInit(funcObj):\n"
                    "    \"\"\"Constant initialization; automatically checks reload.\"\"\"\n"
                    "    funcName = getObjectPathName(funcObj)\n"
                    "    if not funcName in _QConstStatic.initFuncSet:\n"
                    "        _QConstStatic.initFuncSet.add(funcName)\n"
                    "        TRY_EXEC_FUN(funcObj)\n"
                    "    return funcObj\n")) {
        std::cerr << "failed to create test fixture\n";
        return 1;
    }

    mcdk::python_review::ReviewAnalyzer analyzer;
    const mcdk::python_review::ReviewOptions options;
    const auto report = analyzer.review(root, options);

    const std::string compact_markdown = mcdk::python_review::render_markdown(report);
    const std::string human_markdown = mcdk::python_review::render_human_markdown(report);
    if (compact_markdown.find("# Python AI Code Review") != 0 ||
        human_markdown.find("# Python 代码审查报告") != 0 ||
        human_markdown.find("## 扫描概览") == std::string::npos ||
        human_markdown.find("## 建议修复") == std::string::npos ||
        human_markdown.find("## 需要人工确认") == std::string::npos ||
        human_markdown.find("<details>") == std::string::npos) {
        std::cerr << "markdown renderer variants have unexpected structure\n";
        fs::remove_all(root, ec);
        return 1;
    }

    int unicode_findings = 0;
    int restricted_import_findings = 0;
    int dynamic_code_findings = 0;
    int reflective_bypass_findings = 0;
    for (const auto& finding : report.findings) {
        if (finding.rule_id == "encoding.unicode-default-encoding") {
            ++unicode_findings;
            if (finding.line != 1 || finding.severity != mcdk::python_review::Severity::Warning) {
                std::cerr << "unicode finding has unexpected location or severity\n";
                fs::remove_all(root, ec);
                return 1;
            }
        } else if (finding.rule_id == "platform.restricted-module-import") {
            ++restricted_import_findings;
            if ((finding.line < 5 || finding.line > 13 || finding.line == 9 ||
                 finding.line == 10) ||
                finding.severity != mcdk::python_review::Severity::Warning) {
                std::cerr << "restricted import finding has unexpected location or severity\n";
                fs::remove_all(root, ec);
                return 1;
            }
        } else if (finding.rule_id == "platform.dynamic-code-execution") {
            ++dynamic_code_findings;
            if (finding.line < 15 || finding.line > 18 ||
                finding.severity != mcdk::python_review::Severity::Warning) {
                std::cerr << "dynamic code finding has unexpected location or severity\n";
                fs::remove_all(root, ec);
                return 1;
            }
        } else if (finding.rule_id == "platform.reflective-security-bypass") {
            if (finding.file == "legal.py") {
                std::cerr << "legal QConstInit wrapper was incorrectly flagged\n";
                fs::remove_all(root, ec);
                return 1;
            }
            ++reflective_bypass_findings;
            const bool expected = ((finding.line >= 19 && finding.line <= 21) ||
                                   (finding.line >= 23 && finding.line <= 29 &&
                                    finding.line != 25) || finding.line == 37 ||
                                   finding.line == 44 || finding.line == 53 ||
                                   finding.line == 54) &&
                finding.severity == mcdk::python_review::Severity::Warning &&
                finding.actionability == mcdk::python_review::Actionability::AdvisoryVerify;
            if (!expected) {
                std::cerr << "reflective bypass finding has unexpected classification\n";
                fs::remove_all(root, ec);
                return 1;
            }
            if ((finding.line == 37 || finding.line == 44 || finding.line == 53) &&
                (finding.evidence.empty() ||
                 finding.evidence.front().find("跨语句变量链") == std::string::npos)) {
                std::cerr << "split reflective chain lacks dataflow evidence\n";
                fs::remove_all(root, ec);
                return 1;
            }
        }
    }

    fs::remove_all(root, ec);
    if (unicode_findings != 1) {
        std::cerr << "expected exactly one unicode default-encoding finding, got "
                  << unicode_findings << "\n";
        return 1;
    }
    if (restricted_import_findings != 7) {
        std::cerr << "expected seven restricted module findings, got "
                  << restricted_import_findings << "\n";
        return 1;
    }
    if (dynamic_code_findings != 4) {
        std::cerr << "expected four dynamic code findings, got "
                  << dynamic_code_findings << "\n";
        return 1;
    }
    if (reflective_bypass_findings != 13) {
        std::cerr << "expected thirteen reflective bypass findings, got "
                  << reflective_bypass_findings << "\n";
        return 1;
    }
    return 0;
}
