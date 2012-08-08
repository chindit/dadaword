/*
Copyright (c) 2011, Volker Götz
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 01/08/12
  Merci de ne pas supprimer cette notice.
  Toutes les modifications apportées à ce document (disponible ici : http://qt-project.org/wiki/Spell-Checking-with-Hunspell )
  sont soumises à la licence générale du programme (GPL v3)
  */

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QString>
#include <QApplication>

class Hunspell;

class SpellChecker : public QObject{

   Q_OBJECT

public:
    SpellChecker(const QString &dictionaryPath, const QString &userDictionary);
    ~SpellChecker();

    bool spell(const QString &word);
    QStringList suggest(const QString &word);
    void ignoreWord(const QString &word);
    void addToUserWordlist(const QString &word);
    QString replacement() const;

private:
    void put_word(const QString &word);
    Hunspell *instance_hunspell;
    QString UserDictionary;
    QString encodage;
    QTextCodec *codec;
};

#endif // SPELLCHECKER_H
