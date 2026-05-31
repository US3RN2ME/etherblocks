#ifndef ETHERBLOCKS_DETAIL_GLOBJECT_HPP
#define ETHERBLOCKS_DETAIL_GLOBJECT_HPP

namespace etherblocks::detail {

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

} // namespace etherblocks::detail

#endif // ETHERBLOCKS_DETAIL_GLOBJECT_HPP
