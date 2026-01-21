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
 * @details This MUST match the name used in `GetContext` calls for various APIs
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

void GetManifestData(SPF_ManifestData_C& out_manifest) {
    // This function defines all the metadata for your plugin. The framework calls this
    // function *before* loading your plugin DLL to understand what it is.

    // --- 2.1. Plugin Information (SPF_InfoData_C) ---
    // This section provides the basic identity of your plugin.
    {
        auto& info = out_manifest.info;
        strncpy_s(info.name, PLUGIN_NAME, sizeof(info.name));
        strncpy_s(info.version, "1.0.0", sizeof(info.version));
        strncpy_s(info.min_framework_version, "1.0.7", sizeof(info.min_framework_version));
        strncpy_s(info.author, "Track'n'Truck Devs", sizeof(info.author));
        strncpy_s(info.descriptionKey, "plugin.description", sizeof(info.descriptionKey));

        strncpy_s(info.email, "mailto:spf.framework@gmail.com", sizeof(info.email));
        strncpy_s(info.youtubeUrl, "https://www.youtube.com/@TrackAndTruck", sizeof(info.youtubeUrl));
        strncpy_s(info.patreonUrl, "https://www.patreon.com/TrackAndTruckDevs", sizeof(info.patreonUrl));
    }


    {
        auto& policy = out_manifest.configPolicy;

        // `allowUserConfig`: Set to `true` if you want a `settings.json` file to be created
        // for your plugin, allowing users (or the framework UI) to override default settings.
        policy.allowUserConfig = true;

        // `userConfigurableSystemsCount`: The number of framework systems (e.g., "settings", "logging", "localization", "ui")
        // that should have a configuration section generated in the settings UI for your plugin.
        // IMPORTANT: Always initialize this to 0 if you are not listing any systems to avoid errors.
        policy.userConfigurableSystemsCount = 2; //To enable configurable systems, uncomment the block below and set the count accordingly
        // strncpy_s(policy.userConfigurableSystems[0], "logging", sizeof(policy.userConfigurableSystems[0]));
        strncpy_s(policy.userConfigurableSystems[0], "settings", sizeof(policy.userConfigurableSystems[0]));
        strncpy_s(policy.userConfigurableSystems[1], "localization", sizeof(policy.userConfigurableSystems[1]));
        // strncpy_s(policy.userConfigurableSystems[1], "ui", sizeof(policy.userConfigurableSystems[1]));

        // `requiredHooksCount`: List any game hooks your plugin absolutely requires to function.
        // The framework will ensure these hooks are enabled whenever your plugin is active,
        // regardless of user settings.
        // IMPORTANT: Always initialize this to 0 if you are not listing any hooks to avoid errors.
        policy.requiredHooksCount = 0; // To enable required hooks, uncomment the lines below and set the count accordingly.
        // strncpy_s(policy.requiredHooks[0], "GameConsole", sizeof(policy.requiredHooks[0])); // Example: Requires GameConsole hook
    }

    // --- 2.3. Custom Settings (settingsJson) ---
    // A JSON string literal that defines the default values for your plugin's custom settings.
    // If `policy.allowUserConfig` is true, the framework creates a `settings.json` file.
    // The JSON object you provide here will be inserted under a top-level key named "settings".
    // Example: Define some default custom settings.
    // To provide user-friendly names and descriptions, see `customSettingsMetadata` at the end.
    out_manifest.settingsJson = R"json(
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
    )json";

    // --- 2.4. Default Settings for Framework Systems ---
    // Here you can provide default configurations for various framework systems.

    // --- Logging ---
    // Requires: SPF_Logger_API.h
    {
        auto& logging = out_manifest.logging;
        // `level`: Default minimum log level for this plugin (e.g., "trace", "debug", "info", "warn", "error", "critical").
        strncpy_s(logging.level, "info", sizeof(logging.level));
        // `sinks.file`: If true, logs from this plugin will be written to a dedicated file
        // (e.g., `SPF_FrontalBlindspotViewer/logs/SPF_FrontalBlindspotViewer.log`) in addition to the main framework log.
        logging.sinks.file = false;
    }

    // --- Localization ---
    // Requires: SPF_Localization_API.h
    // Uncomment if your plugin uses localized strings.
    {
        auto& localization = out_manifest.localization;
        // `language`: Default language code (e.g., "en", "de", "uk").
        strncpy_s(localization.language, "en", sizeof(localization.language));
    }

    // --- Keybinds ---
    // Requires: SPF_KeyBinds_API.h
    // Uncomment and configure if your plugin needs custom keybinds.
    auto& keybinds = out_manifest.keybinds;
    keybinds.actionCount = 1; // Number of distinct actions defined by your plugin.
    {
        // --- Action 0: A sample keybind to toggle a window ---
        auto& action = keybinds.actions[0];
        // `groupName`: Logical grouping for actions, used to avoid name collisions.
        // Best practice: "{PluginName}.{Feature}".
        strncpy_s(action.groupName, "SPF_FrontalBlindspotViewer", sizeof(action.groupName));
        // `actionName`: Specific action (e.g., "toggle", "activate").
        strncpy_s(action.actionName, "toggle", sizeof(action.actionName));

        // Define one or more default key combinations for this action.
        action.definitionCount = 1;
        {
            // --- Definition 0 ---
            auto& def = action.definitions[0];
            // `type`: "keyboard" or "gamepad".
            strncpy_s(def.type, "keyboard", sizeof(def.type));
            // `key`: Key name (see VirtualKeyMapping.cpp or GamepadButtonMapping.cpp).
            strncpy_s(def.key, "KEY_F10", sizeof(def.key));
            // `pressType`: "short" (tap) or "long" (hold).
            strncpy_s(def.pressType, "short", sizeof(def.pressType));
            // `pressThresholdMs`: For "long" press, time in ms to hold.
            def.pressThresholdMs = 0;
            // `consume`: When to consume input: "never", "on_ui_focus", "always".
            strncpy_s(def.consume, "always", sizeof(def.consume));
            // `behavior`: How action behaves. Valid values: "toggle" (on/off), "hold" (while pressed).
            strncpy_s(def.behavior, "toggle", sizeof(def.behavior));
        }
    }

    // --- Custom Settings Metadata ---
    // Provide titles and descriptions for the settings defined in `settingsJson`.
    
    out_manifest.customSettingsMetadataCount = 12;
    {
        //--- Metadata for target_camera.position.x ---
        auto& meta_pos_x = out_manifest.customSettingsMetadata[0];
        strncpy_s(meta_pos_x.keyPath, "target_camera.position.x", sizeof(meta_pos_x.keyPath));
        strncpy_s(meta_pos_x.titleKey, "settings.target_camera.position.x.title", sizeof(meta_pos_x.titleKey));
        strncpy_s(meta_pos_x.keyPath, "target_camera.position.x", sizeof(meta_pos_x.keyPath));
        strncpy_s(meta_pos_x.descriptionKey, "settings.target_camera.position.x.desc", sizeof(meta_pos_x.descriptionKey));
        strncpy_s(meta_pos_x.widget, "slider", sizeof(meta_pos_x.widget));
        meta_pos_x.widget_params.slider.min_val = -5.0f;
        meta_pos_x.widget_params.slider.max_val = 5.0f;
        strncpy_s(meta_pos_x.widget_params.slider.format, "%.3f", sizeof(meta_pos_x.widget_params.slider.format));

        //--- Metadata for target_camera.position.y ---
        auto& meta_pos_y = out_manifest.customSettingsMetadata[1];
        strncpy_s(meta_pos_y.keyPath, "target_camera.position.y", sizeof(meta_pos_y.keyPath));
        strncpy_s(meta_pos_y.titleKey, "settings.target_camera.position.y.title", sizeof(meta_pos_y.titleKey));
        strncpy_s(meta_pos_y.descriptionKey, "settings.target_camera.position.y.desc", sizeof(meta_pos_y.descriptionKey));
        strncpy_s(meta_pos_y.widget, "slider", sizeof(meta_pos_y.widget));
        meta_pos_y.widget_params.slider.min_val = -5.0f;
        meta_pos_y.widget_params.slider.max_val = 5.0f;
        strncpy_s(meta_pos_y.widget_params.slider.format, "%.3f", sizeof(meta_pos_y.widget_params.slider.format));

        //--- Metadata for target_camera.position.z ---
        auto& meta_pos_z = out_manifest.customSettingsMetadata[2];
        strncpy_s(meta_pos_z.keyPath, "target_camera.position.z", sizeof(meta_pos_z.keyPath));
        strncpy_s(meta_pos_z.titleKey, "settings.target_camera.position.z.title", sizeof(meta_pos_z.titleKey));
        strncpy_s(meta_pos_z.descriptionKey, "settings.target_camera.position.z.desc", sizeof(meta_pos_z.descriptionKey));
        strncpy_s(meta_pos_z.widget, "slider", sizeof(meta_pos_z.widget));
        meta_pos_z.widget_params.slider.min_val = -5.0f;
        meta_pos_z.widget_params.slider.max_val = 5.0f;
        strncpy_s(meta_pos_z.widget_params.slider.format, "%.3f", sizeof(meta_pos_z.widget_params.slider.format));

        //--- Metadata for target_camera.rotation.yaw ---
        auto& meta_rot_yaw = out_manifest.customSettingsMetadata[3];
        strncpy_s(meta_rot_yaw.keyPath, "target_camera.rotation.yaw", sizeof(meta_rot_yaw.keyPath));
        strncpy_s(meta_rot_yaw.titleKey, "settings.target_camera.rotation.yaw.title", sizeof(meta_rot_yaw.titleKey));
        strncpy_s(meta_rot_yaw.descriptionKey, "settings.target_camera.rotation.yaw.desc", sizeof(meta_rot_yaw.descriptionKey));
        strncpy_s(meta_rot_yaw.widget, "slider", sizeof(meta_rot_yaw.widget));
        meta_rot_yaw.widget_params.slider.min_val = -3.1415f;
        meta_rot_yaw.widget_params.slider.max_val = 3.1415f;
        strncpy_s(meta_rot_yaw.widget_params.slider.format, "%.3f", sizeof(meta_rot_yaw.widget_params.slider.format));

        //--- Metadata for target_camera.rotation.pitch ---
        auto& meta_rot_pitch = out_manifest.customSettingsMetadata[4];
        strncpy_s(meta_rot_pitch.keyPath, "target_camera.rotation.pitch", sizeof(meta_rot_pitch.keyPath));
        strncpy_s(meta_rot_pitch.titleKey, "settings.target_camera.rotation.pitch.title", sizeof(meta_rot_pitch.titleKey));
        strncpy_s(meta_rot_pitch.descriptionKey, "settings.target_camera.rotation.pitch.desc", sizeof(meta_rot_pitch.descriptionKey));
        strncpy_s(meta_rot_pitch.widget, "slider", sizeof(meta_rot_pitch.widget));
        meta_rot_pitch.widget_params.slider.min_val = -1.571f;
        meta_rot_pitch.widget_params.slider.max_val = 1.571f;
        strncpy_s(meta_rot_pitch.widget_params.slider.format, "%.3f", sizeof(meta_rot_pitch.widget_params.slider.format));

        //--- Metadata for target_camera.fov ---
        auto& meta_fov = out_manifest.customSettingsMetadata[5];
        strncpy_s(meta_fov.keyPath, "target_camera.fov", sizeof(meta_fov.keyPath));
        strncpy_s(meta_fov.titleKey, "settings.target_camera.fov.title", sizeof(meta_fov.titleKey));
        strncpy_s(meta_fov.descriptionKey, "settings.target_camera.fov.desc", sizeof(meta_fov.descriptionKey));
        strncpy_s(meta_fov.widget, "slider", sizeof(meta_fov.widget));
        meta_fov.widget_params.slider.min_val = 30.0f;
        meta_fov.widget_params.slider.max_val = 120.0f;
        strncpy_s(meta_fov.widget_params.slider.format, "%.1f", sizeof(meta_fov.widget_params.slider.format));

        //--- Metadata for animation.speed ---
        auto& meta_speed = out_manifest.customSettingsMetadata[6];
        strncpy_s(meta_speed.keyPath, "animation.speed", sizeof(meta_speed.keyPath));
        strncpy_s(meta_speed.titleKey, "settings.animation.speed.title", sizeof(meta_speed.titleKey));
        strncpy_s(meta_speed.descriptionKey, "settings.animation.speed.desc", sizeof(meta_speed.descriptionKey));
        strncpy_s(meta_speed.widget, "slider", sizeof(meta_speed.widget));
        meta_speed.widget_params.slider.min_val = 0.1f;
        meta_speed.widget_params.slider.max_val = 3.0f;
        strncpy_s(meta_speed.widget_params.slider.format, "%.1f", sizeof(meta_speed.widget_params.slider.format));

        //--- Metadata for animation.type ---
        auto& meta_type = out_manifest.customSettingsMetadata[7];
        strncpy_s(meta_type.keyPath, "animation.type", sizeof(meta_type.keyPath));
        strncpy_s(meta_type.titleKey, "settings.animation.type.title", sizeof(meta_type.titleKey));
        strncpy_s(meta_type.descriptionKey, "settings.animation.type.desc", sizeof(meta_type.descriptionKey));
        strncpy_s(meta_type.widget, "combo", sizeof(meta_type.widget));
        const char* animation_type_options = R"json([
            { "value": "linear", "labelKey": "settings.animation_type_options.Linear" },
            { "value": "live", "labelKey": "settings.animation_type_options.Live" }
        ])json";
        strncpy_s(meta_type.widget_params.choice.options_json, animation_type_options, sizeof(meta_type.widget_params.choice.options_json));

        //--- Metadata for the "target_camera" group label ---
        auto& meta_group_cam = out_manifest.customSettingsMetadata[8];
        strncpy_s(meta_group_cam.keyPath, "target_camera", sizeof(meta_group_cam.keyPath));
        strncpy_s(meta_group_cam.titleKey, "settings.groups.target_camera.title", sizeof(meta_group_cam.titleKey));
        strncpy_s(meta_group_cam.descriptionKey, "settings.groups.target_camera.desc", sizeof(meta_group_cam.descriptionKey));

        //--- Metadata for the "animation" group label ---
        auto& meta_group_anim = out_manifest.customSettingsMetadata[9];
        strncpy_s(meta_group_anim.keyPath, "animation", sizeof(meta_group_anim.keyPath));
        strncpy_s(meta_group_anim.titleKey, "settings.groups.animation.title", sizeof(meta_group_anim.titleKey));
        strncpy_s(meta_group_anim.descriptionKey, "settings.groups.animation.desc", sizeof(meta_group_anim.descriptionKey));

        //--- Metadata for the "position" group label ---
        auto& meta_group_pos = out_manifest.customSettingsMetadata[10];
        strncpy_s(meta_group_pos.keyPath, "target_camera.position", sizeof(meta_group_pos.keyPath));
        strncpy_s(meta_group_pos.titleKey, "settings.groups.target_camera.position.title", sizeof(meta_group_pos.titleKey));
        strncpy_s(meta_group_pos.descriptionKey, "settings.groups.target_camera.position.desc", sizeof(meta_group_pos.descriptionKey));

        //--- Metadata for the "rotation" group label ---
        auto& meta_group_rot = out_manifest.customSettingsMetadata[11];
        strncpy_s(meta_group_rot.keyPath, "target_camera.rotation", sizeof(meta_group_rot.keyPath));
        strncpy_s(meta_group_rot.titleKey, "settings.groups.target_camera.rotation.title", sizeof(meta_group_rot.titleKey));
        strncpy_s(meta_group_rot.descriptionKey, "settings.groups.target_camera.rotation.desc", sizeof(meta_group_rot.descriptionKey));
    }

    // --- Keybinds Metadata ---
    // Provide titles and descriptions for the actions defined in `keybinds`.

    out_manifest.keybindsMetadataCount = 1; // To enable keybinds metadata, uncomment the lines below and set the count accordingly.
    {
        auto& meta = out_manifest.keybindsMetadata[0];
        strncpy_s(meta.groupName, "SPF_FrontalBlindspotViewer", sizeof(meta.groupName)); // Must match the action's groupName
        strncpy_s(meta.actionName, "toggle", sizeof(meta.actionName));           // Must match the action's actionName
        strncpy_s(meta.titleKey, "keybinds.toggle.title", sizeof(meta.titleKey));
        strncpy_s(meta.descriptionKey, "keybinds.toggle.desc", sizeof(meta.descriptionKey));
    }

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
        g_ctx.loggerHandle = g_ctx.loadAPI->logger->GetLogger(PLUGIN_NAME);
        g_ctx.formattingAPI = g_ctx.loadAPI->formatting;

        if (g_ctx.loggerHandle && g_ctx.formattingAPI) {
            char log_buffer[256];
            g_ctx.formattingAPI->Format(log_buffer, sizeof(log_buffer), "%s has been loaded!", PLUGIN_NAME);
            g_ctx.loadAPI->logger->Log(g_ctx.loggerHandle, SPF_LOG_INFO, log_buffer);
        }
    }

    // --- Optional API Initialization (Uncomment if needed) ---
    // Remember to also uncomment the relevant #include directives in SPF_FrontalBlindspotViewer.hpp
    // and add corresponding members to the PluginContext struct.

    // Config API
    // Requires: SPF_Config_API.h
    if (g_ctx.loadAPI && g_ctx.loadAPI->config) {
        g_ctx.configHandle = g_ctx.loadAPI->config->GetContext(PLUGIN_NAME);
    }
    
    // Localization API
    // Requires: SPF_Localization_API.h
    if (g_ctx.loadAPI && g_ctx.loadAPI->localization) {
        g_ctx.localizationHandle = g_ctx.loadAPI->localization->GetContext(PLUGIN_NAME);
    }
}

