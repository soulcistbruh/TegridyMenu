#pragma once
#include "il2api.h"
#include <vector>
#include <string>

// ========================================
// Feature Patch Functions
// ========================================
namespace FeaturePatch
{
    // ========================================
    // Player Features
    // ========================================
    void ApplyGodMode(Il2CppImage* image);
    void ApplyInfiniteStamina(Il2CppImage* image);
    void ApplyNoFallDamage(Il2CppImage* image);
    void ApplyNoEnergyDrain(Il2CppImage* image);
    void ApplyNoHydrationDrain(Il2CppImage* image);
    void ApplyNoFatigue(Il2CppImage* image);

    // ========================================
    // Combat Features
    // ========================================
    void ApplyNoRecoil(Il2CppImage* image);
    void ApplyNoWeaponDurability(Il2CppImage* image);
    void ApplyNoWeaponMalfunction(Il2CppImage* image);
    void ApplyNoWeaponOverheating(Il2CppImage* image);
    void ApplyInstantADS(Il2CppImage* image);

    // ========================================
    // AI Features
    // ========================================
    void ApplyDisableAITracking(Il2CppImage* image);
    void ApplyDisableAIAttacks(Il2CppImage* image);

    // ========================================
    // Environment Features
    // ========================================
    void ApplyNoBarbedWireDamage(Il2CppImage* image);
    void ApplyNoMinefieldDamage(Il2CppImage* image);
    void ApplyNoSniperZones(Il2CppImage* image);

    // ========================================
    // Loot Features
    // ========================================
    void ApplyInstantSearch(Il2CppImage* image);
    void ApplyLuckySearch(Il2CppImage* image);

    // ========================================
    // Utility Features
    // ========================================
    void ApplyBreachAllDoors(Il2CppImage* image);

    // ========================================
    // Master Function - Apply All Enabled Features
    // ========================================
    void ApplyAllEnabledFeatures(Il2CppImage* image);

    // ========================================
    // Raid Location Updater (runs in main loop)
    // ========================================
    void UpdateRaidSelectedLocation(Il2CppImage* image);

    // ========================================
    // God Mode Updater (runs in main loop - requires continuous patching)
    // ========================================
    void UpdateGodMode(Il2CppImage* image);
}

// ========================================
// Patch Helper Functions (exposed for features.cpp)
// ========================================
namespace PatchHelpers
{
    void PatchMethodsWithLogging(Il2CppClass* klass, const char* namespaceName, const char* className, const std::vector<std::string>& methodNames);
    void PatchBoolGetter(void* methodPtr, bool returnValue);
    void PatchFloatGetter(void* methodPtr, float returnValue);
}