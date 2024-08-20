#pragma once

#include <optional>
#include <string>

std::optional<std::string> tryCompress(const std::string& decompressed);
std::optional<std::string> tryDecompress(const std::string& compressed);

