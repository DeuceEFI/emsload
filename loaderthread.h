#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include <QThread>
#include <QPair>

#include "qserialport.h"


class LoaderThread : public QThread
{
	Q_OBJECT
public:
	explicit LoaderThread(QObject *parent = 0);
	void startLoad(QString filename,QString portname);
	void startRip(QString filename,QString portname);
protected:
	void run();
private:
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
	void done();
public slots:
	
};

#endif // LOADERTHREAD_H
