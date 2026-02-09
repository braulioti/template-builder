#pragma once

#include <string>
#include <vector>

namespace TemplateBuilder {

/**
 * Encapsulates the list of command-line arguments (argv) for easier parsing.
 */
class ArgumentList {
public:
    ArgumentList() = default;

    /** Build from main(int argc, char* argv[]). Copies argument strings. */
    ArgumentList(int argc, char* argv[]);

    [[nodiscard]] bool empty() const noexcept { return m_args.empty(); }
    [[nodiscard]] size_t size() const noexcept { return m_args.size(); }

    /** 1-based index (index 1 = first argument after program name). */
    [[nodiscard]] const std::string& at(size_t index) const;
    [[nodiscard]] const std::string& operator[](size_t index) const { return at(index); }

    [[nodiscard]] std::vector<std::string>::const_iterator begin() const { return m_args.begin(); }
    [[nodiscard]] std::vector<std::string>::const_iterator end() const { return m_args.end(); }

    /** Downloads the list from the given URL, shows interactive selection (Up/Down, Enter), prints selected line. Returns false on error. */
    static bool fetchAndPrintList(const std::string& listUrl);

private:
    std::vector<std::string> m_args;
};

} // namespace TemplateBuilder
