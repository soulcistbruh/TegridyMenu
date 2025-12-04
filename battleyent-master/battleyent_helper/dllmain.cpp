#include <Windows.h>
#include <iostream>
#include <vector>
#include <cstdarg>
#include "il2api.h"
#include "menu.h"
#include "features.h"
#include "DX11Hook.h"
#include "Overlay.h"
#include "MenuRenderer.h"

// ========================================
// Present Hook Implementation
// ========================================

HRESULT __stdcall DX11Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    // First call - initialize overlay
    if (!Overlay::IsInitialized())
    {
        if (Overlay::Init(pSwapChain))
        {
            printf("[+] ImGui overlay initialized from Present hook\n");
        }
    }

    // Render overlay every frame
    if (Overlay::IsInitialized())
    {
        Overlay::BeginFrame();

        // Render hint text (always visible)
        MenuRenderer::RenderHintText();

        // Render main menu (only if menu_open)
        MenuRenderer::RenderMainMenu();

        Overlay::RenderFrame();
    }

    // Call original Present
    return oPresent(pSwapChain, SyncInterval, Flags);
}

// ========================================
// IL2CPP Function Pointer Definitions
// ========================================

il2cpp_get_root_domain_prot il2cpp_get_root_domain = nullptr;
il2cpp_thread_attach_prot il2cpp_thread_attach = nullptr;
il2cpp_domain_get_assemblies_prot il2cpp_domain_get_assemblies = nullptr;
il2cpp_class_from_name_prot il2cpp_class_from_name = nullptr;
il2cpp_class_get_methods_prot il2cpp_class_get_methods = nullptr;
il2cpp_method_get_name_prot il2cpp_method_get_name = nullptr;
il2cpp_assembly_get_image_prot il2cpp_assembly_get_image = nullptr;
il2cpp_image_get_name_prot il2cpp_image_get_name = nullptr;
il2cpp_runtime_invoke_prot il2cpp_runtime_invoke = nullptr;

std::vector<const Il2CppMethod*> found_methods;
std::vector<const Il2CppMethod*> error_screen_methods{};

void* g_TarkovApplicationInstance = nullptr;
void* g_CameraManagerInstance = nullptr;

// Raid location tracking
uintptr_t g_selectedLocation = 0;

// ========================================
// Static Fields Offset
// ========================================
#define STATIC_FIELDS_OFFSET 0xB8

// ========================================
// Updated Offsets (from NewOffsets.txt)
// ========================================

// --- CameraManager Offsets ---
#define OFFSET_EFFECTS_CONTROLLER 0x18
#define OFFSET_THERMAL_VISION 0x48

// --- LocalPlayer Offsets (from EffectsController + OFFSET_LOCAL_PLAYER) ---
#define OFFSET_LOCAL_PLAYER 0x108
#define OFFSET_PHYSICAL 0x8F8
#define OFFSET_HEALTH_CONTROLLER 0x940
#define OFFSET_HANDS_CONTROLLER 0x960
#define OFFSET_PROFILE 0x8E0
#define OFFSET_INVENTORY_CONTROLLER 0x958

// --- Physical Offsets ---
#define OFFSET_STAMINA 0x68
#define OFFSET_HANDS_STAMINA 0x70
#define OFFSET_OXYGEN 0x78
#define OFFSET_FLOAT_CURRENT 0x10  // Inside Stamina/HandsStamina/Oxygen

// --- HealthController Offsets ---
#define OFFSET_METABOLISM_DISABLED 0x25  // byte
#define OFFSET_DAMAGE_COEFF 0x34         // float

// --- HandsController Offsets ---
#define OFFSET_WEAPON_PREFAB 0xD8
#define OFFSET_WEAPON_DATA 0x180
#define OFFSET_COMPONENTS 0x40
#define OFFSET_WEAPON 0x10
#define OFFSET_WEAPON_COMPONENT 0x20
#define OFFSET_WEAPON_COMPONENT2 0x30

