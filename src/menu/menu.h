#pragma once
#include "../../ImGui/smooth_api/smooth_api.h"
#include <ctime>
#include <algorithm>

float approachValue(float current, float target, float delta);

typedef void(menu_fun)();
namespace menu
{
    inline bool is_visible = true;
    inline bool is_transitioning = true;
    inline float transition_time = 0.0f;

    namespace image
    {
        inline ID3D11ShaderResourceView* logo;
    }

    namespace api
    {
        void draw_particle();
        void draw_menu();
        void set_menu_flags();
        void set_menu_window_size();
        ImGuiWindowFlags set_menu_window_flags();
        void draw_menu_pad(ImDrawList* draw_list, ImVec2 window_pos);
        void draw_menu_logo();
        void draw_menu_toolbar(ImDrawList* draw_list, ImVec2 window_pos);
        void draw_menu_toolbar_text();
        void draw_menu_toolbar_line(ImDrawList* draw_list, ImVec2 window_pos);
        void create_menu_parent_window(menu_fun* fun, const char** id, ImVec2 size, ImVec2 pos = ImVec2(0, 0));
        void draw_menu_parent_window(ImDrawList* draw_list, ImVec2 window_pos);

        namespace function
        {
            namespace aim_bot
            {
                void draw_menu_aim_bot_Legitbot_0();
                void draw_menu_aim_bot_Ragebot_0();
                void draw_menu_aim_bot_Ragebot_1();
                void draw_menu_aim_bot_Ragebot_2();
            }

            namespace anti_aim
            {
                void draw_menu_anti_aim_Antiaim_0();
            }

            namespace vis_uals
            {
                void draw_menu_vis_uals_Player_0();
                void draw_menu_vis_uals_Player_1();
                void draw_menu_vis_uals_Local_0();
                void draw_menu_vis_uals_World_0();
                void draw_menu_vis_uals_Misc_0();
            }

            namespace mi_sc
            {
                void draw_menu_mi_sc_particle_0();
            }
        }
    }

    namespace font
    {
        inline ImFont* menu_icon;
        inline ImFont* menu_text;
        inline ImFont* menu_weapon;
    }

    namespace component
    {
        namespace toolbar
        {
            inline const char* menu_toolbar_titles[4] = { "B", "A", "V", "M" };
            inline bool menu_toolbar_index[4] = { true, false, false, false };

            namespace toolbar_1
            {
                inline const char* menu_toolbar_titles_0[2] = { "Legitbot", "Ragebot" };
                inline bool menu_toolbar_index_0[2] = { true, false };

                inline const char* menu_toolbar_titles_1[1] = { "Antiaim" };
                inline bool menu_toolbar_index_1[1] = { true };

                inline const char* menu_toolbar_titles_2[4] = { "Player", "Local", "World", "Misc" };
                inline bool menu_toolbar_index_2[4] = { true, false, false, false };

                inline const char* menu_toolbar_titles_3[3] = { "Misc", "Cfg", "Particle" };
                inline bool menu_toolbar_index_3[3] = { true, false ,false };
            }
        }
    }

    namespace aim_bot
    {
        namespace Legitbot
        {
            inline bool Enabled = false; //全局
        }

        namespace Ragebot
        {
            inline bool Enabled = false; //全局
            inline bool SilentAim = false; //静默
            namespace Settings
            {
                inline bool AutoShoot = false; //自动开枪
            }
            inline float FOV = 0.0f; //FOV
            inline char FOV_Input[60]{}; //FOV用户输入
        }
    }

    namespace anti_aim
    {
        namespace Antiaim
        {
            inline bool Enabled = false; //全局
        }
    }