void OnActivated(const SPF_Core_API* core_api) {
    g_ctx.coreAPI = core_api;

    if (g_ctx.loggerHandle && g_ctx.formattingAPI) {
        char log_buffer[256];
        g_ctx.formattingAPI->Format(log_buffer, sizeof(log_buffer), "%s has been activated!", PLUGIN_NAME);
        g_ctx.loadAPI->logger->Log(g_ctx.loggerHandle, SPF_LOG_INFO, log_buffer);
    }

    // --- Optional API Initialization & Callback Registration (Uncomment if needed) ---
    // Remember to also uncomment the relevant #include directives in SPF_FrontalBlindspotViewer.hpp
    // and add corresponding members to the PluginContext struct.

    // Keybinds API
    // Requires: SPF_KeyBinds_API.h
    if (g_ctx.coreAPI && g_ctx.coreAPI->keybinds) {
        g_ctx.keybindsHandle = g_ctx.coreAPI->keybinds->GetContext(PLUGIN_NAME);
        if (g_ctx.keybindsHandle) {
            // Register the callback for our "toggle" action.
            g_ctx.coreAPI->keybinds->Register(g_ctx.keybindsHandle, "SPF_FrontalBlindspotViewer.toggle", OnKeybindAction);
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
        g_ctx.formattingAPI->Format(log_buffer, sizeof(log_buffer), "%s is being unloaded.", PLUGIN_NAME);
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
    g_ctx.target_pos[0] = config->GetFloat(g_ctx.configHandle, "settings.target_camera.position.x", g_ctx.target_pos[0]);
    g_ctx.target_pos[1] = config->GetFloat(g_ctx.configHandle, "settings.target_camera.position.y", g_ctx.target_pos[1]);
    g_ctx.target_pos[2] = config->GetFloat(g_ctx.configHandle, "settings.target_camera.position.z", g_ctx.target_pos[2]);

    // Load target camera rotation
    g_ctx.target_rot[0] = config->GetFloat(g_ctx.configHandle, "settings.target_camera.rotation.yaw", g_ctx.target_rot[0]);
    g_ctx.target_rot[1] = config->GetFloat(g_ctx.configHandle, "settings.target_camera.rotation.pitch", g_ctx.target_rot[1]);

    // Load target FOV
    g_ctx.target_fov = config->GetFloat(g_ctx.configHandle, "settings.target_camera.fov", g_ctx.target_fov);

    // Load animation speed
    g_ctx.animation_speed = config->GetFloat(g_ctx.configHandle, "settings.animation.speed", g_ctx.animation_speed);

    // Load animation type
    char anim_type_buffer[32];
    config->GetString(g_ctx.configHandle, "settings.animation.type", g_ctx.animation_type.c_str(), anim_type_buffer, sizeof(anim_type_buffer));
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
        g_ctx.cameraAPI->SetInteriorSeatPos(end_pos_ptr[0], end_pos_ptr[1], end_pos_ptr[2]);
        g_ctx.cameraAPI->SetInteriorHeadRot(end_rot_ptr[0], end_rot_ptr[1]);
        g_ctx.cameraAPI->SetInteriorFov(end_fov);
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
    g_ctx.cameraAPI->SetInteriorSeatPos(current_pos[0], current_pos[1], current_pos[2]);
    g_ctx.cameraAPI->SetInteriorHeadRot(current_rot[0], current_rot[1]);
    g_ctx.cameraAPI->SetInteriorFov(current_fov);
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
            g_ctx.cameraAPI->SetInteriorSeatPos(g_ctx.target_pos[0], g_ctx.target_pos[1], g_ctx.target_pos[2]);
            g_ctx.cameraAPI->SetInteriorHeadRot(g_ctx.target_rot[0], g_ctx.target_rot[1]);
            g_ctx.cameraAPI->SetInteriorFov(g_ctx.target_fov);
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
        g_ctx.cameraAPI->GetInteriorSeatPos(&g_ctx.original_pos[0], &g_ctx.original_pos[1], &g_ctx.original_pos[2]);
        g_ctx.cameraAPI->GetInteriorHeadRot(&g_ctx.original_rot[0], &g_ctx.original_rot[1]);
        g_ctx.cameraAPI->GetInteriorFov(&g_ctx.original_fov);

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
        out_api->GetManifestData = GetManifestData;
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
