#pragma once

#include <QChar>
#include <QString>

namespace ascii_converter::ascii {

// Conjunto ordenado de caracteres para representar luminancia. Convención:
// el PRIMER carácter es el de MAYOR densidad visual (para zonas oscuras);
// el ÚLTIMO es el de MENOR densidad (para zonas claras) — así, los
// caracteres más densos representan zonas oscuras del original y los más
// dispersos representan zonas claras.
class CharacterRamp {
public:
    explicit CharacterRamp(QString characters);

    // luminance en [0, 255] (0 = negro, 255 = blanco). Si la rampa está
    // vacía devuelve siempre ' ' en vez de crashear.
    QChar characterForLuminance(int luminance) const;

    int size() const;
    const QString& characters() const;

    static CharacterRamp standard();  // "@#S%?*+;:,."
    static CharacterRamp dense();     // "@#8&o:*."
    static CharacterRamp blocks();    // "█▓▒░"

private:
    QString m_characters;
};

}  // namespace ascii_converter::ascii