    namespace vis_uals
    {
        namespace Player
        {
            inline bool Enabled = false; //全局
            inline bool Box = false; //方框
            inline ImColor BoxColor = ImColor(255, 255, 255); //方框颜色
            inline bool Bone = false; //骨骼
            inline ImColor BoneVisualColor = ImColor(255, 255, 255); //可视骨骼颜色
            inline ImColor BoneInVisualColor = ImColor(0, 0, 0); //不可视骨骼颜色
            inline bool Health = false; //血条
            inline ImColor HealthColor = ImColor(0, 255, 125); //血条颜色
            inline bool Name = false; //名称
            inline ImColor NameColor = ImColor(255, 255, 255); //名称颜色
            inline bool Weapon = false; //武器     
            namespace Weapon_setting
            {
                inline bool Icon = false; //武器图标
                inline ImColor IconColor = ImColor(0, 255, 255); //武器图标颜色
                inline bool Name = false; //武器名称
                inline ImColor NameColor = ImColor(255, 255, 255); //武器名称颜色
                inline bool Ammo = false; //弹药条
                inline ImColor AmmoColor = ImColor(0, 78, 113); //武器弹药颜色
            }
            inline void draw_weapon_setting()
            {
                //静态变量初始化
                static float Vertex = 40.0f;
                static float Spacing = 25.0f;
                static float weaponTextHighlight = 0.0f;  //文本高亮动画进度
                static bool showPopup = false;            //弹窗显示状态标记
                static float popupAlpha = 0.0f;           //弹窗透明度（0-1）
                static float popupScale = 0.9f;           //弹窗缩放比例（0.9-1.0）
                static float showProgress = 0.0f;         //显示动画进度（0-1）
                static bool isTransitioning = false;      //是否正在动画过渡

                //动画参数配置
                const float highlightEasing = 10.0f;      //文本高亮动画速度
                const float animationSpeed = 8.0f;        //动画速度系数
                const float minProgress = 0.001f;         //最小进度阈值

                //1.绘制可点击文本
                const float textX = 245.0f;
                const float textY = Vertex + Spacing * 5.0f;
                ImGui::SetCursorPosX(textX);
                ImGui::SetCursorPosY(textY);
                ImGui::PushFont(menu::font::menu_icon);

                const char* clickableText = "S";
                const ImVec2 textSize = ImGui::CalcTextSize(clickableText);
                const ImVec2 textScreenPos = ImGui::GetCursorScreenPos();

                //文本区域检测（用于悬停判断）
                const ImRect textRect(
                    textScreenPos,
                    ImVec2(textScreenPos.x + textSize.x, textScreenPos.y + textSize.y)
                );
                const bool isHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max);

                //文本高亮动画逻辑
                const float targetHighlight = isHovered ? 1.0f : 0.0f;
                weaponTextHighlight = approachValue(
                    weaponTextHighlight,
                    targetHighlight,
                    ImGui::GetIO().DeltaTime * highlightEasing
                );

                //文本颜色计算（手动插值）
                const ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);    //默认灰白色
                const ImVec4 highlightColor = ImVec4(0.9f, 0.6f, 0.8f, 1.0f);  //高亮粉色
                const float r = defaultColor.x + (highlightColor.x - defaultColor.x) * weaponTextHighlight;
                const float g = defaultColor.y + (highlightColor.y - defaultColor.y) * weaponTextHighlight;
                const float b = defaultColor.z + (highlightColor.z - defaultColor.z) * weaponTextHighlight;
                const float a = defaultColor.w + (highlightColor.w - defaultColor.w) * weaponTextHighlight;

                //绘制文本并恢复样式
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
                ImGui::Text(clickableText);
                ImGui::PopStyleColor();
                ImGui::PopFont();

