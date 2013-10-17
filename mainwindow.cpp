#include "mainwindow.h"
#include <qserialportinfo.h>
#include <QTimer>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	QTimer *porttimer = new QTimer(this);
	connect(porttimer,SIGNAL(timeout()),this,SLOT(comPortRefresh()));
	porttimer->start(1000);

	connect(ui.loadFirmwarePushButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
	connect(ui.selectFirmwarePushButton,SIGNAL(clicked()),this,SLOT(selectFileButtonClicked()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::comPortRefresh()
{
	QString currentport = ui.portNameComboBox->currentText();
	int currentindex = -1;
	ui.portNameComboBox->clear();
	QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
	for (int i=0;i<portlist.size();i++)
	{
		ui.portNameComboBox->addItem(portlist[i].portName(),QVariant(portlist[i].systemLocation()));
		//qDebug() << portlist[i].portName() << portlist[i].systemLocation() << portlist[i].description();
		if (portlist[i].portName() == currentport)
		{
			currentindex = i;
		}
	}
	if (currentindex >= 0)
	{
		ui.portNameComboBox->setCurrentIndex(currentindex);
	}

}
void MainWindow::loadButtonClicked()
{
	m_loaderThread = new LoaderThread();
	connect(m_loaderThread,SIGNAL(progress(quint64,quint64)),this,SLOT(loaderProgress(quint64,quint64)));
	connect(m_loaderThread,SIGNAL(done()),this,SLOT(loaderDone()));
	m_loaderThread->startLoad(ui.lineEdit->text(),ui.portNameComboBox->itemData(ui.portNameComboBox->currentIndex()).toString());
}

void MainWindow::selectFileButtonClicked()
{

}
void MainWindow::loaderProgress(quint64 current,quint64 total)
{
	if (ui.progressBar->maximum() != total)
	{
		ui.progressBar->setMaximum(total);
	}
	ui.progressBar->setValue(current);
}

void MainWindow::loaderDone()
{
	ui.progressBar->setValue(ui.progressBar->maximum());

}
