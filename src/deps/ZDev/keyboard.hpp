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

    // KEYBOARD

    // Utility
    unsigned char getKeyIndex(const unsigned short key) const { for (unsigned char i = 0; i < 104; ++i) { if (glfwKeys[i] == key) return i; } return 0; }
    unsigned char updateKeyState(const unsigned short key) {
        unsigned char index = getKeyIndex(key);
        unsigned char &cache = keys[index];
        if (keys[index + 104] == 0) {
            if (glfwGetKey(window, key) == GLFW_PRESS) {
                if (cache == KEY_IDLE) { cache = KEY_HIT; } 
                else { cache = KEY_PRESSED; }
            }
            else {
                if (cache > KEY_RELEASED) { cache = KEY_RELEASED; } 
                else { cache = KEY_IDLE; }
            }
            keys[index + 104] = 1;
        }
        return cache;
    }

    // Casual
    bool keyPressed(const unsigned short key) { return glfwGetKey(window, key) == GLFW_PRESS; }
    bool keyHit(const unsigned short key) { return updateKeyState(key) == KEY_HIT; }
    bool keyReleased(const unsigned short key) { return updateKeyState(key) == KEY_RELEASED; }
    bool keyIdle(const unsigned short key) { return glfwGetKey(window, key) == GLFW_RELEASE; }
    bool keysPressed(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (glfwGetKey(window, keysArray[i]) != GLFW_PRESS) return false; } return true; }
    bool keysHit(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) != KEY_HIT) return false; } return true; }
    bool keysReleased(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) != KEY_RELEASED) return false; } return true; }
    bool keysIdle(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (glfwGetKey(window, keysArray[i]) != GLFW_RELEASE) return false; } return true; }
    bool anyKeyPressed() { for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) return true; } return false; }
    bool anyKeyHit() { for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_HIT) return true; } return false; }
    bool anyKeyReleased() { for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_RELEASED) return true; } return false; }
    bool anyKeyIdle() { for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_RELEASE) return true; } return false; }
    short whatKeyPressed() { for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) return glfwKeys[i]; } return -1; }
    short whatKeyHit() { for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_HIT) return glfwKeys[i]; } return -1; }
    short whatKeyReleased() { for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_RELEASED) return glfwKeys[i]; } return -1; }
    short whatKeyIdle() { for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_RELEASE) return glfwKeys[i]; } return -1; }
    vector<short> whatKeysPressed() {
        vector<short> keysPressed;
        for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) keysPressed.push_back(glfwKeys[i]); }
        return keysPressed;
    }
    vector<short> whatKeysHit() {
        vector<short> keysHit;
        for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_HIT) keysHit.push_back(glfwKeys[i]); }
        return keysHit;
    }
    vector<short> whatKeysReleased() {
        vector<short> keysReleased;
        for (unsigned char i = 0; i < 104; i++) { if (updateKeyState(glfwKeys[i]) == KEY_RELEASED) keysReleased.push_back(glfwKeys[i]);}
        return keysReleased;
    }
    vector<short> whatKeysIdle() {
        vector<short> keysIdle;
        for (unsigned char i = 0; i < 104; i++) { if (glfwGetKey(window, glfwKeys[i]) == GLFW_RELEASE) keysIdle.push_back(glfwKeys[i]); }
        return keysIdle;
    }

    // Activity
    vector<unsigned short> activeKeys() {
        vector<unsigned short> keysPressed;
        for (unsigned char i = 0; i < 104; i++) if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) { keysPressed.push_back(glfwKeys[i]); }
        return keysPressed;
    }
    unsigned char countKeysPressed() {
        unsigned char keysPressed;
        for (unsigned char i = 0; i < 104; i++) if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) { keysPressed++; }
        return keysPressed;
    }
    unsigned char countKeysHit() {
        unsigned char keysHit;
        for (unsigned char i = 0; i < 104; i++) if (updateKeyState(glfwKeys[i]) == KEY_HIT) { keysHit++; }
        return keysHit;
    }
    unsigned char countKeysReleased() {
        unsigned char keysReleased;
        for (unsigned char i = 0; i < 104; i++) if (updateKeyState(glfwKeys[i]) == KEY_RELEASED) { keysReleased++; }
        return keysReleased;
    }
    unsigned char countKeysIdle() {
        unsigned char keysIdle;
        for (unsigned char i = 0; i < 104; i++) if (glfwGetKey(window, glfwKeys[i]) == GLFW_PRESS) { keysIdle++; }
        return keysIdle;
    }

    // Keybinds
    struct KeyBind {
        unsigned short key;
        unsigned short rebindedKey;
    };
    void addKeyBind(KeyBind keyBind) { keyBinds.push_back(keyBind); }
    void replaceKeyBind(const unsigned short key, KeyBind newKeyBind) { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { keyBinds[i] = newKeyBind; break; }}}
    void deleteKeyBind(const unsigned short key) { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { keyBinds.erase(keyBinds.begin() + i); break; }}}
    void deleteAllKeyBinds() { keyBinds.clear(); }
    bool isKeyBound(const unsigned short key) { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) return true; } return false;}
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
    bool anyKeyBindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (glfwGetKey(window, keyBinds[i].key) != GLFW_PRESS) return false; } return true; }
    bool anyKeyRebindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (glfwGetKey(window, keyBinds[i].rebindedKey) != GLFW_PRESS) return false; } return true; }
    short whatKeyBindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (glfwGetKey(window, keyBinds[i].key) == GLFW_PRESS) return keyBinds[i].key; } return -1; }
    short whatKeyRebindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (glfwGetKey(window, keyBinds[i].rebindedKey) == GLFW_PRESS) return keyBinds[i].rebindedKey; } return -1; }

    // Simulation
    void simulateKey(const unsigned short key, const unsigned char newKeyState) {
        int index = getKeyIndex(key);
        keys[index] = newKeyState;
        keys[index + 104] = 0;
    }
    void simulateKeys(const unsigned short* keysArray, const unsigned char numKeys, const unsigned char newKeyState) {
        for (unsigned char i = 0; i < numKeys; i++) {
            int index = getKeyIndex(keysArray[i]);
            keys[index] = newKeyState;
            keys[index + 104] = 0;
        }
    }
    bool keyPressedSimulated(const unsigned short key) { return keys[getKeyIndex(key)] == GLFW_PRESS; }
    bool keysPressedSimulated(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (keys[getKeyIndex(keysArray[i])] == GLFW_PRESS) return true; } return false; }
    bool anyKeyPressedSimulated() { for (unsigned char i = 0; i < 104; i++) { if (keys[getKeyIndex(keys[i])] == GLFW_PRESS) return true; } return false; }

    // Modifiers
    void applyModifiers() {
        if (updateKeyState(GLFW_KEY_CAPS_LOCK) == KEY_HIT) { modifiers[0] = !modifiers[0]; }
        if (updateKeyState(GLFW_KEY_NUM_LOCK) == KEY_HIT) { modifiers[1] = !modifiers[1]; }
    }
    bool capsLock() { return modifiers[0]; }
    bool numLock() { return modifiers[1]; }

    // Reset
    void resetKeys() { std::memset(keys + 104, 0, 104); }
    void resetKeyStates() { std::memset(keys, 0, 104); }
    void fullResetKeys() { std::memset(keys, 0, 208); }
    void hitAllKeys() { std::memset(keys, 2, 104); }
    void pressAllKeys() { std::memset(keys, 3, 104); }
    void releaseAllKeys() { std::memset(keys, 1, 104); }
    void lockAllKeys() { std::memset(keys + 104, 1, 104); }

    // MOUSE
    void updateMouse() {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mouseState[0] = true;
            if (mouseState[1] == KEY_IDLE) { mouseState[1] = KEY_HIT; }
            else { mouseState[1] = KEY_PRESSED; }
        }
        else {
            mouseState[0] = false;
            if (mouseState[1] >= KEY_HIT) { mouseState[1] = KEY_RELEASED; } 
            else { mouseState[1] = KEY_IDLE; }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            mouseState[2] = true;
            if (mouseState[3] == KEY_IDLE) { mouseState[3] = KEY_HIT; }
            else { mouseState[3] = KEY_PRESSED; }
        }
        else {
            mouseState[2] = false;
            if (mouseState[3] > KEY_RELEASED) { mouseState[3] = KEY_RELEASED; } 
            else { mouseState[3] = KEY_IDLE; }
        }
    }

    float[2] getMousePosition() { glfwGetCursorPos(window, &mousePosition[0], &mousePosition[1]); return float[2](mousePosition[0], mousePosition[1]); }
    void setMousePosition(double newPosition[2]) { glfwSetCursorPos(window, newPosition[0], newPosition[1]); }
    bool LMBPressed() { return mouseState[0]; }
    bool RMBPressed() { return mouseState[2]; }
    bool LMBHit() { return mouseState[1] == KEY_HIT; }
    bool RMBHit() { return mouseState[3] == KEY_HIT; }
    bool LMBReleased() { return mouseState[1] == KEY_RELEASED; }
    bool RMBReleased() { return mouseState[3] == KEY_RELEASED; }

