#include "features.h"
#include "menu.h"
#include "il2api.h"
#include <iostream>

// External TarkovApplication instance (from dllmain.cpp)
extern void* g_TarkovApplicationInstance;

// External CameraManager instance (from dllmain.cpp)
extern void* g_CameraManagerInstance;

// Global for tracking selected location
extern uintptr_t g_selectedLocation;

// Raid offsets (from NewOffsets.txt)
#define OFFSET_RAID_SETTINGS 0xD0
#define OFFSET_SELECTED_LOCATION 0xA8

// ========================================
// Patch Helper Implementations
// ========================================

namespace PatchHelpers
{
    void PatchMethodsWithLogging(Il2CppClass* klass, const char* namespaceName, const char* className, const std::vector<std::string>& methodNames)
    {
        if (!klass)
        {
            printf("[-] Class %s.%s not found\n", namespaceName, className);
            return;
        }

        for (const auto& methodName : methodNames)
        {
            void* iter = nullptr;
            const Il2CppMethod* method;
            bool found = false;

            while ((method = il2cpp_class_get_methods(klass, &iter)))
            {
                const char* name = il2cpp_method_get_name(method);
                if (!name) continue;

                if (strcmp(name, methodName.c_str()) == 0)
                {
                    // Patch method to return immediately (RET)
                    void* fn = nullptr;
                    while (!(fn = *(void**)method))
                    {
                        Sleep(50);
                    }

                    DWORD oldProtect;
                    if (VirtualProtect(fn, 16, PAGE_EXECUTE_READWRITE, &oldProtect))
                    {
                        uint8_t* code = (uint8_t*)fn;
                        code[0] = 0xC3; // RET
                        VirtualProtect(fn, 16, oldProtect, &oldProtect);
                    }

                    printf("[+] Patched %s.%s::%s\n", namespaceName, className, methodName.c_str());
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                printf("[-] Method %s not found in %s.%s\n", methodName.c_str(), namespaceName, className);
            }
        }
    }

    void PatchBoolGetter(void* methodPtr, bool returnValue)
    {
        if (!methodPtr) return;

        DWORD oldProtect;
        if (VirtualProtect(methodPtr, 16, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            uint8_t* code = (uint8_t*)methodPtr;
            // mov al, returnValue; ret
            code[0] = 0xB0;
            code[1] = returnValue ? 0x01 : 0x00;
            code[2] = 0xC3;
            VirtualProtect(methodPtr, 16, oldProtect, &oldProtect);
        }
    }

    void PatchFloatGetter(void* methodPtr, float returnValue)
    {
        if (!methodPtr) return;

        DWORD oldProtect;
        if (VirtualProtect(methodPtr, 32, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            uint8_t* code = (uint8_t*)methodPtr;
            uint32_t floatBits = *(uint32_t*)&returnValue;

            // mov eax, bits; movd xmm0, eax; ret
            code[0] = 0xB8; // mov eax, imm32
            *(uint32_t*)(code + 1) = floatBits;
            code[5] = 0x66; code[6] = 0x0F; code[7] = 0x6E; code[8] = 0xC0; // movd xmm0, eax
            code[9] = 0xC3; // ret

            VirtualProtect(methodPtr, 32, oldProtect, &oldProtect);
        }
    }
}

// ========================================
// Player Feature Implementations
// ========================================

namespace FeaturePatch
{
    void ApplyGodMode(Il2CppImage* image)
    {
        // God mode is applied continuously in UpdateGodMode() (called from main loop)
        // This function is called once at raid start when the feature is enabled
        printf("[*] God Mode enabled - damage coefficient will be continuously set to -1.0f\n");
    }

    void ApplyInfiniteStamina(Il2CppImage* image)
    {
        printf("[*] Applying Infinite Stamina...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "", "Stamina"),
            "", "Stamina",
            { "Process", "Consume" }
        );
    }

    void ApplyNoFallDamage(Il2CppImage* image)
    {
        printf("[*] Applying No Fall Damage...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.HealthSystem", "ActiveHealthController"),
            "EFT.HealthSystem", "ActiveHealthController",
            { "HandleFall" }
        );
    }

    void ApplyNoEnergyDrain(Il2CppImage* image)
    {
        printf("[*] Applying No Energy Drain...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.HealthSystem", "ActiveHealthController"),
            "EFT.HealthSystem", "ActiveHealthController",
            { "ChangeEnergy" }
        );
    }

    void ApplyNoHydrationDrain(Il2CppImage* image)
    {
        printf("[*] Applying No Hydration Drain...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.HealthSystem", "ActiveHealthController"),
            "EFT.HealthSystem", "ActiveHealthController",
            { "ChangeHydration" }
        );
    }

