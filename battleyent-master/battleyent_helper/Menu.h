#pragma once
#include <Windows.h>
#include <vector>
#include <string>

// ========================================
// Feature Toggle States
// ========================================
namespace Features
{
    // Player Features
    extern bool god_mode;
    extern bool infinite_stamina;
    extern bool no_fall_damage;
    extern bool no_energy_drain;
    extern bool no_hydration_drain;
    extern bool no_fatigue;

    // Combat Features
    extern bool no_recoil;
    extern bool no_weapon_durability;
    extern bool no_weapon_malfunction;
    extern bool no_weapon_overheating;
    extern bool instant_ads;

    // AI Features
    extern bool disable_ai_tracking;
    extern bool disable_ai_attacks;

    // Environment Features
    extern bool no_barbed_wire_damage;
    extern bool no_minefield_damage;
    extern bool no_sniper_zones;

    // Loot Features
    extern bool instant_search;
    extern bool lucky_search;

    // Utility Features
    extern bool breach_all_doors;

    // Menu State
    extern bool show_menu;
    extern bool menu_open;
}

// ========================================
// Menu Control Functions (No rendering, just state)
// ========================================
namespace Menu
{
    // Toggle menu visibility
    void Toggle();

    // Check if menu key was pressed
    void CheckInput();
}