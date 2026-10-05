#include "ascii/CharacterRamp.h"

#include <algorithm>
#include <cmath>

namespace ascii_converter::ascii {

CharacterRamp::CharacterRamp(QString characters) : m_characters(std::move(characters)) {}

QChar CharacterRamp::characterForLuminance(int luminance) const {
    if (m_characters.isEmpty()) {
        return QLatin1Char(' ');
    }

    const int clampedLuminance = std::clamp(luminance, 0, 255);
    const int lastIndex = m_characters.size() - 1;

    // luminancia baja (oscuro) -> índice bajo -> carácter denso ('@').
    // luminancia alta (claro) -> índice alto -> carácter disperso ('.').
    const int index = static_cast<int>(std::lround((clampedLuminance / 255.0) * lastIndex));
    return m_characters.at(std::clamp(index, 0, lastIndex));
}

int CharacterRamp::size() const {
    return m_characters.size();
}

const QString& CharacterRamp::characters() const {
    return m_characters;
}

CharacterRamp CharacterRamp::standard() {
    return CharacterRamp(QStringLiteral("@#S%?*+;:,."));
}

CharacterRamp CharacterRamp::dense() {
    return CharacterRamp(QStringLiteral("@#8&o:*."));
}

CharacterRamp CharacterRamp::blocks() {
    return CharacterRamp(QStringLiteral("\u2588\u2593\u2592\u2591"));  // █▓▒░
}

}  // namespace ascii_converter::ascii
