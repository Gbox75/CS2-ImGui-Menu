#include "Menu.h"
#include <algorithm>
#include "../myimgui.h"

void menu::api::set_menu_flags()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.Colors[ImGuiCol_WindowBg] = ImColor(23, 23, 23, 255);
}

void menu::api::set_menu_window_size()
{
    ImGui::SetNextWindowSize(ImVec2(800, 550));
}

ImGuiWindowFlags menu::api::set_menu_window_flags()
{
    ImGuiWindowFlags BeginFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings;
    return BeginFlags;
}

void menu::api::draw_menu_pad(ImDrawList* draw_list, ImVec2 window_pos)
{
    ImVec2 rect1_min, rect1_max;
    rect1_min.x = window_pos.x;
    rect1_min.y = window_pos.y;
    rect1_max.x = rect1_min.x + 800;
    rect1_max.y = rect1_min.y + 60;
    draw_list->AddRectFilled(rect1_min, rect1_max, ImColor(37, 37, 37), 10.0f, ImDrawFlags_RoundCornersTop);
    ImVec2 p1(window_pos.x, window_pos.y + 60);
    ImVec2 p2(window_pos.x + 800, window_pos.y + 60);
    draw_list->AddLine(p1, p2, ImColor(207, 155, 166), 1.0f);
}

void menu::api::draw_menu_logo()
{
    ImGui::SetCursorPosX(15);
    ImGui::SetCursorPosY(10);
    ImGui::Image((ImTextureID)image::logo, ImVec2(140, 40));
}

void menu::api::draw_particle()
{
    RECT clientRect;
    GetClientRect(myimgui::g_hwnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;
    static bool firstFrame = true;
    if (firstFrame)
    {
        EnhancedSnowEffect::getInstance().init(60, ImVec2(clientWidth, clientHeight));
        firstFrame = false;
    }
    float originalWindowRounding = ImGui::GetStyle().WindowRounding;
    ImGui::SetNextWindowSize(ImVec2(1920, 1080));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(menu::mi_sc::particle::ParticleBgColor);
    if (ImGui::Begin("cs2_cheat_particle", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_Modal))
    {
        if (menu::mi_sc::particle::Enabled)
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 window_pos = ImGui::GetWindowPos();
            EnhancedSnowEffect::getInstance().setWindParameters(menu::mi_sc::particle::Direction, menu::mi_sc::particle::Strength, menu::mi_sc::particle::Variation);
            EnhancedSnowEffect::getInstance().setFallIntensity(menu::mi_sc::particle::Descent);
            EnhancedSnowEffect::getInstance().update();
            EnhancedSnowEffect::getInstance().draw(draw_list, window_pos);
        }
    }
    ImGui::End();
    ImGui::GetStyle().WindowRounding = originalWindowRounding;
}

