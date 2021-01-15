#include "operatorandlist.h"
#include <QStandardItemModel>
#include <QBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QStringListModel>
#include <QProgressBar>
#include <QHeaderView>
#include <QProcessEnvironment>
#include <QBuffer>
#include <QMessageBox>
#include <QVBoxLayout>

#include <thread>

OperatorAndList::OperatorAndList(QWidget *parent,View* view)
    : QWidget(parent),m_view(view)
{
    QVBoxLayout* vbox = new QVBoxLayout(this);
    QGridLayout* gbox = new QGridLayout();

    tableList = new QTableWidget(0,1);
    QStringList header;
    header<<"index";
    tableList->setHorizontalHeaderLabels(header);
    tableList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableList->horizontalHeader()->setStretchLastSection(true);

    connect(tableList,&QTableWidget::pressed,this,[this](const QModelIndex &index){
        m_view->dataFileChang(index,testPointsList[index.row()],4096);
    });

    clearButton = new QPushButton("clear");

    connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(clearButtonclicked(bool)));

    vbox->setMargin(2);
    vbox->addWidget(tableList);
    vbox->addLayout(gbox);
    gbox->addWidget(clearButton,0,0);

    connect(this,SIGNAL(signaldataFileCount(int)),m_view,SLOT(dataFileCount(int)));
    connect(this,SIGNAL(signaldataStatusClear()),m_view,SLOT(slotsdataStatusClear()));

    QStringList filetmp;
    QVector<QPointF> tmp;
    for(int i = 0;i<13;i++)
    {
        filetmp << "No."+QString::number(i);
        tmp.clear();
        for(int j = 0;j<4096;j++)
        {
            tmp.append(QPointF(j,qrand()%((i+1)*100)));
        }
        testPointsList.append(tmp);
    }
    slotDataFileList(filetmp);
    slotDataLoadFinish();
}

void OperatorAndList::clearButtonclicked(bool checked)
{
    emit signaldataStatusClear();
}

void OperatorAndList::slotDataFileList(QStringList list)
{
    for(const auto& fileitem : list)
    {
        int row = tableList->rowCount();
        tableList->insertRow(row);
        QTableWidgetItem *itemname = new QTableWidgetItem(fileitem);
        tableList->setItem(row, 0, itemname);
    }

    if(list.count() > 0)
    {
        tableList->selectRow(0);
        emit signaldataFileCount(list.count());
    }
}

void OperatorAndList::slotDataLoadFinish()
{
    if(tableList->rowCount() > 0)
    {
        emit tableList->pressed(tableList->currentIndex());
    }
}
