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

   /**
    * @brief Immediate-mode UI layer for panels, text, buttons, and images.
    */
   class UiLayer {
   public:
      /**
       * @brief Colored 2D UI vertex.
       */
      struct Vertex {
         /**
          * @brief Screen-space vertex position.
          */
         glm::vec2 position{};

         /**
          * @brief Vertex color.
          */
         glm::vec4 color{};
      };

      /**
       * @brief Textured 2D UI vertex.
       */
      struct ImageVertex {
         /**
          * @brief Screen-space vertex position.
          */
         glm::vec2 position{};

         /**
          * @brief Texture coordinate.
          */
         glm::vec2 textureCoordinate{};

         /**
          * @brief Vertex tint color.
          */
         glm::vec4 color{};
      };

      /**
       * @brief Construct UI rendering resources.
       */
      UiLayer();

      /**
       * @brief Start collecting UI draw commands for a frame.
       *
       * @param screenSize Current screen size in pixels.
       * @param input Current input state.
       * @param inputEnabled True when widgets should react to input.
       */
      void begin(glm::ivec2 screenSize, const system::Input& input, bool inputEnabled = true);

      /**
       * @brief Queue a textured image.
       *
       * @param texture Texture to draw.
       * @param position Top-left position in pixels.
       * @param size Draw size in pixels.
       * @param tint Image tint color.
       */
      void image(const graphics::Texture& texture, glm::vec2 position, glm::vec2 size, graphics::Color tint);

      /**
       * @brief Queue a filled rectangular panel.
       *
       * @param position Top-left position in pixels.
       * @param size Panel size in pixels.
       * @param color Panel color.
       */
      void panel(glm::vec2 position, glm::vec2 size, graphics::Color color);

      /**
       * @brief Queue a button and report whether it was clicked this frame.
       *
       * @param label Button text.
       * @param position Top-left position in pixels.
       * @param size Button size in pixels.
       *
       * @return True when the button was clicked.
       */
      [[nodiscard]] bool button(std::string_view label, glm::vec2 position, glm::vec2 size);

      /**
       * @brief Queue bitmap text.
       *
       * @param text Text to draw.
       * @param position Top-left position in pixels.
       * @param scale Glyph scale factor.
       * @param color Text color.
       */
      void text(std::string_view text, glm::vec2 position, float scale, graphics::Color color);

      /**
       * @brief Upload queued UI geometry and draw it.
       *
       * @param renderer Renderer used for drawing.
       */
      void end(const graphics::Renderer& renderer);

   private:
      void rect(glm::vec2 position, glm::vec2 size, graphics::Color color);
      void border(glm::vec2 position, glm::vec2 size, float thickness, graphics::Color color);
      void glyph(char character, glm::vec2 position, float scale, graphics::Color color);
      [[nodiscard]] bool contains(glm::vec2 position, glm::vec2 size) const noexcept;

      graphics::Mesh mesh_;
      graphics::Material material_;
      graphics::Mesh imageMesh_;
      graphics::Material imageMaterial_;
      std::vector<Vertex> vertices_;
      std::vector<ImageVertex> imageVertices_;
      const graphics::Texture* imageTexture_{};
      const system::Input* input_{};
      glm::ivec2 screenSize_{};
      bool inputEnabled_{true};
   };

} // namespace etherblocks::engine::ui

#endif // ETHERBLOCKS_ENGINE_UI_UILAYER_HPP