void menu::api::draw_menu()
{
    static float alpha = 1.0f;
    const float TRANSITION_DURATION = 0.14f;

    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false))
    {
        if (!is_transitioning)
        {
            is_transitioning = true;
            transition_time = 0.0f;
            is_visible = !is_visible;
        }
    }

    if (is_transitioning)
    {
        transition_time += ImGui::GetIO().DeltaTime;
        float t = transition_time / TRANSITION_DURATION;

        if (is_visible)
        {
            alpha = (1.0f - cosf(t * IM_PI)) * 0.5f;
        }
        else
        {
            alpha = (1.0f + cosf(t * IM_PI)) * 0.5f;
        }

        if (t >= 1.0f)
        {
            is_transitioning = false;
            alpha = is_visible ? 1.0f : 0.0f;
        }
    }
    if (alpha <= 0.0f && !is_transitioning)
        return;

    ImGuiIO& io = ImGui::GetIO();

    if (is_visible)
    {
        io.MouseDrawCursor = true;
    }
    else
    {
        io.MouseDrawCursor = false;
    }

    draw_particle();
    set_menu_flags();
    set_menu_window_size();
    ImGuiStyle backup_style = ImGui::GetStyle();
    ImGui::GetStyle().Alpha = alpha;
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    if (ImGui::Begin("cs2_cheat_primordial", nullptr, set_menu_window_flags()))
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();

        int cmd_count = draw_list->CmdBuffer.Size;

        draw_menu_pad(draw_list, window_pos);
        draw_menu_logo();
        draw_menu_toolbar(draw_list, window_pos);
        draw_menu_toolbar_text();
        draw_menu_toolbar_line(draw_list, window_pos);
        draw_menu_parent_window(draw_list, window_pos);

        for (int i = cmd_count; i < draw_list->CmdBuffer.Size; i++)
        {
            ImDrawCmd* cmd = &draw_list->CmdBuffer[i];
            cmd->ClipRect.w *= alpha;
            cmd->ClipRect.z *= alpha;

            if (cmd->UserCallback == nullptr)
            {
                for (int j = 0; j < draw_list->VtxBuffer.Size; j++)
                {
                    ImDrawVert& vert = draw_list->VtxBuffer[j];
                    vert.col = IM_COL32(
                        (vert.col >> 0) & 0xFF,
                        (vert.col >> 8) & 0xFF,
                        (vert.col >> 16) & 0xFF,
                        (uint8_t)(((vert.col >> 24) & 0xFF) * alpha)
                    );
                }
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::GetStyle() = backup_style;
}

void menu::api::draw_menu_toolbar(ImDrawList* draw_list, ImVec2 window_pos)
{
    ImGui::PushFont(font::menu_text);
    ImGui::SetCursorPosX(5);
    ImGui::SetCursorPosY(80);
    ImVec2 toolbar_pos = ImGui::GetCursorScreenPos();
    if (component::toolbar::menu_toolbar_index[0])
    {
        smooth_api::Toolbar::SmoothVerticalRadioToolbar::Draw("aim_bot", component::toolbar::toolbar_1::menu_toolbar_titles_0, component::toolbar::toolbar_1::menu_toolbar_index_0, 2, 4.0f, 0.0f, 5.0f);
    }
    if (component::toolbar::menu_toolbar_index[1])
    {
        smooth_api::Toolbar::SmoothVerticalRadioToolbar::Draw("anti_aim", component::toolbar::toolbar_1::menu_toolbar_titles_1, component::toolbar::toolbar_1::menu_toolbar_index_1, 1, 4.0f, 0.0f, 5.0f);
    }
    if (component::toolbar::menu_toolbar_index[2])
    {
        smooth_api::Toolbar::SmoothVerticalRadioToolbar::Draw("vis_uals", component::toolbar::toolbar_1::menu_toolbar_titles_2, component::toolbar::toolbar_1::menu_toolbar_index_2, 4, 4.0f, 0.0f, 5.0f);
    }
    if (component::toolbar::menu_toolbar_index[3])
    {
        smooth_api::Toolbar::SmoothVerticalRadioToolbar::Draw("mi_sc", component::toolbar::toolbar_1::menu_toolbar_titles_3, component::toolbar::toolbar_1::menu_toolbar_index_3, 3, 4.0f, 0.0f, 5.0f);
    }
    ImVec2 line_start = toolbar_pos;
    line_start.x += 170.0f;
    line_start.y -= 19.0f;
    ImVec2 line_end = line_start;
    line_end.y += 430.0f;
    draw_list->AddLine(line_start, line_end, ImColor(0, 0, 0), 3.0f);
    draw_list->AddLine(line_start, line_end, ImColor(0, 0, 0), 2.0f);
    draw_list->AddLine(line_start, line_end, ImColor(40, 40, 40), 1.0f);
    ImGui::PopFont();

    ImGui::SetCursorPosX(0);
    ImGui::SetCursorPosY(489);
    ImGui::PushFont(font::menu_icon);
    smooth_api::Toolbar::SmoothRadioToolbar::Draw("toolbar", component::toolbar::menu_toolbar_titles, component::toolbar::menu_toolbar_index, 4, 4.0f, 10.0f, 8.0f);
    ImGui::PopFont();
}

void menu::api::draw_menu_toolbar_text()
{
    ImGui::PushFont(font::menu_text);
    ImGui::SetCursorPosX(266);
    ImGui::SetCursorPosY(525);
    ImGui::Text("Aimbot");

    ImGui::SetCursorPosX(340);
    ImGui::SetCursorPosY(525);
    ImGui::Text("Antiaim");

    ImGui::SetCursorPosX(418);
    ImGui::SetCursorPosY(525);
    ImGui::Text("Visuals");

    ImGui::SetCursorPosX(499);
    ImGui::SetCursorPosY(525);
    ImGui::Text("Misc");
    ImGui::PopFont();
}

void menu::api::draw_menu_toolbar_line(ImDrawList* draw_list, ImVec2 window_pos)
{
    ImVec2 p1(window_pos.x, window_pos.y + 488);
    ImVec2 p2(window_pos.x + 800, window_pos.y + 488);
    draw_list->AddLine(p1, p2, ImColor(207, 155, 166), 1.0f);
}

void menu::api::create_menu_parent_window(menu_fun* fun, const char** id, ImVec2 size, ImVec2 pos)
{
    ImGui::SetCursorPosX(pos.x);
    ImGui::SetCursorPosY(pos.y);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(34, 34, 34).Value);
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));

    ImVec2 childSize = size;
    ImColor shadowColor1(0, 0, 0, 120);
    ImColor shadowColor2(0, 0, 0, 60);

    ImVec2 currentPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(currentPos.x + 4, currentPos.y + 4));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, shadowColor1.Value);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 17.0f);
    ImGui::BeginChild(id[3], ImVec2(childSize.x + 2, childSize.y + 2), false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(currentPos.x + 2, currentPos.y + 2));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, shadowColor2.Value);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.0f);
    ImGui::BeginChild(id[2], ImVec2(childSize.x + 1, childSize.y + 1), false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(currentPos);
    ImGui::BeginChild(id[1], childSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();
        float windowWidth = ImGui::GetWindowWidth();
        float fixedHeight = 30.0f;
        ImVec2 rect1_min = windowPos;
        ImVec2 rect1_max = ImVec2(windowPos.x + windowWidth, windowPos.y + fixedHeight);
        draw_list->AddRectFilled(rect1_min, rect1_max, ImColor(41, 41, 41), 10.0f, ImDrawFlags_RoundCornersTop);
        draw_list->AddRect(rect1_min, rect1_max, ImColor(0, 0, 0), 10.0f, ImDrawFlags_RoundCornersTop);
        ImVec2 p1 = ImVec2(windowPos.x, windowPos.y + fixedHeight);
        ImVec2 p2 = ImVec2(windowPos.x + windowWidth, windowPos.y + fixedHeight);
        draw_list->AddLine(p1, p2, ImColor(207, 155, 166), 1.0f);
        ImGui::SetCursorPosX(10);
        ImGui::SetCursorPosY(6);
        ImGui::PushFont(font::menu_text);
        ImGui::Text(id[0]);
        ImGui::PopFont();
        float separatorLocalY = (windowPos.y + fixedHeight) - windowPos.y;
        ImGui::SetCursorPosY(separatorLocalY + 10.0f);
        ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
        fun();
    }
    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void menu::api::draw_menu_parent_window(ImDrawList* draw_list, ImVec2 window_pos)
{
    //aim_bot
    if (component::toolbar::menu_toolbar_index[0])
    {
        //Legitbot
        if (component::toolbar::toolbar_1::menu_toolbar_index_0[0]) {
            static const char* Legitbot_0_id[4] = { "General", "#Legitbot_General","##Legitbot_General" ,"###Legitbot_General" };
            create_menu_parent_window(function::aim_bot::draw_menu_aim_bot_Legitbot_0, Legitbot_0_id, ImVec2(280, 68), ImVec2(200, 80));
        }
        //Ragebot
        if (component::toolbar::toolbar_1::menu_toolbar_index_0[1])
        {
            static const char* Ragebot_0_id[4] = { "General", "#Ragebot_General","##Ragebot_General","###Ragebot_General" };
            create_menu_parent_window(function::aim_bot::draw_menu_aim_bot_Ragebot_0, Ragebot_0_id, ImVec2(280, 130), ImVec2(200, 80));

            static const char* Ragebot_1_id[4] = { "Targe", "#Ragebot_Targe","##Ragebot_Targe","###Ragebot_Targe" };
            create_menu_parent_window(function::aim_bot::draw_menu_aim_bot_Ragebot_1, Ragebot_1_id, ImVec2(280, 340), ImVec2(495, 80));

            static const char* Ragebot_2_id[4] = { "Accuracy", "#Ragebot_Accuracy","##Ragebot_Accuracy","###Ragebot_Accuracy" };
            create_menu_parent_window(function::aim_bot::draw_menu_aim_bot_Ragebot_2, Ragebot_2_id, ImVec2(280, 130), ImVec2(200, 225));
        }
    }

    //anti_aim
    if (component::toolbar::menu_toolbar_index[1])
    {
        //Antiaim
        if (component::toolbar::toolbar_1::menu_toolbar_index_1[0]) {
            static const char* Antiaim_0_id[4] = { "General", "#Antiaim_General","##Antiaim_General","###Antiaim_General" };
            create_menu_parent_window(function::anti_aim::draw_menu_anti_aim_Antiaim_0, Antiaim_0_id, ImVec2(280, 68), ImVec2(200, 80));
        }
    }

    //vis_uals
    if (component::toolbar::menu_toolbar_index[2])
    {
        //Player
        if (component::toolbar::toolbar_1::menu_toolbar_index_2[0]) {
            static const char* Player_0_id[4] = { "General", "#Player_General","##Player_General" ,"###Player_General" };
            create_menu_parent_window(function::vis_uals::draw_menu_vis_uals_Player_0, Player_0_id, ImVec2(280, 390), ImVec2(200, 80));

            static const char* Player_1_id[4] = { "Preview", "#Player_Preview","##Player_Preview","###Player_Preview" };
            create_menu_parent_window(function::vis_uals::draw_menu_vis_uals_Player_1, Player_1_id, ImVec2(280, 390), ImVec2(495, 80));
        }
        //Local
        if (component::toolbar::toolbar_1::menu_toolbar_index_2[1]) {
            static const char* Local_0_id[4] = { "General", "#Local_General","##Local_General" ,"###Local_General" };
            create_menu_parent_window(function::vis_uals::draw_menu_vis_uals_Local_0, Local_0_id, ImVec2(280, 68), ImVec2(200, 80));
        }
        //World
        if (component::toolbar::toolbar_1::menu_toolbar_index_2[2]) {
            static const char* World_0_id[4] = { "General", "#World_General","##World_General" ,"###World_General" };
            create_menu_parent_window(function::vis_uals::draw_menu_vis_uals_World_0, World_0_id, ImVec2(280, 68), ImVec2(200, 80));
        }
        //Misc
        if (component::toolbar::toolbar_1::menu_toolbar_index_2[3]) {
            static const char* Misc_0_id[4] = { "General", "#Misc_General","##Misc_General" ,"###Misc_General" };
            create_menu_parent_window(function::vis_uals::draw_menu_vis_uals_Misc_0, Misc_0_id, ImVec2(280, 68), ImVec2(200, 80));
        }
    }

    //mi_sc
    if (component::toolbar::menu_toolbar_index[3])
    {
        //Misc
        if (component::toolbar::toolbar_1::menu_toolbar_index_3[0]) {
            //create_menu_parent_window();
        }
        //Cfg
        if (component::toolbar::toolbar_1::menu_toolbar_index_3[1]) {
            //create_menu_parent_window();
        }
        //Particle
        if (component::toolbar::toolbar_1::menu_toolbar_index_3[2]) {
            static const char* Particle_0_id[4] = { "General", "#Particle_General","##Particle_General","###Particle_General" };
            create_menu_parent_window(function::mi_sc::draw_menu_mi_sc_particle_0, Particle_0_id, ImVec2(280, 180), ImVec2(200, 80));
        }
    }
}

//aim_bot
void menu::api::function::aim_bot::draw_menu_aim_bot_Legitbot_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#Legitbot_Enabled", &menu::aim_bot::Legitbot::Enabled);
}

void menu::api::function::aim_bot::draw_menu_aim_bot_Ragebot_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#Ragebot_Enabled", &menu::aim_bot::Ragebot::Enabled);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Silent Aim", "#Ragebot_SilentAim", &menu::aim_bot::Ragebot::SilentAim);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    smooth_api::Slider::SmoothSliderfloat::Draw("FOV", "#Ragebot_FOV", &menu::aim_bot::Ragebot::FOV, 0.0f, 180.0f);

    ImGui::SetCursorPosX(35);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    sprintf(menu::aim_bot::Ragebot::FOV_Input, "%d", (int)menu::aim_bot::Ragebot::FOV);
    if (smooth_api::Input::SmoothInput::Draw("", menu::aim_bot::Ragebot::FOV_Input, sizeof(menu::aim_bot::Ragebot::FOV_Input)))
    {
        float FOV_Input_Value = atof(menu::aim_bot::Ragebot::FOV_Input);
        FOV_Input_Value = std::clamp(FOV_Input_Value, 0.0f, 180.0f);
        menu::aim_bot::Ragebot::FOV = FOV_Input_Value;
        sprintf(menu::aim_bot::Ragebot::FOV_Input, "%d", (int)FOV_Input_Value);
    }
}

