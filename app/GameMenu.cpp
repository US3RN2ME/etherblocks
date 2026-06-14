#include "GameMenu.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace etherblocks::app {
   namespace {
      [[nodiscard]] int resolutionPresetIndex(glm::ivec2 size, const std::vector<glm::ivec2>& resolutions) noexcept {
         if (resolutions.empty()) {
            return 0;
         }
         auto best = 0;
         auto bestDistance = std::abs(size.x - resolutions[0].x) + std::abs(size.y - resolutions[0].y);
         for (auto i = 1; i < static_cast<int>(resolutions.size()); ++i) {
            const auto distance = std::abs(size.x - resolutions[static_cast<std::size_t>(i)].x) +
                                  std::abs(size.y - resolutions[static_cast<std::size_t>(i)].y);
            if (distance < bestDistance) {
               best = i;
               bestDistance = distance;
            }
         }
         return best;
      }

      [[nodiscard]] std::string resolutionLabel(glm::ivec2 size) {
         return std::to_string(size.x) + "X" + std::to_string(size.y);
      }

      [[nodiscard]] std::string boolLabel(bool value) {
         return value ? "ON" : "OFF";
      }

      [[nodiscard]] int colorChannel(float value) {
         return static_cast<int>(std::clamp(value, 0.0f, 1.0f) * 100.0f);
      }

      void setColorChannel(float& channel, float delta) {
         channel = std::clamp(channel + delta * 0.01f, 0.0f, 1.0f);
      }
   } // namespace

   MenuResult GameMenu::draw(const engine::graphics::Renderer& renderer, const system::Input& input, glm::ivec2 screenSize,
                             AppSettings& settings, const std::vector<WorldInfo>& worlds, int activeWorldIndex,
                             const std::vector<glm::ivec2>& resolutions) {
      const auto size = glm::vec2{screenSize};
      uiLayer_.begin(screenSize, input);
      uiLayer_.panel({0.0f, 0.0f}, size, {0.02f, 0.04f, 0.06f, 0.58f});

      const auto panelWidth = std::min(720.0f, size.x - 24.0f);
      const auto panelHeight = std::min(page_ == Page::Main ? 430.0f : 560.0f, size.y - 32.0f);
      const auto panelPosition = glm::vec2{(size.x - panelWidth) * 0.5f, std::max(16.0f, (size.y - panelHeight) * 0.5f)};
      uiLayer_.panel(panelPosition, {panelWidth, panelHeight}, {0.07f, 0.11f, 0.14f, 0.94f});
      uiLayer_.text("ETHER BLOCKS", {panelPosition.x + 38.0f, panelPosition.y + 32.0f}, 4.0f, {0.78f, 0.93f, 1.0f, 1.0f});

      auto result = MenuResult{};
      switch (page_) {
         case Page::Main:
            result = drawMain(panelPosition, panelWidth, panelHeight);
            break;
         case Page::Worlds:
            result = drawWorlds(panelPosition, panelWidth, panelHeight, worlds, activeWorldIndex);
            break;
         case Page::Settings:
            result = drawSettings(panelPosition, panelWidth, panelHeight, settings, resolutions);
            break;
      }
      uiLayer_.end(renderer);
      return result;
   }

   MenuResult GameMenu::drawMain(glm::vec2 panelPosition, float panelWidth, float panelHeight) {
      const auto buttonPosition = glm::vec2{panelPosition.x + 42.0f, panelPosition.y + 104.0f};
      const auto buttonSize = glm::vec2{panelWidth - 84.0f, 46.0f};
      if (uiLayer_.button("RESUME", buttonPosition, buttonSize)) {
         return {MenuAction::Resume};
      }
      if (uiLayer_.button("WORLDS", {buttonPosition.x, buttonPosition.y + 58.0f}, buttonSize)) {
         worldsPage_ = 0;
         page_ = Page::Worlds;
      }
      if (uiLayer_.button("SETTINGS", {buttonPosition.x, buttonPosition.y + 116.0f}, buttonSize)) {
         settingsPage_ = 0;
         page_ = Page::Settings;
      }
      if (uiLayer_.button("QUIT", {buttonPosition.x, buttonPosition.y + 174.0f}, buttonSize)) {
         return {MenuAction::Quit};
      }

      uiLayer_.text("ESC MENU", {panelPosition.x + 42.0f, panelPosition.y + panelHeight - 42.0f}, 2.0f,
                    {0.46f, 0.60f, 0.68f, 1.0f});
      return {};
   }

   MenuResult GameMenu::drawWorlds(glm::vec2 panelPosition, float panelWidth, float panelHeight,
                                   const std::vector<WorldInfo>& worlds, int activeWorldIndex) {
      uiLayer_.text("WORLDS", {panelPosition.x + 42.0f, panelPosition.y + 92.0f}, 3.0f, {0.78f, 0.93f, 1.0f, 1.0f});

      auto y = panelPosition.y + 136.0f;
      const auto buttonX = panelPosition.x + 42.0f;
      const auto buttonWidth = panelWidth - 84.0f;
      const auto bottomY = panelPosition.y + panelHeight - 218.0f;
      const auto listHeight = std::max(40.0f, bottomY - y - 12.0f);
      const auto worldsPerPage = std::max(1, static_cast<int>(listHeight / 48.0f));
      const auto pageCount = std::max(1, static_cast<int>((worlds.size() + worldsPerPage - 1) / worldsPerPage));
      worldsPage_ = std::clamp(worldsPage_, 0, pageCount - 1);
      const auto firstWorld = static_cast<std::size_t>(worldsPage_ * worldsPerPage);
      const auto lastWorld = std::min(worlds.size(), firstWorld + static_cast<std::size_t>(worldsPerPage));
      for (auto i = firstWorld; i < lastWorld; ++i) {
         auto label = worlds[i].name;
         if (static_cast<int>(i) == activeWorldIndex) {
            label = "> " + label;
         }
         if (uiLayer_.button(label, {buttonX, y}, {buttonWidth, 40.0f})) {
            return {MenuAction::SelectWorld, static_cast<int>(i)};
         }
         y += 48.0f;
      }

      const auto pageY = panelPosition.y + 96.0f;
      uiLayer_.text("PAGE " + std::to_string(worldsPage_ + 1) + ":" + std::to_string(pageCount),
                    {buttonX + buttonWidth - 180.0f, pageY + 8.0f}, 1.75f, {0.46f, 0.60f, 0.68f, 1.0f});
      if (uiLayer_.button("-", {buttonX + buttonWidth - 82.0f, pageY}, {30.0f, 30.0f})) {
         worldsPage_ = std::max(0, worldsPage_ - 1);
      }
      if (uiLayer_.button("+", {buttonX + buttonWidth - 42.0f, pageY}, {30.0f, 30.0f})) {
         worldsPage_ = std::min(pageCount - 1, worldsPage_ + 1);
      }
      if (uiLayer_.button("LOAD SELECTED", {buttonX, bottomY}, {buttonWidth, 40.0f})) {
         return {MenuAction::LoadWorld};
      }
      if (uiLayer_.button("NEW WORLD", {buttonX, bottomY + 50.0f}, {buttonWidth, 40.0f})) {
         return {MenuAction::CreateWorld};
      }
      if (uiLayer_.button("DELETE SELECTED", {buttonX, bottomY + 100.0f}, {buttonWidth, 40.0f})) {
         return {MenuAction::DeleteWorld};
      }
      if (uiLayer_.button("BACK", {buttonX, bottomY + 150.0f}, {buttonWidth, 40.0f})) {
         page_ = Page::Main;
      }
      return {};
   }

   MenuResult GameMenu::drawSettings(glm::vec2 panelPosition, float panelWidth, float panelHeight, AppSettings& settings,
                                     const std::vector<glm::ivec2>& resolutions) {
      auto action = MenuAction::None;
      uiLayer_.text("SETTINGS", {panelPosition.x + 42.0f, panelPosition.y + 92.0f}, 3.0f, {0.78f, 0.93f, 1.0f, 1.0f});
      const auto buttonSize = glm::vec2{panelWidth - 84.0f, 42.0f};
      const auto buttonX = panelPosition.x + 42.0f;
      const auto bottomY = panelPosition.y + panelHeight - 116.0f;
      settingsPage_ = std::clamp(settingsPage_, 0, 3);

      auto rowY = panelPosition.y + 140.0f;
      if (settingsPage_ == 0) {
         uiLayer_.text("VIDEO", {buttonX, rowY - 28.0f}, 2.0f, {0.46f, 0.60f, 0.68f, 1.0f});
         settingRow("RES", resolutionLabel(settings.window.size), rowY, buttonX, panelWidth - 84.0f,
                    [&settings, &action, &resolutions](int delta) {
                       const auto fallback = std::vector<glm::ivec2>{{800, 600}, {1280, 720}, {1600, 900}, {1920, 1080}};
                       const auto& options = resolutions.empty() ? fallback : resolutions;
                       const auto current = resolutionPresetIndex(settings.window.size, options);
                       const auto next = std::clamp(current + delta, 0, static_cast<int>(options.size()) - 1);
                       settings.window.size = options[static_cast<std::size_t>(next)];
                       action = MenuAction::ApplyWindowSettings;
                    });
         rowY += 54.0f;
         settingRow("VSYNC", boolLabel(settings.window.vsync), rowY, buttonX, panelWidth - 84.0f, [&settings, &action](int) {
            settings.window.vsync = !settings.window.vsync;
            action = MenuAction::ApplyWindowSettings;
         });
         rowY += 54.0f;
         settingRow("FULLSCREEN", boolLabel(settings.window.fullscreen), rowY, buttonX, panelWidth - 84.0f,
                    [&settings, &action](int) {
                       settings.window.fullscreen = !settings.window.fullscreen;
                       action = MenuAction::ApplyWindowSettings;
                    });
         rowY += 54.0f;
         settingRow("VIEW", static_cast<int>(settings.rendering.viewDistance), rowY, buttonX, panelWidth - 84.0f, 4.0f,
                    [&settings, &action](float delta) {
                       settings.rendering.viewDistance =
                           std::clamp(settings.rendering.viewDistance + delta, 8.0f, settings.rendering.farPlane);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("NEAR", static_cast<int>(settings.rendering.nearPlane * 100.0f), rowY, buttonX, panelWidth - 84.0f, 1.0f,
                    [&settings, &action](float delta) {
                       settings.rendering.nearPlane =
                           std::clamp(settings.rendering.nearPlane + delta * 0.01f, 0.01f, settings.rendering.farPlane - 1.0f);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("FAR", static_cast<int>(settings.rendering.farPlane), rowY, buttonX, panelWidth - 84.0f, 10.0f,
                    [&settings, &action](float delta) {
                       settings.rendering.farPlane =
                           std::clamp(settings.rendering.farPlane + delta, settings.rendering.nearPlane + 1.0f, 500.0f);
                       action = MenuAction::SaveSettings;
                    });
      } else if (settingsPage_ == 1) {
         uiLayer_.text("LIGHT", {buttonX, rowY - 28.0f}, 2.0f, {0.46f, 0.60f, 0.68f, 1.0f});
         settingRow("FOG START", static_cast<int>(settings.rendering.fogStart), rowY, buttonX, panelWidth - 84.0f, 1.0f,
                    [&settings, &action](float delta) {
                       settings.rendering.fogStart =
                           std::clamp(settings.rendering.fogStart + delta, 0.0f, settings.rendering.fogEnd - 1.0f);
                       action = MenuAction::ApplySettings;
                    });
         rowY += 54.0f;
         settingRow("FOG END", static_cast<int>(settings.rendering.fogEnd), rowY, buttonX, panelWidth - 84.0f, 1.0f,
                    [&settings, &action](float delta) {
                       settings.rendering.fogEnd = std::clamp(settings.rendering.fogEnd + delta,
                                                              settings.rendering.fogStart + 1.0f, settings.rendering.farPlane);
                       action = MenuAction::ApplySettings;
                    });
         rowY += 54.0f;
         settingRow("CLEAR R", colorChannel(settings.rendering.clearColor.r), rowY, buttonX, panelWidth - 84.0f, 5.0f,
                    [&settings, &action](float delta) {
                       setColorChannel(settings.rendering.clearColor.r, delta);
                       action = MenuAction::ApplySettings;
                    });
         rowY += 54.0f;
         settingRow("CLEAR G", colorChannel(settings.rendering.clearColor.g), rowY, buttonX, panelWidth - 84.0f, 5.0f,
                    [&settings, &action](float delta) {
                       setColorChannel(settings.rendering.clearColor.g, delta);
                       action = MenuAction::ApplySettings;
                    });
         rowY += 54.0f;
         settingRow("CLEAR B", colorChannel(settings.rendering.clearColor.b), rowY, buttonX, panelWidth - 84.0f, 5.0f,
                    [&settings, &action](float delta) {
                       setColorChannel(settings.rendering.clearColor.b, delta);
                       action = MenuAction::ApplySettings;
                    });
      } else if (settingsPage_ == 2) {
         uiLayer_.text("INPUT", {buttonX, rowY - 28.0f}, 2.0f, {0.46f, 0.60f, 0.68f, 1.0f});
         settingRow("REACH", static_cast<int>(settings.player.interactionRange), rowY, buttonX, panelWidth - 84.0f, 1.0f,
                    [&settings, &action](float delta) {
                       settings.player.interactionRange = std::clamp(settings.player.interactionRange + delta, 1.0f, 32.0f);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("CURSOR", boolLabel(settings.window.cursorDisabled), rowY, buttonX, panelWidth - 84.0f,
                    [&settings, &action](int) {
                       settings.window.cursorDisabled = !settings.window.cursorDisabled;
                       action = MenuAction::ApplyInputSettings;
                    });
         rowY += 54.0f;
         settingRow("RAW MOUSE", boolLabel(settings.window.rawMouseMotion), rowY, buttonX, panelWidth - 84.0f,
                    [&settings, &action](int) {
                       settings.window.rawMouseMotion = !settings.window.rawMouseMotion;
                       action = MenuAction::ApplyInputSettings;
                    });
      }

      if (settingsPage_ == 3) {
         rowY = panelPosition.y + 140.0f;
         uiLayer_.text("WORLD", {buttonX, rowY - 28.0f}, 2.0f, {0.46f, 0.60f, 0.68f, 1.0f});
         settingRow("WORLD X", settings.world.size.x, rowY, buttonX, panelWidth - 84.0f, 16.0f,
                    [&settings, &action](float delta) {
                       settings.world.size.x = std::clamp(settings.world.size.x + static_cast<int>(delta), 16, 512);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("WORLD Y", settings.world.size.y, rowY, buttonX, panelWidth - 84.0f, 8.0f,
                    [&settings, &action](float delta) {
                       settings.world.size.y = std::clamp(settings.world.size.y + static_cast<int>(delta), 16, 256);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("WORLD Z", settings.world.size.z, rowY, buttonX, panelWidth - 84.0f, 16.0f,
                    [&settings, &action](float delta) {
                       settings.world.size.z = std::clamp(settings.world.size.z + static_cast<int>(delta), 16, 512);
                       action = MenuAction::SaveSettings;
                    });
         rowY += 54.0f;
         settingRow("CHUNK", settings.world.chunkSize, rowY, buttonX, panelWidth - 84.0f, 16.0f,
                    [&settings, &action](float delta) {
                       settings.world.chunkSize = std::clamp(settings.world.chunkSize + static_cast<int>(delta), 16, 64);
                       action = MenuAction::SaveSettings;
                    });
      }

      const auto pageY = panelPosition.y + 96.0f;
      uiLayer_.text("PAGE " + std::to_string(settingsPage_ + 1) + ":4", {buttonX + panelWidth - 264.0f, pageY + 8.0f}, 1.75f,
                    {0.46f, 0.60f, 0.68f, 1.0f});
      if (uiLayer_.button("-", {buttonX + panelWidth - 166.0f, pageY}, {30.0f, 30.0f})) {
         settingsPage_ = std::max(0, settingsPage_ - 1);
      }
      if (uiLayer_.button("+", {buttonX + panelWidth - 126.0f, pageY}, {30.0f, 30.0f})) {
         settingsPage_ = std::min(3, settingsPage_ + 1);
      }
      if (uiLayer_.button("BACK", {buttonX, panelPosition.y + panelHeight - 64.0f}, buttonSize)) {
         page_ = Page::Main;
      }
      return {action};
   }

} // namespace etherblocks::app
