#include "MenuRenderer.h"
#include "menu.h"
#include "imgui.h"

namespace MenuRenderer
{
    void RenderHintText()
    {
        // Always show hint in top-right corner
        ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 210, 10), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 60), ImGuiCond_Always);

        ImGui::Begin("##Hint", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoBackground);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "   PvE Tegridy Menu");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "  INSERT - Open Menu");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "  END - Quit Game Safely");
        ImGui::End();
    }

    void RenderPlayerTab()
    {
        ImGui::BeginChild("PlayerTab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Player Features");
        ImGui::Separator();
        ImGui::Spacing();

        // God Mode
        if (ImGui::Checkbox("God Mode", &Features::god_mode))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("That tickles cuhh..");
        }

        // Infinite Stamina
        if (ImGui::Checkbox("Infinite Stamina", &Features::infinite_stamina))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Crackhead energy..");
        }

        // No Fall Damage
        if (ImGui::Checkbox("No Fall Damage", &Features::no_fall_damage))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Aye yo cuhh a cat now.");
        }

        // No Energy Drain
        if (ImGui::Checkbox("No Energy Drain", &Features::no_energy_drain))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Anti-Munchies who needs food anyways?");
        }

        // No Hydration Drain
        if (ImGui::Checkbox("No Hydration Drain", &Features::no_hydration_drain))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Don't let em call you a thirsty boi.");
        }

        // No Fatigue
        if (ImGui::Checkbox("No Fatigue", &Features::no_fatigue))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Chuck Norris could never be overweight/encumbered bruh.");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderCombatTab()
    {
        ImGui::BeginChild("CombatTab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Combat Features");
        ImGui::Separator();
        ImGui::Spacing();

        // No Recoil
        if (ImGui::Checkbox("No Recoil", &Features::no_recoil))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Skrt SKrt SKrt");
        }

        // No Weapon Durability Loss
        if (ImGui::Checkbox("No Weapon Durability Loss", &Features::no_weapon_durability))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Weapons never degrade or jam duh");
        }

        // Instant ADS (placeholder)
        ImGui::BeginDisabled();
        ImGui::Checkbox("Instant ADS", &Features::instant_ads);
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "(Not implemented yet)");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderAITab()
    {
        ImGui::BeginChild("AITab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "AI Features");
        ImGui::Separator();
        ImGui::Spacing();

        // Disable AI Tracking
        if (ImGui::Checkbox("Disable AI Sight", &Features::disable_ai_tracking))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("You mean a ghost ghost cuz?");
        }

        // Disable AI Attacks
        if (ImGui::Checkbox("Disable AI Attacks", &Features::disable_ai_attacks))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Bots will not attack you");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderEnvironmentTab()
    {
        ImGui::BeginChild("EnvironmentTab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Environment Features");
        ImGui::Separator();
        ImGui::Spacing();

        // No Barbed Wire Damage
        if (ImGui::Checkbox("No Barbed Wire Damage", &Features::no_barbed_wire_damage))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Walk through barbed wire without damage");
        }

        // No Minefield Damage
        if (ImGui::Checkbox("No Minefield Damage", &Features::no_minefield_damage))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Mines could never lay a scratch on Chuck mf Norris..");
        }

        // No Sniper Zone Damage
        if (ImGui::Checkbox("No Sniper Zone Damage", &Features::no_sniper_zones))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("You won't even get shot by sniper zone bruh");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderLootTab()
    {
        ImGui::BeginChild("LootTab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Loot Features");
        ImGui::Separator();
        ImGui::Spacing();

        // Lucky Search
        if (ImGui::Checkbox("Instant Search", &Features::lucky_search))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Patch Lucky search skill, Instant search.");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderUtilityTab()
    {
        ImGui::BeginChild("UtilityTab", ImVec2(0, 0), false);

        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "Utility Features");
        ImGui::Separator();
        ImGui::Spacing();

        // Breach All Doors
        if (ImGui::Checkbox("Breach Any Door", &Features::breach_all_doors))
        {
            // Feature will be applied on next patch cycle
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("*Knock *Knock, OPEN UP ITS TONYS PIZZA!");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Toggled Features are active until raid ends. ");

        ImGui::EndChild();
    }

    void RenderMainMenu()
    {
        // Only render if menu is open
        if (!Features::menu_open)
            return;

        // Set window size and position
        ImGui::SetNextWindowSize(ImVec2(650, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

        // Main window
        if (ImGui::Begin("Tegridy Farms", &Features::menu_open, ImGuiWindowFlags_NoCollapse))
        {
            // Header
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "PvE Tegridy Menu");
            ImGui::Separator();
            ImGui::Spacing();

            // Tab bar
            if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Player"))
                {
                    RenderPlayerTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Combat"))
                {
                    RenderCombatTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("AI"))
                {
                    RenderAITab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Environment"))
                {
                    RenderEnvironmentTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Loot"))
                {
                    RenderLootTab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Utility"))
                {
                    RenderUtilityTab();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Press INSERT to close menu");
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Press END to exit game safely");
        }
        ImGui::End();
    }
}