// --- WeaponComponent Offsets ---
#define OFFSET_DURABILITY 0x1C
#define OFFSET_MAX_DURABILITY 0x18
#define OFFSET_WEAPON_ITEM 0x10
#define OFFSET_WEAPON_TEMPLATE 0x20

// --- WeaponTemplate Offsets ---
#define OFFSET_ALLOW_JAM 0x308
#define OFFSET_ALLOW_FEED 0x309
#define OFFSET_ALLOW_MISFIRE 0x30A
#define OFFSET_ALLOW_SLIDE 0x30B
#define OFFSET_ALLOW_OVERHEAT 0x320

// --- Magazine Offsets ---
#define OFFSET_MAG_SLOT_CACHE 0xC8
#define OFFSET_MAG_CONTAINED_ITEM 0x48
#define OFFSET_CARTRIDGES 0xA8
#define OFFSET_CARTRIDGES_ITEMS 0x18
#define OFFSET_CARTRIDGES_ITEM 0x10
#define OFFSET_CARTRIDGE_ITEM0 0x20
#define OFFSET_CARTRIDGE_STACK 0x24

// --- Bullet Offsets ---
#define OFFSET_BULLET_TEMPLATE 0x60
#define OFFSET_BULLET_DAMAGE 0x150
#define OFFSET_BULLET_PENETRATION_POWER 0x164

// --- TarkovApplication Offsets ---
#define OFFSET_RAID_SETTINGS 0xD0
#define OFFSET_SELECTED_LOCATION 0xA8

// --- SelectedLocation Offsets ---
#define OFFSET_MAP_ID 0x28
#define OFFSET_FORCE_ONLINE_RAID 0x32   // byte
#define OFFSET_FORCE_OFFLINE_RAID 0x33  // byte
#define OFFSET_ESCAPE_TIME_LIMIT 0x84
#define OFFSET_MAX_BOTS_PER_ZONE 0x90
#define OFFSET_WAVES 0xB8

// --- MapId Offsets ---
#define OFFSET_MAP_NAME 0x14

// --- Waves Offsets ---
#define OFFSET_WILD_SPAWN_WAVE0 0x20
#define OFFSET_WILD_SPAWN_WAVE1 0x28
#define OFFSET_WILD_SPAWN_WAVE2 0x30
#define OFFSET_WILD_SPAWN_WAVE3 0x38
#define OFFSET_WILD_SPAWN_WAVE4 0x40
#define OFFSET_WILD_SPAWN_WAVE5 0x48
#define OFFSET_WILD_SPAWN_WAVE6 0x50
#define OFFSET_WILD_SPAWN_WAVE7 0x58

// --- SpawnWave Offsets ---
#define OFFSET_TIME_MIN 0x10
#define OFFSET_TIME_MAX 0x14
#define OFFSET_SLOTS_MIN 0x18
#define OFFSET_SLOTS_MAX 0x1C
#define OFFSET_WILD_SPAWN_TYPE 0x2C

// --- ThermalVision Offsets ---
#define OFFSET_TV_ON 0x20
#define OFFSET_TV_IS_NOISY 0x21
#define OFFSET_TV_IS_MOTION_BLURRED 0x23
#define OFFSET_TV_IS_GLITCH 0x24
#define OFFSET_TV_UNSHARP_RADIUS_BLUR 0x90
#define OFFSET_TV_UNSHARP_BIAS 0x94

// --- ProceduralWeaponAnimations Offsets ---
#define OFFSET_FIREARMS 0xD0
#define OFFSET_PROCEDURAL_WEAPON_ANIMATIONS 0x20
#define OFFSET_SHOOTING 0x58
#define OFFSET_NEW_SHOT_RECOIL 0x20

// --- NewShotRecoil Offsets ---
#define OFFSET_USE_OFFSET 0x10
#define OFFSET_RECOIL_EFFECT_ON 0x11
#define OFFSET_PROGRESS_POWER_ON 0x12
#define OFFSET_PROGRESS_POWER_SHOT 0x14

