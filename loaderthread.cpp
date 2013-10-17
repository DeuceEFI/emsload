#include "loaderthread.h"
#include <QFile>
#include <QDebug>
LoaderThread::LoaderThread(QObject *parent) : QThread(parent)
{
}

/*
 +-------------------//------------------//-----------------------------+
 | type | count | address  |            data                 | checksum |
 +-------------------//------------------//-----------------------------+
 S2     14      F880B0    B746B7C687B7467D3044FD30421AEEFD      60
 */
//^^ shamelessly stolen from Seank's loader source :)
void LoaderThread::run()
{
	m_port = new QSerialPort();
	m_port->setPortName(m_portName);
	if (!m_port->open(QIODevice::ReadWrite))
	{
		qDebug() << "Unable to open port";
		return;
	}
	m_port->setBaudRate(115200);
	m_port->setParity(QSerialPort::NoParity);
	m_port->write(QByteArray().append(0x0D));
	m_port->waitForBytesWritten(1);
	QByteArray verifybuf;
	int verifylen = readBytes(&verifybuf,3);
	qDebug() << "Verify len:" << verifylen;
	if ((unsigned char)verifybuf[0] == 0xE0)
	{
		if ((unsigned char)verifybuf[2] == 0x3E)
		{
			qDebug() << "In SM mode";
		}
	}
	else if ((unsigned char)verifybuf[0] == 0xE1)
	{
		if ((unsigned char)verifybuf[2] == 0x3E)
		{
			qDebug() << "In SM mode two";
		}
	}
	else
	{
		qDebug() << "Bad return:" << QString::number((unsigned char)verifybuf[0],16) << QString::number((unsigned char)verifybuf[2],16);
		m_port->close();
		delete m_port;
		return;
	}

	unsigned char currpage = 0;
	for (int i=0;i<m_addressList.size();i++)
	{
		emit progress(i,m_addressList.size());
		bool ok = false;
		unsigned int address = m_addressList[i].first.toInt(&ok,16);
		QByteArray internal;
		for (int j=0;j<m_addressList[i].second.size()-3;j+=2)
		{
			internal.append(QString(m_addressList[i].second[j]).append(m_addressList[i].second[j+1]).toInt(&ok,16));
		}
		unsigned char newpage = (address >> 16) & 0xFF;
		QByteArray packet;
		QByteArray newpacket;
		if (newpage != currpage)
		{
			currpage = newpage;
			qDebug() << "Selecting page:" << currpage;
			m_port->write(QByteArray().append(0xA2).append((char)0x0).append(0x30).append(currpage));
			m_port->waitForBytesWritten(1);
			int ret = readBytes(&newpacket,3,1000);
			if (ret != 3)
			{
				qDebug() << "Bad return" << ret;
				return;
			}
			else
			{
				if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
				{
					//qDebug() << "smVerify good";
				}
				else
				{
					qDebug() << "Bad verify";
				}
			}
			m_port->write(QByteArray().append(0xB8));
			m_port->waitForBytesWritten(1);
			int size = readBytes(&newpacket,3);
			if (size == 3)
			{
		if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
		{
		    //qDebug() << "Read:" << QString::number((unsigned char)newpacket[0],16) << QString::number((unsigned char)newpacket[2],16);
		}
		else
		{
		    qDebug() << "Bad verify";
		}
			}
			else
			{
				qDebug() << "Bad read:" << size;
			}
		}


		packet.append(0xA8);
		packet.append((address >> 8) & 0xFF);
		packet.append((address) & 0xFF);
		packet.append(internal.size()-1);
		packet.append(internal);
		//qDebug() << "Writing" << internal.size() << "bytes to" << QString::number(address & 0xFFFF,16).toUpper();
		m_port->write(packet);
		m_port->waitForBytesWritten(1);

		int ret = readBytes(&newpacket,3,3000);
		if (ret != 3)
		{
			qDebug() << "Bad return" << ret;
			return;
		}
		else
		{
			if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
			{
				//qDebug() << "smVerify good";
			}
			else
			{
				qDebug() << "Bad verify";
				qDebug() << QString::number((unsigned char)newpacket[0],16) << QString::number((unsigned char)newpacket[2],16);
			}
		}
		//msleep(1000);
	}
	m_port->close();
	delete m_port;
	m_port = 0;
	emit done();
}

void LoaderThread::startLoad(QString load,QString com)
{
	qDebug() << "Loading:" << load;
	qDebug() << "Com port:" << com;
	m_portName = com;
	QFile fwfile(load);
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
				m_addressList.append(QPair<QString,QString>(address,data));
				//qDebug() << "Address:" << address << "Line" << data;
			}
		}
	}
	fwfile.close();
	qDebug() << m_addressList.size() << "records loaded";
	start();
}

int LoaderThread::readBytes(QByteArray *buf,int len,int timeout)
{
	if (m_privBuffer.size() >= len)
	{
		*buf = m_privBuffer.mid(0,len);
		m_privBuffer.remove(0,len);
		return len;
	}
	while (m_port->waitForReadyRead(timeout))
	{
		m_privBuffer.append(m_port->readAll());
		//qDebug() << "Read:" << m_privBuffer.size();
		if (m_privBuffer.size() >= len)
		{
			*buf = m_privBuffer.mid(0,len);
			m_privBuffer.remove(0,len);
			return len;
		}
	}
	return 0;
}
