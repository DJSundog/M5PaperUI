#include "widget_label.hpp"
#include "Free_Fonts.h"

void Label::Init() {
  Widget::Init();
  canvas_->setTextDatum(convert_alignment());
  canvas_->setTextSize(text_size_);
  // Default the text color to Black.
  canvas_->setTextColor(Grayscale::G15);
  // canvas_->setFreeFont(FF17);

  // if the widget is drawn with border, we need to adjust the drawing cursor
  // position.
  int16_t border_offset = 0;
  if (widget_style_ == WidgetStyle::BORDER ||
      widget_style_ == WidgetStyle::FILL_W_BORDER) {
    border_offset += padding_;
  }
  // The canvas is created according to the x and y positions, which means, only
  // border adjustment is necessary.
  int16_t x = has_own_canvas_ ? 0 : x_;
  int16_t y = has_own_canvas_ ? 0 : y_;

  log_d("Label offset %d %d", border_offset, widget_style_);

  // We have to adjust the drawing position by half of the width;
  if (h_align_ == MIDDLE) {
    x = x + border_offset + width_ / 2;
  } else {
    x += border_offset;
  }

  if (v_align_ == CENTER) {
    y = y + border_offset + height_ / 2;
  } else {
    y += border_offset;
  }

  canvas_->drawString(text_.c_str(), x, y);
}

uint8_t Label::convert_alignment() const {
  if (v_align_ == TOP) {
    if (h_align_ == LEFT) {
      return TL_DATUM;
    } else if (h_align_ == MIDDLE) {
      return TC_DATUM;
    } else {
      // Right
      return TR_DATUM;
    }
  } else if (v_align_ == CENTER) {
    if (h_align_ == LEFT) {
      return CL_DATUM;
    } else if (h_align_ == MIDDLE) {
      return CC_DATUM;
    } else {
      // Right
      return CR_DATUM;
    }
  } else {
    // Bottom
    if (h_align_ == LEFT) {
      return BL_DATUM;
    } else if (h_align_ == MIDDLE) {
      return BC_DATUM;
    } else {
      // Right
      return BR_DATUM;
    }
  }
}