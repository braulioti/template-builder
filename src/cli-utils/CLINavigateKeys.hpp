#pragma once

namespace TemplateBuilder {

/**
 * Handles keyboard input for CLI navigation (list selection, etc.).
 * Isolates key-event logic to keep CLINavigate simpler.
 */
class CLINavigateKeys {
public:
    enum class Key { None, Up, Down, Enter, Escape, Space };

    /**
     * Blocks until a key is read; returns the corresponding Key.
     * Safe to call in a loop: each call reads exactly one key event; no buffer accumulation.
     */
    static Key read();
};

} // namespace TemplateBuilder
