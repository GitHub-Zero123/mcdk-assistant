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
                          "d = codec.unicode(value)\n")) {
        std::cerr << "failed to create test fixture\n";
        return 1;
    }

    mcdk::python_review::ReviewAnalyzer analyzer;
    const mcdk::python_review::ReviewOptions options;
    const auto report = analyzer.review(root, options);

    int unicode_findings = 0;
    for (const auto& finding : report.findings) {
        if (finding.rule_id != "encoding.unicode-default-encoding") continue;
        ++unicode_findings;
        if (finding.line != 1 || finding.severity != mcdk::python_review::Severity::Warning) {
            std::cerr << "unicode finding has unexpected location or severity\n";
            fs::remove_all(root, ec);
            return 1;
        }
    }

    fs::remove_all(root, ec);
    if (unicode_findings != 1) {
        std::cerr << "expected exactly one unicode default-encoding finding, got "
                  << unicode_findings << "\n";
        return 1;
    }
    return 0;
}
