#pragma once

namespace TemplateBuilder {

/**
 * Central place for config-related constants (section names, key names, default values).
 */
struct ConfigConstants {
    /** Config section for CLI/arguments-related options. */
    static constexpr const char* SECTION_ARGUMENTS = "ARGUMENTS";

    /** Key for the URL used by the list command (-l / --list). */
    static constexpr const char* KEY_LIST_URL = "ListURL";

    /** Default URL for the templates list when not set in config.ini. */
    static constexpr const char* DEFAULT_LIST_URL = "https://brau.io/downloads/list.txt";
};

} // namespace TemplateBuilder
