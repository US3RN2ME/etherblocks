#include <etherblocks/Material.hpp>
#include <etherblocks/Mesh.hpp>
#include <etherblocks/Renderer.hpp>
#include <glad/glad.h>
#include <limits>
#include <stdexcept>

namespace etherblocks {

   namespace {
      GLenum toGl(Primitive primitive) noexcept {
         switch (primitive) {
            case Primitive::Points:
               return GL_POINTS;
            case Primitive::Lines:
               return GL_LINES;
            case Primitive::Triangles:
               return GL_TRIANGLES;
         }
         return GL_TRIANGLES;
      }

      GLenum toGl(RenderFeature feature) noexcept {
         switch (feature) {
            case RenderFeature::DepthTest:
               return GL_DEPTH_TEST;
            case RenderFeature::StencilTest:
               return GL_STENCIL_TEST;
            case RenderFeature::Blending:
               return GL_BLEND;
            case RenderFeature::CullFace:
               return GL_CULL_FACE;
            case RenderFeature::Multisample:
               return GL_MULTISAMPLE;
            case RenderFeature::ProgramPointSize:
               return GL_PROGRAM_POINT_SIZE;
         }
         return GL_INVALID_ENUM;
      }

      GLbitfield toGl(ClearBuffer buffers) noexcept {
         GLbitfield mask = 0;
         const auto value = static_cast<unsigned>(buffers);
         if ((value & static_cast<unsigned>(ClearBuffer::Color)) != 0)
            mask |= GL_COLOR_BUFFER_BIT;
         if ((value & static_cast<unsigned>(ClearBuffer::Depth)) != 0)
            mask |= GL_DEPTH_BUFFER_BIT;
         if ((value & static_cast<unsigned>(ClearBuffer::Stencil)) != 0)
            mask |= GL_STENCIL_BUFFER_BIT;
         return mask;
      }
   } // namespace

   void Renderer::draw(const Mesh& mesh, const Material& material, Primitive primitive) const {
      material.bind();
      draw(mesh, primitive);
   }

   void Renderer::draw(const Mesh& mesh, Primitive primitive) const {
      if (mesh.vertexCount() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
         throw std::overflow_error{"Mesh vertex count exceeds the OpenGL limit"};
      }
      mesh.bind();
      glDrawArrays(toGl(primitive), 0, static_cast<GLsizei>(mesh.vertexCount()));
   }

   void Renderer::clear(Color color, ClearBuffer buffers) const noexcept {
      glClearColor(color.r, color.g, color.b, color.a);
      glClear(toGl(buffers));
   }

   void Renderer::enable(RenderFeature feature) const noexcept {
      glEnable(toGl(feature));
   }

   void Renderer::disable(RenderFeature feature) const noexcept {
      glDisable(toGl(feature));
   }

   void Renderer::setLineWidth(float width) const noexcept {
      glLineWidth(width);
   }

   void Renderer::setViewport(glm::ivec2 size) const noexcept {
      glViewport(0, 0, size.x, size.y);
   }

} // namespace etherblocks
