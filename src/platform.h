#pragma once
#include <exception>
#include <string>

struct SDL_Window;

class PlatformException : public std::exception {
public:
    PlatformException(const std::string& message)
        : m_message("Platform exception: " + message + "\n") {}
    virtual const char* what() const {
        return m_message.c_str();
    }
private:
    const std::string m_message;
};

enum {
    WND_W = 1024,
    WND_H = 768
};

constexpr const char* pWndTitle = "Breadkill";

class Platform {
public:
    Platform();
    ~Platform();
    SDL_Window* GetWindowHandle() const;
    void HandleEvents();
    bool WindowShouldClose() const;
    void GetMousePosition(float* pX, float* pY, bool* clickIsPressed);
    void GetWindowSize(int* pWidth, int* pHeight);
private:
    SDL_Window* m_pWindow;
    bool m_shouldClose = false;
};
