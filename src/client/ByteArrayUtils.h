#pragma once

#include <QByteArray>

namespace comm {

std::optional<std::size_t> findSequence(const QByteArray& data, const char* sequence, std::size_t sequenceSize);

uint32_t calcCheckSum(const QByteArray& iterable);

}  // namespace comm

