#pragma once

#include <string>

namespace TemplateBuilder {

class DetailsBuilder {
public:
    DetailsBuilder() = default;
    ~DetailsBuilder() = default;

    void displayAll(const std::string& name, const std::string& author, const std::string& email,
                    const std::string& url, const std::string& language);

    void displayName(const std::string& name);
    void displayAuthor(const std::string& author);
    void displayEmail(const std::string& email);
    void displayUrl(const std::string& url);
    void displayLanguage(const std::string& language);
};

} // namespace TemplateBuilder
