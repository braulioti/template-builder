#include "types/RemoteFileType.hpp"

namespace TemplateBuilder {

RemoteFileData::RemoteFileData(const std::string& uri, const std::string& path)
    : m_uri(uri), m_path(path) {
}

} // namespace TemplateBuilder
