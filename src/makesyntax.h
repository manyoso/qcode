#ifndef MAKESYNTAX_H
#define MAKESYNTAX_H

#include <QSyntaxHighlighter>

class MakeSyntax : public QSyntaxHighlighter {
    Q_OBJECT

public:
    MakeSyntax(QTextDocument *parent = 0);
    virtual ~MakeSyntax();

 protected:
     virtual void highlightBlock(const QString &text);
};

#endif
