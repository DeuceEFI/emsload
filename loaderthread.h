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

#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include <QThread>
#include <QPair>
#include "s19file.h"

#include "qserialport.h"


class LoaderThread : public QThread
{
	Q_OBJECT
public:
	explicit LoaderThread(QObject *parent = 0);
	//void startLoad(QString filename,QString portname);
	void startLoad(S19File *file,QString portname);
	void startRip(QString filename,QString portname);
protected:
	void run();
private:
	void clearBuffers();
	S19File *m_s19File;
	int readBytes(QByteArray *buf,int len,int timeout=1000);
	QSerialPort *m_port;
	QByteArray m_privBuffer;
	bool verifyBlock(unsigned short address,QByteArray block);
	bool openPort();
	bool verifySM();
	QList<QPair<QString,QString> > m_addressList;
	QString m_portName;
	QString operation;
	QString m_fwFileName;
	bool selectPage(unsigned char page);
	bool readBlockToS19(unsigned char page,unsigned short address,unsigned char reqsize,QString *returnval);
	bool writeBlock(unsigned short address,QByteArray block);
	bool eraseBlock();
signals:
	void progress(quint64 current,quint64 total);
	void done(quint64 msecs);
	void error(QString error);
public slots:
	
};

#endif // LOADERTHREAD_H
