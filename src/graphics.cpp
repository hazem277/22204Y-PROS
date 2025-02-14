#include "main.h"

// Creates generic button based on position, size, and parent object
lv_obj_t *graphics::createButton(lv_obj_t *parent,
                                 lv_coord_t x,
                                 lv_coord_t y,
                                 lv_coord_t width,
                                 lv_coord_t height,
                                 const char *title) {
  // Creates button and attaches to parent object
  lv_obj_t *btn = lv_btn_create(parent);
  // Sets position on parent object
  lv_obj_set_pos(btn, x, y);
  // Sets size of button
  lv_obj_set_size(btn, width, height);

  // Creates text on button and sets it to title
  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, title);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 5);

  return btn;
}

// Creates a tab to be used in a tabview that supports flexible layouts
lv_obj_t *graphics::createFlexTab(lv_obj_t *parent, const char *title) {
  // Creates tab
  lv_obj_t *tab = lv_tabview_add_tab(parent, title);

  // Enables flexible layout
  lv_obj_set_flex_flow(tab, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_flex_main_place(tab, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
  lv_obj_set_style_layout(tab, LV_LAYOUT_FLEX, 0);

  return tab;
}

// Create a bar that has a blue to red gradient for use in displaying
// temperature
lv_obj_t *graphics::createTempBar(lv_obj_t *parent,
                                  int min,
                                  int max,
                                  lv_coord_t width,
                                  lv_coord_t height,
                                  const char *title) {
  // Creates base object for bar and label and sets size
  lv_obj_t *base = lv_obj_create(parent);
  lv_obj_set_size(base, width, height);

  // Gets rid of ugly, unnecessary scrollbar
  lv_obj_clear_flag(base, LV_OBJ_FLAG_SCROLLABLE);

  // Creates bar and sets gradient
  lv_obj_t *bar = lv_bar_create(base);
  lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(
      bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
  lv_obj_set_style_bg_grad_color(
      bar, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
  lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_VER, LV_PART_INDICATOR);

  // Sets size
  lv_obj_set_size(bar, 20, lv_pct(80));

  // Sets value range
  lv_bar_set_range(bar, min, max);

  // Aligns bar to the center top of the base object
  lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);

  // Creates label based on title
  lv_obj_t *label = lv_label_create(base);
  lv_label_set_text(label, title);

  // Aligns the label underneath the bar
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

  return bar;
}

// Sets temp for a temp bar (mostly for formatting neatness)
void graphics::set_temp(lv_obj_t *bar, int32_t temp) {
  lv_bar_set_value(bar, temp, LV_ANIM_ON);
}