#include "menu.h"
#include <iostream>

// ========================================
// Feature Toggle State Definitions
// ========================================
namespace Features
{
    // Player Features
    bool god_mode = false;
    bool infinite_stamina = false;
    bool no_fall_damage = false;
    bool no_energy_drain = false;
    bool no_hydration_drain = false;
    bool no_fatigue = false;

    // Combat Features
    bool no_recoil = false;
    bool no_weapon_durability = false;
    bool no_weapon_malfunction = false;
    bool no_weapon_overheating = false;
    bool instant_ads = false;

    // AI Features
    bool disable_ai_tracking = false;
    bool disable_ai_attacks = false;

    // Environment Features
    bool no_barbed_wire_damage = false;
    bool no_minefield_damage = false;
    bool no_sniper_zones = false;

    // Loot Features
    bool instant_search = false;
    bool lucky_search = false;

    // Utility Features
    bool breach_all_doors = false;

    // Menu State
    bool show_menu = false;
    bool menu_open = false;
}

// ========================================
// Menu Control Functions
// ========================================

namespace Menu
{
    void Toggle()
    {
        Features::menu_open = !Features::menu_open;

        if (Features::menu_open)
        {
            printf("[*] Menu opened (INSERT to close)\n");
        }
        else
        {
            printf("[*] Menu closed\n");
        }
    }

    void CheckInput()
    {
        // Check for INSERT key (toggle menu)
        static bool insertPressed = false;
        if (GetAsyncKeyState(VK_INSERT) & 0x8000)
        {
            if (!insertPressed)
            {
                Toggle();
                insertPressed = true;
            }
        }
        else
        {
            insertPressed = false;
        }
    }
}