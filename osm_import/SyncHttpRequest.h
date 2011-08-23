#ifndef SYNCHTTPREQUEST_H
#define SYNCHTTPREQUEST_H

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SyncHttpRequest: public QObject {
private:
	Q_OBJECT

	QEventLoop m_eventLoop;
	QString m_errorString;
	QNetworkAccessManager m_http;
	QNetworkReply *m_lastReply;

	QNetworkReply* _waitForReply();
public:
	SyncHttpRequest();

	QString errorString() {
		return m_errorString;
	}

#if 0
	QNetworkReply deleteResource(const QUrl &url);
	QNetworkReply get(const QUrl &url);
	QNetworkReply post(const QUrl &url);
	QNetworkReply put(const QUrl &url);
#endif

	QNetworkReply* deleteResource(const QNetworkRequest &request);
	QNetworkReply* get(const QNetworkRequest &request);
	QNetworkReply* post(const QNetworkRequest &request, const QByteArray &data);
	QNetworkReply* post(const QNetworkRequest &request, QIODevice *data);
	QNetworkReply* put(const QNetworkRequest &request, const QByteArray &data);
	QNetworkReply* put(const QNetworkRequest &request, QIODevice *data);


private slots:
	void _requestFinished(QNetworkReply *reply);
};

#endif // SYNCHTTPREQUEST_H
