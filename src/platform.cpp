#include "platform.h"
#include "SDL3/SDL.h"

Platform::Platform() {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        throw PlatformException("Could not initialize SDL");
    }
    m_pWindow = SDL_CreateWindow(pWndTitle, WND_W, WND_H, 0);
    if (m_pWindow == nullptr) {
        throw PlatformException("Could not create window!");
    }
}

Platform::~Platform() {
    SDL_DestroyWindow(m_pWindow);
}

SDL_Window* Platform::GetWindowHandle() const {
    return m_pWindow;
}

void Platform::HandleEvents() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
            case SDL_EVENT_QUIT:
                m_shouldClose = true;
                break;
        }
    }
}

bool Platform::WindowShouldClose() const {
    return m_shouldClose;
}

void Platform::GetMousePosition(float* pX, float* pY, bool* clickIsPressed) {
    SDL_MouseButtonFlags flags = SDL_GetMouseState(pX, pY);
    if (clickIsPressed != nullptr) {
        if (flags & SDL_BUTTON_LMASK) {
            *clickIsPressed = true;
        }
        else {
            *clickIsPressed = false;
        }
    }
}

void Platform::GetWindowSize(int* pWidth, int* pHeight) {
    SDL_GetWindowSize(m_pWindow, pWidth, pHeight);
}
