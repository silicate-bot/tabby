#include "widgets/input.hpp"
#include "imgui_internal.h"
#include "imgui.h"
#include "widgets/animated.hpp"

TABBY_NS_BEGIN

struct InputTextData {
    std::string* buffer;
};

static int historyCallback(ImGuiInputTextCallbackData* data) {
    InputTextData* inputData = static_cast<InputTextData*>(data->UserData);
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        inputData->buffer->resize(data->BufTextLen);
        data->Buf = (char*)inputData->buffer->c_str();
    }

    return 0;
}

WidgetState input_text(
    std::string_view label,
    std::string_view hint,
    std::string& buffer
) {
    InputTextData inputData = { &buffer };

    if (ImGui::InputTextEx(
        label.data(),
        hint.data(),
        buffer.data(),
        buffer.capacity() + 1,
        ImVec2(0.0f, 0.0f),
        ImGuiInputTextFlags_CallbackResize,
        historyCallback,
        &inputData
    )) {
        return WidgetState {
            .pressed = ImGui::IsItemActivated(),
            .hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenOverlappedByItem),
            .held = ImGui::IsItemActive(),
            .changed = true
        };
    }

    return WidgetState {
        .pressed = ImGui::IsItemActivated(),
        .hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenOverlappedByItem),
        .held = ImGui::IsItemActive(),
        .changed = false
    };
}

static int inputCallback(ImGuiInputTextCallbackData* data) {
    AutocompleteState* state = static_cast<AutocompleteState*>(data->UserData);
    if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        state->showPopup = true;

        if (data->EventKey == ImGuiKey_UpArrow) {
            if (state->selectedIndex > 0) {
                state->selectedIndex--;
                state->needsScroll = true;
            }
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (state->selectedIndex < static_cast<int>(state->suggestions.size()) - 1) {
                state->selectedIndex++;
                state->needsScroll = true;
            }
        }

    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        state->buffer->resize(data->BufTextLen);
        data->Buf = (char*)state->buffer->c_str();
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        state->showPopup = true;
    }

    return 0;
}

static void drawPopup(
    std::string_view label,
    AutocompleteState& state,
    const ImVec2& pos,
    const ImVec2& size,
    std::function<void()> renderPopup = nullptr
) {
    struct WindowSize {
        float width;
        float height;
        float opacity;
    };

    WindowSize& w = tabby::getStoredAnimValue(std::string(label) + std::string("_popup"), WindowSize{ 0.0f, 0.0f, 1.0f });
    if (!state.showPopup || state.suggestions.empty()) {
        w.height = tabby::interpolateValue(w.height, 0.0f);
        w.opacity = tabby::interpolateValue(w.opacity, 0.0f);
        return;
    }

    w.height = tabby::interpolateValue(w.height, size.y);
    w.opacity = tabby::interpolateValue(w.opacity, 1.0f);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x, w.height));

    // float opacity = tabby::getStoredAnimValue("INTERNAL_autocomplete_opacity", 0.0f);
    // opacity = tabby::interpolateValue(opacity, state.showPopup ? 1.0f : 0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, w.opacity);

    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f * uiScale, 12.0f * uiScale));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f * uiScale);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, size);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 12.0f * uiScale, 6.0f * uiScale });
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    // ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar          |
        ImGuiWindowFlags_NoResize            |
        ImGuiWindowFlags_NoMove              |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_Tooltip;

    ImGui::Begin("Autocomplete Suggestions", nullptr, flags);

    if (renderPopup) {
        renderPopup();
    }

    // enumerate suggestions
    for (size_t i = 0; i < state.suggestions.size(); ++i) {
        auto& suggestion = state.suggestions.at(i);
        bool isSelected = (state.selectedIndex == static_cast<int>(i));
        bool isHighlighted = (state.highlightedIndex == static_cast<int>(i));

        if (isSelected || isHighlighted) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        }

        if (isSelected && state.needsScroll) {
            ImGui::SetScrollHereY();
            state.needsScroll = false;
        }

        ImGui::PushID(static_cast<int>(i)); // Ensure unique ID for each suggestion

        if (ImGui::Selectable(suggestion.c_str(), false, ImGuiSelectableFlags_NoAutoClosePopups)) {
            printf("Selected suggestion: %s\n", suggestion.c_str());
            state.buffer->clear();
            state.buffer->append(suggestion);
            state.showPopup = false;
        }

        if (ImGui::IsItemHovered()) {
            state.highlightedIndex = static_cast<int>(i);
        } else if (state.highlightedIndex == static_cast<int>(i) && !ImGui::IsItemHovered()) {
            state.highlightedIndex = -1;
        }

        ImGui::PopID();

        // if (isSelected) {
            // ImGui::SetScrollHereY();
        // }

        ImGui::PopStyleColor();
    }

    // close if clicking outside the popup
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0)) {
        state.showPopup = false;
        state.selectedIndex = -1;
        state.clickedIndex = -1;
        state.highlightedIndex = -1;
        state.needsScroll = false;
    }

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
        state.showPopup = false;
        state.selectedIndex = -1;
        state.clickedIndex = -1;
        state.highlightedIndex = -1;
        state.needsScroll = false;
    }

    ImGui::End();
    ImGui::PopStyleVar(5);
    // ImGui::PopStyleColor();
}

