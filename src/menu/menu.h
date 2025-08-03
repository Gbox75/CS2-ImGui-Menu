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
            inline bool Enabled = false; //ȫ��
        }

        namespace Ragebot
        {
            inline bool Enabled = false; //ȫ��
            inline bool SilentAim = false; //��Ĭ
            namespace Settings
            {
                inline bool AutoShoot = false; //�Զ���ǹ
            }
            inline float FOV = 0.0f; //FOV
            inline char FOV_Input[60]{}; //FOV�û�����
        }
    }

    namespace anti_aim
    {
        namespace Antiaim
        {
            inline bool Enabled = false; //ȫ��
        }
    }

    namespace vis_uals
    {
        namespace Player
        {
            inline bool Enabled = false; //ȫ��
            inline bool Box = false; //����
            inline ImColor BoxColor = ImColor(255, 255, 255); //������ɫ
            inline bool Bone = false; //����
            inline ImColor BoneVisualColor = ImColor(255, 255, 255); //���ӹ�����ɫ
            inline ImColor BoneInVisualColor = ImColor(0, 0, 0); //�����ӹ�����ɫ
            inline bool Health = false; //Ѫ��
            inline ImColor HealthColor = ImColor(0, 255, 125); //Ѫ����ɫ
            inline bool Name = false; //����
            inline ImColor NameColor = ImColor(255, 255, 255); //������ɫ
            inline bool Weapon = false; //����     
            namespace Weapon_setting
            {
                inline bool Icon = false; //����ͼ��
                inline ImColor IconColor = ImColor(0, 255, 255); //����ͼ����ɫ
                inline bool Name = false; //��������
                inline ImColor NameColor = ImColor(255, 255, 255); //����������ɫ
                inline bool Ammo = false; //��ҩ��
                inline ImColor AmmoColor = ImColor(0, 78, 113); //������ҩ��ɫ
            }
            inline void draw_weapon_setting()
            {
                //��̬������ʼ��
                static float Vertex = 40.0f;
                static float Spacing = 25.0f;
                static float weaponTextHighlight = 0.0f;  //�ı�������������
                static bool showPopup = false;            //������ʾ״̬���
                static float popupAlpha = 0.0f;           //����͸���ȣ�0-1��
                static float popupScale = 0.9f;           //�������ű�����0.9-1.0��
                static float showProgress = 0.0f;         //��ʾ�������ȣ�0-1��
                static bool isTransitioning = false;      //�Ƿ����ڶ�������

                //������������
                const float highlightEasing = 10.0f;      //�ı����������ٶ�
                const float animationSpeed = 8.0f;        //�����ٶ�ϵ��
                const float minProgress = 0.001f;         //��С������ֵ

                //1.���ƿɵ���ı�
                const float textX = 245.0f;
                const float textY = Vertex + Spacing * 5.0f;
                ImGui::SetCursorPosX(textX);
                ImGui::SetCursorPosY(textY);
                ImGui::PushFont(menu::font::menu_icon);

                const char* clickableText = "S";
                const ImVec2 textSize = ImGui::CalcTextSize(clickableText);
                const ImVec2 textScreenPos = ImGui::GetCursorScreenPos();

                //�ı������⣨������ͣ�жϣ�
                const ImRect textRect(
                    textScreenPos,
                    ImVec2(textScreenPos.x + textSize.x, textScreenPos.y + textSize.y)
                );
                const bool isHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max);

                //�ı����������߼�
                const float targetHighlight = isHovered ? 1.0f : 0.0f;
                weaponTextHighlight = approachValue(
                    weaponTextHighlight,
                    targetHighlight,
                    ImGui::GetIO().DeltaTime * highlightEasing
                );

                //�ı���ɫ���㣨�ֶ���ֵ��
                const ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);    //Ĭ�ϻҰ�ɫ
                const ImVec4 highlightColor = ImVec4(0.9f, 0.6f, 0.8f, 1.0f);  //������ɫ
                const float r = defaultColor.x + (highlightColor.x - defaultColor.x) * weaponTextHighlight;
                const float g = defaultColor.y + (highlightColor.y - defaultColor.y) * weaponTextHighlight;
                const float b = defaultColor.z + (highlightColor.z - defaultColor.z) * weaponTextHighlight;
                const float a = defaultColor.w + (highlightColor.w - defaultColor.w) * weaponTextHighlight;

                //�����ı����ָ���ʽ
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
                ImGui::Text(clickableText);
                ImGui::PopStyleColor();
                ImGui::PopFont();

                //2.�ı��������������ȼ���
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered)
                {
                    if (showPopup)
                    {
                        //ֱ�����أ���ʹ�ö���
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                    else
                    {
                        //��ʼ��ʾ����
                        isTransitioning = true;
                        showProgress = 0.0f;  //������ʾ����
                        showPopup = true;     //�������Ϊ��ʾ״̬
                    }
                    return;
                }

                //3.�ⲿ����رյ����߼�
                if (showPopup && !isTransitioning && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    //1.����Ƿ����˵�ǰ�����ڵ��κ�ImGui�ؼ�
                    bool isClickingWidget = ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();

                    //2.����Ƿ����˵������ڱ���
                    const ImVec2 popupPos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImRect popupRect(
                        popupPos,
                        ImVec2(popupPos.x + 200.0f, popupPos.y + 150.0f)
                    );
                    bool isClickingWindowArea = popupRect.Contains(ImGui::GetMousePos());

                    //3.ֻ�е���û�е���ؼ�Ҳû�е����������ʱ���Źرյ���
                    if (!isClickingWidget && !isClickingWindowArea)
                    {
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                }

                //4.�������������߼� - ֻ������ʾ����
                const float deltaTime = ImGui::GetIO().DeltaTime;
                bool animationComplete = false;

                if (isTransitioning && showPopup)
                {
                    //��ʾ��������0��1
                    showProgress += deltaTime * animationSpeed;
                    showProgress = ImMin(showProgress, 1.0f);

                    if (showProgress >= 1.0f)
                    {
                        animationComplete = true;
                    }

                    //������ɺ����״̬
                    if (animationComplete)
                    {
                        isTransitioning = false;
                    }
                }

                //�������ն�������
                if (showPopup)
                {
                    //��ʾ״̬��ʹ����ʾ���Ȼ���ȫ��ʾ
                    const float progress = isTransitioning ? showProgress : 1.0f;
                    popupAlpha = progress;
                    popupScale = 0.9f + 0.1f * progress;
                }
                else
                {
                    //����״̬��ֱ������Ϊ��ʼֵ
                    popupAlpha = 0.0f;
                    popupScale = 0.9f;
                }

                //5.���Ƶ��� - ֻҪ��͸���Ⱦͻ���
                if (popupAlpha > minProgress)
                {
                    const ImVec2 popupBasePos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImVec2 popupBaseSize(200.0f, 120.0f);  //���ڻ����ߴ�

                    //��������ƫ�ƣ����ֶ����������ţ�
                    const float offsetX = (popupBaseSize.x * (1.0f - popupScale)) * 0.5f;
                    const float offsetY = 0.0f;  //�������룬Y����ƫ��

                    //���õ���λ�úʹ�С�������ţ�
                    ImGui::SetNextWindowPos(ImVec2(
                        popupBasePos.x - offsetX,
                        popupBasePos.y - offsetY
                    ));
                    ImGui::SetNextWindowSize(ImVec2(
                        popupBaseSize.x * popupScale,
                        popupBaseSize.y * popupScale
                    ));

                    //��ȡ�����б����òü�����
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    drawList->PushClipRect(
                        ImGui::GetWindowPos(),
                        ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
                        true
                    );

                    //���͵�����ʽ
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f * popupScale);

                    //����ԭʼ��ɫ��ֻ�޸�͸����
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

                    //���Ƶ�������
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

                    //�ָ���ʽ�ͻ����б�
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    drawList->PopClipRect();
                }
                //��������ȫ�����Ҳ��ڹ����У�������ʾ״̬
                else if (!isTransitioning && !showPopup)
                {
                    showProgress = 0.0f;
                }
            }
            inline bool Glow = false; //����
            inline ImColor GlowColor = ImColor(255, 255, 255); //������ɫ
            inline bool BehindTheWall = false; //ǽ��
            namespace Preview_setting
            {
                inline bool Ping = false; //���Ping
                inline ImColor PingColor = ImColor(255, 0, 0); //���Ping��ɫ
                inline bool Money = false; //��ҽ�Ǯ
                inline ImColor MoneyColor = ImColor(0, 173, 55); //��ҽ�Ǯ��ɫ
                inline bool IsScoped = false; //�Ƿ񿪾�
                inline ImColor IsScopedColor = ImColor(0, 255, 255); //�Ƿ񿪾���ɫ
                inline bool C4 = false; //�Ƿ�Я��C4
                inline ImColor C4Color = ImColor(255, 0, 0); //Я��C4��ɫ
                inline bool HasDefuser = false; //�Ƿ�Я������
                inline ImColor HasDefuserColor = ImColor(255, 255, 255); //Я��������ɫ
            }
            inline void draw_Preview_setting()
            {
                //��̬������ʼ��
                static float Vertex = 40.0f;
                static float Spacing = 25.0f;
                static float weaponTextHighlight = 0.0f;  //�ı�������������
                static bool showPopup = false;            //������ʾ״̬���
                static float popupAlpha = 0.0f;           //����͸���ȣ�0-1��
                static float popupScale = 0.9f;           //�������ű�����0.9-1.0��
                static float showProgress = 0.0f;         //��ʾ�������ȣ�0-1��
                static bool isTransitioning = false;      //�Ƿ����ڶ�������

                //������������
                const float highlightEasing = 10.0f;      //�ı����������ٶ�
                const float animationSpeed = 8.0f;        //�����ٶ�ϵ��
                const float minProgress = 0.001f;         //��С������ֵ

                //1.���ƿɵ���ı�
                const float textX = 10.0f;
                const float textY = Vertex;
                ImGui::SetCursorPosX(textX);
                ImGui::SetCursorPosY(textY);
                ImGui::PushFont(menu::font::menu_icon);

                const char* clickableText = "S";
                const ImVec2 textSize = ImGui::CalcTextSize(clickableText);
                const ImVec2 textScreenPos = ImGui::GetCursorScreenPos();

                //�ı������⣨������ͣ�жϣ�
                const ImRect textRect(
                    textScreenPos,
                    ImVec2(textScreenPos.x + textSize.x, textScreenPos.y + textSize.y)
                );
                const bool isHovered = ImGui::IsMouseHoveringRect(textRect.Min, textRect.Max);

                //�ı����������߼�
                const float targetHighlight = isHovered ? 1.0f : 0.0f;
                weaponTextHighlight = approachValue(
                    weaponTextHighlight,
                    targetHighlight,
                    ImGui::GetIO().DeltaTime * highlightEasing
                );

                //�ı���ɫ���㣨�ֶ���ֵ��
                const ImVec4 defaultColor = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);    //Ĭ�ϻҰ�ɫ
                const ImVec4 highlightColor = ImVec4(0.9f, 0.6f, 0.8f, 1.0f);  //������ɫ
                const float r = defaultColor.x + (highlightColor.x - defaultColor.x) * weaponTextHighlight;
                const float g = defaultColor.y + (highlightColor.y - defaultColor.y) * weaponTextHighlight;
                const float b = defaultColor.z + (highlightColor.z - defaultColor.z) * weaponTextHighlight;
                const float a = defaultColor.w + (highlightColor.w - defaultColor.w) * weaponTextHighlight;

                //�����ı����ָ���ʽ
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, a));
                ImGui::Text(clickableText);
                ImGui::PopStyleColor();
                ImGui::PopFont();

                //2.�ı��������������ȼ���
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered)
                {
                    if (showPopup)
                    {
                        //ֱ�����أ���ʹ�ö���
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                    else
                    {
                        //��ʼ��ʾ����
                        isTransitioning = true;
                        showProgress = 0.0f;  //������ʾ����
                        showPopup = true;     //�������Ϊ��ʾ״̬
                    }
                    return;
                }

                //3.�ⲿ����رյ����߼�
                if (showPopup && !isTransitioning && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    //1.����Ƿ����˵�ǰ�����ڵ��κ�ImGui�ؼ�
                    bool isClickingWidget = ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();

                    //2.����Ƿ����˵������ڱ���
                    const ImVec2 popupPos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImRect popupRect(
                        popupPos,
                        ImVec2(popupPos.x + 200.0f, popupPos.y + 150.0f)
                    );
                    bool isClickingWindowArea = popupRect.Contains(ImGui::GetMousePos());

                    //3.ֻ�е���û�е���ؼ�Ҳû�е����������ʱ���Źرյ���
                    if (!isClickingWidget && !isClickingWindowArea)
                    {
                        showPopup = false;
                        showProgress = 0.0f;
                        popupAlpha = 0.0f;
                        popupScale = 0.9f;
                    }
                }

                //4.�������������߼� - ֻ������ʾ����
                const float deltaTime = ImGui::GetIO().DeltaTime;
                bool animationComplete = false;

                if (isTransitioning && showPopup)
                {
                    //��ʾ��������0��1
                    showProgress += deltaTime * animationSpeed;
                    showProgress = ImMin(showProgress, 1.0f);

                    if (showProgress >= 1.0f)
                    {
                        animationComplete = true;
                    }

                    //������ɺ����״̬
                    if (animationComplete)
                    {
                        isTransitioning = false;
                    }
                }

                //�������ն�������
                if (showPopup)
                {
                    //��ʾ״̬��ʹ����ʾ���Ȼ���ȫ��ʾ
                    const float progress = isTransitioning ? showProgress : 1.0f;
                    popupAlpha = progress;
                    popupScale = 0.9f + 0.1f * progress;
                }
                else
                {
                    //����״̬��ֱ������Ϊ��ʼֵ
                    popupAlpha = 0.0f;
                    popupScale = 0.9f;
                }

                //5.���Ƶ��� - ֻҪ��͸���Ⱦͻ���
                if (popupAlpha > minProgress)
                {
                    const ImVec2 popupBasePos(
                        textScreenPos.x,
                        textScreenPos.y + textSize.y - 10.0f
                    );
                    const ImVec2 popupBaseSize(200.0f, 120.0f);  //���ڻ����ߴ�

                    //��������ƫ�ƣ����ֶ����������ţ�
                    const float offsetX = (popupBaseSize.x * (1.0f - popupScale)) * 0.5f;
                    const float offsetY = 0.0f;  //�������룬Y����ƫ��

                    //���õ���λ�úʹ�С�������ţ�
                    ImGui::SetNextWindowPos(ImVec2(
                        popupBasePos.x - offsetX,
                        popupBasePos.y - offsetY
                    ));
                    ImGui::SetNextWindowSize(ImVec2(
                        popupBaseSize.x * popupScale,
                        popupBaseSize.y * popupScale
                    ));

                    //��ȡ�����б����òü�����
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    drawList->PushClipRect(
                        ImGui::GetWindowPos(),
                        ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                            ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
                        true
                    );

                    //���͵�����ʽ
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f * popupScale);

                    //����ԭʼ��ɫ��ֻ�޸�͸����
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

                    //���Ƶ�������
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

                    //�ָ���ʽ�ͻ����б�
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    drawList->PopClipRect();
                }
                //��������ȫ�����Ҳ��ڹ����У�������ʾ״̬
                else if (!isTransitioning && !showPopup)
                {
                    showProgress = 0.0f;
                }
            }
        }

        namespace Local
        {
            inline bool Enabled = false; //ȫ��
        }

        namespace World
        {
            inline bool Enabled = false; //ȫ��
        }

        namespace Misc
        {

        }
    }

    namespace mi_sc
    {
        namespace particle
        {
            inline bool Enabled = false; //ȫ��
            inline ImColor ParticleColor = ImColor(255, 255, 255, 255); //������ɫ
            inline ImColor ParticleBgColor = ImColor(14, 14, 14, 141); //���ӱ�����ɫ
            inline float Descent = 0.0f; //�����ٶ�
            inline float Direction = 1.0f; //����
            inline float Strength = 0.0f; //����ǿ��
            inline float Variation = 0.0f; //����仯����
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
    // ѩ�����ӽṹ��
    struct Particle {
        ImVec2 pos;           // λ��
        ImVec2 vel;           // �ٶ�
        float size;           // ��С
        ImColor color;        // ��ɫ

        // ��������
        float rotation;       // ��ת�Ƕ�
        float rotationSpeed;  // ��ת�ٶ�
        float windResistance; // ����ϵ����Сѩ�������
        float gravity;        // ��������
        float gravityScale;   // �������ţ����ڶ�̬��������ǿ�ȣ�
        float oscillation;    // �ڶ�����
        float oscillationSpeed; // �ڶ��ٶ�
        float oscillationPhase; // �ڶ���λ������ͬ����
    };

    // ��Ա����
    std::vector<Particle> particles;  // ��������
    ImVec2 areaSize;                  // ���������С
    bool initialized;                 // ��ʼ����־
    float windDirection;              // ����-1.0�� ~ 1.0�ң�
    float windStrength;               // ����ǿ��
    float windVariation;              // �����仯����
    float windNoiseTimer;             // �����Ŷ���ʱ��
    float globalGravityScale;         // ȫ���������ţ���������ǿ�ȣ�

    // ����ģʽ˽�й��캯��
    EnhancedSnowEffect() :
        initialized(false),
        windDirection(0.0f),
        windStrength(1.0f),
        windVariation(0.5f),
        windNoiseTimer(0.0f),
        globalGravityScale(1.0f) {
    }

    // ��ֹ����
    EnhancedSnowEffect(const EnhancedSnowEffect&) = delete;
    EnhancedSnowEffect& operator=(const EnhancedSnowEffect&) = delete;

    // ��˹�ֲ������������Ȼ�����Ч����
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

    // �����Ŷ����㣨ģ����Ȼ��磩
    float getWindPerturbation(float deltaTime) {
        windNoiseTimer += deltaTime;
        // ������Ҳ�����ģ����Ȼ��Ĳ�����仯
        float noise = sin(windNoiseTimer * 0.5f) * 0.3f;
        noise += sin(windNoiseTimer * 1.7f) * 0.2f;
        noise += sin(windNoiseTimer * 3.1f) * 0.1f;
        return noise;
    }

public:
    // ��ȡ����ʵ��
    static EnhancedSnowEffect& getInstance() {
        static EnhancedSnowEffect instance;
        return instance;
    }

    // ���÷�������
    // direction: ����-1.0����1.0���ң�
    // strength: ����ǿ�ȣ�>0��
    // variation: �����仯���ȣ�>0��
    void setWindParameters(float direction, float strength, float variation) {
        windDirection = std::clamp(direction, -1.0f, 1.0f);
        windStrength = std::max(0.1f, strength);
        windVariation = std::max(0.0f, variation);
    }

    // ��������ǿ�ȣ�ֵԽ������Խ�죩
    void setFallIntensity(float intensity) {
        globalGravityScale = std::clamp(intensity, 0.5f, 5.0f);

        // ʵʱ������������
        if (initialized) {
            for (auto& p : particles) {
                p.gravityScale = globalGravityScale;
            }
        }
    }

    // ��ʼ��ѩ��Ч��
    // count: ��������
    // areaSize: ���������С
    // minSize/maxSize: ѩ����С��Χ
    void init(int count, ImVec2 areaSize,
        float minSize = 0.5f, float maxSize = 3.0f) {
        this->areaSize = areaSize;
        particles.clear();
        particles.reserve(count);
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        for (int i = 0; i < count; ++i) {
            Particle p;

            // ��ʼλ�ã���ֱ������΢�������򶥲����γɳ���Ʈ��Ч����
            p.pos = ImVec2(
                static_cast<float>(rand()) / RAND_MAX * areaSize.x,
                static_cast<float>(rand()) / RAND_MAX * areaSize.y * 1.5f - areaSize.y * 0.5f
            );

            // ѩ����С������ֲ���min-max֮�䣩
            float sizeFactor = minSize + (static_cast<float>(rand()) / RAND_MAX) * (maxSize - minSize);
            p.size = sizeFactor;

            // �������ԣ����С��������������Ȼ���ɣ�
            p.gravity = 0.5f + (sizeFactor / maxSize) * 1.0f;  // ��ѩ����������
            p.gravityScale = globalGravityScale;                // Ӧ��ȫ����������
            p.windResistance = 0.2f + (1.0f - sizeFactor / maxSize) * 0.8f;  // Сѩ�������ܷ�Ӱ��

            // ��ʼ�ٶȣ���ֱ�����г�ʼ�ٶȣ�
            p.vel = ImVec2(0, p.gravity * 0.8f);

            // ��ת���ԣ������ʼ�ǶȺ���ת�ٶȣ�
            p.rotation = static_cast<float>(rand()) / RAND_MAX * 360.0f;
            p.rotationSpeed = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f;

            // �ڶ����ԣ�С��������ҡ�ڣ�
            p.oscillation = 0.08f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f * p.windResistance;
            p.oscillationSpeed = 0.5f + static_cast<float>(rand()) / RAND_MAX * 1.5f;
            p.oscillationPhase = static_cast<float>(rand()) / RAND_MAX * IM_PI * 2.0f;

            // ��ɫ����ɫ��͸���ȱ仯��Сѩ����͸����
            float alpha = 0.5f + (0.5f * (sizeFactor / maxSize)) +
                (static_cast<float>(rand()) / RAND_MAX * 0.2f - 0.1f);
            alpha = std::clamp(alpha, 0.3f, 0.9f);
            p.color = ImColor(255, 255, 255, static_cast<ImU32>(alpha * 255));

            particles.push_back(p);
        }
        initialized = true;
    }

    // ����ѩ��״̬��ÿ֡���ã�
    void update(float deltaTime = 0.016f) {
        if (!initialized) return;

        // ���㵱ǰ������������+����Ŷ���
        float baseWind = windDirection * windStrength;
        float windPerturb = getWindPerturbation(deltaTime) * windVariation;
        float currentWind = baseWind + windPerturb;

        for (auto& p : particles) {
            // Ӧ����������������������ϵ����
            p.vel.y = p.gravity * p.gravityScale;

            // Ӧ�÷������ܷ���Ӱ�죩
            float windInfluence = currentWind * p.windResistance;

            // Ӧ�ðڶ�Ч������������ģ������ҡ�ڣ�
            float swing = sin(p.oscillationPhase + p.oscillationSpeed * windNoiseTimer) * p.oscillation;
            p.vel.x = windInfluence + swing;

            // ����λ�ã�����ʱ���������ٶ�ϵ����
            p.pos.x += p.vel.x * deltaTime * 30.0f;
            p.pos.y += p.vel.y * deltaTime * 35.0f;  // ��ֱ�ٶ��Կ죬ͻ������Ч��

            // ������ת
            p.rotation += p.rotationSpeed * deltaTime * 60.0f;

            // �߽紦��ѭ����ʾ������ѩ����ʧ��
            // X��߽磨����ѭ����
            if (p.pos.x < 0) {
                p.pos.x = areaSize.x;
            }
            else if (p.pos.x > areaSize.x) {
                p.pos.x = 0;
            }

            // Y��߽磨�䵽�ײ���Ӷ������³��֣�
            if (p.pos.y > areaSize.y) {
                p.pos.y = -10.0f;  // �Ӷ�����һ��㿪ʼ
                p.pos.x = static_cast<float>(rand()) / RAND_MAX * areaSize.x;

                // ���ò������ԣ����������
                p.rotationSpeed = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f;
                p.oscillationPhase = static_cast<float>(rand()) / RAND_MAX * IM_PI * 2.0f;
            }
        }
    }

    // ����ѩ����ÿ֡���ã�
    void draw(ImDrawList* draw_list, ImVec2 windowPos = ImVec2(0, 0)) {
        if (!initialized) return;

        for (const auto& p : particles) {
            // �������λ�ã����ϴ���ƫ�ƣ�
            ImVec2 drawPos = ImVec2(
                p.pos.x + windowPos.x,
                p.pos.y + windowPos.y
            );

            // ���ݴ�С���Ʋ�ͬ��״������ʵ��ѩ����ۣ�
            if (p.size < 1.0f) {
                // Сѩ����Բ��
                draw_list->AddCircleFilled(drawPos, p.size, menu::mi_sc::particle::ParticleColor);
            }
            else if (p.size < 2.0f) {
                // �е�ѩ�����Ľ���
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
                // ��ѩ���ð˽��Σ�������ʵѩ����
                const int points = 8;
                ImVec2 pointsArray[points];
                for (int i = 0; i < points; ++i) {
                    float angle = IM_PI * 2.0f * i / points + p.rotation * IM_PI / 180.0f;
                    // ����뾶ģ��ѩ�����
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

    // ����Ч��
    void reset() {
        initialized = false;
        particles.clear();
    }
};