                //2.文本点击处理（最高优先级）
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered)
                {
                    if (showPopup)
                    {
                        //直接隐藏，不使用动画
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                    else
                    {
                        //开始显示动画
                        isTransitioning = true;
                        showProgress = 0.0f;  //重置显示进度
                        showPopup = true;     //立即标记为显示状态
                    }
                    return;
                }

                //3.外部点击关闭弹窗逻辑
                if (showPopup && !isTransitioning && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    //1.检查是否点击了当前弹窗内的任何ImGui控件
                    bool isClickingWidget = ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();

                    //2.检查是否点击了弹窗窗口本身
                    const ImVec2 popupPos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImRect popupRect(
                        popupPos,
                        ImVec2(popupPos.x + 200.0f, popupPos.y + 150.0f)
                    );
                    bool isClickingWindowArea = popupRect.Contains(ImGui::GetMousePos());

                    //3.只有当既没有点击控件也没有点击窗口区域时，才关闭弹窗
                    if (!isClickingWidget && !isClickingWindowArea)
                    {
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                }

                //4.弹窗动画核心逻辑 - 只保留显示动画
                const float deltaTime = ImGui::GetIO().DeltaTime;
                bool animationComplete = false;

                if (isTransitioning && showPopup)
                {
                    //显示动画：从0到1
                    showProgress += deltaTime * animationSpeed;
                    showProgress = ImMin(showProgress, 1.0f);

                    if (showProgress >= 1.0f)
                    {
                        animationComplete = true;
                    }

                    //动画完成后更新状态
                    if (animationComplete)
                    {
                        isTransitioning = false;
                    }
                }

                //计算最终动画参数
                if (showPopup)
                {
                    //显示状态：使用显示进度或完全显示
                    const float progress = isTransitioning ? showProgress : 1.0f;
                    popupAlpha = progress;
                    popupScale = 0.9f + 0.1f * progress;
                }
                else
                {
                    //隐藏状态：直接设置为初始值
                    popupAlpha = 0.0f;
                    popupScale = 0.9f;
                }

                //5.绘制弹窗 - 只要有透明度就绘制
                if (popupAlpha > minProgress)
                {
                    const ImVec2 popupBasePos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImVec2 popupBaseSize(200.0f, 120.0f);  //窗口基础尺寸

                    //计算缩放偏移（保持顶部居中缩放）
                    const float offsetX = (popupBaseSize.x * (1.0f - popupScale)) * 0.5f;
                    const float offsetY = 0.0f;  //顶部对齐，Y轴无偏移

                    //设置弹窗位置和大小（带缩放）
                    ImGui::SetNextWindowPos(ImVec2(
                        popupBasePos.x - offsetX,
                        popupBasePos.y - offsetY
                    ));
                    ImGui::SetNextWindowSize(ImVec2(
                        popupBaseSize.x * popupScale,
                        popupBaseSize.y * popupScale
                    ));

                    //获取绘制列表并设置裁剪区域
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    drawList->PushClipRect(
                        ImGui::GetWindowPos(),
                        ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
                        true
                    );

                    //推送弹窗样式
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f * popupScale);

                    //保存原始颜色并只修改透明度
                    ImGuiStyle& style = ImGui::GetStyle();
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(
                        style.Colors[ImGuiCol_WindowBg].x,
                        style.Colors[ImGuiCol_WindowBg].y,
                        style.Colors[ImGuiCol_WindowBg].z,
                        style.Colors[ImGuiCol_WindowBg].w * popupAlpha
                    ));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(
                        style.Colors[ImGuiCol_Border].x,
                        style.Colors[ImGuiCol_Border].y,
                        style.Colors[ImGuiCol_Border].z,
                        style.Colors[ImGuiCol_Border].w * popupAlpha
                    ));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(
                        style.Colors[ImGuiCol_Text].x,
                        style.Colors[ImGuiCol_Text].y,
                        style.Colors[ImGuiCol_Text].z,
                        style.Colors[ImGuiCol_Text].w * popupAlpha
                    ));

                    //绘制弹窗内容
                    if (ImGui::Begin("#Weapon Setting", nullptr,
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoSavedSettings))
                    {
                        ImGui::Text(u8"Weapon Setting");
                        ImGui::Separator();

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Icon", "#Player_Weapon_Setting_Icon", &menu::vis_uals::Player::Weapon_setting::Icon);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Weapon_Setting_Icon_Color", &menu::vis_uals::Player::Weapon_setting::IconColor, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Name", "#Player_Weapon_Setting_Name", &menu::vis_uals::Player::Weapon_setting::Name);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Weapon_Setting_Name_Color", &menu::vis_uals::Player::Weapon_setting::NameColor, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing * 2);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Ammo", "#Player_Weapon_Setting_Ammo", &menu::vis_uals::Player::Weapon_setting::Ammo);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing * 2);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Weapon_Setting_Ammo_Color", &menu::vis_uals::Player::Weapon_setting::AmmoColor, 2.0f);

                        ImGui::End();
                    }

                    //恢复样式和绘制列表
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    drawList->PopClipRect();
                }
                //当弹窗完全隐藏且不在过渡中，重置显示状态
                else if (!isTransitioning && !showPopup)
                {
                    showProgress = 0.0f;
                }
            }
            inline bool Glow = false; //发光
            inline ImColor GlowColor = ImColor(255, 255, 255); //发光颜色
            inline bool BehindTheWall = false; //墙后
            namespace Preview_setting
            {
                inline bool Ping = false; //玩家Ping
                inline ImColor PingColor = ImColor(255, 0, 0); //玩家Ping颜色
                inline bool Money = false; //玩家金钱
                inline ImColor MoneyColor = ImColor(0, 173, 55); //玩家金钱颜色
                inline bool IsScoped = false; //是否开镜
                inline ImColor IsScopedColor = ImColor(0, 255, 255); //是否开镜颜色
                inline bool C4 = false; //是否携带C4
                inline ImColor C4Color = ImColor(255, 0, 0); //携带C4颜色
                inline bool HasDefuser = false; //是否携带拆弹器
                inline ImColor HasDefuserColor = ImColor(255, 255, 255); //携带拆弹器颜色
            }
            inline void draw_Preview_setting()
            {
                //静态变量初始化
                static float Vertex = 40.0f;
                static float Spacing = 25.0f;
                static float weaponTextHighlight = 0.0f;  //文本高亮动画进度
                static bool showPopup = false;            //弹窗显示状态标记
                static float popupAlpha = 0.0f;           //弹窗透明度（0-1）
                static float popupScale = 0.9f;           //弹窗缩放比例（0.9-1.0）
                static float showProgress = 0.0f;         //显示动画进度（0-1）
                static bool isTransitioning = false;      //是否正在动画过渡

                //动画参数配置
                const float highlightEasing = 10.0f;      //文本高亮动画速度
                const float animationSpeed = 8.0f;        //动画速度系数
                const float minProgress = 0.001f;         //最小进度阈值

                //1.绘制可点击文本
                const float textX = 10.0f;
                const float textY = Vertex;
                ImGui::SetCursorPosX(textX);
                ImGui::SetCursorPosY(textY);
                ImGui::PushFont(menu::font::menu_icon);

                const char* clickableText = "S";
                const ImVec2 textSize = ImGui::CalcTextSize(clickableText);
                const ImVec2 textScreenPos = ImGui::GetCursorScreenPos();

                //文本区域检测（用于悬停判断）
                const ImRect textRect(
                    textScreenPos,
                    ImVec2(textScreenPos.x + textSize.x, textScreenPos.y + textSize.y)
                );
                const bool isHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max);

                //文本高亮动画逻辑
                const float targetHighlight = isHovered ? 1.0f : 0.0f;
                weaponTextHighlight = approachValue(
                    weaponTextHighlight,
                    targetHighlight,
                    ImGui::GetIO().DeltaTime * highlightEasing
                );

                //文本颜色计算（手动插值）
                const ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);    //默认灰白色
                const ImVec4 highlightColor = ImVec4(0.9f, 0.6f, 0.8f, 1.0f);  //高亮粉色
                const float r = defaultColor.x + (highlightColor.x - defaultColor.x) * weaponTextHighlight;
                const float g = defaultColor.y + (highlightColor.y - defaultColor.y) * weaponTextHighlight;
                const float b = defaultColor.z + (highlightColor.z - defaultColor.z) * weaponTextHighlight;
                const float a = defaultColor.w + (highlightColor.w - defaultColor.w) * weaponTextHighlight;

                //绘制文本并恢复样式
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
                ImGui::Text(clickableText);
                ImGui::PopStyleColor();
                ImGui::PopFont();

                //2.文本点击处理（最高优先级）
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered)
                {
                    if (showPopup)
                    {
                        //直接隐藏，不使用动画
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                    else
                    {
                        //开始显示动画
                        isTransitioning = true;
                        showProgress = 0.0f;  //重置显示进度
                        showPopup = true;     //立即标记为显示状态
                    }
                    return;
                }

                //3.外部点击关闭弹窗逻辑
                if (showPopup && !isTransitioning && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    //1.检查是否点击了当前弹窗内的任何ImGui控件
                    bool isClickingWidget = ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();

                    //2.检查是否点击了弹窗窗口本身
                    const ImVec2 popupPos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImRect popupRect(
                        popupPos,
                        ImVec2(popupPos.x + 200.0f, popupPos.y + 150.0f)
                    );
                    bool isClickingWindowArea = popupRect.Contains(ImGui::GetMousePos());

                    //3.只有当既没有点击控件也没有点击窗口区域时，才关闭弹窗
                    if (!isClickingWidget && !isClickingWindowArea)
                    {
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                }

                //4.弹窗动画核心逻辑 - 只保留显示动画
                const float deltaTime = ImGui::GetIO().DeltaTime;
                bool animationComplete = false;

                if (isTransitioning && showPopup)
                {
                    //显示动画：从0到1
                    showProgress += deltaTime * animationSpeed;
                    showProgress = ImMin(showProgress, 1.0f);

                    if (showProgress >= 1.0f)
                    {
                        animationComplete = true;
                    }

                    //动画完成后更新状态
                    if (animationComplete)
                    {
                        isTransitioning = false;
                    }
                }

                //计算最终动画参数
                if (showPopup)
                {
                    //显示状态：使用显示进度或完全显示
                    const float progress = isTransitioning ? showProgress : 1.0f;
                    popupAlpha = progress;
                    popupScale = 0.9f + 0.1f * progress;
                }
                else
                {
                    //隐藏状态：直接设置为初始值
                    popupAlpha = 0.0f;
                    popupScale = 0.9f;
                }

                //5.绘制弹窗 - 只要有透明度就绘制
                if (popupAlpha > minProgress)
                {
                    const ImVec2 popupBasePos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImVec2 popupBaseSize(200.0f, 120.0f);  //窗口基础尺寸

                    //计算缩放偏移（保持顶部居中缩放）
                    const float offsetX = (popupBaseSize.x * (1.0f - popupScale)) * 0.5f;
                    const float offsetY = 0.0f;  //顶部对齐，Y轴无偏移

                    //设置弹窗位置和大小（带缩放）
                    ImGui::SetNextWindowPos(ImVec2(
                        popupBasePos.x - offsetX,
                        popupBasePos.y - offsetY
                    ));
                    ImGui::SetNextWindowSize(ImVec2(
                        popupBaseSize.x * popupScale,
                        popupBaseSize.y * popupScale
                    ));

                    //获取绘制列表并设置裁剪区域
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    drawList->PushClipRect(
                        ImGui::GetWindowPos(),
                        ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
                        true
                    );

                    //推送弹窗样式
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f * popupScale);

                    //保存原始颜色并只修改透明度
                    ImGuiStyle& style = ImGui::GetStyle();
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(
                        style.Colors[ImGuiCol_WindowBg].x,
                        style.Colors[ImGuiCol_WindowBg].y,
                        style.Colors[ImGuiCol_WindowBg].z,
                        style.Colors[ImGuiCol_WindowBg].w * popupAlpha
                    ));
                    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(
                        style.Colors[ImGuiCol_Border].x,
                        style.Colors[ImGuiCol_Border].y,
                        style.Colors[ImGuiCol_Border].z,
                        style.Colors[ImGuiCol_Border].w * popupAlpha
                    ));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(
                        style.Colors[ImGuiCol_Text].x,
                        style.Colors[ImGuiCol_Text].y,
                        style.Colors[ImGuiCol_Text].z,
                        style.Colors[ImGuiCol_Text].w * popupAlpha
                    ));

                    //绘制弹窗内容
                    if (ImGui::Begin("#Preview Setting", nullptr,
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoSavedSettings))
                    {
                        ImGui::Text(u8"Preview Setting");
                        ImGui::Separator();

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Ping", "#Player_Preview_Setting_Ping", &menu::vis_uals::Player::Preview_setting::Ping);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Player_Preview_Setting_Ping_Color", &menu::vis_uals::Player::Preview_setting::PingColor, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Money", "#Player_Preview_Setting_Money", &menu::vis_uals::Player::Preview_setting::Money);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Player_Preview_Setting_Money_Color", &menu::vis_uals::Player::Preview_setting::MoneyColor, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing * 2);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("Scoped", "#Player_Preview_Setting_IsScoped", &menu::vis_uals::Player::Preview_setting::IsScoped);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing * 2);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Player_Preview_Setting_IsScoped_Color", &menu::vis_uals::Player::Preview_setting::IsScopedColor, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing * 3);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("C4", "#Player_Preview_Setting_C4", &menu::vis_uals::Player::Preview_setting::C4);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing * 3);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Player_Preview_Setting_C4_Color", &menu::vis_uals::Player::Preview_setting::C4Color, 2.0f);

                        ImGui::SetCursorPosX(10);
                        ImGui::SetCursorPosY(Vertex + Spacing * 4);
                        smooth_api::CheckBox::SmoothCheckbox::Draw("HasDefuser", "#Player_Preview_Setting_HasDefuser", &menu::vis_uals::Player::Preview_setting::HasDefuser);

                        ImGui::SetCursorPosX(167);
                        ImGui::SetCursorPosY(Vertex + Spacing * 4);
                        smooth_api::Colors::SmoothColorPicker::Draw("#Player_Preview_Setting_HasDefuser_Color", &menu::vis_uals::Player::Preview_setting::HasDefuserColor, 2.0f);

                        ImGui::End();
                    }

                    //恢复样式和绘制列表
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    drawList->PopClipRect();
                }
                //当弹窗完全隐藏且不在过渡中，重置显示状态
                else if (!isTransitioning && !showPopup)
                {
                    showProgress = 0.0f;
                }
            }
        }

        namespace Local
        {
            inline bool Enabled = false; //全局
        }

        namespace World
        {
            inline bool Enabled = false; //全局
        }

        namespace Misc
        {

        }
    }

    namespace mi_sc
    {
        namespace particle
        {
            inline bool Enabled = false; //全局
            inline ImColor ParticleColor = ImColor(255, 255, 255, 255); //粒子颜色
            inline ImColor ParticleBgColor = ImColor(14, 14, 14, 141); //粒子背景颜色
            inline float Descent = 0.0f; //下落速度
            inline float Direction = 1.0f; //风向
            inline float Strength = 0.0f; //风力强度
            inline float Variation = 0.0f; //风向变化幅度
        }
    }
}

