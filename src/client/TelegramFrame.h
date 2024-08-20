#pragma once

#include <memory>

#include <QByteArray>

#include "TelegramHeader.h"

namespace comm {

struct TelegramFrame {
  TelegramHeader header;
  QByteArray body;
};
using TelegramFramePtr = std::shared_ptr<TelegramFrame>;

}  // namespace comm

