#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP

#include <glm/glm.hpp>
#include <utility>

namespace etherblocks::engine::graphics {

   /**
    * @brief RGBA color with normalized channels.
    */
   struct Color {
      /**
       * @brief Red channel.
       */
      float r{};

      /**
       * @brief Green channel.
       */
      float g{};

      /**
       * @brief Blue channel.
       */
      float b{};

      /**
       * @brief Alpha channel.
       */
      float a{};
   };

   /**
    * @brief Clearable framebuffer attachments.
    */
   enum class ClearBuffer : unsigned {
      Color = 1 << 0,
      Depth = 1 << 1,
      Stencil = 1 << 2,
   };

   /**
    * @brief Combine framebuffer clear flags.
    *
    * @param a First clear buffer flag.
    * @param b Second clear buffer flag.
    *
    * @return Combined clear buffer flags.
    */
   constexpr ClearBuffer operator|(ClearBuffer a, ClearBuffer b) noexcept {
      // Bitmask combinations are valid even if they are not named enum values.
      // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
      return static_cast<ClearBuffer>(std::to_underlying(a) | std::to_underlying(b));
   }

   /**
    * @brief Primitive topology used for draw calls.
    */
   enum class Primitive { Points, Lines, Triangles };

   /**
    * @brief Toggleable renderer capabilities.
    */
   enum class RenderFeature {
      DepthTest,
      StencilTest,
      Blending,
      CullFace,
      Multisample,
      ProgramPointSize,
   };

   class Material;
   class Mesh;

   /**
    * @brief Stateless wrapper around common OpenGL rendering commands.
    */
   class Renderer {
   public:
      /**
       * @brief Draw a mesh with a material.
       *
       * @param mesh Mesh to draw.
       * @param material Material to bind before drawing.
       * @param primitive Primitive topology.
       */
      void draw(const Mesh& mesh, const Material& material, Primitive primitive = Primitive::Triangles) const;

      /**
       * @brief Draw a mesh with the currently bound shader state.
       *
       * @param mesh Mesh to draw.
       * @param primitive Primitive topology.
       */
      void draw(const Mesh& mesh, Primitive primitive = Primitive::Triangles) const;

      /**
       * @brief Clear selected framebuffer attachments.
       *
       * @param color Clear color.
       * @param buffers Attachments to clear.
       */
      void clear(Color color, ClearBuffer buffers) const noexcept;

      /**
       * @brief Enable a renderer feature.
       *
       * @param feature Feature to enable.
       */
      void enable(RenderFeature feature) const noexcept;

      /**
       * @brief Disable a renderer feature.
       *
       * @param feature Feature to disable.
       */
      void disable(RenderFeature feature) const noexcept;

      /**
       * @brief Configure standard alpha blending.
       */
      void setAlphaBlending() const noexcept;

      /**
       * @brief Set the rasterized line width.
       *
       * @param width Line width in pixels.
       */
      void setLineWidth(float width) const noexcept;

      /**
       * @brief Set the viewport to cover the given size.
       *
       * @param size Viewport size in pixels.
       */
      void setViewport(glm::ivec2 size) const noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP
