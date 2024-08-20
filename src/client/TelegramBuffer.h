#pragma once

#include <optional>
#include <string>
#include <vector>

#include "ByteArrayUtils.h"
#include "TelegramFrame.h"

namespace comm {

/**
 * @brief Implements Telegram Buffer as a wrapper over BytesArray
 *
 * It aggregates received bytes and return only well filled frames, separated by
 * telegram delimerer byte.
 */
class TelegramBuffer {
  static const std::size_t DEFAULT_SIZE_HINT = 1024;

 public:
  TelegramBuffer(std::size_t sizeHint = DEFAULT_SIZE_HINT) {
      m_rawBuffer.reserve(sizeHint);
  }
  ~TelegramBuffer() = default;

  bool empty() { return m_rawBuffer.isEmpty(); }

  void clear() { m_rawBuffer.clear(); }

  void append(const QByteArray&);
  void takeTelegramFrames(std::vector<TelegramFramePtr>&);
  std::vector<TelegramFramePtr> takeTelegramFrames();
  void takeErrors(std::vector<std::string>&);

  const QByteArray& data() const { return m_rawBuffer; }

 private:
  QByteArray m_rawBuffer;
  std::vector<std::string> m_errors;
  std::optional<TelegramHeader> m_headerOpt;

  bool checkRawBuffer();
};

}  // namespace comm

