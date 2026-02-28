<h1 align="center">SPF Frontal Blindspot Viewer Plugin</h1>

<p align="center">
    <a href="https://github.com/TrackAndTruckDevs/SPF_FrontalBlindspotViewer/releases/latest/" target="_blank" title="SPF Frontal Blindspot Viewer Plugin"><img alt="GitHub Release" src="https://img.shields.io/github/v/release/TrackAndTruckDevs/SPF_FrontalBlindspotViewer"></a>
    <a href="/LICENSE" title="SPF Red Light Camera Plugin license"><img alt="GitHub License" src="https://img.shields.io/github/license/TrackAndTruckDevs/SPF_FrontalBlindspotViewer"></a>
</p>

<p align="center">
    <a href="https://www.patreon.com/TrackAndTruckDevs" target="_blank" title="Support us on Patreon"><img alt="Patreon" src="https://img.shields.io/badge/patreon-Becoming a patron-3404021712?style=flat&logo=patreon"></a>
    <a href="https://github.com/TrackAndTruckDevs/SPF_FrontalBlindspotViewer/stargazers" title="Liked it? Starred"><img src="https://img.shields.io/github/stars/TrackAndTruckDevs/SPF_FrontalBlindspotViewer?style=flat&logo=github" alt="Stars" /></a>
    <a href="https://youtube.com/@TrackAndTruck" target="_blank" title="Subscribe to our channel"><img alt="Youtube" src="https://img.shields.io/badge/youtube-subscribe-orange?logo=youtube&style=flat"></a>
</p>

---

A plugin for American Truck Simulator and Euro Truck Simulator 2 that provides a smooth, animated camera movement to let you easily see traffic lights when they are obstructed by your truck's A-pillar.

### See It In Action

[Watch a demonstration of the plugin on YouTube](https://youtu.be/6zO-zVAVivo)

## Features

*   Smooth, configurable camera animation to peek over the dashboard.
*   Two distinct animation styles: a realistic "Live" mode that mimics human movement, and a fast "Linear" mode.
*   Interactive, real-time configuration: adjust the camera's final position while the view is active to perfectly match any truck.
*   Fully customizable keybinds through the SPF Framework menu, including "Toggle" and "Hold" modes.
*   Adjustable animation speed to fine-tune the feel of the movement.

## Support the Project

If you enjoy this plugin and want to support the development of future projects, consider supporting us on Patreon.

► **[Support on Patreon](https://www.patreon.com/TrackAndTruckDevs)**

## How to Build 🛠️

This is a standard CMake project. To build it from source:

1.  Clone this repository.
2.  Ensure you have **CMake** and a compatible C++ compiler with the **MSVC toolchain** (e.g., Visual Studio) installed.
3.  Create a `build` directory inside the project folder.
4.  Run CMake from the `build` directory to generate project files (e.g., `cmake ..`).
5.  Build the project using your chosen build tool (e.g., run `cmake --build .` or open the generated `.sln` file in Visual Studio and build from there).

## Installation

### Prerequisites

You must have the **SPF Framework** installed for this plugin to work.
*   **[Download the SPF-Framework here](https://github.com/TrackAndTruckDevs/SPF-Framework)**

### Steps

1.  If you haven't already, download and install the SPF Framework according to its instructions.
2.  Download the latest release of this plugin from the **[Releases](https://github.com/TrackAndTruckDevs/SPF_FrontalBlindspotViewer/releases)** page.
3.  You will have a folder named `SPF_FrontalBlindspotViewer`. Copy this entire folder into your game's `\bin\win_x64\plugins\spfPlugins\` directory.

The final folder structure should look like this:

```
...your_game_root\bin\win_x64\plugins\spfPlugins\
└───SPF_FrontalBlindspotViewer
    │   SPF_FrontalBlindspotViewer.dll
    │
    └───localization
            en.json
```

## How to Use

1.  Start the game.
2.  Press the `DELETE` key to open the main SPF Framework window.
3.  In the plugin list, find **SPF_FrontalBlindspotViewer** and enable it.
4.  The feature is activated by pressing the `F10` key by default. You can change this in the "Key Binds" tab.
5.  To adjust the camera, go to the "Plugin Settings" tab, select **SPF_FrontalBlindspotViewer**, and use the sliders. You can configure the final camera position, rotation, FOV, animation speed, and animation type ("Linear" or "Live"). The changes are applied instantly in a live preview when the peek view is active.
