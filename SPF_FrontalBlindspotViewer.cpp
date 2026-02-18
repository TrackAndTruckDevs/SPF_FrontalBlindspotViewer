/**
 * @file SPF_FrontalBlindspotViewer.cpp
 * @brief The main implementation file for the SPF_FrontalBlindspotViewer.
 */

#define _USE_MATH_DEFINES
#include "SPF_FrontalBlindspotViewer.hpp" // Always include your own header first
#include <cstring>      // For C-style string manipulation functions like strncpy_s.
#include <chrono>       // For std::chrono for deltaTime calculation
#include <cmath>        // For std::sin, std::cos, std::fmod etc.

namespace SPF_FrontalBlindspotViewer {

// =================================================================================================
// 1. Constants & Global State
// =================================================================================================

/**
 * @brief A constant for the plugin's name.
 * @details This MUST match the name used in `Cfg_GetContext` calls for various APIs
 * and the plugin's directory name.
 */
const char* PLUGIN_NAME = "SPF_FrontalBlindspotViewer";

/**
 * @brief The single, global instance of the plugin's context.
 * @details This is the central point for accessing all plugin state.
 */
PluginContext g_ctx;

// =================================================================================================
// 2. Manifest Implementation
// =================================================================================================

    void BuildManifest(SPF_Manifest_Builder_Handle* h, const SPF_Manifest_Builder_API* api) {
        // This function defines all the metadata for your plugin. The framework calls this
        // function *before* loading your plugin DLL to understand what it is.
        // --- 2.1. Plugin Information ---
        // This section provides the basic identity of your plugin.
        {
            api->Info_SetName(h, PLUGIN_NAME);
            api->Info_SetVersion(h, "1.0.2");
            api->Info_SetMinFrameworkVersion(h, "1.1.3");
            api->Info_SetAuthor(h, "Track'n'Truck Devs");
            api->Info_SetDescriptionKey(h, "plugin.description");

            api->Info_SetEmail(h, "mailto:spf.framework@gmail.com");
            api->Info_SetYoutubeUrl(h, "https://www.youtube.com/@TrackAndTruck");
            api->Info_SetPatreonUrl(h, "https://www.patreon.com/TrackAndTruckDevs");
        }


        {
            // `allowUserConfig`: Set to `true` if you want a `settings.json` file to be created
            // for your plugin, allowing users (or the framework UI) to override default settings.
            api->Policy_SetAllowUserConfig(h, true);

            // Enable specific systems in the settings UI.
            api->Policy_AddConfigurableSystem(h, "settings");
            api->Policy_AddConfigurableSystem(h, "localization");
        }

        // --- 2.3. Custom Settings Defaults ---
        // A JSON string defines the default values for your plugin's custom settings.
        api->Settings_SetJson(h, R"json(
            {
                "target_camera": {
                    "position": { "x": -0.06, "y": -0.10, "z": -0.88 },
                    "rotation": { "yaw": -0.03, "pitch": 0.58 },
                    "fov": 80.0
                },
                "animation": {
                    "speed": 1.1,
                    "type": "live"
                }
            }
        )json");

        // --- 2.4. Default Settings for Framework Systems ---

        // Logging
        api->Defaults_SetLogging(h, "info", false);

        // Localization
        api->Defaults_SetLocalization(h, "en");

        // Keybinds
        {
            api->Defaults_AddKeybind(h, "SPF_FrontalBlindspotViewer", "toggle", "keyboard", "KEY_F10", "always");
        }

        // =============================================================================================
        // 2.5. Metadata for UI Display (Optional)
        // =============================================================================================

        // --- Custom Settings Metadata ---
        
        auto AddSliderMeta = [&](const char *key, const char *title, const char *desc, float min, float max, const char *format)
        {
            std::string params = "{ \"min\": " + std::to_string(min) + 
                                 ", \"max\": " + std::to_string(max) + 
                                 ", \"format\": \"" + format + "\" }";
            api->Meta_AddCustomSetting(h, key, title, desc, "slider", params.c_str(), false);
        };

        //--- Metadata for target_camera.position.x ---
        AddSliderMeta("target_camera.position.x", "settings.target_camera.position.x.title", "settings.target_camera.position.x.desc", -5.0f, 5.0f, "%.3f");

        //--- Metadata for target_camera.position.y ---
        AddSliderMeta("target_camera.position.y", "settings.target_camera.position.y.title", "settings.target_camera.position.y.desc", -5.0f, 5.0f, "%.3f");

        //--- Metadata for target_camera.position.z ---
        AddSliderMeta("target_camera.position.z", "settings.target_camera.position.z.title", "settings.target_camera.position.z.desc", -5.0f, 5.0f, "%.3f");

        //--- Metadata for target_camera.rotation.yaw ---
        AddSliderMeta("target_camera.rotation.yaw", "settings.target_camera.rotation.yaw.title", "settings.target_camera.rotation.yaw.desc", -3.1415f, 3.1415f, "%.3f");

        //--- Metadata for target_camera.rotation.pitch ---
        AddSliderMeta("target_camera.rotation.pitch", "settings.target_camera.rotation.pitch.title", "settings.target_camera.rotation.pitch.desc", -1.571f, 1.571f, "%.3f");

        //--- Metadata for target_camera.fov ---
        AddSliderMeta("target_camera.fov", "settings.target_camera.fov.title", "settings.target_camera.fov.desc", 30.0f, 120.0f, "%.1f");

        //--- Metadata for animation.speed ---
        AddSliderMeta("animation.speed", "settings.animation.speed.title", "settings.animation.speed.desc", 0.1f, 3.0f, "%.1f");

        //--- Metadata for animation.type ---
        const char* animation_type_options = R"json({ "options": [
            { "value": "linear", "labelKey": "settings.animation_type_options.Linear" },
            { "value": "live", "labelKey": "settings.animation_type_options.Live" }
        ]})json";
        api->Meta_AddCustomSetting(h, "animation.type", "settings.animation.type.title", "settings.animation.type.desc", "combo", animation_type_options, false);

        //--- Metadata for the group labels ---
        api->Meta_AddCustomSetting(h, "target_camera", "settings.groups.target_camera.title", "settings.groups.target_camera.desc", nullptr, nullptr, false);
        api->Meta_AddCustomSetting(h, "animation", "settings.groups.animation.title", "settings.groups.animation.desc", nullptr, nullptr, false);
        api->Meta_AddCustomSetting(h, "target_camera.position", "settings.groups.target_camera.position.title", "settings.groups.target_camera.position.desc", nullptr, nullptr, false);
        api->Meta_AddCustomSetting(h, "target_camera.rotation", "settings.groups.target_camera.rotation.title", "settings.groups.target_camera.rotation.desc", nullptr, nullptr, false);

        // Keybind Metadata
        api->Meta_AddKeybind(h, "SPF_FrontalBlindspotViewer", "toggle", "keybinds.toggle.title", "keybinds.toggle.desc");
    }

