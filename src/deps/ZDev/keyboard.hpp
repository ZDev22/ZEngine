/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

v1.8.1

keyboard.hpp is a lightweight cross-platform single-header cpp keyboard abstraction library focused on HIGH-PREFORMANCE KEY DETECTION
Built around GLFW, but now only runs of RGFW so it's currently missing RGFW-specific-optimizations (sigh).

HOW-TO-USE: Keyboard keyboard;
keyboard.keyHit(RGFW_a);
*/

#pragma once

#define KEY_IDLE 0
#define KEY_RELEASED 1
#define KEY_HIT 2
#define KEY_PRESSED 3

#include "engine/window.hpp"

#include <string.h>
#include <vector>

struct Keyboard {
public:
    // KEYBOARD

    Keyboard(RGFW_window* window) : window(window) {}

    // Utility
    unsigned char getKeyIndex(const unsigned short key) const { for (unsigned char i = 0; i < 97; ++i) { if (rgfwKeys[i] == key) return i; } return 0; }
    unsigned char updateKeyState(const unsigned short key) {
        unsigned char index = getKeyIndex(key);
        unsigned char &cache = keys[index];
        if (keys[index + 97] == 0) {
            if (RGFW_window_isKeyPressed(window, key)) {
                if (cache == KEY_IDLE) { cache = KEY_HIT; }
                else { cache = KEY_PRESSED; }
            }
            else {
                if (cache > KEY_RELEASED) { cache = KEY_RELEASED; }
                else { cache = KEY_IDLE; }
            }
            keys[index + 97] = 1;
        }
        return cache;
    }

