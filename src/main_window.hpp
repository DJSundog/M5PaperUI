#pragma once

#include <memory>

#include "settings.hpp"
#include "widgetlib.hpp"

class MainWindow : public std::enable_shared_from_this<MainWindow> {

public:
  MainWindow(Settings *s) : settings_(s) {}
  void Start(const WidgetContext::ptr_t &ctx);

private:
  void InitializeTopBar(const WidgetContext::ptr_t &);

  void InitializeBottomBar(const WidgetContext::ptr_t &);

  void InitializeHomeScreen(const WidgetContext::ptr_t &);

  Frame::ptr_t home_screen_;
  Frame::ptr_t top_bar_;
  Frame::ptr_t bottom_bar_;
  Settings *settings_;
};