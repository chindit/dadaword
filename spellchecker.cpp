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
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 15/08/12
  Merci de ne pas supprimer cette notice.
  Toutes les modifications apportées à ce document (disponible ici : http://qt-project.org/wiki/Spell-Checking-with-Hunspell )
  sont soumises à la licence générale du programme (GPL v3)
  */

#include "spellchecker.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QVector>
#include <QDebug>

#include "hunspell/hunspell.hxx"
#include "errorManager.h"

SpellChecker::SpellChecker(const QString &dictionaryPath, const QString &userDictionaryPath) {
    userDictionary = userDictionaryPath;

    QString dictionaryFile;
    QString affixFile;
    if (!dictionaryPath.endsWith(".dic")) {
        dictionaryFile = dictionaryPath + ".dic";
        affixFile = dictionaryPath + ".aff";
    } else {
        dictionaryFile = dictionaryPath;
        affixFile = QString(dictionaryPath).remove(".dic") + ".aff";
    }
    QByteArray dictFilePathBA = dictionaryFile.toLocal8Bit();
    QByteArray affixFilePathBA = affixFile.toLocal8Bit();
    hunspell = new Hunspell(affixFilePathBA.constData(), dictFilePathBA.constData());

    // detect encoding analyzing the SET option in the affix file
    QString encoding = "UTF-8";
    QFile _affixFile(affixFile);
    if (_affixFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&_affixFile);
        QRegExp encodingDetector(R"(^\s*SET\s+([A-Z0-9\-]+)\s*)", Qt::CaseInsensitive);
        for(QString line = stream.readLine(); !line.isEmpty(); line = stream.readLine()) {
            if (encodingDetector.indexIn(line) > -1) {
                encoding = encodingDetector.cap(1);
                qDebug() << "Default encoding: " << encoding;
                break;
            }
        }
        _affixFile.close();
    }
    codec = QTextCodec::codecForName(encoding.toLatin1().constData());

    if (!userDictionary.isEmpty()) {
        QFile userDictionaryFile(userDictionary);
        if (userDictionaryFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&userDictionaryFile);
            for (QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine()) {
                addWordToIgnoreList(word);
            }
            userDictionaryFile.close();
        } else {
            qInfo() << "Unable to open user dictionary";
        }
    } else {
        qInfo() << "Undefined user dictionary";
    }
}


SpellChecker::~SpellChecker()
{
    delete hunspell;
}


bool SpellChecker::spell(const QString &word)
{
    // Encode from Unicode to the encoding used by current dictionary
    return hunspell->spell(std::string(codec->fromUnicode(word).constData())) != 0;
}


QStringList SpellChecker::suggest(const QString &word)
{
    // Encode from Unicode to the encoding used by current dictionary
    std::vector<std::string> numSuggestions = hunspell->suggest(std::string(codec->fromUnicode(word).constData()));
    QVector<std::string> vectorWithStdString = QVector<std::string>::fromStdVector(numSuggestions);
    QList<std::string> listWithStdString = QList<std::string>::fromVector(vectorWithStdString);
    QStringList suggestions = QStringList();
    QList<std::string>::iterator i;
    for (i = listWithStdString.begin(); i != listWithStdString.end(); ++i) {
        suggestions.append(QString::fromStdString(*i));
    }

    return suggestions;
}


void SpellChecker::addWordToIgnoreList(const QString &word) {
    hunspell->add(std::string(codec->fromUnicode(word).constData()));
}


void SpellChecker::addToUserWordList(const QString &word) {
    addWordToIgnoreList(word);

    if (!userDictionary.isEmpty()) {
        QFile userDictionaryFile(userDictionary);
        if (userDictionaryFile.open(QIODevice::Append)) {
            QTextStream stream(&userDictionaryFile);
            stream << word << "\n";
            userDictionaryFile.close();
        } else {
            qWarning() << "User dictionary in " << userDictionary << "could not be opened for appending a new word";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}
