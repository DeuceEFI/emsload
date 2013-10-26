#include "serialmonitor.h"
#include <QDebug>

SerialMonitor::SerialMonitor(QObject *parent) : QObject(parent)
{
}
bool SerialMonitor::verifyBlock(unsigned short address,QByteArray block)
{
	m_port->write(QByteArray().append(0xA7).append(address >> 8).append(address).append(block.size()));
	m_port->waitForBytesWritten(1);
	QByteArray newpacket;
	int size = readBytes(&newpacket,block.size()+4); //Read 16 bytes, + 4 bytes of reponse.
	if (size != block.size()+4)
	{
		qDebug() << "Bad size read from verifyBlock" << size << block.size()+4;
		//qDebug() << "Error on page:" << "0x" + QString::number(page,16).toUpper() << "address:" << "0x" + QString::number(address,16).toUpper();
		return false;
	}
	else
	{
		for (int i=0;i<block.size();i++)
		{
			if (((unsigned char)newpacket[i]) != ((unsigned char)block[i]))
			{
				return false;
			}
		}
		return true;
	}

}

void SerialMonitor::closePort()
{
	m_port->close();
	delete m_port;
	m_port = 0;
}

bool SerialMonitor::openPort(QString portname)
{
	m_port = new QSerialPort();
	m_port->setPortName(portname);
	if (!m_port->open(QIODevice::ReadWrite))
	{
		qDebug() << "Unable to open port";
		return false;
	}
	m_port->setBaudRate(115200);
	m_port->setParity(QSerialPort::NoParity);
	return true;
}

bool SerialMonitor::verifySM()
{
	int retry = 0;
	while (retry++ <= 3)
	{
		//m_port->clear();
		//m_port->flush();
		//m_privBuffer.clear();
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
				return true;
			}
		}
		else if ((unsigned char)verifybuf[0] == 0xE1)
		{
			if ((unsigned char)verifybuf[2] == 0x3E)
			{
				qDebug() << "In SM mode two";
				return true;
			}
		}
		else
		{
			qDebug() << "Bad return:" << QString::number((unsigned char)verifybuf[0],16) << QString::number((unsigned char)verifybuf[2],16);
		}
	}
	//Timed out.
	return false;
}

bool SerialMonitor::selectPage(unsigned char page)
{
	m_port->write(QByteArray().append(0xA2).append((char)0x0).append(0x30).append(page));
	m_port->waitForBytesWritten(1);
	QByteArray newpacket;
	int ret = readBytes(&newpacket,3,1000);
	if (ret != 3)
	{
		qDebug() << "Bad return" << ret;
		return false;
	}
	else
	{
		if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
		{
			//qDebug() << "smVerify good";
			return true;
		}
		else
		{
			qDebug() << "Bad verify" << QString::number(newpacket[0],16).toUpper() << QString::number(newpacket[2],16).toUpper();
			qDebug() << "Verify len:" <<m_privBuffer.size();
			return false;
		}
	}
}

bool SerialMonitor::readBlockToS19(unsigned char page,unsigned short address,unsigned char reqsize,QString *returnval)
{
	m_port->write(QByteArray().append(0xA7).append(address >> 8).append(address).append(reqsize));
	m_port->waitForBytesWritten(1);
	QByteArray newpacket;
	int size = readBytes(&newpacket,reqsize+4); //Read 16 bytes, + 4 bytes of reponse.
	if (size != reqsize+4)
	{
		qDebug() << "Bad size read from S19";
		qDebug() << "Error on page:" << "0x" + QString::number(page,16).toUpper() << "address:" << "0x" + QString::number(address,16).toUpper();
		return false;
	}
	else
	{
		//qDebug() << "Read from page:" << "0x" + QString::number(page,16).toUpper() << "address:" << "0x" + QString::number(address,16).toUpper();
		QString record = "S214";
		record += ((page <= 0xF) ? "0" : "") + QString::number(page,16).toUpper();
		record += QString((address <= 0xFFF) ? "0" : "") + ((address <= 0xFF) ? "0" : "") + ((address <= 0xF) ? "0" : "") + QString::number(address,16).toUpper();
		unsigned char checksum = reqsize+4 + (page & 0xFF) + ((unsigned char)(address >> 8) & 0xFF) + ((unsigned char)address & 0xFF);
		bool blank = true;
		for (int i=0;i<reqsize;i++)
		{
			if ((unsigned char)newpacket[i] != 0xFF)
			{
				blank = false;
			}
			record += ((unsigned char)newpacket[i] <= 0xF ? "0" : "") + QString::number(((unsigned char)newpacket[i]),16).toUpper();
			checksum += (unsigned char)newpacket[i];
		}
		checksum = ~(checksum);
		record += (checksum <= 0xF ? "0" : "") + QString::number(checksum,16).toUpper();
		record += "\r\n";
		if (!blank) //Only write non blank records.
		{
			//output.write(record.toAscii());
			//output.flush();
			*returnval = record;
			return true;
		}
		*returnval = QString("");
		return true;
	}
}

bool SerialMonitor::writeBlock(unsigned short address,QByteArray block)
{
	QByteArray packet;
	packet.append(0xA8);
	packet.append((address >> 8) & 0xFF);
	packet.append((address) & 0xFF);
	packet.append(block.size()-1);
	packet.append(block);
	//qDebug() << "Writing" << internal.size() << "bytes to" << QString::number(address & 0xFFFF,16).toUpper();
	m_port->write(packet);
	m_port->waitForBytesWritten(1);

	QByteArray newpacket;
	int ret = readBytes(&newpacket,3,3000);
	if (ret != 3)
	{
		qDebug() << "Bad return" << ret;
		return false;
	}
	else
	{
		if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
		{
			//qDebug() << "smVerify good";
			return true;
		}
		else
		{
			qDebug() << "Bad verify on write block";
			qDebug() << QString::number((unsigned char)newpacket[0],16) << QString::number((unsigned char)newpacket[2],16);
			qDebug() << "Verify len:" <<m_privBuffer.size();
			return false;
		}
	}
}

bool SerialMonitor::eraseBlock()
{
	m_port->write(QByteArray().append(0xB8));
	m_port->waitForBytesWritten(1);
	QByteArray newpacket;
	int size = readBytes(&newpacket,3);
	if (size == 3)
	{
		if ((unsigned char)newpacket[0] ==  0xE0 && (unsigned char)newpacket[2] == 0x3E)
		{
			//qDebug() << "Read:" << QString::number((unsigned char)newpacket[0],16) << QString::number((unsigned char)newpacket[2],16);
			return true;
		}
		else
		{
			qDebug() << "Bad verify on erase block";
			return false;
		}
	}
	else
	{
		qDebug() << "Bad read on erase block:" << size;
		return false;
	}
}

int SerialMonitor::readBytes(QByteArray *buf,int len,int timeout)
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

void SerialMonitor::sendReset()
{
	m_port->write(QByteArray().append(0xB4)); //reset
	m_port->waitForBytesWritten(1);
}
