#pragma once

#define KEY_IDLE 0
#define KEY_HIT 2
#define KEY_PRESSED 3
#define KEY_RELEASED 1

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../glfw/glfw3.h"
#include "../../vulkan/window.hpp"

#include <unordered_map>
#include <cstring>
#include <vector>
#include <array>

class Keyboard {
public:
    explicit Keyboard(AppWindow& appWindow) : window{appWindow.getWindow()} {
        std::memset(keys.data(), 0, keys.size());
        for (int i = 0; i < glfwKeys.size(); ++i) { keyIndexMap[glfwKeys[i]] = i; }
    }

    unsigned char updateKeyState(int key) {
        auto keyState = keyIndexMap.find(key);

        if (keys[keyState->second + 122] == 0) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                if (keys[keyState->second] == KEY_IDLE) { keys[keyState->second] = KEY_HIT; }
                else { keys[keyState->second] = KEY_PRESSED; }
            }
            else {
                if (keys[keyState->second] >= KEY_HIT) { keys[keyState->second] = KEY_RELEASED; }
                else { keys[keyState->second] = KEY_IDLE; }
            }
            keys[keyState->second + 122] = 1;
        }
        return keys[keyState->second];
    }

    bool keyPressed(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }
    bool keyHit(int key) { return updateKeyState(key) == KEY_HIT; }
    bool keyReleased(int key) { return updateKeyState(key) == KEY_RELEASED; }
    bool keysPressed(std::vector<int>& keys) {
        for (size_t k = 0; k < keys.size(); k++) { if (!glfwGetKey(window, keys[k]) == GLFW_PRESS) { return false; } }
        return true;
    }

    void simulateKey(int key, int newKeyState) {
        auto keyState = keyIndexMap.find(key);
        keys[keyState->second] = newKeyState;
        keys[keyState->second + 122] = 0;
    }

    void resetKeys() { std::memset(keys.data() + 122, 0, (122)); }

private:
    GLFWwindow* window;

    std::unordered_map<int, int> keyIndexMap;
    std::array<int, 122> glfwKeys = {
        /*Special  */ GLFW_KEY_SPACE, GLFW_KEY_APOSTROPHE, GLFW_KEY_COMMA, GLFW_KEY_MINUS, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, GLFW_KEY_SEMICOLON, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_GRAVE_ACCENT,
        /*Numbers  */ GLFW_KEY_0, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9,
        /*Letters  */ GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T, GLFW_KEY_U, GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y, GLFW_KEY_Z,
        /*Function */ GLFW_KEY_ESCAPE, GLFW_KEY_ENTER, GLFW_KEY_TAB, GLFW_KEY_BACKSPACE, GLFW_KEY_INSERT, GLFW_KEY_DELETE, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_DOWN, GLFW_KEY_UP, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN, GLFW_KEY_HOME, GLFW_KEY_END, GLFW_KEY_CAPS_LOCK, GLFW_KEY_NUM_LOCK, GLFW_KEY_SCROLL_LOCK, GLFW_KEY_PRINT_SCREEN, GLFW_KEY_PAUSE, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6, GLFW_KEY_F7, GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10, GLFW_KEY_F11, GLFW_KEY_F12, GLFW_KEY_F13, GLFW_KEY_F14, GLFW_KEY_F15, GLFW_KEY_F16, GLFW_KEY_F17, GLFW_KEY_F18, GLFW_KEY_F19, GLFW_KEY_F20, GLFW_KEY_F21, GLFW_KEY_F22, GLFW_KEY_F23, GLFW_KEY_F24, GLFW_KEY_F25,
        /*Keypad   */ GLFW_KEY_KP_0, GLFW_KEY_KP_1, GLFW_KEY_KP_2, GLFW_KEY_KP_3, GLFW_KEY_KP_4, GLFW_KEY_KP_5, GLFW_KEY_KP_6, GLFW_KEY_KP_7, GLFW_KEY_KP_8, GLFW_KEY_KP_9, GLFW_KEY_KP_DECIMAL, GLFW_KEY_KP_DIVIDE, GLFW_KEY_KP_MULTIPLY, GLFW_KEY_KP_SUBTRACT, GLFW_KEY_KP_ADD, GLFW_KEY_KP_ENTER, GLFW_KEY_KP_EQUAL,
        /*Modifiers*/ GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT, GLFW_KEY_LEFT_SUPER, GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_SUPER, GLFW_KEY_MENU
    };
    std::array<unsigned char, 244> keys;
};