#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP

#include <glm/glm.hpp>
#include <utility>

namespace etherblocks::engine::graphics {

   struct Color {
      float r{};
      float g{};
      float b{};
      float a{};
   };

   enum class ClearBuffer : unsigned {
      Color = 1 << 0,
      Depth = 1 << 1,
      Stencil = 1 << 2,
   };

   constexpr ClearBuffer operator|(ClearBuffer a, ClearBuffer b) noexcept {
      // Bitmask combinations are valid even if they are not named enum values.
      // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
      return static_cast<ClearBuffer>(std::to_underlying(a) | std::to_underlying(b));
   }

   enum class Primitive { Points, Lines, Triangles };

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

   class Renderer {
   public:
      void draw(const Mesh& mesh, const Material& material, Primitive primitive = Primitive::Triangles) const;
      void draw(const Mesh& mesh, Primitive primitive = Primitive::Triangles) const;
      void clear(Color color, ClearBuffer buffers) const noexcept;
      void enable(RenderFeature feature) const noexcept;
      void disable(RenderFeature feature) const noexcept;
      void setLineWidth(float width) const noexcept;
      void setViewport(glm::ivec2 size) const noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_RENDERER_HPP
