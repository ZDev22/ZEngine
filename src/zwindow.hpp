/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

v2.8.2

zwindow.hpp is a lightweight cross-platform single-header cpp window abstraction library built off the latest RGFW
works best with zengine, but can be a solo library

HOW-TO-USE:
ZWindow zwindow{windowdata};
zwindow.keyHit(RGFW_a);
*/

#pragma once

#define RGFW_VULKAN
#include "deps/RGFW.h"

#include <string.h>
#include <vector>

struct ZWindow {
public:
    // ZWINDOW
    ZWindow(RGFW_window* window) : window(window) {}

    // KEYBOARD --------------------------------------------------------------------------

    // Keys
    inline bool keyPressed(const unsigned char key) const { return RGFW_isKeyDown(key); } /* check if a key is pressed */
    inline bool keyHit(const unsigned char key) const { return RGFW_isKeyPressed(key); } /* check if a key is hit */
    inline bool keyReleased(const unsigned char key) const { return RGFW_isKeyReleased(key); } /* check if a key is released */
    bool keysPressed(const unsigned char* keysArray, const unsigned char numKeys) const { for (unsigned char i = 0; i < numKeys; i++) { if (!RGFW_isKeyDown(keysArray[i])) return false; } return true; } /* pass an array of keys and check if they are all pressed */
    bool keysHit(const unsigned char* keysArray, const unsigned char numKeys) const { for (unsigned char i = 0; i < numKeys; i++) { if (!RGFW_isKeyPressed(keysArray[i])) return false; } return true; } /* pass an array of keys and check if they are all hit */
    bool keysReleased(const unsigned char* keysArray, const unsigned char numKeys) const { for (unsigned char i = 0; i < numKeys; i++) { if (!RGFW_isKeyReleased(keysArray[i])) return false; } return true; } /* pass an array of keys and check if they are all released */
    bool anyKeyPressed() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyDown(Keys)) return true; } return false; } /* check if any key is pressed */
    bool anyKeyHit() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyPressed(Keys)) return true; } return false; } /* check if any key is hit */
    bool anyKeyReleased() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyReleased(Keys)) return true; } return false; } /* check if any key is released */
    unsigned char whatKeyPressed() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyDown(Keys)) return Keys; } return -1; } /* find what key is being pressed */
    unsigned char whatKeyHit() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyPressed(Keys)) return Keys; } return -1; } /* find what key is being hit */
    unsigned char whatKeyReleased() { for (auto Keys : rgfwKeys) { if (RGFW_isKeyReleased(Keys)) return Keys; } return -1; } /* find what key is being released*/
    std::vector<unsigned char> whatKeysPressed() { /* find all keys being pressed */
        std::vector<unsigned char> keysPressed;
        for (auto Keys : rgfwKeys) { if (RGFW_isKeyDown(Keys)) keysPressed.push_back(Keys); }
        return keysPressed;
    }
    std::vector<unsigned char> whatKeysHit() { /* find all keys being hit */
        std::vector<unsigned char> keysHit;
        for (auto Keys : rgfwKeys) { if (RGFW_isKeyPressed(Keys)) keysHit.push_back(Keys); }
        return keysHit;
    }
    std::vector<unsigned char> whatKeysReleased() { /* find all keys being released*/
        std::vector<unsigned char> keysReleased;
        for (auto Keys : rgfwKeys) { if (RGFW_isKeyReleased(Keys)) keysReleased.push_back(Keys);}
        return keysReleased;
    }

    // Keybind utilities
    struct KeyBind {
        unsigned char key;
        unsigned char rebind;
    };

    inline void addKeyBind(KeyBind keyBind) { keyBinds.push_back(keyBind); } /* add a keybind */
    void addKeyBind(unsigned char key, unsigned char rebind) { /* add a keybind */
        KeyBind keyBind;
        keyBind.key = key;
        keyBind.rebind = rebind;
        keyBinds.push_back(keyBind);
    }
    inline void deleteKeyBind(unsigned char index) { keyBinds.erase(keyBinds.begin() + index); } /* delete a keyBind */
    inline void deleteAllKeyBinds() { keyBinds.clear(); } /* clear all keybinds */
    void replaceKeyBind(unsigned char key, KeyBind newKeyBind) { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) { keyBinds[i] = newKeyBind; break; }}} /* replace a keybind */
    bool isKeyBound(unsigned char key) { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) return true; } return false; } /* check if there is a keybind for a specific key */
    unsigned char findKeyBind(unsigned char key) { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].key == key) return i; } return 0; } /* find the index of a key that was bound */
    unsigned char findKeyReBind(unsigned char key) { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (keyBinds[i].rebind == key) return i; } return 0; } /* find the index of a bounded key */

    // Keybinds
    inline bool keyPressedKeyBind(unsigned char index) { return RGFW_isKeyDown(keyBinds[index].rebind); } /* check if a keybind is pressed */
    inline bool keyHitKeyBind(unsigned char index) { return RGFW_isKeyPressed(keyBinds[index].rebind); } /* check if a keyBind is hit */
    inline bool keyReleasedKeyBind(unsigned char index) { return RGFW_isKeyReleased(keyBinds[index].rebind); } /* check if a keyBind is released */
    bool anyKeyBindPressed() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyDown(keyBinds[i].rebind)) return true; } return false; } /* check if any key that was bound is pressed */
    bool anyKeyReBindPressed() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyDown(keyBinds[i].key)) return true; } return false; } /* check if any bound attatched to a key is pressed */
    bool anyKeyBindHit() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyPressed(keyBinds[i].rebind)) return true; } return false; } /* check if any key that was bound is hit */
    bool anyKeyReBindHit() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyPressed(keyBinds[i].key)) return true; } return false; } /* check if any bound attatched to a key is hit */
    bool anyKeyBindReleased() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyReleased(keyBinds[i].rebind)) return true; } return false; } /* check if any key that was bound is released */
    bool anyKeyReBindReleased() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyReleased(keyBinds[i].key)) return true; } return false; } /* check if any bound attatched to a key is released */
    unsigned char whatKeyBindPressed() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyDown(keyBinds[i].rebind)) return keyBinds[i].key; } return -1; } /* check what key that was bound is pressed */
    unsigned char whatKeyReBindPressed() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyDown(keyBinds[i].key)) return keyBinds[i].rebind; } return -1; } /* check what bound attatched to a key is pressed */
    unsigned char whatKeyBindHit() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyPressed(keyBinds[i].rebind)) return keyBinds[i].key; } return -1; } /* check what key that was bound is hit */
    unsigned char whatKeyReBindHit() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyPressed(keyBinds[i].key)) return keyBinds[i].rebind; } return -1; } /* check what bound attatched to a key is hit */
    unsigned char whatKeyBindReleased() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyReleased(keyBinds[i].rebind)) return keyBinds[i].key; } return -1; } /* check what key that was bound is released */
    unsigned char whatKeyReBindReleased() { for (unsigned char i = 0; i < keyBinds.size(); i++) { if (RGFW_isKeyReleased(keyBinds[i].key)) return keyBinds[i].rebind; } return -1; } /* check what bound attatched to a key is released */

    // Modifiers
    void updateModifiers() {
        if (RGFW_isKeyPressed(RGFW_capsLock)) { modifiers[0] = !modifiers[0]; }
        if (RGFW_isKeyPressed(RGFW_numLock)) { modifiers[1] = !modifiers[1]; }
    }
    inline bool capsLock() { return modifiers[0]; }
    inline bool numLock() { return modifiers[1]; }

    // MOUSE --------------------------------------------------------------------------

    // Position
    inline void setMousePosition(i32 x, i32 y) { RGFW_window_moveMouse(window, x, y); }
    inline void updateMousePosition() { RGFW_window_getMouse(window, &mousePos[0], &mousePos[1]); }
    inline i32 getMouseX() { return mousePos[0]; }
    inline i32 getMouseY() { return mousePos[1]; }

    // Clicks
    inline bool LMBPressed() { return RGFW_isMouseDown(RGFW_mouseLeft); }
    inline bool RMBPressed() { return RGFW_isMouseDown(RGFW_mouseRight); }
    inline bool LMBHit() { return RGFW_isMousePressed(RGFW_mouseLeft); }
    inline bool RMBHit() { return RGFW_isMousePressed(RGFW_mouseRight); }
    inline bool LMBReleased() { return RGFW_isMouseReleased(RGFW_mouseLeft); }
    inline bool RMBReleased() { return RGFW_isMouseReleased(RGFW_mouseRight); }

    // Textures
    inline void setMouseTexture(u8* pixels, i32 size) { RGFW_loadMouse(pixels, size, size, RGFW_formatRGBA8); }
    inline void setMouseTexture(RGFW_mouse* mouse) { RGFW_window_setMouse(window, mouse); }

    // Capture
    inline void captureMouse(bool state) { RGFW_window_captureMouse(window, state); }
    inline void holdMouse() { RGFW_window_holdMouse(window); }
    inline void unholdMouse() { RGFW_window_unholdMouse(window); }
    inline bool isCaptured() { return RGFW_window_isCaptured(window); }

    // Visibility
    inline void showMouse(bool show) { RGFW_window_showMouse(window, show); }
    inline bool isMouseHidded() { return RGFW_window_isMouseHidden(window); }
    inline bool didMouseLeave() { return RGFW_window_didMouseLeave(window); }
    inline bool didMouseEnter() { return RGFW_window_didMouseEnter(window); }
    inline bool isMouseInside() { return RGFW_window_isMouseInside(window); }

    // WINDOW --------------------------------------------------------------------------

    // Position & scale
    inline void move(i32 x, i32 y) { RGFW_window_move(window, x, y); }
    inline void resize(i32 x, i32 y) { RGFW_window_resize(window, x, y); }
    inline void center() { RGFW_window_center(window); }
    inline void setMinSize(i32 x, i32 y) { RGFW_window_setMinSize(window, x, y); }
    inline void setMaxSize(i32 x, i32 y) { RGFW_window_setMaxSize(window, x, y); }
    inline void setAspectRatio(i32 x, i32 y) { RGFW_window_setAspectRatio(window, x, y); }

    // State
    inline void focus() { RGFW_window_focus(window); }
    inline void raise() { RGFW_window_raise(window); }
    inline void maximize() { RGFW_window_maximize(window); }
    inline void minimize() { RGFW_window_minimize(window); }
    inline void restore() { RGFW_window_restore(window); }
    inline void fullscreen() { RGFW_window_setFullscreen(window, true); }
    inline void setFloat(bool shouldFloat) { RGFW_window_setFloating(window, shouldFloat); }
    inline void setBorder(bool border) { RGFW_window_setBorder(window, border); }
    inline void setOpacity(u8 opacity) { RGFW_window_setOpacity(window, opacity); }
    inline bool isInFocus() { return RGFW_window_isInFocus(window); }
    inline bool isFullscreen() { return RGFW_window_isFullscreen(window); }
    inline bool isBorderless() { return RGFW_window_borderless(window); }

    // Visibility
    inline void setName(const char* name) { RGFW_window_setName(window, name); }
    inline void setIcon(u8* data, i32 width, i32 height) { RGFW_window_setIcon(window, data, width, height, RGFW_formatRGBA8); }
    inline void hide() { RGFW_window_hide(window); }
    inline void show() { RGFW_window_show(window); }
    inline void close() { RGFW_window_close(window); }

