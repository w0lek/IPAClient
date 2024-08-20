#include "TelegramBuffer.h"

#include <optional>

namespace comm {

void TelegramBuffer::append(const QByteArray& bytes) {
  m_rawBuffer.append(bytes);
}

bool TelegramBuffer::checkRawBuffer() {
  std::optional<std::size_t> signatureStartIndexOpt = findSequence(m_rawBuffer, TelegramHeader::SIGNATURE, TelegramHeader::SIGNATURE_SIZE);
  if (signatureStartIndexOpt) {
    std::size_t signatureStartIndex{signatureStartIndexOpt.value()};
    if (signatureStartIndex != 0) {
      m_rawBuffer.remove(0, signatureStartIndex);
    }
    return true;
  }
  return false;
}

void TelegramBuffer::takeTelegramFrames(
    std::vector<comm::TelegramFramePtr>& result) {
  if (m_rawBuffer.size() <= TelegramHeader::size()) {
    return;
  }

  bool mayContainFullTelegram = true;
  while (mayContainFullTelegram) {
    mayContainFullTelegram = false;
    if (!m_headerOpt) {
      if (checkRawBuffer()) {
        TelegramHeader header(m_rawBuffer);
        if (header.isValid()) {
          m_headerOpt = std::move(header);
        }
      }
    }

    if (m_headerOpt) {
      const TelegramHeader& header = m_headerOpt.value();
      std::size_t wholeTelegramSize =
          TelegramHeader::size() + header.bodyBytesNum();
      if (m_rawBuffer.size() >= wholeTelegramSize) {
        QByteArray body = m_rawBuffer.left(wholeTelegramSize);
        body.remove(0, TelegramHeader::size());

        uint32_t actualCheckSum = calcCheckSum(body);
        if (actualCheckSum == header.bodyCheckSum()) {
          TelegramFramePtr telegramFramePtr = std::make_shared<TelegramFrame>(
              TelegramFrame{header, std::move(body)});
          body.clear();
          result.push_back(telegramFramePtr);
        } else {
          m_errors.push_back("wrong checkSums " +
                             std::to_string(actualCheckSum) + " for " +
                             header.info() + " , drop this chunk");
        }
        m_rawBuffer.remove(0, wholeTelegramSize);
        m_headerOpt.reset();
        mayContainFullTelegram = true;
      }
    }
  }
}

std::vector<comm::TelegramFramePtr> TelegramBuffer::takeTelegramFrames() {
  std::vector<comm::TelegramFramePtr> result;
  takeTelegramFrames(result);
  return result;
}

void TelegramBuffer::takeErrors(std::vector<std::string>& errors) {
  errors.clear();
  std::swap(errors, m_errors);
}

}  // namespace comm