// --- Profile Offsets ---
#define OFFSET_SKILLS 0x80
#define OFFSET_FENCE_INFO 0x120
#define OFFSET_STANDING 0x60

// --- Skills Offsets ---
#define OFFSET_SKILL_PISTOL 0x230
#define OFFSET_SKILL_REVOLVER 0x238
#define OFFSET_SKILL_SMG 0x240
#define OFFSET_SKILL_ASSAULT 0x248
#define OFFSET_SKILL_SHOTGUN 0x250
#define OFFSET_SKILL_SNIPER 0x258
#define OFFSET_SKILL_LMG 0x260
#define OFFSET_SKILL_HMG 0x268
#define OFFSET_SKILL_LAUNCHER 0x270
#define OFFSET_SKILL_ATTACHED_LAUNCHER 0x278
#define OFFSET_SKILL_MELEE 0x288
#define OFFSET_SKILL_DMR 0x290
#define OFFSET_SKILL_THROWING 0x298
#define OFFSET_SKILL_AIM_DRILLS 0x2A0
#define OFFSET_SKILL_RECOIL_CONTROL 0x2A8
#define OFFSET_SKILL_TROUBLES_SHOOTING 0x2B0
#define OFFSET_SKILL_PERCEPTION 0x528
#define OFFSET_SKILL_INTELLECT 0x530
#define OFFSET_SKILL_ATTENTION 0x538
#define OFFSET_SKILL_IMMUNITY 0x540
#define OFFSET_SKILL_CHARISMA 0x548
#define OFFSET_SKILL_ENDURANCE 0x558
#define OFFSET_SKILL_STRENGTH 0x560
#define OFFSET_SKILL_VITALITY 0x568
#define OFFSET_SKILL_HEALTH 0x570
#define OFFSET_SKILL_METABOLISM 0x578
#define OFFSET_SKILL_STRESS_RESISTANCE 0x580
#define OFFSET_SKILL_SNIPING 0x588
#define OFFSET_SKILL_COVERT_MOVEMENT 0x590
#define OFFSET_SKILL_PRONE_MOVEMENT 0x598
#define OFFSET_SKILL_LIGHT_VESTS 0x5A0
#define OFFSET_SKILL_HEAVY_VESTS 0x5A8
#define OFFSET_SKILL_WEAPON_MODDING 0x5B0
#define OFFSET_SKILL_ADVANCED_MODDING 0x5B8
#define OFFSET_SKILL_LOCKPICKING 0x5C0
#define OFFSET_SKILL_SURGERY 0x5C8
#define OFFSET_SKILL_SEARCH 0x5D0
#define OFFSET_SKILL_WEAPON_TREATMENT 0x5D8
#define OFFSET_SKILL_MAG_DRILLS 0x5E0
#define OFFSET_SKILL_FREETRADING 0x5E8
#define OFFSET_SKILL_BARTER 0x5F0
#define OFFSET_SKILL_CRAFTING 0x5F8
#define OFFSET_SKILL_HIDEOUT_MANAGEMENT 0x600
#define OFFSET_SKILL_BOT_RELOAD 0x750

// --- Skill Value Offset ---
#define OFFSET_SKILL_CURRENT_VALUE 0x28  // float, 51000 = max level

// --- InventoryController Offsets ---
#define OFFSET_INVENTORY 0x100
#define OFFSET_EQUIPMENT 0x18
#define OFFSET_SLOTS 0x80
#define OFFSET_SLOT_BACKPACK 0x60

// --- Backpack Offsets ---
#define OFFSET_BP_CONTAINED_ITEM 0x48
#define OFFSET_BP_GRIDS 0x78
#define OFFSET_SEARCHABLE_GRID_0 0x20
#define OFFSET_BP_ITEM_COLLECTION 0x48
#define OFFSET_BP_ITEM_LIST 0x18
#define OFFSET_BP_ITEMS 0x10

