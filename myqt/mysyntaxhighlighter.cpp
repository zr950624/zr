#include "mysyntaxhighlighter.h"

MySyntaxHighlighter::MySyntaxHighlighter(QTextDocument * parent) :
    QSyntaxHighlighter(parent)
{

}
void MySyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat myFormat;
    myFormat.setFontWeight(QFont::Bold);
    myFormat.setForeground(Qt::blue);
    QString string1 = "\\bint\\b";
    QRegExp expression(string1);
    int index = text.indexOf(expression);

    while(index>=0)
    {
        int lenght = expression.matchedLength();
        setFormat(index,lenght,myFormat);
        index = text.indexOf(expression,index+lenght);
    }
}