private:
    RGFW_window* window;
    unsigned char rgfwKeys[97] = { /* ordered by order of rarities so more common keys get checked for first in loops */
        /* common    */ RGFW_a, RGFW_s, RGFW_w, RGFW_d, RGFW_space, RGFW_e, RGFW_q, RGFW_z, RGFW_x, RGFW_c, RGFW_t, RGFW_f, RGFW_g, RGFW_h, RGFW_i, RGFW_j, RGFW_k, RGFW_l, RGFW_right, RGFW_left, RGFW_down, RGFW_up, RGFW_v, RGFW_1, RGFW_2, RGFW_3, RGFW_4, RGFW_5, RGFW_6, RGFW_7, RGFW_8, RGFW_9, RGFW_0,
        /* rare      */ RGFW_shiftR, RGFW_shiftL, RGFW_return, RGFW_escape, RGFW_tab, RGFW_backSpace, RGFW_insert, RGFW_delete, RGFW_period, RGFW_slash, RGFW_semicolon, RGFW_comma, RGFW_superL,
        /* legendary */ RGFW_b, RGFW_m, RGFW_n, RGFW_o, RGFW_p, RGFW_r, RGFW_u, RGFW_y, RGFW_kp0, RGFW_kp1, RGFW_kp2, RGFW_kp3, RGFW_kp4, RGFW_kp5, RGFW_kp6, RGFW_kp7, RGFW_kp8, RGFW_kp9, RGFW_kpPeriod, RGFW_kpSlash, RGFW_kpMultiply, RGFW_kpMinus, RGFW_kpReturn, RGFW_home, RGFW_end, RGFW_capsLock, RGFW_numLock, RGFW_F1, RGFW_F2, RGFW_F3, RGFW_F4, RGFW_F5, RGFW_F6, RGFW_F7, RGFW_F8, RGFW_F9, RGFW_F10, RGFW_F11, RGFW_F12,
        /* mythic    */ RGFW_controlL, RGFW_altL, RGFW_controlR, RGFW_altR, RGFW_superR, RGFW_apostrophe, RGFW_minus, RGFW_equals, RGFW_bracket, RGFW_backSlash, RGFW_pageUp, RGFW_pageDown
    };
    std::vector<KeyBind> keyBinds;
    bool modifiers[2] = {false};
    i32 mousePos[2] = {0};
};