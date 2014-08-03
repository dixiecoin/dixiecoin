#include "freicoinunits.h"

#include <QStringList>

DixiecoinUnits::DixiecoinUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<DixiecoinUnits::Unit> DixiecoinUnits::availableUnits()
{
    QList<DixiecoinUnits::Unit> unitlist;
    unitlist.append(DXC);
    unitlist.append(mDXC);
    unitlist.append(uDXC);
    return unitlist;
}

bool DixiecoinUnits::valid(int unit)
{
    switch(unit)
    {
    case DXC:
    case mDXC:
    case uDXC:
        return true;
    default:
        return false;
    }
}

QString DixiecoinUnits::name(int unit)
{
    switch(unit)
    {
    case DXC: return QString("DXC");
    case mDXC: return QString("mDXC");
    case uDXC: return QString::fromUtf8("μDXC");
    default: return QString("???");
    }
}

QString DixiecoinUnits::description(int unit)
{
    switch(unit)
    {
    case DXC: return QString("Dixiecoins");
    case mDXC: return QString("Milli-Dixiecoins (1 / 1,000)");
    case uDXC: return QString("Micro-Dixiecoins (1 / 1,000,000)");
    default: return QString("???");
    }
}

mpq DixiecoinUnits::factor(int unit)
{
    switch(unit)
    {
    case uDXC: return mpq("100/1");
    case mDXC: return mpq("100000/1");
    default:
    case DXC:  return mpq("100000000/1");
    }
}

int DixiecoinUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case DXC: return 8; // <100,000,000 (# digits, without commas)
    case mDXC: return 11; // <100,000,000,000
    case uDXC: return 14; // <100,000,000,000,000
    default: return 0;
    }
}

int DixiecoinUnits::decimals(int unit)
{
    switch(unit)
    {
    case DXC: return 8;
    case mDXC: return 5;
    case uDXC: return 2;
    default: return 0;
    }
}

QString DixiecoinUnits::format(int unit, const mpq& n, bool fPlus)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    mpq q = n * COIN / factor(unit);
    std::string str = FormatMoney(q, fPlus);
    int diff = 8 - decimals(unit);
    if(diff > 0)
        str.erase(str.length() - diff, diff);
    return QString::fromStdString(str);
}

QString DixiecoinUnits::formatWithUnit(int unit, const mpq& amount, bool plussign)
{
    return format(unit, amount, plussign) + QString(" ") + name(unit);
}

bool DixiecoinUnits::parse(int unit, const QString &value, mpq *val_out)
{
    mpq ret_value;
    if (!ParseMoney(value.toStdString(), ret_value))
        return false;
    if(val_out)
    {
        *val_out = ret_value * factor(unit) / COIN;
    }
    return true;
}

int DixiecoinUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant DixiecoinUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}