inline float approachValue(float current, float target, float delta)
{
    if (current < target)
    {
        current += delta;
        return current > target ? target : current;
    }
    else if (current > target)
    {
        current -= delta;
        return current < target ? target : current;
    }
    return current;
}

class EnhancedSnowEffect {
private:
    // 雪花粒子结构体
    struct Particle {
        ImVec2 pos;           // 位置
        ImVec2 vel;           // 速度
        float size;           // 大小
        ImColor color;        // 颜色

        // 物理属性
        float rotation;       // 旋转角度
        float rotationSpeed;  // 旋转速度
        float windResistance; // 风阻系数（小雪花风阻大）
        float gravity;        // 基础重力
        float gravityScale;   // 重力缩放（用于动态调整下落强度）
        float oscillation;    // 摆动幅度
        float oscillationSpeed; // 摆动速度
        float oscillationPhase; // 摆动相位（避免同步）
    };

    // 成员变量
    std::vector<Particle> particles;  // 粒子容器
    ImVec2 areaSize;                  // 作用区域大小
    bool initialized;                 // 初始化标志
    float windDirection;              // 风向（-1.0左 ~ 1.0右）
    float windStrength;               // 风力强度
    float windVariation;              // 风力变化幅度
    float windNoiseTimer;             // 风力扰动计时器
    float globalGravityScale;         // 全局重力缩放（控制下落强度）

