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
protected:
	void run();
private:
	int readBytes(QByteArray *buf,int len,int timeout=1000);
	QSerialPort *m_port;
	QByteArray m_privBuffer;
	QList<QPair<QString,QString> > m_addressList;
	QString m_portName;
signals:
	void progress(quint64 current,quint64 total);
	void done();
public slots:
	
};

#endif // LOADERTHREAD_H
