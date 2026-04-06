#ifndef COMBOBOXMODEL_H
#define COMBOBOXMODEL_H

#include <QObject>
#include <QStringList>

class ComboBoxModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList comboList READ comboList WRITE setComboList NOTIFY comboListChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setcurrentIndex NOTIFY currentIndexChanged)

public:
    ComboBoxModel(QObject *parent = 0);
    ComboBoxModel(const QStringList &list,int count, QObject *parent = 0);

    const QStringList comboList();
    void setComboList(const QStringList &comboList);

    int count();
    void setCount(int cnt);

    int currentIndex();
    void setcurrentIndex(int index);

    Q_INVOKABLE void addElement(const QString &element);
    Q_INVOKABLE void removeElement(int index);

signals:

    void comboListChanged();
    void countChanged();
    void currentIndexChanged();

public slots:


protected:
    QStringList m_comboList;
    int         m_count;
    int         m_currentIndex;
};

#endif // COMBOBOXMODEL_H
