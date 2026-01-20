#pragma once

#include <string>

namespace TemplateBuilder {

class RemoteFileData {
public:
    // Constructors
    RemoteFileData() = default;
    RemoteFileData(const std::string& uri, const std::string& path);

    // Getters
    [[nodiscard]] const std::string& getUri() const noexcept { return m_uri; }
    [[nodiscard]] const std::string& getPath() const noexcept { return m_path; }

    // Setters
    void setUri(const std::string& uri) { m_uri = uri; }
    void setPath(const std::string& path) { m_path = path; }

    // Utility methods
    [[nodiscard]] bool isEmpty() const noexcept { return m_uri.empty() && m_path.empty(); }
    [[nodiscard]] bool hasUri() const noexcept { return !m_uri.empty(); }
    [[nodiscard]] bool hasPath() const noexcept { return !m_path.empty(); }

private:
    std::string m_uri;
    std::string m_path;
};

} // namespace TemplateBuilder