    // Casual
    bool keyPressed(const unsigned short key) { return RGFW_window_isKeyPressed(window, key); }
    bool keyHit(const unsigned short key) { return updateKeyState(key) == KEY_HIT; }
    bool keyReleased(const unsigned short key) { return updateKeyState(key) == KEY_RELEASED; }
    bool keyIdle(const unsigned short key) { return !RGFW_window_isKeyPressed(window, key); }
    bool keysPressed(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (!RGFW_window_isKeyPressed(window, keysArray[i])) return false; } return true; }
    bool keysHit(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) != KEY_HIT) return false; } return true; }
    bool keysReleased(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (updateKeyState(keysArray[i]) != KEY_RELEASED) return false; } return true; }
    bool keysIdle(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (RGFW_window_isKeyPressed(window, keysArray[i])) return false; } return true; }
    bool anyKeyPressed() { for (unsigned char i = 0; i < 97; i++) { if (RGFW_window_isKeyPressed(window, rgfwKeys[i])) return true; } return false; }
    bool anyKeyHit() { for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_HIT) return true; } return false; }
    bool anyKeyReleased() { for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_RELEASED) return true; } return false; }
    bool anyKeyIdle() { for (unsigned char i = 0; i < 97; i++) { if (!RGFW_window_isKeyPressed(window, rgfwKeys[i])) return true; } return false; }
    short whatKeyPressed() { for (unsigned char i = 0; i < 97; i++) { if (RGFW_window_isKeyPressed(window, rgfwKeys[i])) return rgfwKeys[i]; } return -1; }
    short whatKeyHit() { for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_HIT) return rgfwKeys[i]; } return -1; }
    short whatKeyReleased() { for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_RELEASED) return rgfwKeys[i]; } return -1; }
    short whatKeyIdle() { for (unsigned char i = 0; i < 97; i++) { if (!RGFW_window_isKeyPressed(window, rgfwKeys[i])) return rgfwKeys[i]; } return -1; }
    std::vector<short> whatKeysPressed() {
        std::vector<short> keysPressed;
        for (unsigned char i = 0; i < 97; i++) { if (RGFW_window_isKeyPressed(window, rgfwKeys[i])) keysPressed.push_back(rgfwKeys[i]); }
        return keysPressed;
    }
    std::vector<short> whatKeysHit() {
        std::vector<short> keysHit;
        for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_HIT) keysHit.push_back(rgfwKeys[i]); }
        return keysHit;
    }
    std::vector<short> whatKeysReleased() {
        std::vector<short> keysReleased;
        for (unsigned char i = 0; i < 97; i++) { if (updateKeyState(rgfwKeys[i]) == KEY_RELEASED) keysReleased.push_back(rgfwKeys[i]);}
        return keysReleased;
    }
    std::vector<short> whatKeysIdle() {
        std::vector<short> keysIdle;
        for (unsigned char i = 0; i < 97; i++) { if (!RGFW_window_isKeyPressed(window, rgfwKeys[i])) keysIdle.push_back(rgfwKeys[i]); }
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
        return RGFW_window_isKeyPressed(window, key);
    }
    bool keyHitKeyBind(const unsigned short key) {
        for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { return RGFW_window_isKeyPressed(window, keyBinds[i].rebindedKey); }}
        return updateKeyState(key) == KEY_HIT;
    }
    bool keyReleasedKeyBind(const unsigned short key) {
        for (unsigned short i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { return !RGFW_window_isKeyPressed(window, keyBinds[i].rebindedKey); }}
        return updateKeyState(key) == KEY_RELEASED;
    }
    bool anyKeyBindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (!RGFW_window_isKeyPressed(window, keyBinds[i].key)) return false; } return true; }
    bool anyKeyRebindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (!RGFW_window_isKeyPressed(window, keyBinds[i].rebindedKey)) return false; } return true; }
    short whatKeyBindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (RGFW_window_isKeyPressed(window, keyBinds[i].key)) return keyBinds[i].key; } return -1; }
    short whatKeyRebindPressed() { for (unsigned short i = 0; i < keyBinds.size(); i++) { if (RGFW_window_isKeyPressed(window, keyBinds[i].rebindedKey)) return keyBinds[i].rebindedKey; } return -1; }

    // Simulation
    void simulateKey(const unsigned short key, const unsigned char newKeyState) {
        int index = getKeyIndex(key);
        keys[index] = newKeyState;
        keys[index + 97] = 0;
    }
    void simulateKeys(const unsigned short* keysArray, const unsigned char numKeys, const unsigned char newKeyState) {
        for (unsigned char i = 0; i < numKeys; i++) {
            int index = getKeyIndex(keysArray[i]);
            keys[index] = newKeyState;
            keys[index + 97] = 0;
        }
    }
    bool keyPressedSimulated(const unsigned short key) { return keys[getKeyIndex(key)] == KEY_PRESSED; }
    bool keysPressedSimulated(const unsigned short* keysArray, const unsigned char numKeys) { for (unsigned char i = 0; i < numKeys; i++) { if (keys[getKeyIndex(keysArray[i])] == KEY_PRESSED) return true; } return false; }
    bool anyKeyPressedSimulated() { for (unsigned char i = 0; i < 97; i++) { if (keys[getKeyIndex(keys[i])] == KEY_PRESSED) return true; } return false; }

    // Modifiers
    void updateModifiers() {
        if (updateKeyState(RGFW_capsLock) == KEY_HIT) { modifiers[0] = !modifiers[0]; }
        if (updateKeyState(RGFW_numLock) == KEY_HIT) { modifiers[1] = !modifiers[1]; }
    }
    bool capsLock() { return modifiers[0]; }
    bool numLock() { return modifiers[1]; }

    // Reset
    void resetKeys() { memset(keys + 97, 0, 97); }
    void resetKeyStates() { memset(&keys, 0, 97); }
    void fullResetKeys() { memset(&keys, 0, 194); }
    void hitAllKeys() { memset(&keys, 2, 97); }
    void pressAllKeys() { memset(&keys, 3, 97); }
    void releaseAllKeys() { memset(&keys, 1, 97); }
    void lockAllKeys() { memset(&keys + 97, 1, 97); }

    // MOUSE

    void updateMouse() {
        if (RGFW_window_isMousePressed(window, RGFW_mouseLeft)) {
            mouseState[0] = true;
            if (mouseState[1] == KEY_IDLE) { mouseState[1] = KEY_HIT; }
            else { mouseState[1] = KEY_PRESSED; }
        }
        else {
            mouseState[0] = false;
            if (mouseState[1] >= KEY_HIT) { mouseState[1] = KEY_RELEASED; }
            else { mouseState[1] = KEY_IDLE; }
        }

        if (RGFW_window_isMousePressed(window, RGFW_mouseRight)) {
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

    void updateMousePosition() { RGFW_window_getMouse(window, &mousePosition[0], &mousePosition[1]); }
    void setMousePosition(i32 x,i32 y) { RGFW_window_moveMouse(window, x, y); }
    float getMouseX() { return mousePosition[0]; }
    float getMouseY() { return mousePosition[1]; }
    bool LMBPressed() { return mouseState[0]; }
    bool RMBPressed() { return mouseState[2]; }
    bool LMBHit() { return mouseState[1] == KEY_HIT; }
    bool RMBHit() { return mouseState[3] == KEY_HIT; }
    bool LMBReleased() { return mouseState[1] == KEY_RELEASED; }
    bool RMBReleased() { return mouseState[3] == KEY_RELEASED; }

private:
    RGFW_window* window;
    unsigned char keys[194] = {0};
    const unsigned short rgfwKeys[97] = {
        /*common   */ RGFW_a, RGFW_s, RGFW_w, RGFW_d, RGFW_space, RGFW_e, RGFW_q, RGFW_z, RGFW_x, RGFW_c, RGFW_t, RGFW_f, RGFW_g, RGFW_h, RGFW_i, RGFW_j, RGFW_k, RGFW_l, RGFW_right, RGFW_left, RGFW_down, RGFW_up, RGFW_v, RGFW_1, RGFW_2, RGFW_3, RGFW_4, RGFW_5, RGFW_6, RGFW_7, RGFW_8, RGFW_9, RGFW_0,
        /*rare     */ RGFW_shiftR, RGFW_shiftL, RGFW_return, RGFW_escape, RGFW_tab, RGFW_backSpace, RGFW_insert, RGFW_delete, RGFW_period, RGFW_slash, RGFW_semicolon, RGFW_comma, RGFW_superL,
        /*legendary*/ RGFW_b, RGFW_m, RGFW_n, RGFW_o, RGFW_p, RGFW_r, RGFW_u, RGFW_y, RGFW_kp0, RGFW_kp1, RGFW_kp2, RGFW_kp3, RGFW_kp4, RGFW_kp5, RGFW_kp6, RGFW_kp7, RGFW_kp8, RGFW_kp9, RGFW_kpPeriod, RGFW_kpSlash, RGFW_kpMultiply, RGFW_kpMinus, RGFW_kpReturn, RGFW_home, RGFW_end, RGFW_capsLock, RGFW_numLock, RGFW_F1, RGFW_F2, RGFW_F3, RGFW_F4, RGFW_F5, RGFW_F6, RGFW_F7, RGFW_F8, RGFW_F9, RGFW_F10, RGFW_F11, RGFW_F12,
        /*mythic   */ RGFW_controlL, RGFW_altL, RGFW_controlR, RGFW_altR, RGFW_superR, RGFW_apostrophe, RGFW_minus, RGFW_equals, RGFW_bracket, RGFW_backSlash, RGFW_pageUp, RGFW_pageDown
    };
    std::vector<KeyBind> keyBinds;
    bool modifiers[2] = {false};
	i32 mousePosition[2];
    unsigned char mouseState[4] = {0};
};