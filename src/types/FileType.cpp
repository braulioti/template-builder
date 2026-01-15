#include "types/FileType.hpp"

namespace TemplateBuilder {

FileData::FileData(const std::string& path, const std::string& content)
    : m_path(path), m_content(content) {
}

} // namespace TemplateBuilder
