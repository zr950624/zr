#ifndef MYSYNTAXHIGHLIGHTER_H
#define MYSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class MySyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit MySyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);
signals:
    
public slots:
    
};

#endif // MYSYNTAXHIGHLIGHTER_H
