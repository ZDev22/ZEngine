#pragma once

#define KEY_IDLE 0
#define KEY_RELEASED 1
#define KEY_HIT 2
#define KEY_PRESSED 3
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../glfw/glfw3.h"

#include "vector.hpp"
#include <cstring>

class Keyboard {
public:
    explicit Keyboard(GLFWwindow* window) : window(window) {}

    // Helpful functions
    unsigned char getKeyIndex(const unsigned short key) const { for (unsigned char i = 0; i < 122; ++i) { if (glfwKeys[i] == key) return i; } return 0; }
    unsigned char updateKeyState(const unsigned short key) {
        unsigned char index = getKeyIndex(key);
        if (keys[index + 122] == 0) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                if (keys[index] == KEY_IDLE) { keys[index] = KEY_HIT; } 
                else { keys[index] = KEY_PRESSED; }
            }
            else {
                if (keys[index] >= KEY_HIT) { keys[index] = KEY_RELEASED; } 
                else { keys[index] = KEY_IDLE; }
            }
            keys[index + 122] = 1;
        }
        return keys[index];
    }

    // Average functions
    bool keyPressed(const unsigned short key) { return glfwGetKey(window, key) == GLFW_PRESS; }
    bool keyHit(const unsigned short key) { return updateKeyState(key) == KEY_HIT; }
    bool keyReleased(const unsigned short key) { return updateKeyState(key) == KEY_RELEASED; }
    bool keysPressed(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (glfwGetKey(window, keysArray[i]) == GLFW_PRESS) return true; } return false; }
    bool keysHit(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) == KEY_HIT) return true; } return false; }
    bool keysReleased(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) == KEY_RELEASED) return true; } return fa;se; }
    bool anyKeyPressed() { for (unsigned char i = 0; i < 107; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) return true; } return false; }
    bool anyKeyHit() { for (unsigned char i = 0; i < 107; i++) { if (updateKeyState(glfwKeys[i]) == KEY_HIT) return true; } return false; }
    bool anyKeyReleased() { for (unsigned char i = 0; i < 107; i++) { if (updateKeyState(glfwKeys[i]) == KEY_RELEASED) return true; } return false; }

    // Keybinds
    struct KeyBind {
        unsigned short key;
        unsigned short rebindedKey;
    };
    void addKeyBind(KeyBind keyBind) { keyBinds.push_back(keyBind); }
    void replaceKeyBind(const unsigned short key, KeyBind newKeyBind) { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { keyBinds[i] = newKeyBind; break; }}}
    void deleteKeyBind(const unsigned short key) { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { keyBinds.erase(keyBinds.begin() + i); break; }}}
    void deleteAllKeyBinds() { keyBinds.clear(); }
    bool keyPressedKeyBind(const unsigned short key) {
        for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { return updateKeyState(keyBinds[i].rebindedKey) == KEY_HIT; }}
        return glfwGetKey(window, key) == GLFW_PRESS;
    }
    bool keyHitKeyBind(const unsigned short key) {
        for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { return glfwGetKey(window, keyBinds[i].rebindedKey) == GLFW_PRESS; }}
        return updateKeyState(key) == KEY_HIT;
    }
    bool keyReleasedKeyBind(const unsigned short key) {
        for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { return glfwGetKey(window, keyBinds[i].rebindedKey) == KEY_RELEASED; }}
        return updateKeyState(key) == KEY_RELEASED;
    }

    // Simulate keys
    void simulateKey(const unsigned short key, const unsigned char newKeyState) {
        int index = getKeyIndex(key);
        keys[index] = newKeyState;
        keys[index + 122] = 0;
    }
    void simulateKeys(const unsigned short* keysArray, const unsigned char numKeys, const unsigned char newKeyState) {
        for (unsigned char i = 0; i < numKeys; i++) {
            int index = getKeyIndex(keysArray[i]);
            keys[index] = newKeyState;
            keys[index + 122] = 0;
        }
    }
    bool keyPressedSimulated(const unsigned short key) { return keys[getKeyIndex(key)] == GLFW_PRESS; }
    bool keysPressedSimulated(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (keys[getKeyIndex(keysArray[i])] == GLFW_PRESS) return true; } return false; }
    bool anyKeyPressedSimulated() { for (unsigned char i = 0; i < 107; i++) { if (keys[getKeyIndex(keys[i])] == GLFW_PRESS) return true; } return false; }

    // Reset keys
    void resetKeys() { std::memset(keys + 122, 0, 122); }
    void resetKeyStates() { std::memset(keys, 0, 122); }
    void fullResetKeys() { std::memset(keys, 0, 244); }
    void hitAllKeys() { std::memset(keys, 2, 122); }
    void pressAllKeys() { std::memset(keys, 3, 122); }
    void releaseAllKeys() { std::memset(keys, 1, 122); }
    void lockAllKeys() { std::memset(keys + 122, 1, 122); }

private:
    GLFWwindow* window;

    unsigned char keys[244] = {0};
    const unsigned short glfwKeys[122] = {
        /*common   */ GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_E, GLFW_KEY_Q, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_T, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_DOWN, GLFW_KEY_UP, GLFW_KEY_V, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9, GLFW_KEY_0,
        /*rare     */ GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_TAB, GLFW_KEY_BACKSPACE, GLFW_KEY_INSERT, GLFW_KEY_DELETE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, GLFW_KEY_SEMICOLON, GLFW_KEY_COMMA, GLFW_KEY_LEFT_SUPER,
        /*legendary*/ GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_R, GLFW_KEY_U, GLFW_KEY_Y, GLFW_KEY_KP_0, GLFW_KEY_KP_1, GLFW_KEY_KP_2, GLFW_KEY_KP_3, GLFW_KEY_KP_4, GLFW_KEY_KP_5, GLFW_KEY_KP_6, GLFW_KEY_KP_7, GLFW_KEY_KP_8, GLFW_KEY_KP_9, GLFW_KEY_KP_DECIMAL, GLFW_KEY_KP_DIVIDE, GLFW_KEY_KP_MULTIPLY, GLFW_KEY_KP_SUBTRACT, GLFW_KEY_KP_ADD, GLFW_KEY_KP_ENTER, GLFW_KEY_KP_EQUAL, GLFW_KEY_HOME, GLFW_KEY_END, GLFW_KEY_CAPS_LOCK, GLFW_KEY_NUM_LOCK, GLFW_KEY_SCROLL_LOCK, GLFW_KEY_PRINT_SCREEN, GLFW_KEY_PAUSE, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6, GLFW_KEY_F7, GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10,
        /*mythic   */ GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_SUPER, GLFW_KEY_MENU, GLFW_KEY_APOSTROPHE, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN, GLFW_KEY_WORLD_1, GLFW_KEY_WORLD_2,
        /*secret   */ GLFW_KEY_F11, GLFW_KEY_F12, GLFW_KEY_F13, GLFW_KEY_F14, GLFW_KEY_F15, GLFW_KEY_F16, GLFW_KEY_F17, GLFW_KEY_F18, GLFW_KEY_F19, GLFW_KEY_F20, GLFW_KEY_F21, GLFW_KEY_F22, GLFW_KEY_F23, GLFW_KEY_F24, GLFW_KEY_F25
    };
    vector<KeyBind> keyBinds;
};