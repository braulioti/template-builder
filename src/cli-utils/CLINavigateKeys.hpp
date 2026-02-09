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
     * Safe to call in a loop: each call uses a fixed-size buffer with bounds checks (CWE-120);
     * no buffer accumulation or unbounded input.
     */
    static Key read();
};

} // namespace TemplateBuilder
