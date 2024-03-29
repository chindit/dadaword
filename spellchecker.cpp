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
#include <QStringList>
#include <QVector>
#include <QDebug>

#include "hunspell.hxx"
#include "errorManager.h"

SpellChecker::SpellChecker(const QString &dictionaryPath, const QString &userDictionary)
{
    UserDictionary = userDictionary;
    dictionnaire_standard = dictionaryPath;
    ErrorManager instance_erreur;
    QString dictFile;
    QString affixFile;
    if(!dictionaryPath.endsWith(".dic")){
        dictFile = dictionaryPath + ".dic";
        affixFile = dictionaryPath + ".aff";
    }
    else{
        dictFile = dictionaryPath;
        QString temp = dictionaryPath;
        temp.remove(".dic");
        affixFile = temp + ".aff";
    }
    QByteArray dictFilePathBA = dictFile.toLocal8Bit();
    QByteArray affixFilePathBA = affixFile.toLocal8Bit();
    instance_hunspell = new Hunspell(affixFilePathBA.constData(), dictFilePathBA.constData());

    // detect encoding analyzing the SET option in the affix file
    encodage = "UTF-8";
    QFile _affixFile(affixFile);
    if (_affixFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&_affixFile);
        QRegularExpression enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*");
        for(QString line = stream.readLine(); !line.isEmpty(); line = stream.readLine()) {
            if (enc_detector.match(line).capturedLength() > 0) {
                instance_erreur.Erreur_msg(tr("Encodage par défaut : ") + enc_detector.match(line).captured(), QMessageBox::Ignore);
                break;
            }
        }
        _affixFile.close();
    }
    if(!UserDictionary.isEmpty()){
        QFile userDictonaryFile(UserDictionary);
        if(userDictonaryFile.open(QIODevice::ReadOnly)){
            QTextStream stream(&userDictonaryFile);
            for(QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine())
                put_word(word);
            userDictonaryFile.close();
        }
        else{
            instance_erreur.Erreur_msg(tr("Le dictionnaire ")+UserDictionary+tr(" n'a pu être ouvert"), QMessageBox::Information);
        }
    }
    else{
        instance_erreur.Erreur_msg(tr("Dictionnaire utilisateur non défini."), QMessageBox::Information);
    }
}


SpellChecker::~SpellChecker()
{
    delete instance_hunspell;
}


bool SpellChecker::spell(const QString &word)
{
    // Encode from Unicode to the encoding used by current dictionary
    return instance_hunspell->spell(word.toStdString()) != 0;
}


QStringList SpellChecker::suggest(const QString &word)
{
    // Encode from Unicode to the encoding used by current dictionary
    std::vector<std::string> numSuggestions = instance_hunspell->suggest(word.toStdString());
    QVector<std::string> vectorWithStdString = QVector<std::string>(numSuggestions.begin(), numSuggestions.end());
    QList<std::string> listWithStdString = QList<std::string>::fromVector(vectorWithStdString);
    QStringList suggestions = QStringList();
    QList<std::string>::iterator i;
    for (i = listWithStdString.begin(); i != listWithStdString.end(); ++i) {
        suggestions.append(QString::fromStdString(*i));
    }

    return suggestions;
}


void SpellChecker::ignoreWord(const QString &word)
{
    put_word(word);
}


void SpellChecker::put_word(const QString word){
    instance_hunspell->add(word.toStdString());
    return;
}


void SpellChecker::addToUserWordlist(const QString &word)
{
    put_word(word);
    if(!UserDictionary.isEmpty()) {
        QFile userDictonaryFile(UserDictionary);
        if(userDictonaryFile.open(QIODevice::Append)) {
            QTextStream stream(&userDictonaryFile);
            stream << word << "\n";
            userDictonaryFile.close();
        } else {
            qWarning() << "User dictionary in " << UserDictionary << "could not be opened for appending a new word";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}
