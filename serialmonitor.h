#ifndef SERIALMONITOR_H
#define SERIALMONITOR_H

#include <QObject>
#include "qserialport.h"

class SerialMonitor : public QObject
{
	Q_OBJECT
public:
	explicit SerialMonitor(QObject *parent = 0);
	bool verifyBlock(unsigned short address,QByteArray block);
	bool openPort(QString portname);
	bool verifySM();
	bool selectPage(unsigned char page);
	bool readBlockToS19(unsigned char page,unsigned short address,unsigned char reqsize,QString *returnval);
	bool writeBlock(unsigned short address,QByteArray block);
	bool eraseBlock();
	void closePort();
	void sendReset();
private:
	int readBytes(QByteArray *buf,int len,int timeout=1000);
	QSerialPort *m_port;
	QByteArray m_privBuffer;
signals:
	
public slots:
	
};

#endif // SERIALMONITOR_H