    // 单例模式私有构造函数
    EnhancedSnowEffect() :
        initialized(false),
        windDirection(0.0f),
        windStrength(1.0f),
        windVariation(0.5f),
        windNoiseTimer(0.0f),
        globalGravityScale(1.0f) {
    }

    // 禁止拷贝
    EnhancedSnowEffect(const EnhancedSnowEffect&) = delete;
    EnhancedSnowEffect& operator=(const EnhancedSnowEffect&) = delete;

    // 高斯分布随机数（更自然的随机效果）
    float gaussianRand(float mean, float stddev) {
        static float z0, z1;
        static bool generate = false;
        generate = !generate;

        if (!generate) return z1 * stddev + mean;

        float u1, u2;
        do {
            u1 = static_cast<float>(rand()) / RAND_MAX;
            u2 = static_cast<float>(rand()) / RAND_MAX;
        } while (u1 <= 1e-6);

        float logu = log(u1);
        z0 = sqrt(-2.0f * logu) * cos(2.0f * IM_PI * u2);
        z1 = sqrt(-2.0f * logu) * sin(2.0f * IM_PI * u2);
        return z0 * stddev + mean;
    }

    // 风力扰动计算（模拟自然阵风）
    float getWindPerturbation(float deltaTime) {
        windNoiseTimer += deltaTime;
        // 多层正弦波叠加模拟自然风的不规则变化
        float noise = sin(windNoiseTimer * 0.5f) * 0.3f;
        noise += sin(windNoiseTimer * 1.7f) * 0.2f;
        noise += sin(windNoiseTimer * 3.1f) * 0.1f;
        return noise;
    }

public:
    // 获取单例实例
    static EnhancedSnowEffect& getInstance() {
        static EnhancedSnowEffect instance;
        return instance;
    }

