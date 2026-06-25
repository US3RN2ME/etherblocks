#include "AppSettings.hpp"

#include <algorithm>
#include <etherblocks/system/Logger.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;
      namespace sys = system;

      template <typename T>
      T jsonValue(const nlohmann::json& json, std::string_view key, T fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || it->is_null()) {
            return fallback;
         }
         return it->get<T>();
      }

      glm::ivec2 jsonIvec2(const nlohmann::json& json, std::string_view key, glm::ivec2 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 2) {
            return fallback;
         }
         return {(*it)[0].get<int>(), (*it)[1].get<int>()};
      }

      glm::ivec3 jsonIvec3(const nlohmann::json& json, std::string_view key, glm::ivec3 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 3) {
            return fallback;
         }
         return {(*it)[0].get<int>(), (*it)[1].get<int>(), (*it)[2].get<int>()};
      }

      glm::vec4 jsonVec4(const nlohmann::json& json, std::string_view key, glm::vec4 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 4) {
            return fallback;
         }
         return {(*it)[0].get<float>(), (*it)[1].get<float>(), (*it)[2].get<float>(), (*it)[3].get<float>()};
      }

      graphics::Color jsonColor(const nlohmann::json& json, std::string_view key, graphics::Color fallback) {
         const auto color = jsonVec4(json, key, {fallback.r, fallback.g, fallback.b, fallback.a});
         return {color.r, color.g, color.b, color.a};
      }

      nlohmann::json vec2Json(glm::ivec2 value) {
         return nlohmann::json::array({value.x, value.y});
      }

      nlohmann::json vec3Json(glm::ivec3 value) {
         return nlohmann::json::array({value.x, value.y, value.z});
      }

      nlohmann::json vec4Json(glm::vec4 value) {
         return nlohmann::json::array({value.x, value.y, value.z, value.w});
      }

      nlohmann::json colorJson(graphics::Color value) {
         return nlohmann::json::array({value.r, value.g, value.b, value.a});
      }
   } // namespace

   AppSettings validateSettings(AppSettings settings) {
      settings.window.size.x = std::max(settings.window.size.x, 1);
      settings.window.size.y = std::max(settings.window.size.y, 1);
      settings.world.size.x = std::max(settings.world.size.x, 1);
      settings.world.size.y = std::max(settings.world.size.y, 1);
      settings.world.size.z = std::max(settings.world.size.z, 1);
      settings.world.chunkSize = std::max(settings.world.chunkSize, 1);
      settings.rendering.viewDistance = std::max(settings.rendering.viewDistance, 1.0f);
      settings.rendering.nearPlane = std::max(settings.rendering.nearPlane, 0.01f);
      settings.rendering.farPlane = std::max(settings.rendering.farPlane, settings.rendering.nearPlane + 1.0f);
      settings.rendering.fogEnd = std::max(settings.rendering.fogEnd, settings.rendering.fogStart + 0.01f);
      settings.player.interactionRange = std::max(settings.player.interactionRange, 0.1f);
      return settings;
   }

   AppSettings loadAppSettings(std::string_view path) {
      AppSettings settings{};
      std::ifstream file{std::string(path)};
      if (!file) {
         sys::log(sys::LogLevel::Warning, "Failed to open app settings, using defaults: " + std::string(path));
         return settings;
      }

      try {
         const auto json = nlohmann::json::parse(file);
         const auto window = json.value("window", nlohmann::json::object());
         const auto world = json.value("world", nlohmann::json::object());
         const auto rendering = json.value("rendering", nlohmann::json::object());
         const auto projection = rendering.value("projection", nlohmann::json::object());
         const auto fog = rendering.value("fog", nlohmann::json::object());
         const auto player = json.value("player", nlohmann::json::object());
         const auto assets = json.value("assets", nlohmann::json::object());

         settings.window.title = jsonValue(window, "title", settings.window.title);
         settings.window.size = jsonIvec2(window, "size", settings.window.size);
         settings.window.vsync = jsonValue(window, "vsync", settings.window.vsync);
         settings.window.fullscreen = jsonValue(window, "fullscreen", settings.window.fullscreen);
         settings.window.cursorDisabled = jsonValue(window, "cursor_disabled", settings.window.cursorDisabled);
         settings.window.rawMouseMotion = jsonValue(window, "raw_mouse_motion", settings.window.rawMouseMotion);
         settings.world.size = jsonIvec3(world, "size", settings.world.size);
         settings.world.chunkSize = jsonValue(world, "chunk_size", settings.world.chunkSize);
         settings.world.savePath = jsonValue(world, "save_path", settings.world.savePath);
         settings.world.saveDirectory = jsonValue(world, "save_directory", settings.world.saveDirectory);
         settings.world.activeWorld = jsonValue(world, "active_world", settings.world.activeWorld);
         settings.rendering.clearColor = jsonColor(rendering, "clear_color", settings.rendering.clearColor);
         settings.rendering.skyLight = jsonVec4(rendering, "sky_light", settings.rendering.skyLight);
         settings.rendering.groundLight = jsonVec4(rendering, "ground_light", settings.rendering.groundLight);
         settings.rendering.viewDistance = jsonValue(rendering, "view_distance", settings.rendering.viewDistance);
         settings.rendering.nearPlane = jsonValue(projection, "near", settings.rendering.nearPlane);
         settings.rendering.farPlane = jsonValue(projection, "far", settings.rendering.farPlane);
         settings.rendering.fogStart = jsonValue(fog, "start", settings.rendering.fogStart);
         settings.rendering.fogEnd = jsonValue(fog, "end", settings.rendering.fogEnd);
         settings.player.interactionRange = jsonValue(player, "interaction_range", settings.player.interactionRange);
         settings.assets.blockManifest = jsonValue(assets, "block_manifest", settings.assets.blockManifest);
      } catch (const std::exception& exception) {
         sys::log(sys::LogLevel::Error, "Failed to parse app settings, using defaults: " + std::string(exception.what()));
      }

      return validateSettings(std::move(settings));
   }

   bool saveAppSettings(const AppSettings& settings, std::string_view path) {
      const auto json = nlohmann::json{
          {"schema", 1},
          {"window",
           {{"title", settings.window.title},
            {"size", vec2Json(settings.window.size)},
            {"vsync", settings.window.vsync},
            {"fullscreen", settings.window.fullscreen},
            {"cursor_disabled", settings.window.cursorDisabled},
            {"raw_mouse_motion", settings.window.rawMouseMotion}}},
          {"world",
           {{"size", vec3Json(settings.world.size)},
            {"chunk_size", settings.world.chunkSize},
            {"save_path", settings.world.savePath},
            {"save_directory", settings.world.saveDirectory},
            {"active_world", settings.world.activeWorld}}},
          {"rendering",
           {{"clear_color", colorJson(settings.rendering.clearColor)},
            {"sky_light", vec4Json(settings.rendering.skyLight)},
            {"ground_light", vec4Json(settings.rendering.groundLight)},
            {"view_distance", settings.rendering.viewDistance},
            {"projection", {{"near", settings.rendering.nearPlane}, {"far", settings.rendering.farPlane}}},
            {"fog", {{"start", settings.rendering.fogStart}, {"end", settings.rendering.fogEnd}}}}},
          {"player", {{"interaction_range", settings.player.interactionRange}}},
          {"assets", {{"block_manifest", settings.assets.blockManifest}}},
      };

      std::ofstream file{std::string(path)};
      if (!file) {
         sys::log(sys::LogLevel::Error, "Failed to open app settings for writing: " + std::string(path));
         return false;
      }
      file << json.dump(2) << '\n';
      return true;
   }

} // namespace etherblocks::app
