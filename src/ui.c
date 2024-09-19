#include "ui.h"
#include <string.h>

#define BUTTON_CORNER_RADIUS 5.0f
#define MENU_CORNER_RADIUS 10.0f
#define MENU_ITEM_HEIGHT 30.0f

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
    // if (!menu->isOpen) return;

    // For now, always show the menu

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

void ui_draw(UI* ui) {
    for (int i = 0; i < ui->buttonCount; i++) {
        draw_button(ui->vg, &ui->buttons[i]);
    }
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