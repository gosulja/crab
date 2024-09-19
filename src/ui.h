#ifndef UI_H
#define UI_H

#include "nanovg.h"
#include <stdbool.h>

#define MAX_BUTTONS 100
#define MAX_MENU_ITEMS 5

typedef struct {
    float x, y, width, height;
    const char* label;
    bool (*onClick)(void);
    bool isHovered;
} Button;

typedef struct {
    const char* label;
    void (*action)(void);
} MenuItem;

typedef struct {
    float x, y, width, height;
    const char* title;
    MenuItem items[MAX_MENU_ITEMS];
    int itemCount;
    bool isOpen;
} Menu;

typedef struct {
    Button buttons[MAX_BUTTONS];
    int buttonCount;
    Menu menu;
    NVGcontext* vg;
} UI;

void ui_init(UI* ui, NVGcontext* vg);
void ui_add_button(UI* ui, float x, float y, float width, float height, const char* label, bool (*onClick)(void));
void ui_add_menu_item(UI* ui, const char* label, void (*action)(void));
void ui_draw(UI* ui);
void ui_handle_mouse(UI* ui, float mouseX, float mouseY, bool mousePressed);

#endif