// --- Backpack Items Array Offsets ---
#define OFFSET_BP_ITEM0 0x20
#define OFFSET_BP_ITEM1 0x28
#define OFFSET_BP_ITEM2 0x30
#define OFFSET_BP_ITEM3 0x38
#define OFFSET_BP_ITEM4 0x40
#define OFFSET_BP_ITEM5 0x48
#define OFFSET_BP_ITEM6 0x50
#define OFFSET_BP_ITEM7 0x58
#define OFFSET_BP_ITEM8 0x60
#define OFFSET_BP_ITEM9 0x68
#define OFFSET_BP_ITEM10 0x70

// --- Item Offsets ---
#define OFFSET_STACK_OBJECTS_COUNT 0x24
#define OFFSET_SPAWNED_IN_SESSION 0x68
#define OFFSET_TEMPLATE 0x60

// --- Template Offsets ---
#define OFFSET_ITEM_NAME_PTR 0x10
#define OFFSET_ITEM_NAME 0x14

// ========================================
// IL2CPP Helper Functions
// ========================================

void find_methods_by_names(Il2CppClass* klass, const std::vector<std::string>& names)
{
    found_methods.clear();
    if (!klass) return;
    void* iter = nullptr;
    const Il2CppMethod* method;
    while ((method = il2cpp_class_get_methods(klass, &iter)))
    {
        const char* mname = il2cpp_method_get_name(method);
        if (!mname) continue;
        for (auto& target : names)
        {
            if (strcmp(mname, target.c_str()) == 0)
            {
                found_methods.push_back(method);
            }
        }
    }
}

