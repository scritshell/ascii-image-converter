#include <QtTest/QtTest>

#include "ascii/CharacterRamp.h"

using ascii_converter::ascii::CharacterRamp;

class TstCharacterRamp : public QObject {
    Q_OBJECT

private slots:
    void darkestLuminanceMapsToFirstCharacter();
    void lightestLuminanceMapsToLastCharacter();
    void indexIsMonotonicNonDecreasing();
    void emptyRampReturnsSpace();
    void clampsOutOfRangeLuminance();
    void standardHasExpectedCharacters();
    void denseHasExpectedCharacters();
    void blocksHasExpectedCharacters();
};

void TstCharacterRamp::darkestLuminanceMapsToFirstCharacter() {
    const CharacterRamp ramp = CharacterRamp::standard();
    QCOMPARE(ramp.characterForLuminance(0), ramp.characters().at(0));
}

void TstCharacterRamp::lightestLuminanceMapsToLastCharacter() {
    const CharacterRamp ramp = CharacterRamp::standard();
    QCOMPARE(ramp.characterForLuminance(255), ramp.characters().at(ramp.size() - 1));
}

void TstCharacterRamp::indexIsMonotonicNonDecreasing() {
    const CharacterRamp ramp = CharacterRamp::standard();
    int lastIndex = -1;
    for (int luminance = 0; luminance <= 255; ++luminance) {
        const QChar c = ramp.characterForLuminance(luminance);
        const int index = ramp.characters().indexOf(c);
        QVERIFY(index >= lastIndex);
        lastIndex = index;
    }
}

void TstCharacterRamp::emptyRampReturnsSpace() {
    const CharacterRamp ramp{QString()};
    QCOMPARE(ramp.characterForLuminance(128), QChar(' '));
}

void TstCharacterRamp::clampsOutOfRangeLuminance() {
    const CharacterRamp ramp = CharacterRamp::standard();
    QCOMPARE(ramp.characterForLuminance(-50), ramp.characterForLuminance(0));
    QCOMPARE(ramp.characterForLuminance(9000), ramp.characterForLuminance(255));
}

void TstCharacterRamp::standardHasExpectedCharacters() {
    QCOMPARE(CharacterRamp::standard().characters(), QStringLiteral("@#S%?*+;:,."));
}

void TstCharacterRamp::denseHasExpectedCharacters() {
    QCOMPARE(CharacterRamp::dense().characters(), QStringLiteral("@#8&o:*."));
}

void TstCharacterRamp::blocksHasExpectedCharacters() {
    QCOMPARE(CharacterRamp::blocks().size(), 4);
}

QTEST_MAIN(TstCharacterRamp)
#include "tst_character_ramp.moc"