// =================================================================================================
// 3. Plugin Lifecycle Implementations
// =================================================================================================
// The following functions are the core lifecycle events for the plugin.

void OnLoad(const SPF_Load_API* load_api) {
    // Cache the provided API pointers in our global context.
    g_ctx.loadAPI = load_api;

    // --- Essential API Initialization ---
    // Get and cache the logger and formatting API handles.
    if (g_ctx.loadAPI) {
        g_ctx.loggerHandle = g_ctx.loadAPI->logger->Log_GetContext(PLUGIN_NAME);
        g_ctx.formattingAPI = g_ctx.loadAPI->formatting;

        if (g_ctx.loggerHandle && g_ctx.formattingAPI) {
            char log_buffer[256];
            g_ctx.formattingAPI->Fmt_Format(log_buffer, sizeof(log_buffer), "%s has been loaded!", PLUGIN_NAME);
            g_ctx.loadAPI->logger->Log(g_ctx.loggerHandle, SPF_LOG_INFO, log_buffer);
        }
    }

    // --- Optional API Initialization (Uncomment if needed) ---
    // Remember to also uncomment the relevant #include directives in SPF_FrontalBlindspotViewer.hpp
    // and add corresponding members to the PluginContext struct.

    // Config API
    // Requires: SPF_Config_API.h
    if (g_ctx.loadAPI && g_ctx.loadAPI->config) {
        g_ctx.configHandle = g_ctx.loadAPI->config->Cfg_GetContext(PLUGIN_NAME);
    }
    
    // Localization API
    // Requires: SPF_Localization_API.h
    if (g_ctx.loadAPI && g_ctx.loadAPI->localization) {
        g_ctx.localizationHandle = g_ctx.loadAPI->localization->Loc_GetContext(PLUGIN_NAME);
    }
}

