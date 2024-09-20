#ifndef UI_H
#define UI_H

#include "nanovg.h"
#include <stdbool.h>

#define MAX_BUTTONS 100
#define MAX_MENU_ITEMS 10
#define MAX_SLIDERS 50
#define MAX_TEXT_INPUTS 50

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
    float x, y, width, height;
    const char* label;
    float min, max;
    float* value;
    bool isHovered;
    bool isDragging;
} Slider;

typedef struct {
    char buffer[64];
    float x, y, width, height;
    const char* label;
    float* value;
    bool isEditing;
} TextInput;

typedef struct {
    Button buttons[MAX_BUTTONS];
    Slider sliders[MAX_SLIDERS];
    TextInput textInputs[MAX_TEXT_INPUTS];
    int sliderCount;
    int buttonCount;
    int textInputCount;
    Menu menu;
    NVGcontext* vg;
} UI;

void ui_init(UI* ui, NVGcontext* vg);
void ui_add_button(UI* ui, float x, float y, float width, float height, const char* label, bool (*onClick)(void));
void ui_add_menu_item(UI* ui, const char* label, void (*action)(void));
void ui_add_slider(UI* ui, float x, float y, float width, float height, const char* label, float minValue, float maxValue, float* value);
void ui_add_text_input(UI* ui, float x, float y, float width, float height, const char* label, float* value);
void ui_draw(UI* ui);
void ui_handle_mouse(UI* ui, float mouseX, float mouseY, bool mousePressed);

#endif