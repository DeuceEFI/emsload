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

#include "s19file.h"

#include <QFile>
#include <QDebug>

S19File::S19File() : QThread()
{
}

void S19File::loadFile(QString filename)
{
	m_fileName = filename;
	start();
}

const QPair<unsigned int,QByteArray>& S19File::getCompactRecord(int index)
{
	if (index >= 0 && index < m_compactRecordList.size())
	{
		return m_compactRecordList[index];
	}
	else
	{
		return QPair<unsigned int,QByteArray>(); //Return empty record for an error.
	}
}

const QPair<unsigned int,QByteArray>& S19File::getRecord(int index)
{
	if (index >= 0 && index < m_recordList.size())
	{
		return m_recordList[index];
	}
	else
	{
		return QPair<unsigned int,QByteArray>(); //Return empty record for an error.
	}
}

int S19File::getCompactRecordCount()
{
	return m_compactRecordList.size();
}

int S19File::getRecordCount()
{
	return m_recordList.size();
}

//The run loop is called to load a file
void S19File::run()
{
	QList<QPair<QString,QString> > stringRecordList; //Temporary storage, we want to store addresses in actual format.
	QFile fwfile(m_fileName);
	fwfile.open(QIODevice::ReadOnly);
	while (!fwfile.atEnd())
	{
		QString line = fwfile.readLine().replace("\r","").replace("\n","");
		if (line.startsWith("S2"))
		{
			bool ok = false;
			int linesize = line.mid(2,2).toInt(&ok,16);
			if ((linesize * 2) != line.length()-4)
			{
				qDebug() << "Bad Line" << linesize << line.length();
			}
			else
			{
				QString address = line.mid(4,6);
				QString data = line.mid(10);
				stringRecordList.append(QPair<QString,QString>(address,data));
				//qDebug() << "Address:" << address << "Line" << data;
			}
		}
	}
	fwfile.close();
	qDebug() << stringRecordList.size() << "records loaded";

	m_recordList.clear(); //Converted to actual address (including page) and actual bytes

	m_totalBytes = 0;

	for (int i=0;i<stringRecordList.size();i++)
	{
		bool ok = false;
		unsigned int address = stringRecordList[i].first.toInt(&ok,16);
		QByteArray internal;
		for (int j=0;j<stringRecordList[i].second.size()-3;j+=2)
		{
			internal.append(QString(stringRecordList[i].second[j]).append(stringRecordList[i].second[j+1]).toInt(&ok,16));
		}
		m_recordList.append(QPair<unsigned int,QByteArray>(address,internal));
		m_totalBytes += internal.size();
	}
	qDebug() << "Total bytes to write:" << m_totalBytes;

	m_compactRecordList.clear(); //Condensed, so sequential entries in the S19 are put to a single entry.

	m_compactRecordList.append(QPair<unsigned int,QByteArray>(m_recordList[0].first,m_recordList[1].second));
	for (int i=1;i<m_recordList.size();i++)
	{
		if (m_recordList[i-1].first + m_recordList[i-1].second.size() == m_recordList[i].first)
		{
			//Last address is before our current address. add our current to the last.
			m_compactRecordList[m_compactRecordList.size()-1].second.append(m_recordList[i].second);
		}
		else
		{
			m_compactRecordList.append(QPair<unsigned int,QByteArray>(m_recordList[i].first,m_recordList[i].second));
		}
	}
	emit done();


}
