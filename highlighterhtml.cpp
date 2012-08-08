/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

 /**************************************************************************
 *  adapt par J. Coron pour JcEditHtml - Version pour Windows             *
 *  dveloppe avec Qt Open Source Edition version 4.1.1 - mars 2006       *
 ***************************************************************************/
 /****************************************************************************
 *port sous Linux avec Qt Open Source Edition version 4.2.1 - dcembre 2006 *
 ****************************************************************************/

#include <QtGui>

#include "highlighterhtml.h"

HighlighterHtml::HighlighterHtml(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(Qt::blue);
    QStringList keywordPatterns;
    keywordPatterns << "<" << ">"
                                          << "href=" << "name=" << "src="
                    << "class=" << "lang=" << "dir="
                    << "http-equiv=" << "content=" << "charset="
                    << "rel=" << "type=" << "id=" << "alt="
                    << "align=" << "valign=" << "face="
                    << "border=";
    foreach (QString pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("<![^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void HighlighterHtml::highlightBlock(const QString &text)
{
    foreach (HighlightingRule rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }
}
