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

void input_text(
    std::string_view label,
    std::string_view hint,
    std::string& buffer
) {
    InputTextData inputData = { &buffer };

    ImGui::InputTextEx(
        label.data(), 
        hint.data(), 
        buffer.data(), 
        buffer.capacity() + 1,
        ImVec2(0.0f, 0.0f), 
        ImGuiInputTextFlags_CallbackResize, 
        historyCallback,
        &inputData
    );
}

static int inputCallback(ImGuiInputTextCallbackData* data) {
    AutocompleteState* state = static_cast<AutocompleteState*>(data->UserData);
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {
        // if (state->showPopup && state->selectedIndex != -1) {
        //     state->buffer->clear();
        //     state->buffer->append(state->suggestions.at(state->selectedIndex));

        //     printf("length: %zu\n", state->buffer->size());
        //     memcpy(data->Buf, state->buffer->c_str(), state->buffer->length() + 1);
        //     data->BufTextLen = state->buffer->length();
        //     data->BufDirty = true;

        //     state->showPopup = false;
        //     state->selectedIndex = -1;

        //     // deactivate input
        //     ImGui::SetActiveID(0, 0);
        // }
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        state->showPopup = true;

        if (data->EventKey == ImGuiKey_UpArrow) {
            if (state->selectedIndex > 0) {
                state->selectedIndex--;
            }
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (state->selectedIndex < static_cast<int>(state->suggestions.size()) - 1) {
                state->selectedIndex++;
            }
        }

    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        state->buffer->resize(data->BufTextLen);
        data->Buf = (char*)state->buffer->c_str();
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways) {
        // if (state->clickedIndex != -1) {
        //     state->buffer->clear();
        //     state->buffer->append(state->suggestions.at(state->clickedIndex));

        //     memcpy(data->Buf, state->buffer->c_str(), state->buffer->size() + 1);
        //     data->BufTextLen = state->buffer->size();
        //     data->BufDirty = true;

        //     state->clickedIndex = -1;
        //     state->showPopup = false;
        // }
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        state->showPopup = true;
    }

    return 0;
}

static void drawPopup(
    AutocompleteState& state,
    const ImVec2& pos, 
    const ImVec2& size
) {
    if (!state.showPopup || state.suggestions.empty()) {
        return;
    }

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    // float opacity = tabby::getStoredAnimValue("INTERNAL_autocomplete_opacity", 0.0f);
    // opacity = tabby::interpolateValue(opacity, state.showPopup ? 1.0f : 0.0f);

    // ImGui::PushStyleVar(ImGuiStyleVar_Alpha, opacity);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));




    ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoTitleBar          | 
        ImGuiWindowFlags_NoResize            |
        ImGuiWindowFlags_NoMove              |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_Tooltip;

    ImGui::Begin("Autocomplete Suggestions", nullptr, flags);

    // enumerate suggestions
    for (size_t i = 0; i < state.suggestions.size(); ++i) {
        auto& suggestion = state.suggestions.at(i);
        bool isSelected = (state.selectedIndex == static_cast<int>(i));        

        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        }

        ImGui::PushID(static_cast<int>(i)); // Ensure unique ID for each suggestion

        if (ImGui::Selectable(suggestion.c_str(), false, ImGuiSelectableFlags_NoAutoClosePopups)) {
            printf("Selected suggestion: %s\n", suggestion.c_str());
            state.buffer->clear();
            state.buffer->append(suggestion);
            state.showPopup = false;
        }

        if (ImGui::IsItemHovered()) {
            state.selectedIndex = static_cast<int>(i);
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
    }

    ImGui::End();
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor();
}

void input_text_autocomplete(
    std::string_view label,
    std::string_view hint,
    std::string& buffer,
    AutocompleteState& state
) {
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
            state.showPopup = false;
            state.selectedIndex = -1;
        }
    }

    if (ImGui::IsItemActive()) {
        state.showPopup = true;
    }

    bool lostFocus = !ImGui::IsItemActive();

    drawPopup(
        state,
        ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y + ImGui::GetStyle().ItemSpacing.y),
        ImVec2(ImGui::GetItemRectSize().x, ImGui::GetTextLineHeight() * 5) // Show 5 lines of suggestions
    );

    bool hovering = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

    // if (lostFocus && !hovering) {
    //     state.showPopup = false;
    //     state.selectedIndex = -1;
    //     state.clickedIndex = -1;
    // }
}

TABBY_NS_END