#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <M5EPD.h>

enum class WidgetState { PRE, UDPATE, POST };

class ScreenUpdateMode {
public:
  enum Value : int16_t { NONE = 0, PARTIAL = 1, FULL = 2 };
  constexpr ScreenUpdateMode(Value val) : value_(val) {}
  ScreenUpdateMode &operator|=(const ScreenUpdateMode &other) {
    value_ = static_cast<Value>(static_cast<int16_t>(value_) |
                                static_cast<int16_t>(other.value_));
    return *this;
  }
  constexpr bool operator==(ScreenUpdateMode a) const {
    return value_ == a.value_;
  }

private:
  Value value_;
};

class WidgetContext;

// Forward declaration.
class Widget;

class View {
public:
  using ptr_t = std::shared_ptr<View>;

  virtual void Prepare(WidgetContext *) {}

  virtual void Init(WidgetContext *) = 0;

  /// Returns true if a full update is needed or if a partial update was done
  /// manually.
  virtual ScreenUpdateMode Draw() = 0;

  /// This function is called by the UI main loop when an event is executed. If
  /// the function returns true, this frame can respond to the event.
  virtual bool EventInside(int16_t x, int16_t y) const = 0;

  /// This function is called by the UI main loop to handle a specific touch
  /// event. The touch event contains necessary information about the state of
  /// the touch and some of its raw parameters
  virtual void HandleEvent(TouchEvent evt) = 0;

  inline void Name(const std::string n) { name_ = n; }

  inline std::string name() const { return name_; }

  inline virtual void Reset() {}

private:
  std::string name_;
};

/// The frame is the center piece when displaying content on the screen. A frame
/// represents the canvas that is drawn on the screen. Different frames can be
/// stacked to display content.
///
/// Each frame contains a list of Widgets (@see Widget) that is displayed. Each
/// Widget is initialized and drawn when needed.
class Frame : public View {
public:
  using ptr_t = std::shared_ptr<Frame>;

  struct Rect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    std::string str() {
      std::ostringstream buf("Rect<");
      buf << "x=" << x << ",y=" << y << ",w=" << w << ",h=" << h << ">";
      return buf.str();
    }
  };

  Frame(int16_t x, int16_t y, int16_t w, int16_t h) : Frame() {
    x_ = x, y_ = y, width_ = w, height_ = h;
  }

  /// Initialize the Frame.
  Frame() : canvas_(&M5.EPD) {}

  static ptr_t Create(int16_t x, int16_t y, int16_t w, int16_t h) {
    const auto &ptr = std::make_shared<Frame>(x, y, w, h);
    return ptr;
  }

  /// Initializes all views that have been added to the frame.
  virtual void Init(WidgetContext *) override;

  /// Is called to draw all view elements. View elements are only drawn if
  /// dirty.
  virtual ScreenUpdateMode Draw() override;

  /// Allows setting the update mode for the display. Different update modes
  /// have different properties with regards to refresh time and ghosting on the
  /// display.
  inline void UpdateMode(m5epd_update_mode_t u) { update_mode_ = u; }

  /// Adds a new widget to the list.
  void AddWidget(const std::shared_ptr<Widget> &w);

  /// Return the update mode for the frame.
  m5epd_update_mode_t update_mode() const { return update_mode_; }

  /// Children can update the view state of the frame to require re-drawing the
  /// whole screen.
  inline bool NeedsRedraw() { return state_ == WidgetState::PRE; }

  /// Helper function that will flip the flag and require a redraw of the canvas
  /// of the frame.
  inline void RequireRedraw() { state_ = WidgetState::PRE; }

  bool EventInside(int16_t x, int16_t y) const override;

  void HandleEvent(TouchEvent evt) override;

  Rect dimension() const { return {x_, y_, width_, height_}; }


protected:
  int16_t x_;
  int16_t y_;
  int16_t width_;
  int16_t height_;

  /// Update mode for this widget.
  m5epd_update_mode_t update_mode_ = UPDATE_MODE_NONE;

  /// The list of all widgets that belong to this frame.
  std::vector<std::shared_ptr<Widget>> widgets_;

  WidgetState state_ = WidgetState::PRE;

  /// This is a shared canvas. When used, it is pushed first before any other
  /// canvas is used. A shared canvas is used to allow placement of multiple
  /// widgets at the same time. The reason to separate between shared and
  /// non-shared canvas is that a canvas can only be fully updated. When partial
  /// updates are needed like in a text box or button, an independent canvas
  /// should be used.
  M5EPD_Canvas canvas_;

  bool initialized_ = false;
};