void menu::api::function::aim_bot::draw_menu_aim_bot_Ragebot_1()
{

}

void menu::api::function::aim_bot::draw_menu_aim_bot_Ragebot_2()
{

}

//vis_uals
void menu::api::function::vis_uals::draw_menu_vis_uals_Player_0()
{
    static float Vertex = 40;
    static float Spacing = 25;

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#Player_Enabled", &menu::vis_uals::Player::Enabled);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Box", "#Player_Box", &menu::vis_uals::Player::Box);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex + Spacing);
    smooth_api::Colors::SmoothColorPicker::Draw("#Box_Color", &menu::vis_uals::Player::BoxColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Bone", "#Player_Bone", &menu::vis_uals::Player::Bone);

    ImGui::SetCursorPosX(215);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    smooth_api::Colors::SmoothColorPicker::Draw("#Bone_VisualColor", &menu::vis_uals::Player::BoneVisualColor, 2.0f);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    smooth_api::Colors::SmoothColorPicker::Draw("#Bone_InVisualColor", &menu::vis_uals::Player::BoneInVisualColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 3);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Health", "#Player_Health", &menu::vis_uals::Player::Health);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex + Spacing * 3);
    smooth_api::Colors::SmoothColorPicker::Draw("#Health_Color", &menu::vis_uals::Player::HealthColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 4);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Name", "#Player_Name", &menu::vis_uals::Player::Name);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex + Spacing * 4);
    smooth_api::Colors::SmoothColorPicker::Draw("#Name_Color", &menu::vis_uals::Player::NameColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 5);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Weapon", "#Player_Weapon", &menu::vis_uals::Player::Weapon);
    menu::vis_uals::Player::draw_weapon_setting();

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 6);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Glow", "#Player_Glow", &menu::vis_uals::Player::Glow);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex + Spacing * 6);
    smooth_api::Colors::SmoothColorPicker::Draw("#Glow_Color", &menu::vis_uals::Player::GlowColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 7);
    smooth_api::CheckBox::SmoothCheckbox::Draw("BehindTheWall", "#Player_BehindTheWall", &menu::vis_uals::Player::BehindTheWall);
}