void OnActivated(const SPF_Core_API* core_api) {
    g_ctx.coreAPI = core_api;

    if (g_ctx.loggerHandle && g_ctx.formattingAPI) {
        char log_buffer[256];
        g_ctx.formattingAPI->Fmt_Format(log_buffer, sizeof(log_buffer), "%s has been activated!", PLUGIN_NAME);
        g_ctx.loadAPI->logger->Log(g_ctx.loggerHandle, SPF_LOG_INFO, log_buffer);
    }

    // --- Optional API Initialization & Callback Registration (Uncomment if needed) ---
    // Remember to also uncomment the relevant #include directives in SPF_FrontalBlindspotViewer.hpp
    // and add corresponding members to the PluginContext struct.

    // Keybinds API
    // Requires: SPF_KeyBinds_API.h
    if (g_ctx.coreAPI && g_ctx.coreAPI->keybinds) {
        g_ctx.keybindsHandle = g_ctx.coreAPI->keybinds->Kbind_GetContext(PLUGIN_NAME);
        if (g_ctx.keybindsHandle) {
            // Register the callback for our "toggle" action.
            g_ctx.coreAPI->keybinds->Kbind_Register(g_ctx.keybindsHandle, "SPF_FrontalBlindspotViewer.toggle", OnKeybindAction);
        }
    }

    // Camera API
    // Requires: SPF_Camera_API.h
    if (g_ctx.coreAPI && g_ctx.coreAPI->camera) {
        g_ctx.cameraAPI = g_ctx.coreAPI->camera;
    }

    // Load initial settings
    LoadSettings();

    g_ctx.lastFrameTime = std::chrono::high_resolution_clock::now();
}

void OnUpdate() {
    // This function is called every frame while the plugin is active.
    // Avoid performing heavy or blocking operations here, as it will directly impact game performance.

    if (g_ctx.isAnimating) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - g_ctx.lastFrameTime;
        AnimateCamera(deltaTime.count());
    }
    g_ctx.lastFrameTime = std::chrono::high_resolution_clock::now();

}

void OnUnload() {
    // Perform cleanup. Nullify cached API pointers to prevent use-after-free
    // and ensure a clean shutdown. This is the last chance for cleanup.

    if (g_ctx.loadAPI && g_ctx.loggerHandle && g_ctx.formattingAPI) {
        char log_buffer[256];
        g_ctx.formattingAPI->Fmt_Format(log_buffer, sizeof(log_buffer), "%s is being unloaded.", PLUGIN_NAME);
        g_ctx.loadAPI->logger->Log(g_ctx.loggerHandle, SPF_LOG_INFO, log_buffer);
    }

    // --- Optional API Cleanup (Uncomment if needed) ---
    // Example: Unregistering keybinds (often handled by framework, but good practice if explicitly registered).
    // Requires: SPF_KeyBinds_API.h

    // Nullify all cached API pointers and handles.
    g_ctx.coreAPI = nullptr;
    g_ctx.loadAPI = nullptr;
    g_ctx.loggerHandle = nullptr;
    g_ctx.formattingAPI = nullptr;

    // --- Optional Handles (Nullify if used) ---
    g_ctx.configHandle = nullptr;
    g_ctx.localizationHandle = nullptr;
    g_ctx.keybindsHandle = nullptr;
    g_ctx.cameraAPI = nullptr;
}

// =================================================================================================
// 4. Optional Callback Implementations (Commented Out)
// =================================================================================================

void LoadSettings() {
    if (!g_ctx.configHandle || !g_ctx.loadAPI || !g_ctx.loadAPI->config) {
        return;
    }

    auto config = g_ctx.loadAPI->config;

    // Load target camera position
    g_ctx.target_pos[0] = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.position.x", g_ctx.target_pos[0]);
    g_ctx.target_pos[1] = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.position.y", g_ctx.target_pos[1]);
    g_ctx.target_pos[2] = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.position.z", g_ctx.target_pos[2]);

    // Load target camera rotation
    g_ctx.target_rot[0] = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.rotation.yaw", g_ctx.target_rot[0]);
    g_ctx.target_rot[1] = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.rotation.pitch", g_ctx.target_rot[1]);

    // Load target FOV
    g_ctx.target_fov = config->Cfg_GetFloat(g_ctx.configHandle, "settings.target_camera.fov", g_ctx.target_fov);

    // Load animation speed
    g_ctx.animation_speed = config->Cfg_GetFloat(g_ctx.configHandle, "settings.animation.speed", g_ctx.animation_speed);

    // Load animation type
    char anim_type_buffer[32];
    config->Cfg_GetString(g_ctx.configHandle, "settings.animation.type", g_ctx.animation_type.c_str(), anim_type_buffer, sizeof(anim_type_buffer));
    g_ctx.animation_type = anim_type_buffer;
}