    // 设置风力参数
    // direction: 风向（-1.0向左，1.0向右）
    // strength: 风力强度（>0）
    // variation: 风力变化幅度（>0）
    void setWindParameters(float direction, float strength, float variation) {
        windDirection = std::clamp(direction, -1.0f, 1.0f);
        windStrength = std::max(0.1f, strength);
        windVariation = std::max(0.0f, variation);
    }

    // 设置下落强度（值越大下落越快）
    void setFallIntensity(float intensity) {
        globalGravityScale = std::clamp(intensity, 0.5f, 5.0f);

        // 实时更新现有粒子
        if (initialized) {
            for (auto& p : particles) {
                p.gravityScale = globalGravityScale;
            }
        }
    }

    // 初始化雪花效果
    // count: 粒子数量
    // areaSize: 作用区域大小
    // minSize/maxSize: 雪花大小范围
    void init(int count, ImVec2 areaSize,
        float minSize = 0.5f, float maxSize = 3.0f) {
        this->areaSize = areaSize;
        particles.clear();
        particles.reserve(count);
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        for (int i = 0; i < count; ++i) {
            Particle p;

            // 初始位置（垂直方向稍微超出区域顶部，形成持续飘落效果）
            p.pos = ImVec2(
                static_cast<float>(rand()) / RAND_MAX * areaSize.x,
                static_cast<float>(rand()) / RAND_MAX * areaSize.y * 1.5f - areaSize.y * 0.5f
            );

            // 雪花大小（随机分布在min-max之间）
            float sizeFactor = minSize + (static_cast<float>(rand()) / RAND_MAX) * (maxSize - minSize);
            p.size = sizeFactor;

            // 物理属性（与大小关联，更符合自然规律）
            p.gravity = 0.5f + (sizeFactor / maxSize) * 1.0f;  // 大雪花重力更大
            p.gravityScale = globalGravityScale;                // 应用全局重力缩放
            p.windResistance = 0.2f + (1.0f - sizeFactor / maxSize) * 0.8f;  // 小雪花更易受风影响

            // 初始速度（垂直方向有初始速度）
            p.vel = ImVec2(0, p.gravity * 0.8f);

            // 旋转属性（随机初始角度和旋转速度）
            p.rotation = static_cast<float>(rand()) / RAND_MAX * 360.0f;
            p.rotationSpeed = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f;

            // 摆动属性（小幅度左右摇摆）
            p.oscillation = 0.08f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f * p.windResistance;
            p.oscillationSpeed = 0.5f + static_cast<float>(rand()) / RAND_MAX * 1.5f;
            p.oscillationPhase = static_cast<float>(rand()) / RAND_MAX * IM_PI * 2.0f;

            // 颜色（白色带透明度变化，小雪花更透明）
            float alpha = 0.5f + (0.5f * (sizeFactor / maxSize)) +
                (static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f);
            alpha = std::clamp(alpha, 0.3f, 0.9f);
            p.color = ImColor(255, 255, 255, static_cast<ImU32>(alpha * 255));

            particles.push_back(p);
        }
        initialized = true;
    }

