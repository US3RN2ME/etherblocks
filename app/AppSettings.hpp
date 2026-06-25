#ifndef APP_APPSETTINGS_HPP
#define APP_APPSETTINGS_HPP

#include <etherblocks/engine/graphics/Renderer.hpp>
#include <glm/glm.hpp>
#include <string>
#include <string_view>

namespace etherblocks::app {

   constexpr std::string_view kSettingsPath = "./assets/config/settings.json";

   struct AppSettings {
      struct Window {
         glm::ivec2 size{800, 600};
         std::string title{"etherblocks"};
         bool vsync{true};
         bool fullscreen{false};
         bool cursorDisabled{true};
         bool rawMouseMotion{true};
      };

      struct World {
         glm::ivec3 size{256, 64, 256};
         int chunkSize{16};
         std::string savePath{"./world.ebw"};
         std::string saveDirectory{"./saves"};
         std::string activeWorld{"world_001"};
      };

      struct Rendering {
         engine::graphics::Color clearColor{0.11f, 0.18f, 0.24f, 1.0f};
         glm::vec4 skyLight{1.08f, 1.14f, 1.2f, 1.0f};
         glm::vec4 groundLight{0.42f, 0.46f, 0.52f, 1.0f};
         float viewDistance{29.0f};
         float nearPlane{0.1f};
         float farPlane{100.0f};
         float fogStart{16.0f};
         float fogEnd{29.0f};
      };

      struct Player {
         float interactionRange{8.0f};
      };

      struct Assets {
         std::string blockManifest{"./assets/blocks/manifest.json"};
      };

      Window window{};
      World world{};
      Rendering rendering{};
      Player player{};
      Assets assets{};
   };

   [[nodiscard]] AppSettings validateSettings(AppSettings settings);
   [[nodiscard]] AppSettings loadAppSettings(std::string_view path);
   bool saveAppSettings(const AppSettings& settings, std::string_view path);

} // namespace etherblocks::app

#endif // APP_APPSETTINGS_HPP