void AnimateCamera(float deltaTime) {
    if (!g_ctx.cameraAPI) return;

    // Determine start and end points based on animation direction
    float* start_pos_ptr, * end_pos_ptr;
    float* start_rot_ptr, * end_rot_ptr;
    float start_fov, end_fov;

    if (g_ctx.isPeeking) { // Animating TO target
        start_pos_ptr = g_ctx.original_pos;
        start_rot_ptr = g_ctx.original_rot;
        start_fov = g_ctx.original_fov;
        end_pos_ptr = g_ctx.target_pos;
        end_rot_ptr = g_ctx.target_rot;
        end_fov = g_ctx.target_fov;
    } else { // Animating back FROM target
        start_pos_ptr = g_ctx.target_pos;
        start_rot_ptr = g_ctx.target_rot;
        start_fov = g_ctx.target_fov;
        end_pos_ptr = g_ctx.original_pos;
        end_rot_ptr = g_ctx.original_rot;
        end_fov = g_ctx.original_fov;
    }

    // Update progress
    g_ctx.animation_progress += deltaTime * g_ctx.animation_speed;

    // Check if animation is finished
    if (g_ctx.animation_progress >= 1.0f) {
        g_ctx.animation_progress = 1.0f;
        g_ctx.isAnimating = false;

        // Snap to final position to ensure precision
        g_ctx.cameraAPI->Cam_SetInteriorSeatPos(end_pos_ptr[0], end_pos_ptr[1], end_pos_ptr[2]);
        g_ctx.cameraAPI->Cam_SetInteriorHeadRot(end_rot_ptr[0], end_rot_ptr[1]);
        g_ctx.cameraAPI->Cam_SetInteriorFov(end_fov);
        return;
    }

    float current_pos[3];
    float current_rot[2];
    float current_fov;

    if (g_ctx.animation_type == "live") {
        // --- Live Animation Logic using a Bezier Curve ---
        float p = g_ctx.animation_progress;

        // A single, consistent easing for the entire animation path
        auto easeInOutCubic = [](float t) { return t < 0.5 ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3) / 2; };
        float eased_p = easeInOutCubic(p);

        // --- POSITION: Calculated along a Quadratic Bezier Curve ---
        // This creates a natural arc instead of moving along straight axes.
        // P0 is the start point.
        // P2 is the end point.
        // P1 is the control point that "pulls" the curve into an arc.
        
        float p1[3]; // The control point
        // The horizontal position of the control point depends on the animation direction
        // to make the "lift" feel natural when returning to the seat.
        float horizontal_factor = g_ctx.isPeeking ? 0.2f : 0.8f;
        p1[0] = start_pos_ptr[0] + (end_pos_ptr[0] - start_pos_ptr[0]) * horizontal_factor;
        p1[2] = start_pos_ptr[2] + (end_pos_ptr[2] - start_pos_ptr[2]) * horizontal_factor;
        
        // Use the arc height fine-tuned by the user.
        p1[1] = std::fmax(start_pos_ptr[1], end_pos_ptr[1]) + 0.15f; 

        // Quadratic Bezier curve formula: B(t) = (1-t)^2*P0 + 2*(1-t)*t*P1 + t^2*P2
        auto bezier = [&](float t, float p0_coord, float p1_coord, float p2_coord) {
            float one_minus_t = 1.0f - t;
            return one_minus_t * one_minus_t * p0_coord + 2.0f * one_minus_t * t * p1_coord + t * t * p2_coord;
        };

        current_pos[0] = bezier(eased_p, start_pos_ptr[0], p1[0], end_pos_ptr[0]);
        current_pos[1] = bezier(eased_p, start_pos_ptr[1], p1[1], end_pos_ptr[1]);
        current_pos[2] = bezier(eased_p, start_pos_ptr[2], p1[2], end_pos_ptr[2]);


        // --- ROTATION ---
        // Yaw (left/right rotation) follows the main eased progress.
        current_rot[0] = start_rot_ptr[0] + (end_rot_ptr[0] - start_rot_ptr[0]) * eased_p;

        // Pitch (up/down rotation) for looking up at the end of the movement.
        auto easeInQuad = [](float t) { return t * t; };
        float lookup_progress = std::fmax(0.0f, (p - 0.7f) / 0.3f);
        float eased_lookup_progress = easeInQuad(lookup_progress);
        current_rot[1] = start_rot_ptr[1] + (end_rot_ptr[1] - start_rot_ptr[1]) * eased_lookup_progress;


        // --- FOV ---
        // Field of View animation remains linear for simplicity.
        current_fov = start_fov + (end_fov - start_fov) * p;

    } else {
        // --- Linear Interpolation (LERP) ---
        // Fallback to the original linear animation.
        current_pos[0] = start_pos_ptr[0] + (end_pos_ptr[0] - start_pos_ptr[0]) * g_ctx.animation_progress;
        current_pos[1] = start_pos_ptr[1] + (end_pos_ptr[1] - start_pos_ptr[1]) * g_ctx.animation_progress;
        current_pos[2] = start_pos_ptr[2] + (end_pos_ptr[2] - start_pos_ptr[2]) * g_ctx.animation_progress;

        current_rot[0] = start_rot_ptr[0] + (end_rot_ptr[0] - start_rot_ptr[0]) * g_ctx.animation_progress;
        current_rot[1] = start_rot_ptr[1] + (end_rot_ptr[1] - start_rot_ptr[1]) * g_ctx.animation_progress;

        current_fov = start_fov + (end_fov - start_fov) * g_ctx.animation_progress;
    }

    // Apply the calculated values to the camera
    g_ctx.cameraAPI->Cam_SetInteriorSeatPos(current_pos[0], current_pos[1], current_pos[2]);
    g_ctx.cameraAPI->Cam_SetInteriorHeadRot(current_rot[0], current_rot[1]);
    g_ctx.cameraAPI->Cam_SetInteriorFov(current_fov);
}
// Implement these functions if your plugin needs to react to specific events.
// Remember to also uncomment their prototypes in SPF_FrontalBlindspotViewer.hpp and register them
// in OnActivated or OnRegisterUI as appropriate.