    // 更新雪花状态（每帧调用）
    void update(float deltaTime = 0.016f) {
        if (!initialized) return;

        // 计算当前风力（基础风+随机扰动）
        float baseWind = windDirection * windStrength;
        float windPerturb = getWindPerturbation(deltaTime) * windVariation;
        float currentWind = baseWind + windPerturb;

        for (auto& p : particles) {
            // 应用重力（基础重力×缩放系数）
            p.vel.y = p.gravity * p.gravityScale;

            // 应用风力（受风阻影响）
            float windInfluence = currentWind * p.windResistance;

            // 应用摆动效果（正弦曲线模拟左右摇摆）
            float swing = sin(p.oscillationPhase + p.oscillationSpeed * windNoiseTimer) * p.oscillation;
            p.vel.x = windInfluence + swing;

            // 更新位置（乘以时间增量和速度系数）
            p.pos.x += p.vel.x * deltaTime * 30.0f;
            p.pos.y += p.vel.y * deltaTime * 35.0f;  // 垂直速度略快，突出下落效果

            // 更新旋转
            p.rotation += p.rotationSpeed * deltaTime * 60.0f;

            // 边界处理（循环显示，避免雪花消失）
            // X轴边界（左右循环）
            if (p.pos.x < 0) {
                p.pos.x = areaSize.x;
            }
            else if (p.pos.x > areaSize.x) {
                p.pos.x = 0;
            }

            // Y轴边界（落到底部后从顶部重新出现）
            if (p.pos.y > areaSize.y) {
                p.pos.y = -10.0f;  // 从顶部外一点点开始
                p.pos.x = static_cast<float>(rand()) / RAND_MAX * areaSize.x;

                // 重置部分属性，增加随机性
                p.rotationSpeed = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f;
                p.oscillationPhase = static_cast<float>(rand()) / RAND_MAX * IM_PI * 2.0f;
            }
        }
    }

