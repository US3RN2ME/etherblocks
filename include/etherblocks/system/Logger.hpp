#ifndef ETHERBLOCKS_SYSTEM_LOGGER_HPP
#define ETHERBLOCKS_SYSTEM_LOGGER_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etherblocks::system {

   enum class LogLevel { Debug, Info, Warning, Error };

   struct LogMessage {
      LogLevel level{};
      std::string text;
   };

   class Logger {
   public:
      using Sink = std::function<void(const LogMessage&)>;
      using SinkId = std::uint64_t;

      Logger();

      [[nodiscard]] SinkId addSink(Sink sink);
      void removeSink(SinkId id);
      void log(LogLevel level, std::string_view text) const noexcept;

   private:
      mutable std::mutex mutex_;
      std::vector<std::pair<SinkId, Sink>> sinks_;
      SinkId nextSinkId_{};
   };

   [[nodiscard]] Logger& logger();
   void log(LogLevel level, std::string_view text) noexcept;

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_LOGGER_HPP
