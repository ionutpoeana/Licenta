#ifndef QTCONSTANTS_H
#define QTCONSTANTS_H

#include <QMetaEnum>

#include <string>






template<typename QEnum>
QString QtEnumToString (const QEnum value)
{
    return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
}

#endif // QTCONSTANTS_H
