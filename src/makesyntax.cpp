#include "makesyntax.h"

MakeSyntax::MakeSyntax(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

MakeSyntax::~MakeSyntax()
{
}

void MakeSyntax::highlightBlock(const QString &text)
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setForeground(Qt::darkMagenta);

    QRegExp expression("((\\w|/)+).cpp:(\\d*)|((\\w|/)+).h:(\\d*)");
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}
