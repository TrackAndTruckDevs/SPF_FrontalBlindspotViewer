/**
 * @file SPF_FrontalBlindspotViewer.hpp
 * @brief Internal header for the SPF_FrontalBlindspotViewer.
 */
#pragma once

// =================================================================================================
// 1. SPF API Includes - Core & Essential
// =================================================================================================
#include <SPF_Plugin.h>
#include <SPF_Manifest_API.h>
#include <SPF_Logger_API.h>
#include <SPF_Formatting_API.h>

#include <SPF_Config_API.h>         // For SPF_Config_Handle
#include <SPF_Localization_API.h>   // For SPF_Localization_Handle
#include <SPF_KeyBinds_API.h>       // For SPF_KeyBinds_Handle
#include <SPF_Camera_API.h>         // For SPF_Camera_API

// =================================================================================================
// 2. Standard Library Includes
// =================================================================================================
#include <cstdint>  // For fixed-width integer types like int32_t, useful for consistent data sizes.
#include <chrono>   // For std::chrono
#include <string>   // For std::string

namespace SPF_FrontalBlindspotViewer {

// =================================================================================================
// 3. Core Plugin Architecture
// =================================================================================================

// --- Plugin Context ---

/**
 * @brief Encapsulates all global state for the plugin in a single object.
 *
 * @details This struct follows the "Context Object" design pattern. All plugin-wide state
 * (API pointers, cached handles, settings, runtime flags, etc.) is consolidated into this
 * single `PluginContext` object. A single global instance of this struct (`g_ctx`) is then
 * used throughout the plugin.
 */
struct PluginContext {
  // --- Primary API Pointers (Essential) ---
  // These are the main gateways to the framework's functionality, provided during
  // the plugin's lifecycle.
  const SPF_Load_API* loadAPI = nullptr;
  const SPF_Core_API* coreAPI = nullptr;

  // --- Cached Handles (Essential) ---
  // These handles are obtained from the loadAPI and are used frequently.
  SPF_Logger_Handle* loggerHandle = nullptr;
  const SPF_Formatting_API* formattingAPI = nullptr;

  // --- Cached Handles & Pointers (Optional - Uncomment if needed) ---
  // Uncomment these members if your plugin uses the corresponding API.
  // Remember to also uncomment the relevant #include directives in this .hpp file.

  SPF_Config_Handle* configHandle = nullptr;         // Requires: SPF_Config_API.h
  SPF_Localization_Handle* localizationHandle = nullptr; // Requires: SPF_Localization_API.h
  SPF_KeyBinds_Handle* keybindsHandle = nullptr;     // Requires: SPF_KeyBinds_API.h
  const SPF_Camera_API* cameraAPI = nullptr;               // Requires: SPF_Camera_API.h

  // --- Plugin State Variables (Optional - Uncomment/Add if needed) ---
  // Add any plugin-specific state variables here.
  bool isPeeking = false;
  bool isAnimating = false;
  float animation_progress = 0.0f;

  // Settings cache
  float animation_speed = 0.0f;
  std::string animation_type = "live";
  float target_pos[3] = { 0.0f, 0.0f, 0.0f };
  float target_rot[2] = { 0.0f, 0.0f }; // yaw, pitch
  float target_fov = 0.0f;

  // Original camera state saved before peeking
  float original_pos[3] = { 0.0f, 0.0f, 0.0f };
  float original_rot[2] = { 0.0f, 0.0f }; // yaw, pitch
  float original_fov = 0.0f;
  std::chrono::high_resolution_clock::time_point lastFrameTime; // For deltaTime calculation
};

/**
 * @brief The single global instance of the plugin's context.
 * @details This is defined once in `SPF_FrontalBlindspotViewer.cpp` and declared `extern` here, making it
 * accessible throughout all of the plugin's source files.
 */
extern PluginContext g_ctx;

// =================================================================================================
// 4. Function Prototypes - Core Lifecycle
// =================================================================================================
// These are the mandatory functions that the framework expects the plugin to implement.

/**
 * @brief Constructs the plugin's manifest using the provided Builder API.
 * @details This function is called by the framework *before* the plugin is loaded to learn
 * about its name, version, and other basic properties.
 * @param h An opaque handle to the manifest builder object.
 * @param api A table of functions provided by the framework to populate the manifest.
 */
void BuildManifest(SPF_Manifest_Builder_Handle* h, const SPF_Manifest_Builder_API* api);

/**
 * @brief Called first when the plugin DLL is loaded into memory.
 * @details This is the earliest point for initialization. Only the `load_api` services
 * (logger, config, localization, formatting) are available here.
 * @param load_api A pointer to the Load API.
 */
void OnLoad(const SPF_Load_API* load_api);

/**
 * @brief Called when the plugin is activated by the framework.
 * @details This function is called after `OnLoad` and after the framework has processed the
 * plugin's manifest. At this point, the game is running and all framework services are
 * available via the `core_api`.
 * @param core_api A pointer to the Core API, which contains pointers to all other APIs.
 */
void OnActivated(const SPF_Core_API* core_api);

/**
 * @brief Called every frame while the plugin is active.
 * @details This function is tied to the rendering loop. Avoid doing heavy or blocking work
 * here as it can impact game performance.
 */
void OnUpdate();

/**
 * @brief Called last, just before the plugin is unloaded from memory.
 * @details Use this function to perform all necessary cleanup.
 */
void OnUnload();

/**
* @brief Called when a setting is changed externally (e.g., via the main settings UI or by another plugin).
* @details Uncomment this if your plugin defines custom settings in its manifest and needs to react
*          to changes made by the user or other parts of the framework.
* @param h The configuration context handle for this plugin.
* @param keyPath The full path to the setting that changed (e.g., "settings.some_bool").
 */
void OnSettingChanged(SPF_Config_Handle *h, const char *keyPath);

/**
 * @brief Callback executed when a keybind action is triggered by the user.
 * @details Uncomment this if your plugin defines keybinds in its manifest and needs to react
 *          to user input. You would typically register this callback in `OnActivated`.
 *          Requires: SPF_KeyBinds_API.h (for registration, but callback itself is void(void))
 */
void OnKeybindAction();

// =================================================================================================
// 4.2. Function Prototypes - Optional Helper Functions (Commented Out)
// =================================================================================================
// Add prototypes for any internal helper functions your plugin might need.
void LoadSettings();
void AnimateCamera(float deltaTime);

}  // namespace SPF_FrontalBlindspotViewer
