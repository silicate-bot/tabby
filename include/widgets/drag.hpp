#pragma once
#ifndef TABBY_WIDGETS_DRAG
#define TABBY_WIDGETS_DRAG

#include "util/defines.hpp"
#include "widgets/state.hpp"
#include <type_traits>
#include <string_view>

TABBY_NS_BEGIN

enum class DragType {
    u8,
    u16,
    u32,
    u64,
    i8,
    i16,
    i32,
    i64,
    f32,
    f64
};

template<typename T> requires std::is_arithmetic_v<T>
WidgetState drag(std::string_view label, T& value, T min = 0, T max = 100, float speed = 1.0f, std::string_view format = "{}") {
    if constexpr (std::is_same_v<T, uint8_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::u8, format);
    } else if constexpr (std::is_same_v<T, uint16_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::u16, format);
    } else if constexpr (std::is_same_v<T, uint32_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::u32, format);
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::u64, format);
    } else if constexpr (std::is_same_v<T, int8_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::i8, format);
    } else if constexpr (std::is_same_v<T, int16_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::i16, format);
    } else if constexpr (std::is_same_v<T, int32_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::i32, format);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::i64, format);
    } else if constexpr (std::is_same_v<T, float>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::f32, format);
    } else if constexpr (std::is_same_v<T, double>) {
        return _drag_impl(label, &value, &min, &max, speed, DragType::f64, format);
    } else {
        static_assert(std::is_arithmetic_v<T>, "Unsupported type for drag widget");
    }
}

WidgetState _drag_impl(std::string_view label, void* value, void* min, void* max, float speed, DragType t, std::string_view format);

TABBY_NS_END

#endif
