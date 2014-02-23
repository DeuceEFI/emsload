/************************************************************************************
 * EMSLoader - Open Source S12X Serial Monitor S19 Loader                           *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMSLoader                                                 *
 *                                                                                  *
 * EMSLoader is free software; you can redistribute it and/or modify                *
 * it under the terms of the GNU General Public License as published by             *
 * the Free Software Foundation, either version 3 of the License, or                *
 * (at your option) any later version.                                              *
 *                                                                                  *
 * EMSLoader is distributed in the hope that it will be useful,                     *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * General Public License for more details.                                         *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with EMSLoader.  If not, see <http://www.gnu.org/licenses/>.               *
 ************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "loaderthread.h"
#include "ui_mainwindow.h"
#include "s19file.h"

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
	void loaderDone(quint64 msecs);
	void selectSaveButtonClicked();
	void loadFileDone();
	void loaderError(QString error);
private:
	Ui::MainWindow ui;
	LoaderThread *m_loaderThread;
	S19File *m_loadedS19;
};

#endif // MAINWINDOW_H
