#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "loaderthread.h"

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private slots:
	void comPortRefresh();
	void loadButtonClicked();
	void selectFileButtonClicked();
	void loaderProgress(quint64 current,quint64 total);
	void loaderDone();
private:
	Ui::MainWindow ui;
	LoaderThread *m_loaderThread;
};

#endif // MAINWINDOW_H
