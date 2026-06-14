#ifndef APP_GAMEMENU_HPP
#define APP_GAMEMENU_HPP

#include <algorithm>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/ui/UiLayer.hpp>
#include <etherblocks/system/Input.hpp>
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <vector>

#include "AppSettings.hpp"
#include "WorldPersistence.hpp"

namespace etherblocks::app {

   enum class MenuAction {
      None,
      Resume,
      LoadWorld,
      CreateWorld,
      SelectWorld,
      DeleteWorld,
      Quit,
      ApplySettings,
      ApplyWindowSettings,
      ApplyInputSettings,
      SaveSettings,
   };

   struct MenuResult {
      MenuAction action{MenuAction::None};
      int worldIndex{-1};
   };

   class GameMenu {
   public:
      MenuResult draw(const engine::graphics::Renderer& renderer, const system::Input& input, glm::ivec2 screenSize,
                      AppSettings& settings, const std::vector<WorldInfo>& worlds, int activeWorldIndex,
                      const std::vector<glm::ivec2>& resolutions);

   private:
      enum class Page { Main, Worlds, Settings };

      MenuResult drawMain(glm::vec2 panelPosition, float panelWidth, float panelHeight);
      MenuResult drawWorlds(glm::vec2 panelPosition, float panelWidth, float panelHeight, const std::vector<WorldInfo>& worlds,
                            int activeWorldIndex);
      MenuResult drawSettings(glm::vec2 panelPosition, float panelWidth, float panelHeight, AppSettings& settings,
                              const std::vector<glm::ivec2>& resolutions);

      template <typename OnChange>
      void settingRow(std::string_view label, int value, float y, float x, float width, float step, OnChange onChange) {
         const auto valueX = x + std::min(190.0f, std::max(92.0f, width - 238.0f));
         uiLayer_.panel({x, y}, {width, 42.0f}, {0.10f, 0.15f, 0.18f, 0.82f});
         uiLayer_.text(label, {x + 10.0f, y + 14.0f}, 1.75f, {0.78f, 0.88f, 0.92f, 1.0f});
         uiLayer_.text(std::to_string(value), {valueX, y + 14.0f}, 1.75f, {0.88f, 0.95f, 0.98f, 1.0f});
         if (uiLayer_.button("-", {x + width - 82.0f, y + 6.0f}, {30.0f, 30.0f})) {
            onChange(-step);
         }
         if (uiLayer_.button("+", {x + width - 42.0f, y + 6.0f}, {30.0f, 30.0f})) {
            onChange(step);
         }
      }

      template <typename OnChange>
      void settingRow(std::string_view label, std::string_view value, float y, float x, float width, OnChange onChange) {
         const auto valueX = x + std::min(190.0f, std::max(92.0f, width - 238.0f));
         uiLayer_.panel({x, y}, {width, 42.0f}, {0.10f, 0.15f, 0.18f, 0.82f});
         uiLayer_.text(label, {x + 10.0f, y + 14.0f}, 1.75f, {0.78f, 0.88f, 0.92f, 1.0f});
         uiLayer_.text(value, {valueX, y + 14.0f}, 1.75f, {0.88f, 0.95f, 0.98f, 1.0f});
         if (uiLayer_.button("-", {x + width - 82.0f, y + 6.0f}, {30.0f, 30.0f})) {
            onChange(-1);
         }
         if (uiLayer_.button("+", {x + width - 42.0f, y + 6.0f}, {30.0f, 30.0f})) {
            onChange(1);
         }
      }

      engine::ui::UiLayer uiLayer_;
      Page page_{Page::Main};
      int settingsPage_{0};
      int worldsPage_{0};
   };

} // namespace etherblocks::app

#endif // APP_GAMEMENU_HPP
