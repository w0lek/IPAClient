#include "ByteArrayUtils.h"

namespace comm {

std::optional<std::size_t> findSequence(const QByteArray& data, const char* sequence, std::size_t sequenceSize) {
    const std::size_t mSize = data.size();
    if (mSize >= sequenceSize) {
        for (std::size_t i = 0; i <= mSize - sequenceSize; ++i) {
            bool found = true;
            for (std::size_t j = 0; j < sequenceSize; ++j) {
                if (data.at(i + j) != sequence[j]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return i;
            }
        }
    }
    return std::nullopt;
}

uint32_t calcCheckSum(const QByteArray& iterable) {
    uint32_t sum = 0;
    for (uint8_t c : iterable) {
        sum += static_cast<unsigned int>(c);
    }
    return sum;
}

}  // namespace comm