void menu::api::function::vis_uals::draw_menu_vis_uals_Player_1()
{
    // 基础布局参数
    static float Vertex = 40;
    static float Spacing = 25;

    // 元素透明度变量（含动画过渡）
    static float boxAlpha = 0.0f;
    static float healthBarAlpha = 0.0f;
    static float ammoBarAlpha = 0.0f;
    static float nameAlpha = 0.0f;
    static float weaponTextAlpha = 0.0f;
    static float weaponnameAlpha = 0.0f;
    static float pingAlpha = 0.0f;
    static float moneyAlpha = 0.0f;
    static float scopedAlpha = 0.0f;
    static float c4Alpha = 0.0f;
    static float defuserAlpha = 0.0f;

    // 每个单独元素悬停透明度（每个元素独立控制）
    static float boxHoverAlpha = 0.0f;
    static float healthHoverAlpha = 0.0f;
    static float ammoHoverAlpha = 0.0f;
    static float nameHoverAlpha = 0.0f;
    static float weaponIconHoverAlpha = 0.0f;
    static float weaponNameHoverAlpha = 0.0f;
    static float pingHoverAlpha = 0.0f;
    static float moneyHoverAlpha = 0.0f;
    static float scopedHoverAlpha = 0.0f;
    static float c4HoverAlpha = 0.0f;
    static float defuserHoverAlpha = 0.0f;

    // 拖拽相关变量
    static bool isDraggingBox = false;
    static bool isDraggingHealth = false;
    static bool isDraggingAmmo = false;
    static bool isDraggingName = false;
    static bool isDraggingWeaponIcon = false;
    static bool isDraggingWeaponName = false;
    static bool isDraggingPing = false;
    static bool isDraggingMoney = false;
    static bool isDraggingScoped = false;
    static bool isDraggingC4 = false;
    static bool isDraggingDefuser = false;

    static ImVec2 dragOffset;  // 拖拽时鼠标与元素的偏移量
    static float restoreDuration = 0.5f;  // 还原动画持续时间(秒)
    static float boxRestoreTime = 0.0f;
    static float healthRestoreTime = 0.0f;
    static float ammoRestoreTime = 0.0f;
    static float nameRestoreTime = 0.0f;
    static float weaponIconRestoreTime = 0.0f;
    static float weaponNameRestoreTime = 0.0f;
    static float pingRestoreTime = 0.0f;
    static float moneyRestoreTime = 0.0f;
    static float scopedRestoreTime = 0.0f;
    static float c4RestoreTime = 0.0f;
    static float defuserRestoreTime = 0.0f;

    // 存储相对位置（相对于菜单）和拖拽偏移量
    static ImVec2 originalBoxPosRel;
    static ImVec2 boxDragOffset;
    static ImVec2 originalHealthPosRel;
    static ImVec2 healthDragOffset;
    static ImVec2 originalAmmoPosRel;
    static ImVec2 ammoDragOffset;
    static ImVec2 originalNamePosRel;
    static ImVec2 nameDragOffset;
    static ImVec2 originalWeaponIconPosRel;
    static ImVec2 weaponIconDragOffset;
    static ImVec2 originalWeaponNamePosRel;
    static ImVec2 weaponNameDragOffset;
    static ImVec2 originalPingPosRel;
    static ImVec2 pingDragOffset;
    static ImVec2 originalMoneyPosRel;
    static ImVec2 moneyDragOffset;
    static ImVec2 originalScopedPosRel;
    static ImVec2 scopedDragOffset;
    static ImVec2 originalC4PosRel;
    static ImVec2 c4DragOffset;
    static ImVec2 originalDefuserPosRel;
    static ImVec2 defuserDragOffset;

    const float easingFactor = 8.0f;  // 动画缓动系数

    // 动画周期控制
    static float animationTime = 0.0f;
    static const float animationPeriod = 2.0f;

    // 获取菜单当前位置（实时更新，支持菜单整体拖动）
    ImVec2 menuPos = ImGui::GetWindowPos();
    ImVec2 mousePos = ImGui::GetMousePos();  // 获取鼠标位置
    bool isMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool isMouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    // 玩家方框和弹药条基础参数（固定尺寸）
    const float boxWidth = 120.0f;
    const float boxHeight = 170.0f;
    const float ammoBarMaxWidth = boxWidth;  // 弹药条最大宽度固定为方框宽度
    float startXRel = (ImGui::GetWindowWidth() - boxWidth) / 2;  // 相对X位置
    float startYRel = Vertex + Spacing * 2;  // 相对Y位置

    // 初始化原始相对位置（首次运行时）
    static bool initialized = false;
    if (!initialized)
    {
        // 初始化方框相对位置
        originalBoxPosRel = ImVec2(startXRel, startYRel);
        boxDragOffset = ImVec2(0, 0);

        // 初始化健康条相对位置（完全独立于方框）
        float healthBarWidth = 2.0f;
        float healthBarOffset = 2.0f;
        originalHealthPosRel = ImVec2(startXRel - healthBarWidth - healthBarOffset, startYRel);
        healthDragOffset = ImVec2(0, 0);

        // 初始化弹药条相对位置
        float ammoBarHeight = 2.0f;
        float ammoBarOffset = 4.0f;
        originalAmmoPosRel = ImVec2(startXRel, startYRel + boxHeight + ammoBarOffset);
        ammoDragOffset = ImVec2(0, 0);

        // 初始化玩家名称相对位置
        const char* playerName = "Player";
        ImVec2 nameTextSize = ImGui::CalcTextSize(playerName);
        float nameOffset = 4.0f;
        originalNamePosRel = ImVec2(
            startXRel + (boxWidth - nameTextSize.x) / 2,
            startYRel - nameTextSize.y - nameOffset
        );
        nameDragOffset = ImVec2(0, 0);

        // 初始化武器图标相对位置
        const char* weaponName = "a";
        ImGui::PushFont(menu::font::menu_weapon);
        ImVec2 weaponTextSize = ImGui::CalcTextSize(weaponName);
        ImGui::PopFont();
        float weaponTextOffset = 4.0f;
        originalWeaponIconPosRel = ImVec2(
            startXRel + (boxWidth - weaponTextSize.x) / 2,
            startYRel + boxHeight + 4.0f + weaponTextOffset
        );
        weaponIconDragOffset = ImVec2(0, 0);

        // 初始化武器全名相对位置
        const char* weaponFullName = "SSG-08";
        ImVec2 weaponFullNameSize = ImGui::CalcTextSize(weaponFullName);
        float weaponFullNameOffset = 1.0f;
        originalWeaponNamePosRel = ImVec2(
            startXRel + (boxWidth - weaponFullNameSize.x) / 2 + 6.0f,
            originalWeaponIconPosRel.y + weaponTextSize.y + weaponFullNameOffset
        );
        weaponNameDragOffset = ImVec2(0, 0);

        // 初始化右侧信息相对位置
        const float infoOffsetX = 10.0f;
        const float infoLineHeight = 16.0f;
        const float infoStartXRel = startXRel + boxWidth + infoOffsetX;

        originalPingPosRel = ImVec2(infoStartXRel, startYRel);
        originalMoneyPosRel = ImVec2(infoStartXRel, startYRel + infoLineHeight);
        originalScopedPosRel = ImVec2(infoStartXRel, startYRel + infoLineHeight * 2);
        originalC4PosRel = ImVec2(infoStartXRel, startYRel + infoLineHeight * 3);
        originalDefuserPosRel = ImVec2(infoStartXRel, startYRel + infoLineHeight * 4);

        pingDragOffset = ImVec2(0, 0);
        moneyDragOffset = ImVec2(0, 0);
        scopedDragOffset = ImVec2(0, 0);
        c4DragOffset = ImVec2(0, 0);
        defuserDragOffset = ImVec2(0, 0);

        initialized = true;
    }

    // 计算所有元素的绝对位置（菜单位置 + 相对位置 + 拖拽偏移）
    ImVec2 currentBoxPos = ImVec2(menuPos.x + originalBoxPosRel.x + boxDragOffset.x,
        menuPos.y + originalBoxPosRel.y + boxDragOffset.y);

    // 健康条位置完全独立，不依赖方框的拖拽偏移
    ImVec2 currentHealthPos = ImVec2(menuPos.x + originalHealthPosRel.x + healthDragOffset.x,
        menuPos.y + originalHealthPosRel.y + healthDragOffset.y);

    ImVec2 currentAmmoPos = ImVec2(menuPos.x + originalAmmoPosRel.x + ammoDragOffset.x,
        menuPos.y + originalAmmoPosRel.y + ammoDragOffset.y);
    ImVec2 currentNamePos = ImVec2(menuPos.x + originalNamePosRel.x + nameDragOffset.x,
        menuPos.y + originalNamePosRel.y + nameDragOffset.y);
    ImVec2 currentWeaponIconPos = ImVec2(menuPos.x + originalWeaponIconPosRel.x + weaponIconDragOffset.x,
        menuPos.y + originalWeaponIconPosRel.y + weaponIconDragOffset.y);
    ImVec2 currentWeaponNamePos = ImVec2(menuPos.x + originalWeaponNamePosRel.x + weaponNameDragOffset.x,
        menuPos.y + originalWeaponNamePosRel.y + weaponNameDragOffset.y);
    ImVec2 currentPingPos = ImVec2(menuPos.x + originalPingPosRel.x + pingDragOffset.x,
        menuPos.y + originalPingPosRel.y + pingDragOffset.y);
    ImVec2 currentMoneyPos = ImVec2(menuPos.x + originalMoneyPosRel.x + moneyDragOffset.x,
        menuPos.y + originalMoneyPosRel.y + moneyDragOffset.y);
    ImVec2 currentScopedPos = ImVec2(menuPos.x + originalScopedPosRel.x + scopedDragOffset.x,
        menuPos.y + originalScopedPosRel.y + scopedDragOffset.y);
    ImVec2 currentC4Pos = ImVec2(menuPos.x + originalC4PosRel.x + c4DragOffset.x,
        menuPos.y + originalC4PosRel.y + c4DragOffset.y);
    ImVec2 currentDefuserPos = ImVec2(menuPos.x + originalDefuserPosRel.x + defuserDragOffset.x,
        menuPos.y + originalDefuserPosRel.y + defuserDragOffset.y);

    // 文本内容定义
    const char* playerName = "Player";
    const char* weaponName = "a";
    const char* weaponFullName = "SSG-08";
    const char* scopedText = "Scoped";
    const char* c4Text = "C4";
    const char* defuserText = "r";
    char pingText[32];
    sprintf_s(pingText, "%dms", 35);
    char moneyText[32];
    sprintf_s(moneyText, "$%d", 3500);

    // 字体与布局参数
    float nameFontSize = 14.0f;
    float weaponFontSize = 12.0f;
    float weaponFullNameFontSize = 10.0f;
    float infoFontSize = 12.0f;
    const float infoLineHeight = 16.0f;

    // 计算文本尺寸
    ImVec2 nameTextSize = ImGui::CalcTextSize(playerName);
    ImGui::PushFont(menu::font::menu_weapon);
    ImVec2 weaponTextSize = ImGui::CalcTextSize(weaponName);
    ImGui::PopFont();
    ImVec2 weaponFullNameSize = ImGui::CalcTextSize(weaponFullName);
    ImVec2 pingTextSize = ImGui::CalcTextSize(pingText);
    ImVec2 moneyTextSize = ImGui::CalcTextSize(moneyText);
    ImVec2 scopedTextSize = ImGui::CalcTextSize(scopedText);
    ImVec2 c4TextSize = ImGui::CalcTextSize(c4Text);
    ImVec2 defuserTextSize = ImGui::CalcTextSize(defuserText);

    // 定义每个元素的碰撞区域
    // 1. 方框区域
    ImVec2 boxAreaMin = currentBoxPos;
    ImVec2 boxAreaMax = ImVec2(currentBoxPos.x + boxWidth, currentBoxPos.y + boxHeight);

    // 2. 健康条区域（使用独立位置）
    float healthBarWidth = 2.0f;
    ImVec2 healthAreaMin = currentHealthPos;
    ImVec2 healthAreaMax = ImVec2(currentHealthPos.x + healthBarWidth, currentHealthPos.y + boxHeight);

    // 3. 弹药条区域（使用固定最大宽度）
    float ammoBarHeight = 2.0f;
    ImVec2 ammoAreaMin = currentAmmoPos;
    ImVec2 ammoAreaMax = ImVec2(currentAmmoPos.x + ammoBarMaxWidth, currentAmmoPos.y + ammoBarHeight);

    // 4. 玩家名称区域
    ImVec2 nameAreaMin = ImVec2(currentNamePos.x - 2, currentNamePos.y);
    ImVec2 nameAreaMax = ImVec2(currentNamePos.x + nameTextSize.x + 7, currentNamePos.y + nameTextSize.y + 5);

    // 5. 武器图标区域
    ImVec2 weaponIconAreaMin = ImVec2(currentWeaponIconPos.x - 2, currentWeaponIconPos.y + 1);
    ImVec2 weaponIconAreaMax = ImVec2(currentWeaponIconPos.x + weaponTextSize.x + 12, currentWeaponIconPos.y + weaponTextSize.y + 3);

    // 6. 武器全名区域
    ImVec2 weaponNameAreaMin = ImVec2(currentWeaponNamePos.x - 2, currentWeaponNamePos.y + 1);
    ImVec2 weaponNameAreaMax = ImVec2(currentWeaponNamePos.x + weaponFullNameSize.x - 5, currentWeaponNamePos.y + weaponFullNameSize.y - 2);

    // 7. 右侧各个文本区域
    ImVec2 pingAreaMin = ImVec2(currentPingPos.x - 2, currentPingPos.y - 2);
    ImVec2 pingAreaMax = ImVec2(currentPingPos.x + pingTextSize.x + 2, currentPingPos.y + pingTextSize.y + 2);

    ImVec2 moneyAreaMin = ImVec2(currentMoneyPos.x - 2, currentMoneyPos.y - 2);
    ImVec2 moneyAreaMax = ImVec2(currentMoneyPos.x + moneyTextSize.x + 2, currentMoneyPos.y + moneyTextSize.y + 2);

    ImVec2 scopedAreaMin = ImVec2(currentScopedPos.x - 2, currentScopedPos.y - 2);
    ImVec2 scopedAreaMax = ImVec2(currentScopedPos.x + scopedTextSize.x + 2, currentScopedPos.y + scopedTextSize.y + 2);

    ImVec2 c4AreaMin = ImVec2(currentC4Pos.x - 2, currentC4Pos.y - 2);
    ImVec2 c4AreaMax = ImVec2(currentC4Pos.x + c4TextSize.x + 2, currentC4Pos.y + c4TextSize.y + 2);

    ImVec2 defuserAreaMin = ImVec2(currentDefuserPos.x - 2, currentDefuserPos.y - 2);
    ImVec2 defuserAreaMax = ImVec2(currentDefuserPos.x + defuserTextSize.x + 10, currentDefuserPos.y + defuserTextSize.y + 2);

    // 检测每个元素是否被鼠标悬停
    bool isBoxHovered = (mousePos.x >= boxAreaMin.x && mousePos.x <= boxAreaMax.x &&
        mousePos.y >= boxAreaMin.y && mousePos.y <= boxAreaMax.y);

    bool isHealthHovered = (mousePos.x >= healthAreaMin.x && mousePos.x <= healthAreaMax.x &&
        mousePos.y >= healthAreaMin.y && mousePos.y <= healthAreaMax.y);

    bool isAmmoHovered = (mousePos.x >= ammoAreaMin.x && mousePos.x <= ammoAreaMax.x &&
        mousePos.y >= ammoAreaMin.y && mousePos.y <= ammoAreaMax.y);

    bool isNameHovered = (mousePos.x >= nameAreaMin.x && mousePos.x <= nameAreaMax.x &&
        mousePos.y >= nameAreaMin.y && mousePos.y <= nameAreaMax.y);

    bool isWeaponIconHovered = (mousePos.x >= weaponIconAreaMin.x && mousePos.x <= weaponIconAreaMax.x &&
        mousePos.y >= weaponIconAreaMin.y && mousePos.y <= weaponIconAreaMax.y);

    bool isWeaponNameHovered = (mousePos.x >= weaponNameAreaMin.x && mousePos.x <= weaponNameAreaMax.x &&
        mousePos.y >= weaponNameAreaMin.y && mousePos.y <= weaponNameAreaMax.y);

    bool isPingHovered = (mousePos.x >= pingAreaMin.x && mousePos.x <= pingAreaMax.x &&
        mousePos.y >= pingAreaMin.y && mousePos.y <= pingAreaMax.y);

    bool isMoneyHovered = (mousePos.x >= moneyAreaMin.x && mousePos.x <= moneyAreaMax.x &&
        mousePos.y >= moneyAreaMin.y && mousePos.y <= moneyAreaMax.y);

    bool isScopedHovered = (mousePos.x >= scopedAreaMin.x && mousePos.x <= scopedAreaMax.x &&
        mousePos.y >= scopedAreaMin.y && mousePos.y <= scopedAreaMax.y);

    bool isC4Hovered = (mousePos.x >= c4AreaMin.x && mousePos.x <= c4AreaMax.x &&
        mousePos.y >= c4AreaMin.y && mousePos.y <= c4AreaMax.y);

    bool isDefuserHovered = (mousePos.x >= defuserAreaMin.x && mousePos.x <= defuserAreaMax.x &&
        mousePos.y >= defuserAreaMin.y && mousePos.y <= defuserAreaMax.y);

    // 拖拽逻辑处理
    if (isMouseDown)
    {
        // 开始拖拽（鼠标按下且悬停在元素上）
        if (!isDraggingBox && isBoxHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingBox = true;
            dragOffset = ImVec2(mousePos.x - currentBoxPos.x, mousePos.y - currentBoxPos.y);
            boxRestoreTime = 0.0f;  // 重置还原时间
        }
        else if (!isDraggingHealth && isHealthHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingHealth = true;
            dragOffset = ImVec2(mousePos.x - currentHealthPos.x, mousePos.y - currentHealthPos.y);
            healthRestoreTime = 0.0f;
        }
        else if (!isDraggingAmmo && isAmmoHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingAmmo = true;
            dragOffset = ImVec2(mousePos.x - currentAmmoPos.x, mousePos.y - currentAmmoPos.y);
            ammoRestoreTime = 0.0f;
        }
        else if (!isDraggingName && isNameHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingName = true;
            dragOffset = ImVec2(mousePos.x - currentNamePos.x, mousePos.y - currentNamePos.y);
            nameRestoreTime = 0.0f;
        }
        else if (!isDraggingWeaponIcon && isWeaponIconHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingWeaponIcon = true;
            dragOffset = ImVec2(mousePos.x - currentWeaponIconPos.x, mousePos.y - currentWeaponIconPos.y);
            weaponIconRestoreTime = 0.0f;
        }
        else if (!isDraggingWeaponName && isWeaponNameHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingWeaponName = true;
            dragOffset = ImVec2(mousePos.x - currentWeaponNamePos.x, mousePos.y - currentWeaponNamePos.y);
            weaponNameRestoreTime = 0.0f;
        }
        else if (!isDraggingPing && isPingHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingPing = true;
            dragOffset = ImVec2(mousePos.x - currentPingPos.x, mousePos.y - currentPingPos.y);
            pingRestoreTime = 0.0f;
        }
        else if (!isDraggingMoney && isMoneyHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingMoney = true;
            dragOffset = ImVec2(mousePos.x - currentMoneyPos.x, mousePos.y - currentMoneyPos.y);
            moneyRestoreTime = 0.0f;
        }
        else if (!isDraggingScoped && isScopedHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingScoped = true;
            dragOffset = ImVec2(mousePos.x - currentScopedPos.x, mousePos.y - currentScopedPos.y);
            scopedRestoreTime = 0.0f;
        }
        else if (!isDraggingC4 && isC4Hovered && !ImGui::IsAnyItemActive())
        {
            isDraggingC4 = true;
            dragOffset = ImVec2(mousePos.x - currentC4Pos.x, mousePos.y - currentC4Pos.y);
            c4RestoreTime = 0.0f;
        }
        else if (!isDraggingDefuser && isDefuserHovered && !ImGui::IsAnyItemActive())
        {
            isDraggingDefuser = true;
            dragOffset = ImVec2(mousePos.x - currentDefuserPos.x, mousePos.y - currentDefuserPos.y);
            defuserRestoreTime = 0.0f;
        }

        // 正在拖拽时更新偏移量（相对于菜单）
        if (isDraggingBox)
        {
            boxDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalBoxPosRel.x);
            boxDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalBoxPosRel.y);
        }
        if (isDraggingHealth)
        {
            healthDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalHealthPosRel.x);
            healthDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalHealthPosRel.y);
        }
        if (isDraggingAmmo)
        {
            ammoDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalAmmoPosRel.x);
            ammoDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalAmmoPosRel.y);
        }
        if (isDraggingName)
        {
            nameDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalNamePosRel.x);
            nameDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalNamePosRel.y);
        }
        if (isDraggingWeaponIcon)
        {
            weaponIconDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalWeaponIconPosRel.x);
            weaponIconDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalWeaponIconPosRel.y);
        }
        if (isDraggingWeaponName)
        {
            weaponNameDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalWeaponNamePosRel.x);
            weaponNameDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalWeaponNamePosRel.y);
        }
        if (isDraggingPing)
        {
            pingDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalPingPosRel.x);
            pingDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalPingPosRel.y);
        }
        if (isDraggingMoney)
        {
            moneyDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalMoneyPosRel.x);
            moneyDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalMoneyPosRel.y);
        }
        if (isDraggingScoped)
        {
            scopedDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalScopedPosRel.x);
            scopedDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalScopedPosRel.y);
        }
        if (isDraggingC4)
        {
            c4DragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalC4PosRel.x);
            c4DragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalC4PosRel.y);
        }
        if (isDraggingDefuser)
        {
            defuserDragOffset.x = (mousePos.x - dragOffset.x) - (menuPos.x + originalDefuserPosRel.x);
            defuserDragOffset.y = (mousePos.y - dragOffset.y) - (menuPos.y + originalDefuserPosRel.y);
        }
    }
    // 鼠标释放时结束拖拽
    else
    {
        if (isDraggingBox) isDraggingBox = false;
        if (isDraggingHealth) isDraggingHealth = false;
        if (isDraggingAmmo) isDraggingAmmo = false;
        if (isDraggingName) isDraggingName = false;
        if (isDraggingWeaponIcon) isDraggingWeaponIcon = false;
        if (isDraggingWeaponName) isDraggingWeaponName = false;
        if (isDraggingPing) isDraggingPing = false;
        if (isDraggingMoney) isDraggingMoney = false;
        if (isDraggingScoped) isDraggingScoped = false;
        if (isDraggingC4) isDraggingC4 = false;
        if (isDraggingDefuser) isDraggingDefuser = false;
    }

    // 动画进度计算
    animationTime += ImGui::GetIO().DeltaTime;
    if (animationTime > animationPeriod) animationTime = 0.0f;
    float progress = animationTime / animationPeriod;

    // 缓动函数计算
    float easeFactor;
    if (progress <= 0.5f)
    {
        float t = progress * 2.0f;
        easeFactor = (cos(t * IM_PI) + 1.0f) / 2.0f;  // 余弦缓入
    }
    else
    {
        float t = (progress - 0.5f) * 2.0f;
        easeFactor = (1.0f - cos(t * IM_PI)) / 2.0f;  // 余弦缓出
    }

    // 模拟属性值
    const float baseHealthValue = 85.0f;
    const float baseAmmoValue = 75.0f;
    float healthValue = baseHealthValue * easeFactor;
    float ammoValue = baseAmmoValue * easeFactor;
    const bool hasDefuser = true;

    // 健康条绘制参数（使用独立高度计算，不依赖方框位置）
    float healthBarHeight = boxHeight * (healthValue / 100.0f);
    ImVec2 healthBarMin = ImVec2(currentHealthPos.x, currentHealthPos.y + (boxHeight - healthBarHeight));
    ImVec2 healthBarMax = ImVec2(currentHealthPos.x + healthBarWidth, currentHealthPos.y + boxHeight);

    // 弹药条绘制参数（使用固定最大宽度，不随拖拽变化）
    float ammoBarWidth = ammoBarMaxWidth * (ammoValue / 100.0f);  // 基于固定最大宽度计算
    ImVec2 ammoBarMin = currentAmmoPos;
    ImVec2 ammoBarMax = ImVec2(currentAmmoPos.x + ammoBarWidth, currentAmmoPos.y + ammoBarHeight);
    ImVec2 ammoBarBgMin = currentAmmoPos;
    ImVec2 ammoBarBgMax = ImVec2(currentAmmoPos.x + ammoBarMaxWidth, currentAmmoPos.y + ammoBarHeight);  // 使用固定最大宽度

    // 透明度动画更新
    float delta = ImGui::GetIO().DeltaTime * easingFactor;

    // 基础透明度更新
    float targetBoxAlpha = menu::vis_uals::Player::Box ? 1.0f : 0.0f;
    boxAlpha = approachValue(boxAlpha, targetBoxAlpha, delta);

    float targetHealthAlpha = menu::vis_uals::Player::Health ? 1.0f : 0.0f;
    healthBarAlpha = approachValue(healthBarAlpha, targetHealthAlpha, delta);

    float targetWeaponRelatedAlpha = menu::vis_uals::Player::Weapon && menu::vis_uals::Player::Weapon_setting::Icon ? 1.0f : 0.0f;
    weaponTextAlpha = approachValue(weaponTextAlpha, targetWeaponRelatedAlpha, delta);

    float targetWeaponNameAlpha = menu::vis_uals::Player::Weapon && menu::vis_uals::Player::Weapon_setting::Name ? 1.0f : 0.0f;
    weaponnameAlpha = approachValue(weaponnameAlpha, targetWeaponNameAlpha, delta);

    float targetAmmoRelatedAlpha = menu::vis_uals::Player::Weapon && menu::vis_uals::Player::Weapon_setting::Ammo ? 1.0f : 0.0f;
    ammoBarAlpha = approachValue(ammoBarAlpha, targetAmmoRelatedAlpha, delta);

    float targetNameAlpha = menu::vis_uals::Player::Name ? 1.0f : 0.0f;
    nameAlpha = approachValue(nameAlpha, targetNameAlpha, delta);

    float targetPingAlpha = menu::vis_uals::Player::Preview_setting::Ping ? 1.0f : 0.0f;
    pingAlpha = approachValue(pingAlpha, targetPingAlpha, delta);

    float targetMoneyAlpha = menu::vis_uals::Player::Preview_setting::Money ? 1.0f : 0.0f;
    moneyAlpha = approachValue(moneyAlpha, targetMoneyAlpha, delta);

    float targetScopedAlpha = menu::vis_uals::Player::Preview_setting::IsScoped ? 1.0f : 0.0f;
    scopedAlpha = approachValue(scopedAlpha, targetScopedAlpha, delta);

    float targetC4Alpha = menu::vis_uals::Player::Preview_setting::C4 ? 1.0f : 0.0f;
    c4Alpha = approachValue(c4Alpha, targetC4Alpha, delta);

    float targetDefuserAlpha = menu::vis_uals::Player::Preview_setting::HasDefuser ? 1.0f : 0.0f;
    defuserAlpha = approachValue(defuserAlpha, targetDefuserAlpha, delta);

    // 每个单独元素悬停透明度更新（平滑过渡）
    boxHoverAlpha = approachValue(boxHoverAlpha, isBoxHovered ? 1.0f : 0.0f, delta);
    healthHoverAlpha = approachValue(healthHoverAlpha, isHealthHovered ? 1.0f : 0.0f, delta);
    ammoHoverAlpha = approachValue(ammoHoverAlpha, isAmmoHovered ? 1.0f : 0.0f, delta);
    nameHoverAlpha = approachValue(nameHoverAlpha, isNameHovered ? 1.0f : 0.0f, delta);
    weaponIconHoverAlpha = approachValue(weaponIconHoverAlpha, isWeaponIconHovered ? 1.0f : 0.0f, delta);
    weaponNameHoverAlpha = approachValue(weaponNameHoverAlpha, isWeaponNameHovered ? 1.0f : 0.0f, delta);
    pingHoverAlpha = approachValue(pingHoverAlpha, isPingHovered ? 1.0f : 0.0f, delta);
    moneyHoverAlpha = approachValue(moneyHoverAlpha, isMoneyHovered ? 1.0f : 0.0f, delta);
    scopedHoverAlpha = approachValue(scopedHoverAlpha, isScopedHovered ? 1.0f : 0.0f, delta);
    c4HoverAlpha = approachValue(c4HoverAlpha, isC4Hovered ? 1.0f : 0.0f, delta);
    defuserHoverAlpha = approachValue(defuserHoverAlpha, isDefuserHovered ? 1.0f : 0.0f, delta);

    // 正弦缓动还原位置（当没有拖拽时）
    float dt = ImGui::GetIO().DeltaTime;
    if (!isDraggingBox)
    {
        boxRestoreTime += dt;
        float t = std::min(boxRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));  // 正弦缓动函数
        boxDragOffset.x *= (1.0f - sineEase);
        boxDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingHealth)
    {
        healthRestoreTime += dt;
        float t = std::min(healthRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        healthDragOffset.x *= (1.0f - sineEase);
        healthDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingAmmo)
    {
        ammoRestoreTime += dt;
        float t = std::min(ammoRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        ammoDragOffset.x *= (1.0f - sineEase);
        ammoDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingName)
    {
        nameRestoreTime += dt;
        float t = std::min(nameRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        nameDragOffset.x *= (1.0f - sineEase);
        nameDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingWeaponIcon)
    {
        weaponIconRestoreTime += dt;
        float t = std::min(weaponIconRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        weaponIconDragOffset.x *= (1.0f - sineEase);
        weaponIconDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingWeaponName)
    {
        weaponNameRestoreTime += dt;
        float t = std::min(weaponNameRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        weaponNameDragOffset.x *= (1.0f - sineEase);
        weaponNameDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingPing)
    {
        pingRestoreTime += dt;
        float t = std::min(pingRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        pingDragOffset.x *= (1.0f - sineEase);
        pingDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingMoney)
    {
        moneyRestoreTime += dt;
        float t = std::min(moneyRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        moneyDragOffset.x *= (1.0f - sineEase);
        moneyDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingScoped)
    {
        scopedRestoreTime += dt;
        float t = std::min(scopedRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        scopedDragOffset.x *= (1.0f - sineEase);
        scopedDragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingC4)
    {
        c4RestoreTime += dt;
        float t = std::min(c4RestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        c4DragOffset.x *= (1.0f - sineEase);
        c4DragOffset.y *= (1.0f - sineEase);
    }

    if (!isDraggingDefuser)
    {
        defuserRestoreTime += dt;
        float t = std::min(defuserRestoreTime / restoreDuration, 1.0f);
        float sineEase = (1.0f - cos(t * IM_PI * 0.5f));
        defuserDragOffset.x *= (1.0f - sineEase);
        defuserDragOffset.y *= (1.0f - sineEase);
    }

    // 获取绘制列表
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // 1. 绘制玩家方框（悬停时高亮）
    if (boxAlpha > 0.001f)
    {
        ImVec4 boxColor = menu::vis_uals::Player::BoxColor;
        boxColor.w = boxAlpha;

        // 悬停时增强效果
        float hoverIntensity = boxHoverAlpha * 0.3f;
        boxColor.x = std::min(1.0f, boxColor.x + hoverIntensity);
        boxColor.y = std::min(1.0f, boxColor.y + hoverIntensity);
        boxColor.z = std::min(1.0f, boxColor.z + hoverIntensity);

        draw_list->AddRect(
            currentBoxPos,
            ImVec2(currentBoxPos.x + boxWidth, currentBoxPos.y + boxHeight),
            ImColor(boxColor),
            0.0f,
            ImDrawFlags_RoundCornersAll,
            1.0f + (boxHoverAlpha * 1.0f)  // 边框宽度变化
        );

        // 悬停时添加内部半透明高亮
        if (boxHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                currentBoxPos,
                ImVec2(currentBoxPos.x + boxWidth, currentBoxPos.y + boxHeight),
                ImColor(255, 255, 255, (int)(boxHoverAlpha * 30)),
                0.0f,
                ImDrawFlags_RoundCornersAll
            );
        }
    }

    // 2. 绘制健康条（悬停时高亮）
    if (healthBarAlpha > 0.001f)
    {
        // 健康条背景
        ImColor bgColor(0, 0, 0, (int)(100 * healthBarAlpha));
        draw_list->AddRectFilled(
            healthAreaMin,
            healthAreaMax,
            bgColor,
            1.0f
        );

        // 健康条条前景
        ImVec4 healthColor = menu::vis_uals::Player::HealthColor;
        healthColor.w = healthBarAlpha;

        // 悬停时增强颜色
        float hoverIntensity = healthHoverAlpha * 0.3f;
        healthColor.x = std::min(1.0f, healthColor.x + hoverIntensity);
        healthColor.y = std::min(1.0f, healthColor.y + hoverIntensity);
        healthColor.z = std::min(1.0f, healthColor.z + hoverIntensity);

        draw_list->AddRectFilled(
            healthBarMin,
            healthBarMax,
            ImColor(healthColor),
            1.0f
        );

        // 悬停时添加背景高亮
        if (healthHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                healthAreaMin,
                healthAreaMax,
                ImColor(255, 255, 255, (int)(healthHoverAlpha * 30)),
                1.0f
            );
        }
    }

    // 3. 弹药条（悬停时高亮，固定最大宽度）
    if (ammoBarAlpha > 0.001f)
    {
        // 弹药条背景（使用固定最大宽度）
        ImColor ammoBgColor(0, 0, 0, (int)(100 * ammoBarAlpha));
        draw_list->AddRectFilled(
            ammoBarBgMin,
            ammoBarBgMax,
            ammoBgColor,
            1.0f
        );

        // 弹药条前景（使用固定最大宽度计算）
        ImVec4 ammoColor = menu::vis_uals::Player::Weapon_setting::AmmoColor;
        ammoColor.w = ammoBarAlpha;

        // 悬停时增强颜色
        float hoverIntensity = ammoHoverAlpha * 0.3f;
        ammoColor.x = std::min(1.0f, ammoColor.x + hoverIntensity);
        ammoColor.y = std::min(1.0f, ammoColor.y + hoverIntensity);
        ammoColor.z = std::min(1.0f, ammoColor.z + hoverIntensity);

        draw_list->AddRectFilled(
            ammoBarMin,
            ammoBarMax,
            ImColor(ammoColor),
            1.0f
        );

        // 悬停时添加背景高亮（使用固定最大宽度）
        if (ammoHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                ammoAreaMin,
                ammoAreaMax,
                ImColor(255, 255, 255, (int)(ammoHoverAlpha * 30)),
                1.0f
            );
        }
    }

    // 4. 玩家名称（悬停时高亮）
    if (nameAlpha > 0.001f)
    {
        ImVec4 textColor = menu::vis_uals::Player::NameColor;
        textColor.w = nameAlpha;

        // 悬停时增强效果
        float hoverIntensity = nameHoverAlpha * 0.3f;
        textColor.x = std::min(1.0f, textColor.x + hoverIntensity);
        textColor.y = std::min(1.0f, textColor.y + hoverIntensity);
        textColor.z = std::min(1.0f, textColor.z + hoverIntensity);
        float currentNameFontSize = nameFontSize + (nameHoverAlpha * 2.0f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentNameFontSize,
            ImVec2(currentNamePos.x + 1, currentNamePos.y + 1),
            ImColor(0, 0, 0, (int)(200 * nameAlpha)),
            playerName
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentNameFontSize,
            currentNamePos,
            ImColor(textColor),
            playerName
        );

        // 悬停时添加背景高亮
        if (nameHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                nameAreaMin,
                nameAreaMax,
                ImColor(255, 255, 255, (int)(nameHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }
    }

    // 5. 武器图标（悬停时高亮）
    if (weaponTextAlpha > 0.001f)
    {
        ImGui::PushFont(menu::font::menu_weapon);

        // 悬停时增强效果
        float hoverIntensity = weaponIconHoverAlpha * 0.3f;
        float currentWeaponFontSize = weaponFontSize + (weaponIconHoverAlpha * 2.0f);

        ImVec4 weaponTextColor = menu::vis_uals::Player::Weapon_setting::IconColor;
        weaponTextColor.w = weaponTextAlpha;
        weaponTextColor.x = std::min(1.0f, weaponTextColor.x + hoverIntensity);
        weaponTextColor.y = std::min(1.0f, weaponTextColor.y + hoverIntensity);
        weaponTextColor.z = std::min(1.0f, weaponTextColor.z + hoverIntensity);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentWeaponFontSize,
            ImVec2(currentWeaponIconPos.x + 1, currentWeaponIconPos.y + 1),
            ImColor(0, 0, 0, (int)(200 * weaponTextAlpha)),
            weaponName
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentWeaponFontSize,
            currentWeaponIconPos,
            ImColor(weaponTextColor),
            weaponName
        );

        // 悬停时添加背景高亮
        if (weaponIconHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                weaponIconAreaMin,
                weaponIconAreaMax,
                ImColor(255, 255, 255, (int)(weaponIconHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }
        ImGui::PopFont();
    }

    // 6. 武器全名（悬停时高亮）
    if (weaponnameAlpha > 0.001f)
    {
        ImVec4 weaponFullNameColor = menu::vis_uals::Player::Weapon_setting::NameColor;
        weaponFullNameColor.w = weaponnameAlpha;

        // 悬停时增强效果
        float hoverIntensity = weaponNameHoverAlpha * 0.3f;
        weaponFullNameColor.x = std::min(1.0f, weaponFullNameColor.x + hoverIntensity);
        weaponFullNameColor.y = std::min(1.0f, weaponFullNameColor.y + hoverIntensity);
        weaponFullNameColor.z = std::min(1.0f, weaponFullNameColor.z + hoverIntensity);
        float currentFullNameFontSize = weaponFullNameFontSize + (weaponNameHoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentFullNameFontSize,
            ImVec2(currentWeaponNamePos.x + 1, currentWeaponNamePos.y + 1),
            ImColor(0, 0, 0, (int)(200 * weaponnameAlpha)),
            weaponFullName
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentFullNameFontSize,
            currentWeaponNamePos,
            ImColor(weaponFullNameColor),
            weaponFullName
        );

        // 悬停时添加背景高亮
        if (weaponNameHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                weaponNameAreaMin,
                weaponNameAreaMax,
                ImColor(255, 255, 255, (int)(weaponNameHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }
    }

    // 7. 右侧各个文本元素（单独悬停效果）
    float currentY = currentBoxPos.y;

    // 绘制Ping（单独悬停）
    if (pingAlpha > 0.001f)
    {
        ImVec4 pingColor = menu::vis_uals::Player::Preview_setting::PingColor;
        pingColor.w = pingAlpha;

        // 悬停时增强效果
        float hoverIntensity = pingHoverAlpha * 0.3f;
        pingColor.x = std::min(1.0f, pingColor.x + hoverIntensity);
        pingColor.y = std::min(1.0f, pingColor.y + hoverIntensity);
        pingColor.z = std::min(1.0f, pingColor.z + hoverIntensity);
        float currentInfoFontSize = infoFontSize + (pingHoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            ImVec2(currentPingPos.x + 1, currentPingPos.y + 1),
            ImColor(0, 0, 0, (int)(200 * pingAlpha)),
            pingText
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            currentPingPos,
            ImColor(pingColor),
            pingText
        );

        // 悬停时添加背景高亮
        if (pingHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                pingAreaMin,
                pingAreaMax,
                ImColor(255, 255, 255, (int)(pingHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }

        currentY += infoLineHeight;
    }

    // 绘制金钱（单独悬停）
    if (moneyAlpha > 0.001f)
    {
        ImVec4 moneyColor = menu::vis_uals::Player::Preview_setting::MoneyColor;
        moneyColor.w = moneyAlpha;

        // 悬停时增强效果
        float hoverIntensity = moneyHoverAlpha * 0.3f;
        moneyColor.x = std::min(1.0f, moneyColor.x + hoverIntensity);
        moneyColor.y = std::min(1.0f, moneyColor.y + hoverIntensity);
        moneyColor.z = std::min(1.0f, moneyColor.z + hoverIntensity);
        float currentInfoFontSize = infoFontSize + (moneyHoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            ImVec2(currentMoneyPos.x + 1, currentMoneyPos.y + 1),
            ImColor(0, 0, 0, (int)(200 * moneyAlpha)),
            moneyText
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            currentMoneyPos,
            ImColor(moneyColor),
            moneyText
        );

        // 悬停时添加背景高亮
        if (moneyHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                moneyAreaMin,
                moneyAreaMax,
                ImColor(255, 255, 255, (int)(moneyHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }

        currentY += infoLineHeight;
    }

    // 绘制开镜状态（单独悬停）
    if (scopedAlpha > 0.001f)
    {
        ImVec4 scopedColor = menu::vis_uals::Player::Preview_setting::IsScopedColor;
        scopedColor.w = scopedAlpha;

        // 悬停时增强效果
        float hoverIntensity = scopedHoverAlpha * 0.3f;
        scopedColor.x = std::min(1.0f, scopedColor.x + hoverIntensity);
        scopedColor.y = std::min(1.0f, scopedColor.y + hoverIntensity);
        scopedColor.z = std::min(1.0f, scopedColor.z + hoverIntensity);
        float currentInfoFontSize = infoFontSize + (scopedHoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            ImVec2(currentScopedPos.x + 1, currentScopedPos.y + 1),
            ImColor(0, 0, 0, (int)(200 * scopedAlpha)),
            scopedText
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            currentScopedPos,
            ImColor(scopedColor),
            scopedText
        );

        // 悬停时添加背景高亮
        if (scopedHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                scopedAreaMin,
                scopedAreaMax,
                ImColor(255, 255, 255, (int)(scopedHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }

        currentY += infoLineHeight;
    }

    // 绘制C4状态（单独悬停）
    if (c4Alpha > 0.001f)
    {
        ImVec4 c4Color = menu::vis_uals::Player::Preview_setting::C4Color;
        c4Color.w = c4Alpha;

        // 悬停时增强效果
        float hoverIntensity = c4HoverAlpha * 0.3f;
        c4Color.x = std::min(1.0f, c4Color.x + hoverIntensity);
        c4Color.y = std::min(1.0f, c4Color.y + hoverIntensity);
        c4Color.z = std::min(1.0f, c4Color.z + hoverIntensity);
        float currentInfoFontSize = infoFontSize + (c4HoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            ImVec2(currentC4Pos.x + 1, currentC4Pos.y + 1),
            ImColor(0, 0, 0, (int)(200 * c4Alpha)),
            c4Text
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            currentC4Pos,
            ImColor(c4Color),
            c4Text
        );

        // 悬停时添加背景高亮
        if (c4HoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                c4AreaMin,
                c4AreaMax,
                ImColor(255, 255, 255, (int)(c4HoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }

        currentY += infoLineHeight;
    }

    // 绘制拆弹器状态（单独悬停）
    if (defuserAlpha > 0.001f)
    {
        ImGui::PushFont(menu::font::menu_weapon);
        ImVec4 defuserColor = menu::vis_uals::Player::Preview_setting::HasDefuserColor;
        defuserColor.w = defuserAlpha;

        // 悬停时增强效果
        float hoverIntensity = defuserHoverAlpha * 0.3f;
        defuserColor.x = std::min(1.0f, defuserColor.x + hoverIntensity);
        defuserColor.y = std::min(1.0f, defuserColor.y + hoverIntensity);
        defuserColor.z = std::min(1.0f, defuserColor.z + hoverIntensity);
        float currentInfoFontSize = infoFontSize + (defuserHoverAlpha * 1.5f);

        // 文本阴影
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            ImVec2(currentDefuserPos.x + 1, currentDefuserPos.y + 1),
            ImColor(0, 0, 0, (int)(200 * defuserAlpha)),
            defuserText
        );
        // 文本内容
        draw_list->AddText(
            ImGui::GetFont(),
            currentInfoFontSize,
            currentDefuserPos,
            ImColor(defuserColor),
            defuserText
        );

        // 悬停时添加背景高亮
        if (defuserHoverAlpha > 0.001f)
        {
            draw_list->AddRectFilled(
                defuserAreaMin,
                defuserAreaMax,
                ImColor(255, 255, 255, (int)(defuserHoverAlpha * 50)),
                3.0f,
                ImDrawFlags_RoundCornersAll
            );
        }
        ImGui::PopFont();
    }

    menu::vis_uals::Player::draw_Preview_setting();
}

void menu::api::function::vis_uals::draw_menu_vis_uals_Local_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#Local_Enabled", &menu::vis_uals::Local::Enabled);
}

void menu::api::function::vis_uals::draw_menu_vis_uals_World_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#World_Enabled", &menu::vis_uals::World::Enabled);
}

void menu::api::function::vis_uals::draw_menu_vis_uals_Misc_0()
{

}

//anti_aim
void menu::api::function::anti_aim::draw_menu_anti_aim_Antiaim_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#Antiaim_Enabled", &menu::anti_aim::Antiaim::Enabled);
}

//mi_sc
void menu::api::function::mi_sc::draw_menu_mi_sc_particle_0()
{
    static float Vertex = 40;
    static float Spacing = 25;
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::CheckBox::SmoothCheckbox::Draw("Enabled", "#particle_Enabled", &menu::mi_sc::particle::Enabled);

    ImGui::SetCursorPosX(215);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::Colors::SmoothColorPicker::Draw("#particle_BgColor", &menu::mi_sc::particle::ParticleBgColor, 2.0f);

    ImGui::SetCursorPosX(245);
    ImGui::SetCursorPosY(Vertex);
    smooth_api::Colors::SmoothColorPicker::Draw("#particle_Color", &menu::mi_sc::particle::ParticleColor, 2.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing);
    smooth_api::Slider::SmoothSliderfloat::Draw("Descent", "#particle_Descent", &menu::mi_sc::particle::Descent, 0.0f, 5.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 2);
    smooth_api::Slider::SmoothSliderfloat::Draw("Direction", "#particle_Direction", &menu::mi_sc::particle::Direction, -1.0f, 1.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 3);
    smooth_api::Slider::SmoothSliderfloat::Draw("Strength", "#particle_Strength", &menu::mi_sc::particle::Strength, 0.0f, 100.0f);

    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(Vertex + Spacing * 4);
    smooth_api::Slider::SmoothSliderfloat::Draw("Variation", "#particle_Variation", &menu::mi_sc::particle::Variation, 0.0f, 100.0f);
}