    void ApplyNoFatigue(Il2CppImage* image)
    {
        printf("[*] Applying No Fatigue...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.HealthSystem", "ActiveHealthController"),
            "EFT.HealthSystem", "ActiveHealthController",
            { "SetEncumbered", "SetOverEncumbered", "AddFatigue" }
        );
    }

    // ========================================
    // Combat Feature Implementations
    // ========================================

    void ApplyNoRecoil(Il2CppImage* image)
    {
        printf("[*] Applying No Recoil...\n");

        // Patch all recoil systems
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Animations.NewRecoil", "NewRecoilShotEffect"),
            "EFT.Animations.NewRecoil", "NewRecoilShotEffect",
            { "AddRecoilForce" }
        );

        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Animations", "NewRotationRecoilProcess"),
            "EFT.Animations", "NewRotationRecoilProcess",
            { "CalculateRecoil" }
        );

        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Animations", "OldRecoilShotEffect"),
            "EFT.Animations", "OldRecoilShotEffect",
            { "AddRecoilForce" }
        );

        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Animations", "OldRotationRecoilProcess"),
            "EFT.Animations", "OldRotationRecoilProcess",
            { "CalculateRecoil" }
        );

        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Animations", "RecoilProcessBase"),
            "EFT.Animations", "RecoilProcessBase",
            { "CalculateRecoil" }
        );
    }

    void ApplyNoWeaponDurability(Il2CppImage* image)
    {
        printf("[*] Applying No Weapon Durability...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.InventoryLogic", "Weapon"),
            "EFT.InventoryLogic", "Weapon",
            { "OnShot" }
        );
    }

    void ApplyNoWeaponMalfunction(Il2CppImage* image)
    {
        printf("[*] Applying No Weapon Malfunction...\n");
        Il2CppClass* klass = il2cpp_class_from_name(image, "EFT.InventoryLogic", "Weapon");
        if (!klass)
        {
            printf("[-] Weapon class not found for malfunction patch\n");
            return;
        }

        // Find get_AllowMalfunction method
        void* iter = nullptr;
        const Il2CppMethod* method;
        bool found = false;

        while ((method = il2cpp_class_get_methods(klass, &iter)))
        {
            const char* name = il2cpp_method_get_name(method);
            if (!name) continue;

            if (strcmp(name, "get_AllowMalfunction") == 0)
            {
                void* fn = *(void**)method;
                PatchHelpers::PatchBoolGetter(fn, false);
                printf("[+] Patched Weapon.get_AllowMalfunction @ %p to return FALSE\n", fn);
                found = true;
                break;
            }
        }

        if (!found)
        {
            printf("[-] Weapon.get_AllowMalfunction not found\n");
        }
    }

    void ApplyNoWeaponOverheating(Il2CppImage* image)
    {
        printf("[*] Applying No Weapon Overheating...\n");
        Il2CppClass* klass = il2cpp_class_from_name(image, "EFT.InventoryLogic", "Weapon");
        if (!klass)
        {
            printf("[-] Weapon class not found for overheating patch\n");
            return;
        }

        // Find get_AllowOverheat method
        void* iter = nullptr;
        const Il2CppMethod* method;
        bool found = false;

        while ((method = il2cpp_class_get_methods(klass, &iter)))
        {
            const char* name = il2cpp_method_get_name(method);
            if (!name) continue;

            if (strcmp(name, "get_AllowOverheat") == 0)
            {
                void* fn = *(void**)method;
                PatchHelpers::PatchBoolGetter(fn, false);
                printf("[+] Patched Weapon.get_AllowOverheat @ %p to return FALSE\n", fn);
                found = true;
                break;
            }
        }

        if (!found)
        {
            printf("[-] Weapon.get_AllowOverheat not found\n");
        }
    }

    void ApplyInstantADS(Il2CppImage* image)
    {
        // TODO: Implement instant ADS
        printf("[*] Instant ADS - Not implemented yet\n");
    }

    // ========================================
    // AI Feature Implementations
    // ========================================

    void ApplyDisableAITracking(Il2CppImage* image)
    {
        printf("[*] Applying Disable AI Tracking...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT", "BotMemory"),
            "EFT", "BotMemory",
            { "Spotted", "SetLastTimeSeeEnemy", "LoseVisionCurrentEnemy", "LastEnemyVisionOld" }
        );
    }

    void ApplyDisableAIAttacks(Il2CppImage* image)
    {
        printf("[*] Applying Disable AI Attacks...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT", "BotMemory"),
            "EFT", "BotMemory",
            { "set_GoalEnemy" }
        );
    }

    // ========================================
    // Environment Feature Implementations
    // ========================================

    void ApplyNoBarbedWireDamage(Il2CppImage* image)
    {
        printf("[*] Applying No Barbed Wire Damage...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Interactive", "BarbedWire"),
            "EFT.Interactive", "BarbedWire",
            { "AddPenalty", "ProceedDamage" }
        );
    }

    void ApplyNoMinefieldDamage(Il2CppImage* image)
    {
        printf("[*] Applying No Minefield Damage...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Interactive", "Minefield"),
            "EFT.Interactive", "Minefield",
            { "DealExplosionDamage", "Explode" }
        );
    }

    void ApplyNoSniperZones(Il2CppImage* image)
    {
        printf("[*] Applying No Sniper Zones...\n");
        PatchHelpers::PatchMethodsWithLogging(
            (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Interactive", "SniperFiringZone"),
            "EFT.Interactive", "SniperFiringZone",
            { "Shoot" }
        );
    }

    // ========================================
    // Loot Feature Implementations
    // ========================================

    void ApplyInstantSearch(Il2CppImage* image)
    {
        // TODO: Implement instant search
        printf("[*] Instant Search - Not implemented yet\n");
    }

    void ApplyLuckySearch(Il2CppImage* image)
    {
        printf("[*] Applying Lucky Search...\n");

        auto skill_class = (Il2CppClass*)il2cpp_class_from_name(image, "EFT", "SkillManager");
        if (skill_class)
        {
            find_methods_by_names(skill_class, { "get_AttentionEliteLuckySearchValue" });
            if (!found_methods.empty())
            {
                struct MyIl2CppMethod { void* methodPointer; };
                void* fn = ((MyIl2CppMethod*)found_methods[0])->methodPointer;
                if (fn)
                {
                    PatchHelpers::PatchFloatGetter(fn, 1.0f);
                    printf("[+] Lucky search enabled (AttentionEliteLuckySearchValue = 1.0)\n");
                }
                else
                {
                    printf("[-] Method pointer is null for get_AttentionEliteLuckySearchValue\n");
                }
            }
            else
            {
                printf("[-] get_AttentionEliteLuckySearchValue not found\n");
            }
        }
        else
        {
            printf("[-] SkillManager class not found\n");
        }
    }

    // ========================================
    // Utility Feature Implementations
    // ========================================

    void ApplyBreachAllDoors(Il2CppImage* image)
    {
        printf("[*] Applying Breach All Doors...\n");

        auto door_class = (Il2CppClass*)il2cpp_class_from_name(image, "EFT.Interactive", "Door");
        if (door_class)
        {
            find_methods_by_names(door_class, { "BreachSuccessRoll" });
            if (!found_methods.empty())
            {
                void* fn = *(void**)found_methods[0];
                PatchHelpers::PatchBoolGetter(fn, true);
                printf("[+] Breach all doors enabled (BreachSuccessRoll = true)\n");
            }
            else
            {
                printf("[-] BreachSuccessRoll not found\n");
            }
        }
        else
        {
            printf("[-] Door class not found\n");
        }
    }

    // ========================================
    // Master Function - Apply All Enabled Features
    // ========================================

    void ApplyAllEnabledFeatures(Il2CppImage* image)
    {
        if (!image)
        {
            printf("[-] Image is null, cannot apply features\n");
            return;
        }

        printf("\n========================================\n");
        printf("  Applying Enabled Features\n");
        printf("========================================\n\n");

        // Player Features
        if (Features::god_mode) ApplyGodMode(image);
        if (Features::infinite_stamina) ApplyInfiniteStamina(image);
        if (Features::no_fall_damage) ApplyNoFallDamage(image);
        if (Features::no_energy_drain) ApplyNoEnergyDrain(image);
        if (Features::no_hydration_drain) ApplyNoHydrationDrain(image);
        if (Features::no_fatigue) ApplyNoFatigue(image);

        // Combat Features
        if (Features::no_recoil) ApplyNoRecoil(image);
        if (Features::no_weapon_durability) ApplyNoWeaponDurability(image);
        if (Features::no_weapon_malfunction) ApplyNoWeaponMalfunction(image);
        if (Features::no_weapon_overheating) ApplyNoWeaponOverheating(image);
        if (Features::instant_ads) ApplyInstantADS(image);

        // AI Features
        if (Features::disable_ai_tracking) ApplyDisableAITracking(image);
        if (Features::disable_ai_attacks) ApplyDisableAIAttacks(image);

        // Environment Features
        if (Features::no_barbed_wire_damage) ApplyNoBarbedWireDamage(image);
        if (Features::no_minefield_damage) ApplyNoMinefieldDamage(image);
        if (Features::no_sniper_zones) ApplyNoSniperZones(image);

        // Loot Features
        if (Features::instant_search) ApplyInstantSearch(image);
        if (Features::lucky_search) ApplyLuckySearch(image);

        // Utility Features
        if (Features::breach_all_doors) ApplyBreachAllDoors(image);

        printf("\n[+] All enabled features applied!\n");
        printf("========================================\n\n");
    }

    // ========================================
    // Raid Selected Location Updater
    // (Called from main loop - polls for location changes)
    // ========================================

    void UpdateRaidSelectedLocation(Il2CppImage* image)
    {
        // This logic is from sourceextra.txt lines 593-605
        // Wait until you see "Updated fields..." in console before you press Ready

        if (!g_TarkovApplicationInstance)
            return;

        // Read RaidSettings pointer from TarkovApplication + 0xD0
        uintptr_t raidSettings = *(uintptr_t*)((uintptr_t)g_TarkovApplicationInstance + OFFSET_RAID_SETTINGS);
        if (!raidSettings)
            return;

        // Read SelectedLocation pointer from RaidSettings + 0xA8
        uintptr_t selectedLocation = *(uintptr_t*)(raidSettings + OFFSET_SELECTED_LOCATION);

        // Check if location changed
        if (selectedLocation && g_selectedLocation != selectedLocation)
        {
            printf("[+] SelectedLocation = 0x%llX\n", selectedLocation);

            // Set offline raid flags
            *(bool*)(selectedLocation + 0x32) = false; // ForceOnlineRaidInPVE
            *(bool*)(selectedLocation + 0x33) = false; // ForceOfflineRaidInPVE

            printf("[+] Updated fields for selected location\n");

            // Update tracked location
            g_selectedLocation = selectedLocation;
        }
    }

    // ========================================
    // God Mode Updater
    // (Called from main loop - continuously sets damage coefficient)
    // ========================================

    void UpdateGodMode(Il2CppImage* image)
    {
        // Only run if god mode is enabled
        if (!Features::god_mode)
            return;

        // Requires CameraManager to be found
        if (!g_CameraManagerInstance)
            return;

        // Follow pointer chain to HealthController
        // CameraManager + 0x18 → EffectsController
        uintptr_t EffectsController = *(uintptr_t*)((uintptr_t)g_CameraManagerInstance + 0x18);
        if (!EffectsController)
            return;

        // EffectsController + 0x108 → LocalPlayer
        uintptr_t LocalPlayer = *(uintptr_t*)(EffectsController + 0x108);
        if (!LocalPlayer)
            return;

        // LocalPlayer + 0x940 → HealthController
        uintptr_t HealthController = *(uintptr_t*)(LocalPlayer + 0x940);
        if (!HealthController)
            return;

        // Set damage coefficient to -1.0f (invulnerability)
        // HealthController + 0x34 = DamageCoeff
        *(float*)(HealthController + 0x34) = -1.0f;
    }
}