#include "builders/DetailsBuilder.hpp"
#include <iostream>

namespace TemplateBuilder {

void DetailsBuilder::displayAll(const std::string& name, const std::string& author, const std::string& email,
                                const std::string& url, const std::string& language) {
    displayName(name);
    displayAuthor(author);
    displayEmail(email);
    displayUrl(url);
    displayLanguage(language);

    std::cout << std::endl;
}

void DetailsBuilder::displayName(const std::string& name) {
    if (!name.empty()) {
        std::cout << "Project: " << name << std::endl;
    }
}

void DetailsBuilder::displayAuthor(const std::string& author) {
    if (!author.empty()) {
        std::cout << "Author: " << author << std::endl;
    }
}

void DetailsBuilder::displayEmail(const std::string& email) {
    if (!email.empty()) {
        std::cout << "Email: " << email << std::endl;
    }
}

void DetailsBuilder::displayUrl(const std::string& url) {
    if (!url.empty()) {
        std::cout << "URL: " << url << std::endl;
    }
}

void DetailsBuilder::displayLanguage(const std::string& language) {
    if (!language.empty()) {
        std::cout << "Language: " << language << std::endl;
    }
}

} // namespace TemplateBuilder