    // 绘制雪花（每帧调用）
    void draw(ImDrawList* draw_list, ImVec2 windowPos = ImVec2(0, 0)) {
        if (!initialized) return;

        for (const auto& p : particles) {
            // 计算绘制位置（加上窗口偏移）
            ImVec2 drawPos = ImVec2(
                p.pos.x + windowPos.x,
                p.pos.y + windowPos.y
            );

            // 根据大小绘制不同形状（更真实的雪花外观）
            if (p.size < 1.0f) {
                // 小雪花用圆点
                draw_list->AddCircleFilled(drawPos, p.size, menu::mi_sc::particle::ParticleColor);
            }
            else if (p.size < 2.0f) {
                // 中等雪花用四角形
                const int points = 4;
                ImVec2 pointsArray[points];
                for (int i = 0; i < points; ++i) {
                    float angle = IM_PI * 2.0f * i / points + p.rotation * IM_PI / 180.0f;
                    pointsArray[i] = ImVec2(
                        drawPos.x + cosf(angle) * p.size,
                        drawPos.y + sinf(angle) * p.size
                    );
                }
                draw_list->AddConvexPolyFilled(pointsArray, points, menu::mi_sc::particle::ParticleColor);
            }
            else {
                // 大雪花用八角形（更像真实雪花）
                const int points = 8;
                ImVec2 pointsArray[points];
                for (int i = 0; i < points; ++i) {
                    float angle = IM_PI * 2.0f * i / points + p.rotation * IM_PI / 180.0f;
                    // 交替半径模拟雪花棱角
                    float radius = p.size * (i % 2 == 0 ? 1.0f : 0.6f);
                    pointsArray[i] = ImVec2(
                        drawPos.x + cosf(angle) * radius,
                        drawPos.y + sinf(angle) * radius
                    );
                }
                draw_list->AddConvexPolyFilled(pointsArray, points, menu::mi_sc::particle::ParticleColor);
            }
        }
    }

    // 重置效果
    void reset() {
        initialized = false;
        particles.clear();
    }
};