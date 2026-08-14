/*
 * Copyright (c) 2025 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include "util.h"

LV_IMAGE_DECLARE(bolt);

void rotate_canvas(lv_obj_t *canvas) {
    uint8_t *buf = lv_canvas_get_draw_buf(canvas)->data;
    static uint8_t copy[CANVAS_BUF_SIZE];
    memcpy(copy, buf, sizeof(copy));
    const uint32_t stride = lv_draw_buf_width_to_stride(CANVAS_SIZE, CANVAS_COLOR_FORMAT);
    lv_draw_sw_rotate(copy, buf, CANVAS_SIZE, CANVAS_SIZE, stride, stride,
                      LV_DISPLAY_ROTATION_270, CANVAS_COLOR_FORMAT);
}

void canvas_draw_rect(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                      lv_draw_rect_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_area_t area = {x, y, x + w - 1, y + h - 1};
    lv_draw_rect(&layer, dsc, &area);
    lv_canvas_finish_layer(canvas, &layer);
}

void canvas_draw_text(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t width,
                      lv_draw_label_dsc_t *dsc, const char *text) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    dsc->text = text;
    lv_area_t area = {x, y, x + width, y + CANVAS_SIZE};
    lv_draw_label(&layer, dsc, &area);
    lv_canvas_finish_layer(canvas, &layer);
}

void canvas_draw_img(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, const lv_image_dsc_t *src,
                     lv_draw_image_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    dsc->src = src;
    lv_area_t area = {x, y, x + src->header.w - 1, y + src->header.h - 1};
    lv_draw_image(&layer, dsc, &area);
    lv_canvas_finish_layer(canvas, &layer);
}

void draw_battery(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_rect_dsc_t bg, fg;
    init_rect_dsc(&bg, LVGL_BACKGROUND);
    init_rect_dsc(&fg, LVGL_FOREGROUND);
    canvas_draw_rect(canvas, 0, 2, 29, 12, &fg);
    canvas_draw_rect(canvas, 1, 3, 27, 10, &bg);
    canvas_draw_rect(canvas, 2, 4, (state->battery + 2) / 4, 8, &fg);
    canvas_draw_rect(canvas, 30, 5, 3, 6, &fg);
    canvas_draw_rect(canvas, 31, 6, 1, 4, &bg);
    if (state->charging) {
        lv_draw_image_dsc_t dsc;
        lv_draw_image_dsc_init(&dsc);
        canvas_draw_img(canvas, 9, -1, &bolt, &dsc);
    }
}

void init_label_dsc(lv_draw_label_dsc_t *dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(dsc);
    dsc->color = color;
    dsc->font = font;
    dsc->align = align;
}

void init_rect_dsc(lv_draw_rect_dsc_t *dsc, lv_color_t color) {
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_color = color;
}

void init_line_dsc(lv_draw_line_dsc_t *dsc, lv_color_t color, uint8_t width) {
    lv_draw_line_dsc_init(dsc);
    dsc->color = color;
    dsc->width = width;
}

void init_arc_dsc(lv_draw_arc_dsc_t *dsc, lv_color_t color, uint8_t width) {
    lv_draw_arc_dsc_init(dsc);
    dsc->color = color;
    dsc->width = width;
}

void canvas_draw_line(lv_obj_t *canvas, const lv_point_t points[], uint32_t count,
                      lv_draw_line_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    for (uint32_t i = 1; i < count; i++) {
        dsc->p1.x = points[i - 1].x;
        dsc->p1.y = points[i - 1].y;
        dsc->p2.x = points[i].x;
        dsc->p2.y = points[i].y;
        lv_draw_line(&layer, dsc);
    }
    lv_canvas_finish_layer(canvas, &layer);
}

void canvas_draw_arc(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y, lv_coord_t r,
                     int32_t start, int32_t end, lv_draw_arc_dsc_t *dsc) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    dsc->center.x = x;
    dsc->center.y = y;
    dsc->radius = r;
    dsc->start_angle = start;
    dsc->end_angle = end;
    lv_draw_arc(&layer, dsc);
    lv_canvas_finish_layer(canvas, &layer);
}