private:
    GLFWwindow* window;

    unsigned char keys[208] = {0};
    const unsigned short glfwKeys[104] = {
        /*common   */ GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_W, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_E, GLFW_KEY_Q, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_T, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_DOWN, GLFW_KEY_UP, GLFW_KEY_V, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9, GLFW_KEY_0,
        /*rare     */ GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_TAB, GLFW_KEY_BACKSPACE, GLFW_KEY_INSERT, GLFW_KEY_DELETE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, GLFW_KEY_SEMICOLON, GLFW_KEY_COMMA, GLFW_KEY_LEFT_SUPER,
        /*legendary*/ GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O, GLFW_KEY_P, GLFW_KEY_R, GLFW_KEY_U, GLFW_KEY_Y, GLFW_KEY_KP_0, GLFW_KEY_KP_1, GLFW_KEY_KP_2, GLFW_KEY_KP_3, GLFW_KEY_KP_4, GLFW_KEY_KP_5, GLFW_KEY_KP_6, GLFW_KEY_KP_7, GLFW_KEY_KP_8, GLFW_KEY_KP_9, GLFW_KEY_KP_DECIMAL, GLFW_KEY_KP_DIVIDE, GLFW_KEY_KP_MULTIPLY, GLFW_KEY_KP_SUBTRACT, GLFW_KEY_KP_ADD, GLFW_KEY_KP_ENTER, GLFW_KEY_KP_EQUAL, GLFW_KEY_HOME, GLFW_KEY_END, GLFW_KEY_CAPS_LOCK, GLFW_KEY_NUM_LOCK, GLFW_KEY_PRINT_SCREEN, GLFW_KEY_PAUSE, GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4, GLFW_KEY_F5, GLFW_KEY_F6, GLFW_KEY_F7, GLFW_KEY_F8, GLFW_KEY_F9, GLFW_KEY_F10,
        /*mythic   */ GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_RIGHT_ALT, GLFW_KEY_RIGHT_SUPER, GLFW_KEY_MENU, GLFW_KEY_APOSTROPHE, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN
        /*secret      GLFW_KEY_F11, GLFW_KEY_F12, GLFW_KEY_F13, GLFW_KEY_F14, GLFW_KEY_F15, GLFW_KEY_F16, GLFW_KEY_F17, GLFW_KEY_F18, GLFW_KEY_F19, GLFW_KEY_F20, GLFW_KEY_F21, GLFW_KEY_F22, GLFW_KEY_F23, GLFW_KEY_F24, GLFW_KEY_F25, GLFW_KEY_WORLD_1, GLFW_KEY_WORLD_2 */
    };
    vector<KeyBind> keyBinds;
    bool modifiers[2] = {false};
    double mousePosition[2];
    unsigned char mouseState[4] = {0};
};