WidgetState input_text_autocomplete(
    std::string_view label,
    std::string_view hint,
    std::string& buffer,
    AutocompleteState& state,
    std::function<void()> renderPopup
) {
    WidgetState widgetState = WidgetState::idle();
    state.buffer = &buffer;

    if (ImGui::InputTextEx(
        label.data(),
        hint.data(),
        buffer.data(),
        buffer.size() + 1,
        ImVec2(0.0f, 0.0f),
        ImGuiInputTextFlags_CallbackResize |
        ImGuiInputTextFlags_CallbackCompletion |
        ImGuiInputTextFlags_CallbackHistory |
        ImGuiInputTextFlags_CallbackEdit |
        ImGuiInputTextFlags_EnterReturnsTrue,
        inputCallback,
        &state
    )) {
        if (state.selectedIndex != -1) {
            state.buffer->clear();
            state.buffer->append(state.suggestions.at(state.selectedIndex));
        }

        state.showPopup = false;
        state.selectedIndex = -1;
        state.highlightedIndex = -1;
        state.clickedIndex = -1;
        state.needsScroll = false;
    }

    if (ImGui::IsItemEdited()) {
        widgetState.changed = true;
    }

    bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenOverlappedByItem);

    if (ImGui::IsItemActive()) {
        state.showPopup = true;
    }

    bool lostFocus = !ImGui::IsItemActive();

    if (ImGui::GetItemRectMin().y < ImGui::GetCurrentWindow()->Rect().GetTL().y ||
        ImGui::GetItemRectMax().y > ImGui::GetCurrentWindow()->Rect().GetBL().y) {
        state.showPopup = false;
        state.selectedIndex = -1;
        state.highlightedIndex = -1;
        state.clickedIndex = -1;
        state.needsScroll = false;
    }

    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    drawPopup(
        label,
        state,
        ImVec2((ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) / 2.0f, ImGui::GetItemRectMax().y + ImGui::GetStyle().ItemSpacing.y),
        ImVec2(ImGui::GetItemRectSize().x / 2.0f, ImGui::GetTextLineHeight() * 4 + 6.0f * 4 * uiScale + 24.0f * uiScale), // Show 5 lines of suggestions
        renderPopup
    );

    bool hovering = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

    // if (lostFocus && !hovering) {
    //     state.showPopup = false;
    //     state.selectedIndex = -1;
    //     state.clickedIndex = -1;
    // }

    return WidgetState {
        .pressed = ImGui::IsItemActivated(),
        .hovered = hovered,
        .held = ImGui::IsItemActive(),
        .changed = widgetState.changed
    };
}

TABBY_NS_END
