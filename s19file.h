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

#ifndef S19FILE_H
#define S19FILE_H

#include <QThread>
#include <QString>
#include <QPair>
#include <QByteArray>

class S19File : public QThread
{
	Q_OBJECT
public:
	S19File();
	const QPair<unsigned int,QByteArray>& getCompactRecord(int index);
	const QPair<unsigned int,QByteArray>& getRecord(int index);
	int getCompactRecordCount();
	int getRecordCount();
	void loadFile(QString filename);
	quint64 getTotalSize() { return m_totalBytes; }
private:
	QString m_fileName;
	QList<QPair<unsigned int,QByteArray> > m_compactRecordList;
	QList<QPair<unsigned int,QByteArray> > m_recordList;
	quint64 m_totalBytes;
protected:
	void run();
signals:
	void done();
};

#endif // S19FILE_H
