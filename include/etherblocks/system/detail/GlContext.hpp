
#ifndef ETHERBLOCKS_SYSTEM_DETAIL_GLCONTEXT_HPP
#define ETHERBLOCKS_SYSTEM_DETAIL_GLCONTEXT_HPP

namespace etherblocks::system::detail {
   class GlContext {
   public:
      static GlContext& instance();

      static void load();

      GlContext(const GlContext&) = delete;
      GlContext& operator=(const GlContext&) = delete;

      GlContext(GlContext&&) noexcept = delete;
      GlContext& operator=(GlContext&&) noexcept = delete;

      ~GlContext();

   private:
      GlContext();
   };
} // namespace etherblocks::system::detail

#endif // ETHERBLOCKS_SYSTEM_DETAIL_GLCONTEXT_HPP
