#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_DETAIL_GLOBJECT_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_DETAIL_GLOBJECT_HPP

namespace etherblocks::engine::graphics::detail {

   class GlObject {
   public:
      GlObject(const GlObject&) = delete;
      GlObject& operator=(const GlObject&) = delete;

   protected:
      GlObject() = default;
      GlObject(GlObject&& other) noexcept;

      GlObject& moveAssign(GlObject&& other) noexcept;

      unsigned int id_{0};
   };

} // namespace etherblocks::engine::graphics::detail

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_DETAIL_GLOBJECT_HPP
