#pragma once

#include <QChar>
#include <QString>

namespace ascii_converter::ascii {

// Conjunto ordenado de caracteres para representar luminancia (Sección 12
// del spec original). Convención: el PRIMER carácter es el de MAYOR
// densidad visual (para zonas oscuras); el ÚLTIMO es el de MENOR densidad
// (para zonas claras) — tal como pide explícitamente la Sección 12:
// "Los caracteres con mayor densidad visual representan zonas oscuras.
//  Los caracteres menos densos representan zonas claras."
class CharacterRamp {
public:
    explicit CharacterRamp(QString characters);

    // luminance en [0, 255] (0 = negro, 255 = blanco). Si la rampa está
    // vacía devuelve siempre ' ' en vez de crashear (Sección 24).
    QChar characterForLuminance(int luminance) const;

    int size() const;
    const QString& characters() const;

    static CharacterRamp standard(); // "@#S%?*+;:,."
    static CharacterRamp dense();    // "@#8&o:*."
    static CharacterRamp blocks();   // "█▓▒░"

private:
    QString m_characters;
};

} // namespace ascii_converter::ascii