void patch_method_ret_safe(const Il2CppMethod* method)
{
    if (!method) return;
    void* fn = nullptr;
    while (!(fn = *(void**)method))
    {
        Sleep(50);
    }
    DWORD oldProtect;
    if (VirtualProtect(fn, 16, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        uint8_t* code = (uint8_t*)fn;
        code[0] = 0xC3;
        VirtualProtect(fn, 16, oldProtect, &oldProtect);
    }
}

const Il2CppMethod* find_battleye_init(Il2CppClass* main_application)
{
    if (!main_application) return nullptr;
    void* iter = nullptr;
    const Il2CppMethod* method;
    while ((method = il2cpp_class_get_methods(main_application, &iter)))
    {
        const char* name = il2cpp_method_get_name(method);
        if (!name) continue;
        if ((unsigned char)name[0] == 0xEE && (unsigned char)name[1] == 0x80 && (unsigned char)name[2] == 0x81)
            return method;
        if (strcmp(name, "ValidateAnticheat") == 0) return method;
    }
    return nullptr;
}

const Il2CppMethod* wait_for_battleye_init(Il2CppClass* main_application)
{
    const Il2CppMethod* method = nullptr;
    while (!(method = find_battleye_init(main_application)))
        Sleep(200);
    return method;
}

void find_show_error_screen(Il2CppClass* preloader_ui)
{
    error_screen_methods.clear();
    void* iter = nullptr;
    const Il2CppMethod* method;
    while ((method = il2cpp_class_get_methods(preloader_ui, &iter)))
    {
        const char* name = il2cpp_method_get_name(method);
        if (!name) continue;
        if (strcmp(name, "ShowErrorScreen") == 0)
        {
            error_screen_methods.push_back(method);
        }
    }
}

Il2CppImage* il2cpp_image_loaded(const char* image_name)
{
    Il2CppDomain* domain = il2cpp_get_root_domain();
    if (!domain) return nullptr;
    size_t size = 0;
    const Il2CppAssembly* const* asmbl = il2cpp_domain_get_assemblies(domain, &size);
    if (!asmbl) return nullptr;
    for (size_t i = 0; i < size; i++)
    {
        const Il2CppImage* img = il2cpp_assembly_get_image(asmbl[i]);
        if (!img) continue;
        const char* nm = il2cpp_image_get_name(img);
        if (!nm) continue;
        if (_stricmp(nm, image_name) == 0)
            return (Il2CppImage*)img;
    }
    return nullptr;
}

const Il2CppMethod* g_cachedGetInstanceMethod = nullptr;

void* get_camera_manager_instance(Il2CppImage* image)
{
    auto cameraManagerClass = il2cpp_class_from_name(image, "EFT.CameraControl", "CameraManager");
    if (!cameraManagerClass) return nullptr;
    if (!g_cachedGetInstanceMethod)
    {
        void* iter = nullptr;
        const Il2CppMethod* method;
        while ((method = il2cpp_class_get_methods(cameraManagerClass, &iter)))
        {
            const char* mname = il2cpp_method_get_name(method);
            if (!mname) continue;
            if (strcmp(mname, "get_Instance") == 0)
            {
                g_cachedGetInstanceMethod = method;
                break;
            }
        }
        if (!g_cachedGetInstanceMethod) return nullptr;
    }
    if (!il2cpp_runtime_invoke) return nullptr;
    void* exc = nullptr;
    void* instance = il2cpp_runtime_invoke(g_cachedGetInstanceMethod, nullptr, nullptr, &exc);
    if (!instance || exc) return nullptr;
    return instance;
}

void* get_tarkov_application_instance(Il2CppClass* klass)
{
    uintptr_t static_fields_ptr = *(uintptr_t*)((uintptr_t)klass + STATIC_FIELDS_OFFSET);
    if (!static_fields_ptr) return nullptr;
    uintptr_t* static_fields = (uintptr_t*)static_fields_ptr;
    for (int i = 0; i < 32; i++)
    {
        uintptr_t candidate = static_fields[i];
        if (!candidate) continue;
        Il2CppClass* objClass = *(Il2CppClass**)candidate;
        if (objClass == klass)
            return (void*)candidate;
    }
    return nullptr;
}

// ========================================
// Main Function
// ========================================

void start()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    printf("=================================\n");
    printf("  PvE Tegridy Menu - Loading...\n");
    printf("=================================\n\n");

    HMODULE il2cpp = nullptr;
    while (!(il2cpp = GetModuleHandleA("GameAssembly.dll")))
        Sleep(2000);
    printf("[+] GameAssembly.dll Found\n");

    il2cpp_get_root_domain = (il2cpp_get_root_domain_prot)GetProcAddress(il2cpp, "il2cpp_domain_get");
    il2cpp_thread_attach = (il2cpp_thread_attach_prot)GetProcAddress(il2cpp, "il2cpp_thread_attach");
    il2cpp_domain_get_assemblies = (il2cpp_domain_get_assemblies_prot)GetProcAddress(il2cpp, "il2cpp_domain_get_assemblies");
    il2cpp_class_from_name = (il2cpp_class_from_name_prot)GetProcAddress(il2cpp, "il2cpp_class_from_name");
    il2cpp_class_get_methods = (il2cpp_class_get_methods_prot)GetProcAddress(il2cpp, "il2cpp_class_get_methods");
    il2cpp_method_get_name = (il2cpp_method_get_name_prot)GetProcAddress(il2cpp, "il2cpp_method_get_name");
    il2cpp_assembly_get_image = (il2cpp_assembly_get_image_prot)GetProcAddress(il2cpp, "il2cpp_assembly_get_image");
    il2cpp_image_get_name = (il2cpp_image_get_name_prot)GetProcAddress(il2cpp, "il2cpp_image_get_name");
    il2cpp_runtime_invoke = (il2cpp_runtime_invoke_prot)GetProcAddress(il2cpp, "il2cpp_runtime_invoke");

    Sleep(15000);

    Il2CppDomain* domain = il2cpp_get_root_domain();
    il2cpp_thread_attach(domain);
    printf("[+] Attached to IL2CPP domain\n");

    Sleep(1000);

    Il2CppImage* image = nullptr;
    while (!(image = il2cpp_image_loaded("Assembly-CSharp.dll")))
        Sleep(2000);
    printf("[+] Assembly-CSharp.dll Found\n");

    Sleep(1000);

    auto tarkovApp = il2cpp_class_from_name(image, "EFT", "TarkovApplication");
    if (!tarkovApp)
    {
        printf("[-] TarkovApplication Not Found\n");
        return;
    }
    printf("[+] TarkovApplication Found\n");

    Sleep(1000);

    const Il2CppMethod* battleyeMethod = wait_for_battleye_init(tarkovApp);
    if (battleyeMethod)
    {
        patch_method_ret_safe(battleyeMethod);
        printf("[+] Battleye init patched\n");
    }

    HMODULE beclient = GetModuleHandleA("BEClient_x64.dll");
    if (beclient) FreeLibrary(beclient);

    Sleep(1000);

    auto preloaderUI = il2cpp_class_from_name(image, "EFT.UI", "PreloaderUI");
    if (preloaderUI)
    {
        find_show_error_screen(preloaderUI);
        for (auto m : error_screen_methods)
        {
            patch_method_ret_safe(m);
            printf("[+] ShowErrorScreen --> Patched\n");
        }
        printf("[+] Patching Successful!\n");
    }

    Sleep(1000);
    printf("[*] Cleaning log in 5 seconds...\n");
    Sleep(5000);
    system("cls");

    Sleep(10000);

    // ========================================
    // Initialize DX11 Hook for ImGui Overlay
    // ========================================
    printf("[*] Setting up ImGui overlay...\n");
    if (DX11Hook::Initialize())
    {
        printf("[+] DX11 hook ready - ImGui will initialize on first frame\n");
    }
    else
    {
        printf("[-] DX11 hook failed - continuing without overlay\n");
    }

    Sleep(1000);

    printf("========================================\n");
    printf("  PvE Tegridy - Ready!\n");
    printf("========================================\n\n");
    printf("Controls:\n");
    printf("  INSERT - Toggle menu\n");
    printf("  END    - Emergency exit\n\n");
    printf("[*] Scanning for TarkovApplication + CameraManager...\n\n");

    // ========================================
    // Main Loop - Monitor and Apply Patches
    // ========================================
    bool patchesApplied = false;
    float nextRaidUpdateTime = 0.0f;
    const float raidUpdateInterval = 2.0f; // Check raid location every 2 seconds

    while (true)
    {
        // Check menu input
        Menu::CheckInput();

        // Check for END key press (panic key)
        if (GetAsyncKeyState(VK_END) & 0x8000)
        {
            printf("\n[!] END key detected - Initiating safe shutdown...\n");
            Sleep(500);
            printf("[*] Terminating game process...\n");
            TerminateProcess(GetCurrentProcess(), 0);
            return;
        }

        // Track instances
        if (!g_TarkovApplicationInstance)
        {
            g_TarkovApplicationInstance = get_tarkov_application_instance(tarkovApp);
            if (g_TarkovApplicationInstance)
            {
                printf("[+] TarkovApplication = 0x%llX\n", (uintptr_t)g_TarkovApplicationInstance);
            }
        }

        void* cm = get_camera_manager_instance(image);
        if (cm && cm != g_CameraManagerInstance)
        {
            g_CameraManagerInstance = cm;
            printf("[+] CameraManager = 0x%llX\n\n", (uintptr_t)cm);
        }

        // Track previous feature states to detect changes
        static bool prev_god_mode = false;
        static bool prev_infinite_stamina = false;
        static bool prev_no_fall_damage = false;
        static bool prev_no_energy_drain = false;
        static bool prev_no_hydration_drain = false;
        static bool prev_no_fatigue = false;
        static bool prev_no_recoil = false;
        static bool prev_no_weapon_durability = false;
        static bool prev_no_weapon_malfunction = false;
        static bool prev_no_weapon_overheating = false;
        static bool prev_instant_ads = false;
        static bool prev_disable_ai_tracking = false;
        static bool prev_disable_ai_attacks = false;
        static bool prev_no_barbed_wire_damage = false;
        static bool prev_no_minefield_damage = false;
        static bool prev_no_sniper_zones = false;
        static bool prev_instant_search = false;
        static bool prev_lucky_search = false;
        static bool prev_breach_all_doors = false;

        // Check if any feature toggle has changed
        bool featuresChanged =
            (Features::god_mode != prev_god_mode) ||
            (Features::infinite_stamina != prev_infinite_stamina) ||
            (Features::no_fall_damage != prev_no_fall_damage) ||
            (Features::no_energy_drain != prev_no_energy_drain) ||
            (Features::no_hydration_drain != prev_no_hydration_drain) ||
            (Features::no_fatigue != prev_no_fatigue) ||
            (Features::no_recoil != prev_no_recoil) ||
            (Features::no_weapon_durability != prev_no_weapon_durability) ||
            (Features::no_weapon_malfunction != prev_no_weapon_malfunction) ||
            (Features::no_weapon_overheating != prev_no_weapon_overheating) ||
            (Features::instant_ads != prev_instant_ads) ||
            (Features::disable_ai_tracking != prev_disable_ai_tracking) ||
            (Features::disable_ai_attacks != prev_disable_ai_attacks) ||
            (Features::no_barbed_wire_damage != prev_no_barbed_wire_damage) ||
            (Features::no_minefield_damage != prev_no_minefield_damage) ||
            (Features::no_sniper_zones != prev_no_sniper_zones) ||
            (Features::instant_search != prev_instant_search) ||
            (Features::lucky_search != prev_lucky_search) ||
            (Features::breach_all_doors != prev_breach_all_doors);

        // Apply features only when CameraManager exists and (first time OR features changed)
        if (cm && (!patchesApplied || featuresChanged))
        {
            if (!patchesApplied)
            {
                printf("[*] Game ready - applying feature patches...\n");
            }
            else if (featuresChanged)
            {
                printf("[*] Feature Toggle changed - reapplying patches...\n");
            }

            FeaturePatch::ApplyAllEnabledFeatures(image);

            // Update previous states
            prev_god_mode = Features::god_mode;
            prev_infinite_stamina = Features::infinite_stamina;
            prev_no_fall_damage = Features::no_fall_damage;
            prev_no_energy_drain = Features::no_energy_drain;
            prev_no_hydration_drain = Features::no_hydration_drain;
            prev_no_fatigue = Features::no_fatigue;
            prev_no_recoil = Features::no_recoil;
            prev_no_weapon_durability = Features::no_weapon_durability;
            prev_no_weapon_malfunction = Features::no_weapon_malfunction;
            prev_no_weapon_overheating = Features::no_weapon_overheating;
            prev_instant_ads = Features::instant_ads;
            prev_disable_ai_tracking = Features::disable_ai_tracking;
            prev_disable_ai_attacks = Features::disable_ai_attacks;
            prev_no_barbed_wire_damage = Features::no_barbed_wire_damage;
            prev_no_minefield_damage = Features::no_minefield_damage;
            prev_no_sniper_zones = Features::no_sniper_zones;
            prev_instant_search = Features::instant_search;
            prev_lucky_search = Features::lucky_search;
            prev_breach_all_doors = Features::breach_all_doors;

            if (!patchesApplied)
            {
                patchesApplied = true;
                printf("\n[+] Patches Applied...\n\n");
            }
        }

        // Update raid/SelectedLocation periodically (every 2 seconds)
        float currentTime = GetTickCount64() / 1000.0f;
        if (currentTime >= nextRaidUpdateTime)
        {
            FeaturePatch::UpdateRaidSelectedLocation(image);
            nextRaidUpdateTime = currentTime + raidUpdateInterval;
        }

        // Update god mode (continuously set damage coefficient if enabled)
        // This runs every frame (100ms) to maintain invulnerability
        FeaturePatch::UpdateGodMode(image);

        Sleep(100);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)start, nullptr, 0, nullptr);
    return TRUE;
}