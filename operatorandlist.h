#ifndef OPERATORANDLIST_H
#define OPERATORANDLIST_H

#include <QWidget>
#include <QListView>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <QTimer>
#include <QObject>
#include "view.h"
class OperatorAndList : public QWidget
{
    Q_OBJECT
public:
    explicit OperatorAndList(QWidget *parent = nullptr,View* view = nullptr);

signals:
    void signaldataFileCount(int count);
    void signaldataClean();
    void signaldataStatusClear();

public slots:
    void slotDataFileList(QStringList list);
    void slotDataLoadFinish();

private:
    QTableWidget* tableList;
    QPushButton* clearButton;

    View* m_view;

    QList<QVector<QPointF>> testPointsList;
private slots:
    void clearButtonclicked(bool checked);
};

#endif // OPERATORANDLIST_H
