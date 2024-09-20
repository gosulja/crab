#include "ui.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUTTON_CORNER_RADIUS 5.0f
#define MENU_CORNER_RADIUS 10.0f
#define MENU_ITEM_HEIGHT 30.0f

char* fToString(float* v) {
    char* buffer = (char*)malloc(32 * sizeof(char));
    if (buffer == NULL) {
        return NULL;
    }

    snprintf(buffer, 32, "%.2f", *v);

    return buffer;
}

void ui_init(UI* ui, NVGcontext* vg) {
    ui->buttonCount = 0;
    ui->vg = vg;
    ui->menu.itemCount = 0;
    ui->menu.isOpen = false;
}

void ui_add_button(UI* ui, float x, float y, float width, float height, const char* label, bool (*onClick)(void)) {
    if (ui->buttonCount < MAX_BUTTONS) {
        Button* button = &ui->buttons[ui->buttonCount++];
        button->x = x;
        button->y = y;
        button->width = width;
        button->height = height;
        button->label = label;
        button->onClick = onClick;
        button->isHovered = false;
    }
}

void ui_add_menu_item(UI* ui, const char* label, void (*action)(void)) {
    if (ui->menu.itemCount < MAX_MENU_ITEMS) {
        MenuItem* item = &ui->menu.items[ui->menu.itemCount++];
        item->label = label;
        item->action = action;
    }
}

static void draw_button(NVGcontext* vg, Button* button) {
    nvgBeginPath(vg);
    nvgRoundedRect(vg, button->x, button->y, button->width, button->height, BUTTON_CORNER_RADIUS);
    nvgFillColor(vg, button->isHovered ? nvgRGBA(100, 100, 100, 255) : nvgRGBA(80, 80, 80, 255));
    nvgFill(vg);

    nvgFontSize(vg, 20.0f);
    nvgFontFace(vg, "sans");
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgText(vg, button->x + button->width/2, button->y + button->height/2, button->label, NULL);
}

static void draw_menu(NVGcontext* vg, Menu* menu) {
    if (!menu->isOpen) return;

    // For now, always hide the menu

    float totalHeight = menu->itemCount * MENU_ITEM_HEIGHT + 40.0f;
    
    nvgBeginPath(vg);
    nvgRoundedRect(vg, menu->x, menu->y, menu->width, totalHeight, MENU_CORNER_RADIUS);
    nvgFillColor(vg, nvgRGBA(60, 60, 60, 230));
    nvgFill(vg);

    nvgFontSize(vg, 24.0f);
    nvgFontFace(vg, "sans");
    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgText(vg, menu->x + menu->width/2, menu->y + 20.0f, menu->title, NULL);

    nvgFontSize(vg, 20.0f);
    for (int i = 0; i < menu->itemCount; i++) {
        float itemY = menu->y + 40.0f + i * MENU_ITEM_HEIGHT;
        nvgFillColor(vg, nvgRGBA(200, 200, 200, 255));
        nvgText(vg, menu->x + menu->width/2, itemY + MENU_ITEM_HEIGHT/2, menu->items[i].label, NULL);
    }
}

void ui_add_slider(UI* ui, float x, float y, float width, float height, const char* label, float minValue, float maxValue, float* value) {
    if (ui->sliderCount < MAX_SLIDERS) {
        Slider* slider = &ui->sliders[ui->sliderCount++];
        slider->x = x;
        slider->y = y;
        slider->width = width;
        slider->height = height;
        slider->label = label;
        slider->min = minValue;
        slider->max = maxValue;
        slider->value = value;
    }
}

void ui_draw_slider(UI* ui, Slider* slider) {
    nvgFillColor(ui->vg, nvgRGB(255, 255, 255));

    nvgTextAlign(ui->vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ui->vg, slider->x, slider->y - 5, slider->label, NULL);

    nvgBeginPath(ui->vg);
    nvgRect(ui->vg, slider->x, slider->y + 10, slider->width, slider->height);
    nvgFillColor(ui->vg, nvgRGB(100, 100, 100));
    nvgFill(ui->vg);

    float normalizedValue = (*slider->value - slider->min) / (slider->max - slider->min);
    float sliderPos = slider->x + normalizedValue * slider->width;

    nvgBeginPath(ui->vg);
    nvgRect(ui->vg, sliderPos - 5, slider->y + 10, 10, slider->height);
    nvgFillColor(ui->vg, nvgRGB(70, 70, 70));
    nvgFill(ui->vg);

    nvgTextAlign(ui->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgFillColor(ui->vg, nvgRGB(255, 255, 255));
    nvgText(ui->vg, slider->x + (slider->width / 2), slider->y + slider->height, fToString(slider->value), NULL);
}



void ui_draw(UI* ui) {
    for (int i = 0; i < ui->buttonCount; i++) {
        draw_button(ui->vg, &ui->buttons[i]);
    }

    for (int i = 0; i < ui->sliderCount; i++) {
        ui_draw_slider(ui, &ui->sliders[i]);
    }

    // for (int i = 0; i < ui->textInputCount; i++) {
    //     ui_draw_slider(ui, &ui->textInputs[i]);
    // }

    draw_menu(ui->vg, &ui->menu);
}

void ui_handle_mouse(UI* ui, float mouseX, float mouseY, bool mousePressed) {
    for (int i = 0; i < ui->buttonCount; i++) {
        Button* button = &ui->buttons[i];
        bool isInside = mouseX >= button->x && mouseX <= button->x + button->width &&
                        mouseY >= button->y && mouseY <= button->y + button->height;
        
        button->isHovered = isInside;
        
        if (isInside && mousePressed && button->onClick) {
            button->onClick();
        }
    }

    for (int i = 0; i < ui->sliderCount; i++) {
        Slider* slider = &ui->sliders[i];

        bool isInside = mouseX >= slider->x && mouseX <= slider->x + slider->width &&
                        mouseY >= slider->y && mouseY <= slider->y + slider->height + 20;

        if (isInside && mousePressed) {
            slider->isDragging = true;
        }

        if (slider->isDragging) {
            float newValue = slider->min + ((mouseX - slider->x) / slider->width) * (slider->max - slider->min);
            
            if (newValue < slider->min) newValue = slider->min;
            if (newValue > slider->max) newValue = slider->max;

            *slider->value = newValue;
        }

        if (!mousePressed) {
            slider->isDragging = false;
        }
    }

    if (ui->menu.isOpen) {
        bool isInsideMenu = mouseX >= ui->menu.x && mouseX <= ui->menu.x + ui->menu.width &&
                            mouseY >= ui->menu.y && mouseY <= ui->menu.y + (ui->menu.itemCount * MENU_ITEM_HEIGHT + 40.0f);
        
        if (isInsideMenu && mousePressed) {
            int selectedItem = (mouseY - ui->menu.y - 40.0f) / MENU_ITEM_HEIGHT;
            if (selectedItem >= 0 && selectedItem < ui->menu.itemCount) {
                ui->menu.items[selectedItem].action();
            }
        } else if (!isInsideMenu && mousePressed) {
            ui->menu.isOpen = false;
        }
    }
}