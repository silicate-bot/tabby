#pragma once
#ifndef TABBY_OVERLAY_HPP
#define TABBY_OVERLAY_HPP

#include "util/defines.hpp"

#include <functional>

TABBY_NS_BEGIN

struct OverlayState {
  bool showPopup = false;
};

void overlay(std::function<void()> innerFn, std::function<void()> popupFn, std::function<void()> shaderFn);

TABBY_NS_END

#endif  // TABBY_OVERLAY_HPP
