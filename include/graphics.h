#include "liblvgl/lvgl.h"  // IWYU pragma: keep

#ifndef GRAPHICS_H
#define GRAPHICS_H

namespace graphics {
lv_obj_t *createButton(lv_obj_t *parent,
                       lv_coord_t x,
                       lv_coord_t y,
                       lv_coord_t width,
                       lv_coord_t height,
                       const char *title);

lv_obj_t *createTempBar(lv_obj_t *parent,
                        int min,
                        int max,
                        lv_coord_t width,
                        lv_coord_t height,
                        const char *title);

lv_obj_t *createFlexTab(lv_obj_t *parent, const char *title);

void set_temp(lv_obj_t *bar, int32_t temp);
}  // namespace graphics

#endif