void OnSettingChanged(SPF_Config_Handle* config_handle, const char* keyPath) {
    // A setting has changed. Check the keyPath and use the config_handle
    // with the Config API to get the new value.
    LoadSettings(); // Reload all settings

    // If we are currently peeking, and a camera setting changes,
    // apply it immediately for live preview.
    if (g_ctx.isPeeking && g_ctx.cameraAPI) {
        if (strstr(keyPath, "target_camera.position") ||
            strstr(keyPath, "target_camera.rotation") ||
            strstr(keyPath, "target_camera.fov")) {
            g_ctx.cameraAPI->Cam_SetInteriorSeatPos(g_ctx.target_pos[0], g_ctx.target_pos[1], g_ctx.target_pos[2]);
            g_ctx.cameraAPI->Cam_SetInteriorHeadRot(g_ctx.target_rot[0], g_ctx.target_rot[1]);
            g_ctx.cameraAPI->Cam_SetInteriorFov(g_ctx.target_fov);
        }
    }
}

void OnKeybindAction() {
    // This function name should match what you passed to SPF_KeyBinds_API.Register.
    if (g_ctx.isAnimating || !g_ctx.cameraAPI) {
        return; // Ignore keybind if already animating or camera API is not available
    }

    if (!g_ctx.isPeeking) {
        // Save current camera state
        g_ctx.cameraAPI->Cam_GetInteriorSeatPos(&g_ctx.original_pos[0], &g_ctx.original_pos[1], &g_ctx.original_pos[2]);
        g_ctx.cameraAPI->Cam_GetInteriorHeadRot(&g_ctx.original_rot[0], &g_ctx.original_rot[1]);
        g_ctx.cameraAPI->Cam_GetInteriorFov(&g_ctx.original_fov);

        // Start animating to peek position
        g_ctx.isPeeking = true;
    } else {
        // Start animating back to original position
        g_ctx.isPeeking = false;
    }

    g_ctx.isAnimating = true;
    g_ctx.animation_progress = 0.0f; // Reset animation progress
}

// =================================================================================================
// 6. Plugin Exports
// =================================================================================================
// These are the two mandatory, C-style functions that the plugin DLL must export.
// The `extern "C"` block is essential to prevent C++ name mangling, ensuring the framework
// can find them by name.

extern "C" {

/**
 * @brief Exports the manifest API to the framework.
 * @details This function is mandatory for the framework to properly identify and configure the plugin.
 */
SPF_PLUGIN_EXPORT bool SPF_GetManifestAPI(SPF_Manifest_API* out_api) {
    if (out_api) {
        out_api->BuildManifest = BuildManifest;
        return true;
    }
    return false;
}

/**
 * @brief Exports the plugin's main lifecycle and callback functions to the framework.
 * @details This function is mandatory for the framework to interact with the plugin's lifecycle.
 */
SPF_PLUGIN_EXPORT bool SPF_GetPlugin(SPF_Plugin_Exports* exports) {
    if (exports) {
        // Connect the internal C++ functions to the C-style export struct.
        exports->OnLoad = OnLoad;
        exports->OnActivated = OnActivated;
        exports->OnUnload = OnUnload;
        exports->OnUpdate = OnUpdate;

        exports->OnSettingChanged = OnSettingChanged;
        return true;
    }
    return false;
}

} // extern "C"

} // namespace SPF_FrontalBlindspotViewer
