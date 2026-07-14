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
                          "exec code in globals_dict\n")) {
        std::cerr << "failed to create test fixture\n";
        return 1;
    }

    mcdk::python_review::ReviewAnalyzer analyzer;
    const mcdk::python_review::ReviewOptions options;
    const auto report = analyzer.review(root, options);

    int unicode_findings = 0;
    int restricted_import_findings = 0;
    int dynamic_code_findings = 0;
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
    return 0;
}
