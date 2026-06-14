#ifndef ETHERBLOCKS_ENGINE_UI_UILAYER_HPP
#define ETHERBLOCKS_ENGINE_UI_UILAYER_HPP

#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/system/Input.hpp>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>

namespace etherblocks::engine::ui {

   class UiLayer {
   public:
      struct Vertex {
         glm::vec2 position{};
         glm::vec4 color{};
      };

      UiLayer();

      void begin(glm::ivec2 screenSize, const system::Input& input);
      void panel(glm::vec2 position, glm::vec2 size, graphics::Color color);
      [[nodiscard]] bool button(std::string_view label, glm::vec2 position, glm::vec2 size);
      void text(std::string_view text, glm::vec2 position, float scale, graphics::Color color);
      void end(const graphics::Renderer& renderer);

   private:
      void rect(glm::vec2 position, glm::vec2 size, graphics::Color color);
      void border(glm::vec2 position, glm::vec2 size, float thickness, graphics::Color color);
      void glyph(char character, glm::vec2 position, float scale, graphics::Color color);
      [[nodiscard]] bool contains(glm::vec2 position, glm::vec2 size) const noexcept;

      graphics::Mesh mesh_;
      graphics::Material material_;
      std::vector<Vertex> vertices_;
      const system::Input* input_{};
      glm::ivec2 screenSize_{};
   };

} // namespace etherblocks::engine::ui

#endif // ETHERBLOCKS_ENGINE_UI_UILAYER_HPP
