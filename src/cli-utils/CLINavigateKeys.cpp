#include "cli-utils/CLINavigateKeys.hpp"
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#else
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#endif

namespace TemplateBuilder {

#ifdef _WIN32
namespace {

bool readConsoleKey(unsigned short& key, bool& keyPressed) {
    keyPressed = false;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) return false;
    INPUT_RECORD inputRecord;
    DWORD numRead = 0;
    if (!PeekConsoleInput(hStdin, &inputRecord, 1, &numRead) || numRead == 0) return false;
    if (!ReadConsoleInput(hStdin, &inputRecord, 1, &numRead)) return false;
    if (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown) return false;
    key = inputRecord.Event.KeyEvent.wVirtualKeyCode;
    keyPressed = true;
    return true;
}

} // namespace

CLINavigateKeys::Key CLINavigateKeys::read() {
    unsigned short key = 0;
    bool keyPressed = false;
    while (!readConsoleKey(key, keyPressed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (key == VK_UP) return Key::Up;
    if (key == VK_DOWN) return Key::Down;
    if (key == VK_RETURN) return Key::Enter;
    if (key == VK_ESCAPE) return Key::Escape;
    if (key == VK_SPACE) return Key::Space;
    return Key::None;
}

#else
namespace {

constexpr size_t READ_BUFFER_SIZE = 1;

int readOneByte(unsigned char* byteOut) {
    unsigned char buf[READ_BUFFER_SIZE];
    const ssize_t n = read(STDIN_FILENO, buf, READ_BUFFER_SIZE);
    if (n == 1) {
        *byteOut = buf[0];
        return 1;
    }
    return (n < 0) ? -1 : 0;
}

bool setTerminalRaw(struct termios* saved) {
    if (tcgetattr(STDIN_FILENO, saved) != 0) return false;
    struct termios raw = *saved;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    return tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0;
}

void restoreTerminal(const struct termios* saved) {
    tcsetattr(STDIN_FILENO, TCSANOW, saved);
}

CLINavigateKeys::Key parseEscapeSequence() {
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &raw) != 0) return CLINavigateKeys::Key::Escape;
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    unsigned char c2 = 0;
    unsigned char c3 = 0;
    const int n2 = readOneByte(&c2);
    const int n3 = (n2 == 1 && c2 == '[') ? readOneByte(&c3) : 0;
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    if (n2 != 1 || c2 != '[' || n3 != 1) return CLINavigateKeys::Key::Escape;
    if (c3 == 'A') return CLINavigateKeys::Key::Up;
    if (c3 == 'B') return CLINavigateKeys::Key::Down;
    return CLINavigateKeys::Key::Escape;
}

} // namespace

CLINavigateKeys::Key CLINavigateKeys::read() {
    if (!isatty(STDIN_FILENO)) return Key::Enter;
    struct termios saved;
    if (!setTerminalRaw(&saved)) return Key::Enter;
    Key result = Key::None;
    unsigned char c = 0;
    if (readOneByte(&c) == 1) {
        if (c == '\r' || c == '\n') result = Key::Enter;
        else if (c == ' ') result = Key::Space;
        else if (c == 27) result = parseEscapeSequence();
    }
    restoreTerminal(&saved);
    return result;
}
#endif

} // namespace TemplateBuilder
