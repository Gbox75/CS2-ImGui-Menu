#pragma once
#include "../imgui.h"
#include "../imgui_impl_dx11.h"
#include "../imgui_impl_win32.h"
#include "../imgui_internal.h"
#include "../imstb_rectpack.h"
#include "../imstb_textedit.h"
#include "../imstb_truetype.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <cmath>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <map>
#include <functional>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace smooth_api
{
    namespace Toolbar
    {
        // 平滑过渡单选工具栏类（横向布局）
        // 功能：实现横向排列的单选按钮工具栏，支持状态过渡动画、悬停效果和底部导航条跟随
        class SmoothRadioToolbar {
        private:
            // 按钮状态与动画相关变量的结构体
            struct ButtonState {
                float transitionProgress = 0.0f; // 状态过渡进度（0.0 = 未选中, 1.0 = 选中）
                float hoverAlpha = 0.0f;         // 悬停效果透明度（0.0 = 无效果, 1.0 = 最大效果）
            };

            // 样式与状态存储变量
            std::vector<ButtonState> buttonStates; // 存储所有按钮的状态信息
            ImVec2 toolbarSize;                    // 工具栏整体尺寸（宽×高）
            ImVec2 buttonSize;                     // 单个按钮尺寸（宽×高）
            float buttonSpacing;                   // 按钮之间的间距
            ImVec4 unselectedTitleColor;           // 未选中状态下的标题文字颜色
            ImVec4 selectedTitleColor;             // 选中状态下的标题文字颜色
            ImVec4 unselectedBgColor;              // 未选中状态下的按钮背景色
            ImVec4 selectedBgColor;                // 选中状态下的按钮背景色
            ImVec4 toolbarBgColor;                 // 工具栏整体背景色
            ImVec4 navBarColor;                    // 导航条颜色
            float toolbarRounding;                 // 工具栏边角圆角半径（仅控制左下和右下）
            float buttonRounding;                  // 按钮边角圆角半径
            float navBarCurrentX;                  // 导航条当前X坐标（用于动画过渡）
            float titleOffsetY;                    // 标题垂直偏移量

            // 动画参数
            const float transitionSpeed = 5.0f;    // 状态切换过渡速度
            const float hoverSpeed = 12.0f;        // 悬停效果过渡速度
            const float navBarSpeed = 10.0f;       // 导航条移动速度

            // 缓动函数
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

            // 初始化按钮状态
            void InitButtonStates(int count, const bool* states) {
                buttonStates.resize(count);
                navBarCurrentX = 0.0f;

                for (int i = 0; i < count; ++i) {
                    buttonStates[i].transitionProgress = states[i] ? 1.0f : 0.0f;
                    buttonStates[i].hoverAlpha = 0.0f;

                    if (states[i]) {
                        navBarCurrentX = i * (buttonSize.x + buttonSpacing);
                    }
                }
            }

        public:
            // 绘制工具条（核心方法）
            static void Draw(
                const char* componentId,
                const char** titles,
                bool* states,
                int buttonCount,
                float titleOffsetY = 0.0f,
                float toolbarRounding = 0.0f,
                float buttonRounding = 0.0f,
                const ImVec2& toolbarSize = ImVec2(800, 60),
                const ImVec2& buttonSize = ImVec2(72, 60),
                float buttonSpacing = 3.0f,
                ImColor unselectedTitleColor = ImColor(141, 141, 141),
                ImColor selectedTitleColor = ImColor(205, 163, 173),
                ImColor unselectedBgColor = ImColor(37, 37, 37),
                ImColor selectedBgColor = ImColor(41, 41, 41),
                ImColor toolbarBgColor = ImColor(37, 37, 37),
                ImColor navBarColor = ImColor(207, 155, 166)
            ) {
                // 实例管理
                static std::unordered_map<std::string, SmoothRadioToolbar> instances;
                auto& instance = instances[componentId];

                // 更新实例参数
                instance.toolbarSize = toolbarSize;
                instance.buttonSize = buttonSize;
                instance.buttonSpacing = buttonSpacing;
                instance.toolbarRounding = toolbarRounding;
                instance.buttonRounding = buttonRounding;
                instance.unselectedTitleColor = unselectedTitleColor.Value;
                instance.selectedTitleColor = selectedTitleColor.Value;
                instance.unselectedBgColor = unselectedBgColor.Value;
                instance.selectedBgColor = selectedBgColor.Value;
                instance.toolbarBgColor = toolbarBgColor.Value;
                instance.navBarColor = navBarColor.Value;
                instance.titleOffsetY = titleOffsetY;

                // 初始化按钮状态
                if (instance.buttonStates.size() != buttonCount) {
                    instance.InitButtonStates(buttonCount, states);
                }

                // 开始绘制
                ImGui::PushID(componentId);
                ImVec2 toolbarStartPos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // ---------------------------
                // 绘制工具条背景（兼容旧版本ImGui，无PathClose()）
                // 仅左下和右下应用圆角，上方保持直角
                // ---------------------------
                const float borderRadius = instance.toolbarRounding;
                const ImVec2 topLeft = toolbarStartPos;
                const ImVec2 bottomRight = ImVec2(
                    toolbarStartPos.x + toolbarSize.x,
                    toolbarStartPos.y + toolbarSize.y
                );

                // 手动构建路径（替代PathClose()，兼容旧版本）
                drawList->PathClear();

                // 1. 左上角（直角）
                const ImVec2 topLeftCorner = ImVec2(topLeft.x, topLeft.y);
                drawList->PathLineTo(topLeftCorner);

                // 2. 右上角（直角）
                const ImVec2 topRightCorner = ImVec2(bottomRight.x, topLeft.y);
                drawList->PathLineTo(topRightCorner);

                // 3. 右下角（圆角弧）
                drawList->PathArcTo(
                    ImVec2(bottomRight.x - borderRadius, bottomRight.y - borderRadius), // 圆弧中心
                    borderRadius,                                                       // 半径
                    0.0f,                                                                // 起始角度（右方向）
                    IM_PI * 0.5f,                                                       // 结束角度（上方向）
                    16                                                                   // 分段数（平滑度）
                );

                // 4. 左下角（圆角弧）
                drawList->PathArcTo(
                    ImVec2(topLeft.x + borderRadius, bottomRight.y - borderRadius),     // 圆弧中心
                    borderRadius,                                                       // 半径
                    IM_PI * 0.5f,                                                       // 起始角度（上方向）
                    IM_PI,                                                              // 结束角度（左方向）
                    16                                                                   // 分段数
                );

                // 手动闭合路径（替代PathClose()：从最后一个点连接回起点）
                drawList->PathLineTo(topLeftCorner);

                // 填充路径（使用工具栏背景色）
                drawList->PathFillConvex(ImGui::ColorConvertFloat4ToU32(instance.toolbarBgColor));

                // 计算按钮组布局（居中排列）
                const float totalButtonsWidth = buttonCount * buttonSize.x + (buttonCount - 1) * buttonSpacing;
                const float buttonsStartX = toolbarStartPos.x + ((toolbarSize.x - totalButtonsWidth) > 0 ? (toolbarSize.x - totalButtonsWidth) * 0.5f : 0);
                const float buttonsStartY = toolbarStartPos.y + (toolbarSize.y - buttonSize.y) * 0.5f;

                // 记录点击状态和选中索引
                int clickedIndex = -1;
                int selectedIndex = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedIndex = i;
                        break;
                    }
                }

                // 绘制每个按钮并处理交互
                float currentX = buttonsStartX;
                for (int i = 0; i < buttonCount; ++i) {
                    if (currentX + buttonSize.x > toolbarStartPos.x + toolbarSize.x) break;

                    ImGui::PushID(i);
                    ImVec2 buttonPos = ImVec2(currentX, buttonsStartY);

                    // 交互检测
                    ImGui::SetCursorScreenPos(buttonPos);
                    bool isClicked = ImGui::InvisibleButton("##button", buttonSize, ImGuiButtonFlags_PressedOnClick);
                    if (isClicked) {
                        clickedIndex = i;
                    }

                    // 状态过渡动画
                    const float targetProgress = states[i] ? 1.0f : 0.0f;
                    instance.buttonStates[i].transitionProgress = ImLerp(
                        instance.buttonStates[i].transitionProgress,
                        targetProgress,
                        ImGui::GetIO().DeltaTime * instance.transitionSpeed
                    );
                    const float progress = instance.SmoothStep(instance.buttonStates[i].transitionProgress);

                    // 悬停动画
                    const bool isHovered = ImGui::IsItemHovered();
                    const float hoverTarget = isHovered ? 1.0f : 0.0f;
                    instance.buttonStates[i].hoverAlpha = ImLerp(
                        instance.buttonStates[i].hoverAlpha,
                        hoverTarget,
                        ImGui::GetIO().DeltaTime * instance.hoverSpeed
                    );

                    // 颜色计算
                    const ImVec4 bgColor = ImLerp(
                        instance.unselectedBgColor,
                        instance.selectedBgColor,
                        progress
                    );
                    ImVec4 titleColor = ImLerp(
                        instance.unselectedTitleColor,
                        instance.selectedTitleColor,
                        progress
                    );

                    // 悬停高亮
                    if (instance.buttonStates[i].hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * instance.buttonStates[i].hoverAlpha);
                        titleColor.x = ImSaturate(titleColor.x * hoverFactor);
                        titleColor.y = ImSaturate(titleColor.y * hoverFactor);
                        titleColor.z = ImSaturate(titleColor.z * hoverFactor);
                    }

                    // 绘制按钮背景
                    drawList->AddRectFilled(
                        buttonPos,
                        ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y),
                        ImGui::ColorConvertFloat4ToU32(bgColor),
                        instance.buttonRounding
                    );

                    // 绘制按钮标题
                    const ImVec2 textSize = ImGui::CalcTextSize(titles[i]);
                    const ImVec2 textPos = ImVec2(
                        buttonPos.x + (buttonSize.x - textSize.x) * 0.5f,
                        buttonPos.y + (buttonSize.y - textSize.y) * 0.5f - instance.titleOffsetY
                    );
                    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(titleColor), titles[i]);

                    currentX += buttonSize.x + buttonSpacing;
                    ImGui::PopID();
                }

                // 处理点击事件（单选逻辑）
                if (clickedIndex != -1) {
                    for (int j = 0; j < buttonCount; ++j) {
                        states[j] = (j == clickedIndex);
                    }
                }

                // 单选状态校正
                int selectedCount = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedCount++;
                        selectedIndex = i;
                    }
                }
                if (selectedCount != 1) {
                    for (int i = 0; i < buttonCount; ++i) states[i] = false;
                    states[0] = true;
                    selectedIndex = 0;
                }

                // 导航条动画与绘制
                const float targetNavBarX = buttonsStartX + selectedIndex * (buttonSize.x + buttonSpacing);
                instance.navBarCurrentX = ImLerp(
                    instance.navBarCurrentX,
                    targetNavBarX,
                    ImGui::GetIO().DeltaTime * instance.navBarSpeed
                );

                const float navBarHeight = 2.0f;
                const float navBarY = buttonsStartY + buttonSize.y - navBarHeight;
                drawList->AddRectFilled(
                    ImVec2(instance.navBarCurrentX, navBarY),
                    ImVec2(instance.navBarCurrentX + buttonSize.x, navBarY + navBarHeight),
                    ImGui::ColorConvertFloat4ToU32(instance.navBarColor),
                    navBarHeight * 0.5f
                );

                // 移动光标到工具条下方
                ImGui::SetCursorScreenPos(ImVec2(
                    toolbarStartPos.x,
                    toolbarStartPos.y + toolbarSize.y + ImGui::GetStyle().ItemSpacing.y
                ));

                ImGui::PopID();
            }
        };

        // 平滑过渡竖向单选工具栏类（基于选中背景色的拖尾渐变效果）
        class SmoothVerticalRadioToolbar {
        private:
            // 按钮状态与动画相关变量的结构体
            struct ButtonState {
                float transitionProgress = 0.0f; // 状态过渡进度（0.0 = 未选中, 1.0 = 选中）
                float hoverAlpha = 0.0f;         // 悬停效果透明度
            };

            // 样式与状态存储变量
            std::vector<ButtonState> buttonStates; // 索引与按钮索引对应（0~buttonCount-1）
            ImVec2 toolbarSize;
            ImVec2 buttonSize;
            float baseButtonSpacing;
            float buttonSpacingOffset;
            ImVec4 unselectedTitleColor;
            ImVec4 selectedTitleColor;
            ImVec4 unselectedBgColor;
            ImVec4 selectedBgColor;       // 拖尾基色（改为使用选中背景色）
            ImVec4 toolbarBgColor;
            ImVec4 navBarColor;
            float toolbarRounding;
            float buttonRounding;
            float navBarCurrentY;

            // 动画参数
            const float transitionSpeed = 5.0f;
            const float hoverSpeed = 12.0f;
            const float navBarSpeed = 10.0f;

            // 缓动函数
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

            // 初始化按钮状态
            void InitButtonStates(int count, const bool* states) {
                buttonStates.resize(count); // 确保向量大小与按钮数量一致
                navBarCurrentY = 0.0f;

                for (int i = 0; i < count; ++i) {
                    buttonStates[i].transitionProgress = states[i] ? 1.0f : 0.0f;
                    buttonStates[i].hoverAlpha = 0.0f;

                    if (states[i]) {
                        const float actualSpacing = baseButtonSpacing + buttonSpacingOffset;
                        navBarCurrentY = i * (buttonSize.y + actualSpacing);
                    }
                }
            }

            // 绘制选中按钮的拖尾渐变背景（基于selectedBgColor）
            // 参数：buttonIndex - 当前按钮的索引（0~buttonCount-1）
            void DrawGradientBackground(ImDrawList* drawList, const ImVec2& buttonPos, int buttonIndex) {
                // 拖尾基色：使用选中状态的背景色（selectedBgColor）
                ImVec4 baseColor = selectedBgColor;
                // 渐变段数量（可调整细腻度）
                const int gradientSteps = 10;
                // 每段宽度 = 按钮宽度 / 渐变段数量（基于buttonSize.x）
                const float stepWidth = buttonSize.x / gradientSteps;

                // 遍历渐变段（segmentIdx为段索引，与按钮索引区分）
                for (int segmentIdx = 0; segmentIdx < gradientSteps; ++segmentIdx) {
                    // 计算当前段的位置
                    float segmentX = buttonPos.x + segmentIdx * stepWidth;
                    ImVec2 segmentMin = ImVec2(segmentX, buttonPos.y);
                    ImVec2 segmentMax = ImVec2(segmentX + stepWidth, buttonPos.y + buttonSize.y);

                    // 计算透明度：从左到右逐渐降低（0段=1.0，最后一段≈0.0）
                    float alpha = 1.0f - (static_cast<float>(segmentIdx) / gradientSteps);
                    // 与按钮选中进度融合（过渡动画期间渐变效果逐渐显现）
                    alpha *= buttonStates[buttonIndex].transitionProgress;

                    // 绘制当前段（颜色=选中背景色，透明度随位置衰减）
                    ImVec4 segmentColor = baseColor;
                    segmentColor.w = alpha; // 透明度插值（越靠右越透明）
                    drawList->AddRectFilled(
                        segmentMin,
                        segmentMax,
                        ImGui::ColorConvertFloat4ToU32(segmentColor),
                        buttonRounding
                    );
                }
            }

        public:
            static void Draw(
                const char* componentId,
                const char** titles,
                bool* states,
                int buttonCount,
                float buttonSpacingOffset = 0.0f,
                float toolbarRounding = 0.0f,
                float buttonRounding = 0.0f,
                const ImVec2& toolbarSize = ImVec2(170, 430),
                const ImVec2& buttonSize = ImVec2(170, 45),
                float baseButtonSpacing = 3.0f,
                ImColor unselectedTitleColor = ImColor(141, 141, 141),
                ImColor selectedTitleColor = ImColor(205, 163, 173),
                ImColor unselectedBgColor = ImColor(28, 28, 28),
                ImColor selectedBgColor = ImColor(41, 41, 41), // 拖尾基色（此颜色将用于渐变）
                ImColor toolbarBgColor = ImColor(28, 28, 28),
                ImColor navBarColor = ImColor(207, 155, 166)
            ) {
                static std::unordered_map<std::string, SmoothVerticalRadioToolbar> instances;
                auto& instance = instances[componentId];

                // 更新实例参数（重点：将selectedBgColor存储为拖尾基色）
                instance.toolbarSize = toolbarSize;
                instance.buttonSize = buttonSize;
                instance.baseButtonSpacing = baseButtonSpacing;
                instance.buttonSpacingOffset = buttonSpacingOffset;
                instance.toolbarRounding = toolbarRounding;
                instance.buttonRounding = buttonRounding;
                instance.unselectedTitleColor = unselectedTitleColor.Value;
                instance.selectedTitleColor = selectedTitleColor.Value;
                instance.unselectedBgColor = unselectedBgColor.Value;
                instance.selectedBgColor = selectedBgColor.Value; // 存储选中背景色作为渐变基色
                instance.toolbarBgColor = toolbarBgColor.Value;
                instance.navBarColor = navBarColor.Value;

                // 确保按钮状态向量大小与按钮数量一致（防越界）
                if (instance.buttonStates.size() != buttonCount) {
                    instance.InitButtonStates(buttonCount, states);
                }

                ImGui::PushID(componentId);
                ImVec2 toolbarStartPos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // 绘制工具栏背景
                drawList->AddRectFilled(
                    toolbarStartPos,
                    ImVec2(toolbarStartPos.x + toolbarSize.x, toolbarStartPos.y + toolbarSize.y),
                    ImGui::ColorConvertFloat4ToU32(instance.toolbarBgColor),
                    instance.toolbarRounding
                );

                const float actualButtonSpacing = instance.baseButtonSpacing + instance.buttonSpacingOffset;
                const float buttonsStartX = toolbarStartPos.x;
                const float buttonsStartY = toolbarStartPos.y;

                int clickedIndex = -1;
                int selectedIndex = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedIndex = i;
                        break;
                    }
                }

                // 遍历按钮（i为按钮索引）
                float currentY = buttonsStartY;
                for (int i = 0; i < buttonCount; ++i) {
                    if (currentY + buttonSize.y > toolbarStartPos.y + toolbarSize.y) break;

                    ImGui::PushID(i);
                    ImVec2 buttonPos = ImVec2(buttonsStartX, currentY);

                    // 交互检测
                    ImGui::SetCursorScreenPos(buttonPos);
                    bool isClicked = ImGui::InvisibleButton("##button", buttonSize, ImGuiButtonFlags_PressedOnClick);
                    if (isClicked) {
                        clickedIndex = i;
                    }

                    // 更新过渡进度
                    const float targetProgress = states[i] ? 1.0f : 0.0f;
                    instance.buttonStates[i].transitionProgress = ImLerp(
                        instance.buttonStates[i].transitionProgress,
                        targetProgress,
                        ImGui::GetIO().DeltaTime * instance.transitionSpeed
                    );
                    const float progress = instance.SmoothStep(instance.buttonStates[i].transitionProgress);

                    // 悬停效果
                    const bool isHovered = ImGui::IsItemHovered();
                    const float hoverTarget = isHovered ? 1.0f : 0.0f;
                    instance.buttonStates[i].hoverAlpha = ImLerp(
                        instance.buttonStates[i].hoverAlpha,
                        hoverTarget,
                        ImGui::GetIO().DeltaTime * instance.hoverSpeed
                    );

                    // 绘制基础背景（未选中->选中过渡）
                    const ImVec4 bgColor = ImLerp(
                        instance.unselectedBgColor,
                        instance.selectedBgColor,
                        progress
                    );
                    drawList->AddRectFilled(
                        buttonPos,
                        ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y),
                        ImGui::ColorConvertFloat4ToU32(bgColor),
                        instance.buttonRounding
                    );

                    // 选中时绘制拖尾渐变（基于selectedBgColor，传入按钮索引i）
                    if (progress > 0.1f) { // 过渡进度超过10%时显示
                        instance.DrawGradientBackground(drawList, buttonPos, i);
                    }

                    // 绘制标题文字
                    ImVec4 titleColor = ImLerp(
                        instance.unselectedTitleColor,
                        instance.selectedTitleColor,
                        progress
                    );
                    if (instance.buttonStates[i].hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * instance.buttonStates[i].hoverAlpha);
                        titleColor.x = ImSaturate(titleColor.x * hoverFactor);
                        titleColor.y = ImSaturate(titleColor.y * hoverFactor);
                        titleColor.z = ImSaturate(titleColor.z * hoverFactor);
                    }

                    const ImVec2 textSize = ImGui::CalcTextSize(titles[i]);
                    const ImVec2 textPos = ImVec2(
                        buttonPos.x + (buttonSize.x - textSize.x) * 0.5f,
                        buttonPos.y + (buttonSize.y - textSize.y) * 0.5f
                    );
                    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(titleColor), titles[i]);

                    currentY += buttonSize.y + actualButtonSpacing;
                    ImGui::PopID();
                }

                // 处理点击事件（单选逻辑）
                if (clickedIndex != -1) {
                    for (int j = 0; j < buttonCount; ++j) {
                        states[j] = (j == clickedIndex);
                    }
                }

                // 校正选中状态
                int selectedCount = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) selectedCount++;
                }
                if (selectedCount != 1) {
                    for (int i = 0; i < buttonCount; ++i) states[i] = false;
                    states[0] = true;
                    selectedIndex = 0;
                }

                // 绘制导航条
                const float targetNavBarY = buttonsStartY + selectedIndex * (buttonSize.y + actualButtonSpacing);
                instance.navBarCurrentY = ImLerp(
                    instance.navBarCurrentY,
                    targetNavBarY,
                    ImGui::GetIO().DeltaTime * instance.navBarSpeed
                );

                const float navBarWidth = 2.0f;
                const float navBarX = buttonsStartX;
                drawList->AddRectFilled(
                    ImVec2(navBarX, instance.navBarCurrentY),
                    ImVec2(navBarX + navBarWidth, instance.navBarCurrentY + buttonSize.y),
                    ImGui::ColorConvertFloat4ToU32(instance.navBarColor),
                    navBarWidth * 0.5f
                );

                // 调整光标位置至工具栏右侧
                ImGui::SetCursorScreenPos(ImVec2(
                    toolbarStartPos.x + toolbarSize.x + ImGui::GetStyle().ItemSpacing.x,
                    toolbarStartPos.y
                ));

                ImGui::PopID();
            }
        };
    }

    namespace CheckBox
    {
        // 平滑过渡选择框类（优化对勾居中位置）
        // 功能：实现带有选择框，支持状态过渡动画、悬停效果和自定义样式，对勾在框内精确居中
        class SmoothCheckbox {
        private:
            // 状态与动画相关变量的结构体
            struct CheckboxState {
                float transitionProgress = 0.0f; // 状态过渡进度（0.0 = 未选中, 1.0 = 选中）
                float hoverAlpha = 0.0f;         // 悬停效果透明度（0.0 = 无效果, 1.0 = 最大效果）
            };

            // 动画参数
            const float transitionSpeed = 8.0f;  // 状态切换过渡速度
            const float hoverSpeed = 12.0f;      // 悬停效果过渡速度

            // 缓动函数
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

        public:
            // 绘制选择框（核心方法）
            static void Draw(
                const char* title,                // 标题(如果为空则不绘制标题)
                const char* componentId,          // 组件ID
                bool* value,                      // 接收值
                const ImVec2& size = ImVec2(18, 18), // 组件大小
                ImColor unselectedBgColor = ImColor(28, 28, 28),  // 未选中时的背景颜色
                ImColor selectedBgColor = ImColor(205, 163, 173),    // 选中时的背景颜色
                ImColor unselectedCheckColor = ImColor(141, 141, 141), // 未选中时的对勾颜色
                ImColor selectedCheckColor = ImColor(37, 37, 37),   // 选中时的对勾颜色
                ImColor unselectedTitleColor = ImColor(90, 90, 90), // 未选中时的标题颜色
                ImColor selectedTitleColor = ImColor(255, 255, 255),   // 选中时的标题颜色
                ImColor borderColor = ImColor(60, 60, 60),           // 边框颜色
                float borderThickness = 1.0f,                         // 边框粗细
                float cornerRadius = 3.0f                             // 圆角半径（控制选择框和边框的圆角）
            ) {
                // 实例管理
                static std::unordered_map<std::string, SmoothCheckbox> instances;
                auto& instance = instances[componentId];

                // 状态管理
                static std::unordered_map<std::string, CheckboxState> states;
                auto& state = states[componentId];

                // 开始绘制
                ImGui::PushID(componentId);
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();

                // 计算整个组件（选择框+标题）的尺寸
                ImVec2 totalSize = size;
                if (title && title[0] != '\0') {
                    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    const ImVec2 textSize = ImGui::CalcTextSize(title);
                    totalSize.x = size.x + spacing + textSize.x;
                    totalSize.y = std::max(size.y, textSize.y); // 取高度最大值
                }

                // 处理整个组件区域的交互
                ImGui::SetCursorScreenPos(cursorPos);
                bool isClicked = ImGui::InvisibleButton("##checkbox_total_area", totalSize, ImGuiButtonFlags_PressedOnClick);
                if (isClicked) {
                    *value = !(*value);
                }

                // 检测整个组件区域的悬停状态（包括选择框和标题）
                bool isHovered = ImGui::IsItemHovered();

                // 状态过渡动画
                const float targetProgress = *value ? 1.0f : 0.0f;
                state.transitionProgress = ImLerp(
                    state.transitionProgress,
                    targetProgress,
                    ImGui::GetIO().DeltaTime * instance.transitionSpeed
                );
                const float progress = instance.SmoothStep(state.transitionProgress);

                // 悬停动画 - 使用整个组件的悬停状态
                const float hoverTarget = isHovered ? 1.0f : 0.0f;
                state.hoverAlpha = ImLerp(
                    state.hoverAlpha,
                    hoverTarget,
                    ImGui::GetIO().DeltaTime * instance.hoverSpeed
                );

                // 计算颜色
                const ImVec4 bgColor = ImLerp(
                    unselectedBgColor.Value,
                    selectedBgColor.Value,
                    progress
                );
                const ImVec4 checkColor = ImLerp(
                    unselectedCheckColor.Value,
                    selectedCheckColor.Value,
                    progress
                );

                // 悬停高亮效果（边框）
                ImVec4 currentBorderColor = borderColor.Value;
                if (state.hoverAlpha > 0.01f) {
                    const float hoverFactor = 1.0f + (0.5f * state.hoverAlpha);
                    currentBorderColor.x = ImSaturate(currentBorderColor.x * hoverFactor);
                    currentBorderColor.y = ImSaturate(currentBorderColor.y * hoverFactor);
                    currentBorderColor.z = ImSaturate(currentBorderColor.z * hoverFactor);
                }

                // 绘制背景（使用圆角参数）
                drawList->AddRectFilled(
                    cursorPos,
                    ImVec2(cursorPos.x + size.x, cursorPos.y + size.y),
                    ImGui::ColorConvertFloat4ToU32(bgColor),
                    cornerRadius // 使用自定义圆角半径
                );

                // 绘制边框（**核心修改**：未选中时绘制，选中时不绘制，带过渡效果）
                if (borderThickness > 0.0f) {
                    // 计算边框可见度：未选中时完全可见(1.0)，选中时完全不可见(0.0)
                    const float borderVisibility = 1.0f - progress;

                    // 只有边框可见度大于阈值时才绘制，优化性能
                    if (borderVisibility > 0.05f) {
                        drawList->AddRect(
                            cursorPos,
                            ImVec2(cursorPos.x + size.x, cursorPos.y + size.y),
                            ImGui::ColorConvertFloat4ToU32(currentBorderColor),
                            cornerRadius, // 与背景保持一致的圆角
                            0,            // 无边角修饰
                            borderThickness * borderVisibility // 边框粗细随选中状态渐变
                        );
                    }
                }

                // 绘制对勾（优化：精确居中定位）
                if (progress > 0.01f) {
                    // 计算选择框精确中心（考虑边框厚度，避免视觉偏移）
                    const float centerX = cursorPos.x + size.x * 0.5f;
                    const float centerY = cursorPos.y + size.y * 0.5f;

                    // 根据选择框尺寸计算对勾比例（确保在不同大小下都居中美观）
                    const float baseSize = std::min(size.x, size.y); // 取宽高中较小的值作为基准
                    const float checkScale = baseSize * 0.40f * progress; // 对勾大小为选择框的40%

                    // 对勾三个点的坐标（精确计算确保居中）
                    // 起点（左下方）
                    const ImVec2 startPoint = ImVec2(
                        centerX - checkScale * 0.8f,
                        centerY + checkScale * 0.0f
                    );

                    // 转折点（中间）
                    const ImVec2 midPoint = ImVec2(
                        centerX - checkScale * 0.4f,
                        centerY + checkScale * 0.55f
                    );

                    // 终点（右上方）
                    const ImVec2 endPoint = ImVec2(
                        centerX + checkScale * 0.7f,
                        centerY - checkScale * 0.5f
                    );

                    // 第一段线（从起点到转折点）
                    drawList->AddLine(
                        startPoint,
                        midPoint,
                        ImGui::ColorConvertFloat4ToU32(checkColor),
                        2.0f * progress
                    );

                    // 第二段线（从转折点到终点）- 分阶段动画
                    if (progress > 0.5f) {
                        const float adjustedProgress = (progress - 0.5f) * 2.0f;
                        const ImVec2 animatedEnd = ImVec2(
                            midPoint.x + (endPoint.x - midPoint.x) * adjustedProgress,
                            midPoint.y + (endPoint.y - midPoint.y) * adjustedProgress
                        );
                        drawList->AddLine(
                            midPoint,
                            animatedEnd,
                            ImGui::ColorConvertFloat4ToU32(checkColor),
                            2.0f * progress
                        );
                    }

                    // 第三段线（从转折点绘制小圆点补偿缺口)
                    if (progress > 0.3f) {
                        float dotSize = 0.7f * progress;
                        drawList->AddCircleFilled(
                            midPoint,
                            dotSize,
                            ImGui::ColorConvertFloat4ToU32(checkColor),
                            2.0f * progress
                        );
                    }
                }

                // 绘制标题（如果不为空）
                if (title && title[0] != '\0') {
                    // 计算标题位置（在选择框右侧，垂直居中对齐）
                    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImVec2 textPos = cursorPos;
                    textPos.x += size.x + spacing;
                    textPos.y += (size.y - ImGui::GetTextLineHeight()) * 0.5f;

                    // 标题颜色
                    const ImVec4 titleColor = ImLerp(
                        unselectedTitleColor.Value,
                        selectedTitleColor.Value,
                        progress
                    );

                    // 悬停时标题高亮（现在会正确响应标题区域的悬停）
                    ImVec4 currentTitleColor = titleColor;
                    if (state.hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * state.hoverAlpha);
                        currentTitleColor.x = ImSaturate(currentTitleColor.x * hoverFactor);
                        currentTitleColor.y = ImSaturate(currentTitleColor.y * hoverFactor);
                        currentTitleColor.z = ImSaturate(currentTitleColor.z * hoverFactor);
                    }

                    drawList->AddText(
                        textPos,
                        ImGui::ColorConvertFloat4ToU32(currentTitleColor),
                        title
                    );
                }

                // 调整光标位置
                ImGui::SetCursorScreenPos(ImVec2(
                    cursorPos.x,
                    cursorPos.y + totalSize.y + ImGui::GetStyle().ItemSpacing.y
                ));

                ImGui::PopID();
            }
        };
    }

    namespace Colors
    {
        class SmoothColorPicker {
        private:
            // 颜色存储
            ImColor currentColor;       // 当前选中的颜色
            bool isOpen = false;        // 调色板窗口是否打开
            bool isHovered = false;     // 预览方块是否被悬停
            bool isInitialized = false; // 每个实例独立的初始化状态

            // 动画参数
            float hoverAlpha = 0.0f;    // 悬停效果透明度
            float hoverSpeed = 15.0f;   // 悬停动画速度

            // 样式参数
            ImVec2 previewSize = ImVec2(24, 17.5f);  // 预览方块大小
            ImVec2 pickerSize = ImVec2(220, 250); // 调色板窗口大小
            float rounding = 0.0f;                 // 预览方块圆角（默认0）
            float borderThickness = 1.5f;          // 边框厚度（固定为1像素）

            // 绘制实现
            bool DrawImpl(const char* identifier, ImColor initialColor) {
                ImGui::PushID(identifier);

                // 初始化颜色（每个实例首次使用时）
                if (!isInitialized) {
                    currentColor = initialColor;
                    isInitialized = true;
                }

                // 处理预览方块点击事件
                bool clicked = ImGui::InvisibleButton("##preview", previewSize);
                isHovered = ImGui::IsItemHovered();

                // 获取预览方块位置（调整为边框内侧，避免边框被截断）
                ImVec2 previewPos = ImGui::GetItemRectMin();
                previewPos.x += borderThickness;
                previewPos.y += borderThickness;
                ImVec2 previewEndPos = ImVec2(
                    previewPos.x + previewSize.x - 2 * borderThickness,
                    previewPos.y + previewSize.y - 2 * borderThickness
                );

                // 点击打开/关闭调色板
                if (clicked) {
                    isOpen = !isOpen;
                }

                // 检查调色板窗口是否已关闭（用户点击了外部区域）
                std::string popupId = "##ColorPickerWindow_" + std::string(identifier);
                if (isOpen) {
                    // 计算调色板窗口位置和大小
                    ImVec2 popupPos = ImGui::GetItemRectMin(); // 使用原始位置计算弹窗
                    popupPos.y += previewSize.y + ImGui::GetStyle().ItemSpacing.y;

                    // 检查鼠标是否在调色板窗口内部
                    ImVec2 mousePos = ImGui::GetMousePos();
                    bool isMouseInPicker = (mousePos.x >= popupPos.x &&
                        mousePos.x <= popupPos.x + pickerSize.x &&
                        mousePos.y >= popupPos.y &&
                        mousePos.y <= popupPos.y + pickerSize.y);

                    // 点击了窗口外部且不是预览方块时关闭
                    bool isClickingOutside = ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                        !isMouseInPicker &&
                        !ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(),
                            ImVec2(ImGui::GetItemRectMin().x + previewSize.x,
                                ImGui::GetItemRectMin().y + previewSize.y));

                    if (isClickingOutside) {
                        isOpen = false;
                    }
                }

                // 更新悬停动画
                float targetAlpha = isHovered ? 0.2f : 0.0f;
                hoverAlpha = ImLerp(hoverAlpha, targetAlpha, ImGui::GetIO().DeltaTime * hoverSpeed);

                // 绘制颜色预览方块
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                const ImVec2 originalPos = ImGui::GetItemRectMin(); // 原始未调整的位置
                const ImVec2 originalEndPos = ImVec2(
                    originalPos.x + previewSize.x,
                    originalPos.y + previewSize.y
                );

                // 1. 绘制黑色边框（使用原始位置和预览大小，圆角跟随previewRounding）
                drawList->AddRect(
                    originalPos,
                    originalEndPos,
                    IM_COL32(0, 0, 0, 255), // 纯黑色边框
                    rounding,                // 边框圆角跟随previewRounding参数
                    0,                       // 圆角分段数（0为自动）
                    borderThickness          // 边框厚度
                );

                // 2. 绘制基础颜色块（调整后位置，避免被边框覆盖）
                drawList->AddRectFilled(
                    previewPos,
                    previewEndPos,
                    currentColor,
                    rounding  // 颜色块圆角与边框保持一致
                );

                // 3. 悬停效果（半透明白色覆盖，调整后位置）
                if (hoverAlpha > 0.01f) {
                    drawList->AddRectFilled(
                        previewPos,
                        previewEndPos,
                        IM_COL32(255, 255, 255, (int)(hoverAlpha * 255)),
                        rounding  // 悬停效果圆角与整体保持一致
                    );
                }

                // 绘制调色板窗口（无标题）
                if (isOpen) {
                    // 设置窗口位置（预览方块下方）
                    ImVec2 popupPos = ImGui::GetItemRectMin();
                    popupPos.y += previewSize.y + ImGui::GetStyle().ItemSpacing.y;
                    ImGui::SetNextWindowPos(popupPos);

                    // 设置窗口大小
                    ImGui::SetNextWindowSize(pickerSize);
                    if (ImGui::Begin(popupId.c_str(), nullptr,
                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings)) {
                        // 颜色选择器组件
                        ImVec4 color = currentColor;

                        ImGuiStyle& style = ImGui::GetStyle();
                        ImVec4 originalFrameBg = style.Colors[ImGuiCol_FrameBg];
                        ImVec4 originalFrameBgHovered = style.Colors[ImGuiCol_FrameBgHovered];
                        ImVec4 originalFrameBgActive = style.Colors[ImGuiCol_FrameBgActive];
                        float originalFrameRounding = style.FrameRounding;

                        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);       // 背景色（深灰半透明）
                        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f); // 悬停背景色
                        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);  // 激活背景色
                        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // 文本颜色（白色）

                        // 设置圆角半径
                        style.FrameRounding = 4.0f;

                        // 获取窗口内容区域的边界矩形
                        ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
                        ImVec2 windowMax = ImGui::GetWindowContentRegionMax();
                        ImVec2 windowSize = ImVec2(windowMax.x - windowMin.x, windowMax.y - windowMin.y);

                        // 设置颜色选择器大小与窗口完全一致
                        ImGui::SetNextItemWidth(windowSize.x);

                        // 色相环和饱和度/亮度选择器
                        ImGui::ColorPicker4(
                            "##picker",
                            &color.x,
                            ImGuiColorEditFlags_NoSidePreview |
                            ImGuiColorEditFlags_NoSmallPreview |
                            ImGuiColorEditFlags_NoLabel |
                            ImGuiColorEditFlags_AlphaBar
                        );

                        // 恢复原始样式
                        style.Colors[ImGuiCol_FrameBg] = originalFrameBg;
                        style.Colors[ImGuiCol_FrameBgHovered] = originalFrameBgHovered;
                        style.Colors[ImGuiCol_FrameBgActive] = originalFrameBgActive;
                        style.FrameRounding = originalFrameRounding;

                        // 更新颜色
                        currentColor = color;

                        ImGui::End();
                    }
                }

                ImGui::PopID();
                return clicked;
            }

        public:
            // 静态接口
            static bool Draw(
                const char* identifier,      // 唯一标识符
                ImColor* color,              // 颜色输出指针
                float previewRounding = 0.0f, // 预览方块圆角，默认0
                const ImVec2& previewSize = ImVec2(24, 17.5f),  // 预览方块大小
                const ImVec2& pickerSize = ImVec2(220, 250) // 调色板窗口大小                  
            ) {
                // 管理实例
                std::string key = identifier;
                static std::unordered_map<std::string, SmoothColorPicker> instances;
                auto& instance = instances[key];

                // 初始化样式（首次使用时）
                static std::unordered_set<std::string> initialized;
                if (initialized.find(key) == initialized.end()) {
                    instance.previewSize = previewSize;
                    instance.pickerSize = pickerSize;
                    instance.rounding = previewRounding;  // 初始化圆角参数（边框会使用该值）
                    initialized.insert(key);
                }

                // 绘制并同步颜色
                bool interacted = instance.DrawImpl(identifier, *color);
                *color = instance.currentColor;  // 同步颜色到外部
                return interacted;
            }
        };
    }

    namespace Slider 
    {
        class SmoothSliderfloat {
        private:
            // 滑动值
            float currentValue = 0.0f;
            float displayValue = 0.0f;
            bool isDragging = false;
            bool isHovered = false;
            bool hasFocus = false;
            float fadeAlpha = 1.0f;
            float focusLostTime = 0.0f;
            const float fadeDelay = 3.0f;

            // 动画参数
            float animationSpeed = 10.0f;
            float hoverIntensity = 0.0f;
            float hoverAlpha = 0.0f;
            bool isActive = false;

            // 样式参数
            ImVec2 size = ImVec2(200, 20);
            float thumbWidth = 12.0f;
            float thumbHeight = 20.0f;
            float rounding = 4.0f;
            float thumbRounding = 4.0f;
            float textOffsetX = 20.0f;
            float textOffsetY = 1.0f;

            // 颜色配置
            ImU32 bgColor = IM_COL32(70, 70, 70, 255);
            ImU32 fillColor = IM_COL32(255, 182, 193, 255);
            ImU32 thumbColor = IM_COL32(220, 220, 220, 255);
            ImU32 hoverColor = IM_COL32(255, 255, 255, 50);
            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 titleColor = IM_COL32(220, 220, 220, 255);

            // 缓动函数
            float EaseOut(float t) {
                return 1.0f - (1.0f - t) * (1.0f - t);
            }

            // 绘制实现
            float DrawImpl(const char* label, const char* identifier, float min, float max, float value) {
                ImGui::PushID(identifier);

                // 更新当前值
                currentValue = value;

                // 计算布局
                ImGui::BeginGroup();

                // 绘制标签（如果有） - 固定在滑动条上方
                bool hasLabel = label && label[0] != '\0';
                if (hasLabel) {
                    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(titleColor), "%s", label);
                }

                // 滑动条主体
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::InvisibleButton("##slider", size);

                // 交互处理
                isActive = ImGui::IsItemActive();
                isHovered = ImGui::IsItemHovered();
                bool wasHasFocus = hasFocus;
                hasFocus = isActive || isHovered;

                ImVec2 thumbMin = ImVec2(p.x + (size.x - thumbWidth) * ((value - min) / (max - min)), p.y - (thumbHeight - size.y) / 2);
                ImVec2 thumbMax = ImVec2(thumbMin.x + thumbWidth, thumbMin.y + thumbHeight);

                bool isThumbHovered = ImGui::IsMouseHoveringRect(thumbMin, thumbMax);

                if ((isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) || (isThumbHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))) {
                    float mouseX = ImGui::GetMousePos().x - p.x;
                    currentValue = (mouseX / size.x) * (max - min) + min;
                    currentValue = ImClamp(currentValue, min, max);
                    isDragging = true;
                }
                else if (isDragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    isDragging = false;
                }

                // 更新动画
                float delta = ImGui::GetIO().DeltaTime;
                displayValue = ImLerp(displayValue, currentValue, delta * animationSpeed);

                // 更新悬停效果
                float hoverTarget = isHovered || isActive ? 1.0f : 0.0f;
                hoverAlpha = ImLerp(hoverAlpha, hoverTarget, delta * 15.0f);

                // 计算绘制参数
                float fillRatio = (displayValue - min) / (max - min);
                thumbMin = ImVec2(p.x + fillRatio * (size.x - thumbWidth), p.y - (thumbHeight - size.y) / 2);
                thumbMax = ImVec2(thumbMin.x + thumbWidth, thumbMin.y + thumbHeight);

                // 绘制背景
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // 背景
                drawList->AddRectFilled(
                    p,
                    ImVec2(p.x + size.x, p.y + size.y),
                    bgColor,
                    rounding
                );

                // 填充部分
                drawList->AddRectFilled(
                    p,
                    ImVec2(p.x + fillRatio * size.x, p.y + size.y),
                    fillColor,
                    rounding
                );

                // 悬停效果
                if (hoverAlpha > 0.01f) {
                    ImVec4 hoverCol = ImGui::ColorConvertU32ToFloat4(hoverColor);
                    hoverCol.w *= hoverAlpha;
                    drawList->AddRectFilled(
                        p,
                        ImVec2(p.x + size.x, p.y + size.y),
                        ImGui::GetColorU32(hoverCol),
                        rounding
                    );
                }

                // 滑块（拇指）
                drawList->AddRectFilled(
                    thumbMin,
                    thumbMax,
                    thumbColor,
                    thumbRounding,
                    ImDrawFlags_RoundCornersAll
                );

                ImGui::EndGroup();
                ImGui::PopID();
                return currentValue;
            }

        public:
            // 静态接口
            static float Draw(
                const char* label,                            // 滑动条的标签文本，显示在滑动条旁边或滑动条上
                const char* identifier,                       // 滑动条的唯一标识符，用于区分不同的滑动条
                float* value,                                 // 指向滑动条当前值的指针
                float min,                                    // 滑动条的最小值
                float max,                                    // 滑动条的最大值
                const ImVec2& size = ImVec2(260, 8),          // 滑动条的大小
                float thumbWidth = 7.0f,                      // 滑块的宽度
                float thumbHeight = 13.0f,                    // 滑块的高度
                float thumbRounding = 20.0f,                  // 滑块的圆角半径
                ImU32 bgCol = IM_COL32(29, 29, 29, 255),      // 滑动条背景颜色
                ImU32 fillCol = IM_COL32(205, 163, 173, 255),  // 滑动条填充颜色
                ImU32 thumbCol = IM_COL32(255, 255, 255, 255),// 滑块颜色
                ImU32 textCol = IM_COL32(255, 255, 255, 255), // 文本颜色
                ImU32 titleCol = IM_COL32(220, 220, 220, 255) // 标题颜色
            ) {

                std::string key = identifier ? identifier : "";
                static std::unordered_map<std::string, SmoothSliderfloat> instances;
                auto& instance = instances[key];

                static std::unordered_set<std::string> initialized;
                if (initialized.find(key) == initialized.end()) {
                    instance.size = size;
                    instance.thumbWidth = thumbWidth;
                    instance.thumbHeight = thumbHeight;
                    instance.thumbRounding = thumbRounding;
                    instance.bgColor = bgCol;
                    instance.fillColor = fillCol;
                    instance.thumbColor = thumbCol;
                    instance.textColor = textCol;
                    instance.titleColor = titleCol;
                    initialized.insert(key);
                }

                float newValue = instance.DrawImpl(label, identifier, min, max, *value);
                if (newValue != *value) {
                    *value = newValue;
                }

                return newValue;
            }
        };
    }

    namespace Input
    {
        class SmoothInput {
        private:
            // 状态变量
            float hoverAlpha = 0.0f;
            float focusAlpha = 0.0f;
            bool hasFocus = false;

            // 样式参数
            ImVec2 size = ImVec2(200, 30);
            float rounding = 4.0f;
            float borderWidth = 1.0f;
            bool centerText = false;

            // 颜色配置
            ImU32 bgColor = IM_COL32(40, 40, 40, 255);
            ImU32 borderColor = IM_COL32(100, 100, 100, 255);
            ImU32 hoverColor = IM_COL32(255, 255, 255, 30);
            ImU32 focusColor = IM_COL32(100, 150, 255, 150);

            // 核心绘制实现
            bool DrawImpl(const char* label, char* buf, size_t buf_size) {
                std::string thisId = label ? std::string(label) : "##input";
                ImGui::PushID(thisId.c_str());

                // 1. 获取位置和尺寸
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // 2. 创建交互区域
                ImGui::InvisibleButton("##input_area", size);
                bool hovered = ImGui::IsItemHovered();
                bool clicked = ImGui::IsItemClicked();

                // 3. 焦点管理
                if (clicked) {
                    hasFocus = true;
                }
                if (hasFocus && !ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
                    hasFocus = false;
                }
                bool isActive = hasFocus;

                // 4. 更新动画状态
                float delta = ImGui::GetIO().DeltaTime;
                hoverAlpha = ImLerp(hoverAlpha, hovered ? 1.0f : 0.0f, delta * 10.0f);
                focusAlpha = ImLerp(focusAlpha, isActive ? 1.0f : 0.0f, delta * 15.0f);

                // 5. 绘制背景
                drawList->AddRectFilled(
                    pos,
                    ImVec2(pos.x + size.x, pos.y + size.y),
                    bgColor,
                    rounding
                );

                // 6. 绘制悬停效果
                if (hoverAlpha > 0.01f) {
                    drawList->AddRectFilled(
                        pos,
                        ImVec2(pos.x + size.x, pos.y + size.y),
                        IM_COL32(255, 255, 255, (int)(30 * hoverAlpha)),
                        rounding
                    );
                }

                // 7. 准备输入框样式 - 关键修改：完全隐藏默认边框和背景
                ImGui::SetCursorScreenPos(pos);
                float verticalPadding = (size.y - ImGui::GetTextLineHeight()) * 0.5f;

                // 保存当前样式
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, verticalPadding));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // 关键：隐藏默认边框
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                // 关键：将所有相关颜色设置为透明，消除默认绘制
                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0)); // 边框颜色设为透明
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

                // 8. 设置输入框宽度
                ImGui::PushItemWidth(size.x);

                if (isActive) {
                    ImGui::SetKeyboardFocusHere();
                }

                // 9. 文本居中处理
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                const ImGuiStyle& style = ImGui::GetStyle();
                float originalCursorPosX = window->DC.CursorPos.x;

                if (centerText && buf != nullptr && buf[0] != '\0') {
                    float textWidth = ImGui::CalcTextSize(buf).x;
                    float inputWidth = size.x - (style.FramePadding.x * 2);

                    if (textWidth < inputWidth) {
                        float offsetX = (inputWidth - textWidth) * 0.5f;
                        window->DC.CursorPos.x += offsetX;
                    }
                }

                // 10. 绘制输入框（此时仅处理文本输入，不绘制边框）
                bool changed = ImGui::InputText(
                    "##real_input",
                    buf,
                    buf_size,
                    ImGuiInputTextFlags_None
                );

                // 恢复光标位置
                window->DC.CursorPos.x = originalCursorPosX;

                // 11. 绘制我们自己的边框和焦点效果
                // 绘制正常边框
                drawList->AddRect(
                    ImVec2(pos.x + 0.5f, pos.y + 0.5f),  // 轻微偏移避免抗锯齿导致的暗色边
                    ImVec2(pos.x + size.x - 0.5f, pos.y + size.y - 0.5f),
                    borderColor,
                    rounding,
                    0,
                    borderWidth
                );

                // 绘制焦点边框
                if (focusAlpha > 0.01f) {
                    float focusBorderWidth = borderWidth * (1.0f + focusAlpha * 0.5f);
                    drawList->AddRect(
                        ImVec2(pos.x - focusBorderWidth * 0.5f, pos.y - focusBorderWidth * 0.5f),
                        ImVec2(pos.x + size.x + focusBorderWidth * 0.5f, pos.y + size.y + focusBorderWidth * 0.5f),
                        focusColor,
                        rounding,
                        0,
                        focusBorderWidth
                    );
                }

                // 12. 恢复样式
                ImGui::PopItemWidth();
                ImGui::PopStyleColor(5);  // 恢复5个颜色设置
                ImGui::PopStyleVar(4);    // 恢复4个样式设置
                ImGui::PopID();

                return changed;
            }

        public:
            static bool Draw(
                const char* label,
                char* buf,
                size_t buf_size,
                bool centerText = true,
                const ImVec2& size = ImVec2(28, 15),
                float rounding = 2.0f,
                float borderWidth = 1.0f,
                ImU32 bgCol = IM_COL32(28, 28, 28, 255),
                ImU32 borderCol = IM_COL32(60, 60, 60, 255),
                ImU32 hoverCol = IM_COL32(255, 255, 255, 30),
                ImU32 focusCol = IM_COL32(205, 163, 173, 255)
            ) {
                static std::unordered_map<std::string, SmoothInput> instances;
                std::string id = label ? std::string(label) : "##input";

                SmoothInput& instance = instances[id];
                instance.size = size;
                instance.rounding = rounding;
                instance.borderWidth = borderWidth;
                instance.bgColor = bgCol;
                instance.borderColor = borderCol;
                instance.hoverColor = hoverCol;
                instance.focusColor = focusCol;
                instance.centerText = centerText;

                return instance.DrawImpl(label, buf, buf_size);
            }
        };
    }

    namespace Button
    {
        class SmoothTextButton {
        private:
            // 状态跟踪
            bool isHovered = false;        // 鼠标悬停状态
            bool isClicked = false;        // 点击触发标记
            bool wasActive = false;        // 上一帧的激活状态
            float hoverProgress = 0.0f;    // 悬停动画进度(0-1)
            float clickProgress = 0.0f;    // 点击动画进度(0-1)
            float resetProgress = 0.0f;    // 重置动画进度(0-1)

            // 动画参数（可微调效果）
            const float hoverSpeed = 5.0f;    // 悬停动画速度
            const float clickSpeed = 5.0f;    // 点击动画速度
            const float resetSpeed = 5.0f;    // 重置动画速度
            const float hoverBrightness = 0.25f; // 悬停亮度提升
            const float clickBrightness = -0.15f; // 点击暗度变化
            const float resetBrightness = -0.2f;  // 重置暗度变化

            // 缓动曲线（缓入缓出）
            float EaseInOut(float t) {
                return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            }

            // 检查鼠标是否在指定窗口内（使用更兼容的字符串比较）
            bool IsMouseInWindow(const char* windowName) {
                if (!windowName) return false;

                ImGuiContext& g = *GImGui;
                for (ImGuiWindow* window : g.Windows) {
                    // 检查窗口是否存在且可见（使用strcmp进行字符串比较）
                    if (window->Active && !window->Hidden &&
                        strcmp(window->Name, windowName) == 0) {
                        // 检查鼠标是否在窗口矩形内
                        return ImGui::IsMouseHoveringRect(window->Pos, ImVec2(window->Pos.x + window->Size.x, window->Pos.y + window->Size.y));
                        
                    }
                }
                return false;
            }

            // 检查鼠标是否在任何弹出窗口内（不依赖IsMouseHoveringAnyWindow）
            bool IsMouseInPopup() {
                ImGuiContext& g = *GImGui;
                if (g.OpenPopupStack.Size == 0) return false;

                // 检查鼠标是否在任何弹出窗口区域内
                for (int i = 0; i < g.OpenPopupStack.Size; i++) {
                    ImGuiWindow* popupWindow = g.OpenPopupStack[i].Window;
                    if (popupWindow && popupWindow->Active && !popupWindow->Hidden) {
                        if (ImGui::IsMouseHoveringRect(popupWindow->Pos, ImVec2(popupWindow->Pos.x + popupWindow->Size.x, popupWindow->Pos.y + popupWindow->Size.y))) {
                            return true;
                        }
                    }
                }
                return false;
            }

            // 绘制实现
            void DrawImpl(const char* label, const char* id, bool* isPressed,
                ImColor normalColor, ImColor selectedColor, const char* excludeWindowName) {
                ImGui::PushID(id);

                // 1. 交互区域设置（隐形按钮）
                ImVec2 textSize = ImGui::CalcTextSize(label);
                ImVec2 clickArea(textSize.x + 6, textSize.y + 4); // 略大于文本的点击区

                // 完全透明的交互基底（仅用于检测输入）
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                bool clicked = ImGui::Button("##", clickArea);
                isHovered = ImGui::IsItemHovered();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(3);

                // 2. 点击状态处理与动画触发
                if (clicked) {
                    *isPressed = !*isPressed;
                    isClicked = true;    // 触发点击动画
                    clickProgress = 0.0f; // 重置点击进度
                    resetProgress = 0.0f; // 重置重置动画
                }

                // 3. 点击外部区域自动取消选中（改进版逻辑）
                bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool isMouseInArea = ImGui::IsMouseHoveringRect(
                    ImGui::GetItemRectMin(),
                    ImGui::GetItemRectMax()
                );

                // 检查是否点击了排除窗口
                bool isInExcludeWindow = IsMouseInWindow(excludeWindowName);

                // 检查是否点击了弹出窗口
                bool isInPopup = IsMouseInPopup();

                // 取消选中条件：点击了外部，且不在排除窗口内，且不在弹出窗口内
                if (*isPressed && isMouseClicked && !isMouseInArea &&
                    !isInExcludeWindow && !isInPopup) {
                    *isPressed = false;
                    resetProgress = 0.0f; // 触发重置动画
                }

                // 4. 动画进度更新（独立计算）
                const float dt = ImGui::GetIO().DeltaTime;

                // 悬停动画：趋向目标值（0或1）
                float targetHover = isHovered ? 1.0f : 0.0f;
                hoverProgress = ImLerp(hoverProgress, targetHover, dt * hoverSpeed);

                // 点击动画：单次脉冲（从0→1→0）
                if (isClicked) {
                    clickProgress += dt * clickSpeed;
                    if (clickProgress >= 1.0f) {
                        isClicked = false;
                    }
                }

                // 重置动画：从0→1（用于点击外部取消选中时的过渡效果）
                if (resetProgress > 0.0f || (!*isPressed && wasActive)) {
                    resetProgress += dt * resetSpeed;
                    resetProgress = ImMin(resetProgress, 1.0f);
                }

                // 记录当前状态用于下一帧对比
                wasActive = *isPressed;

                // 5. 颜色计算（叠加三种动画效果）
                ImVec4 base = *isPressed ? selectedColor : normalColor;

                // 悬停效果：亮度提升（应用缓动）
                float easedHover = EaseInOut(hoverProgress);
                ImVec4 hoverColor = base;
                hoverColor.x = ImSaturate(base.x + hoverBrightness * easedHover);
                hoverColor.y = ImSaturate(base.y + hoverBrightness * easedHover);
                hoverColor.z = ImSaturate(base.z + hoverBrightness * easedHover);

                // 点击效果：瞬间暗化再恢复（使用正弦曲线模拟脉冲）
                ImVec4 clickColor = hoverColor;
                if (isClicked) {
                    float clickPulse = sinf(clickProgress * IM_PI); // 0→1→0的正弦曲线
                    clickColor.x = ImSaturate(hoverColor.x + clickBrightness * clickPulse);
                    clickColor.y = ImSaturate(hoverColor.y + clickBrightness * clickPulse);
                    clickColor.z = ImSaturate(hoverColor.z + clickBrightness * clickPulse);
                }

                // 重置效果：取消选中时的过渡动画
                ImVec4 finalColor = clickColor;
                if (resetProgress > 0.0f && resetProgress < 1.0f) {
                    float resetPulse = sinf(resetProgress * IM_PI * 0.5f); // 0→1的正弦曲线
                    finalColor.x = ImSaturate(clickColor.x + resetBrightness * resetPulse);
                    finalColor.y = ImSaturate(clickColor.y + resetBrightness * resetPulse);
                    finalColor.z = ImSaturate(clickColor.z + resetBrightness * resetPulse);
                }

                // 6. 绘制文本（唯一视觉元素）
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 textPos = ImGui::GetItemRectMin();
                // 文本在点击区内居中
                textPos.x += (clickArea.x - textSize.x) * 0.5f;
                textPos.y += (clickArea.y - textSize.y) * 0.5f;

                drawList->AddText(
                    ImGui::GetFont(),
                    ImGui::GetFontSize(),
                    textPos,
                    ImColor(finalColor),
                    label
                );

                ImGui::PopID();
            }

        public:
            // 新增excludeWindowName参数，指定点击该窗口不关闭按钮状态
            static void Draw(const char* label, const char* id, bool* isPressed,
                const char* excludeWindowName = nullptr,
                ImColor normalColor = ImColor(141, 141, 141),
                ImColor selectedColor = ImColor(205, 163, 173)
                ) {
                static std::unordered_map<std::string, SmoothTextButton> instances;
                instances[id].DrawImpl(label, id, isPressed, normalColor, selectedColor, excludeWindowName);
            }
        };
    }
}
