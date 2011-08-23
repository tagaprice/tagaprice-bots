#include "SyncHttpRequest.h"

SyncHttpRequest::SyncHttpRequest() {
	m_lastReply = 0;

	connect(&m_http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_requestFinished(QNetworkReply*)));
}

void SyncHttpRequest::_requestFinished(QNetworkReply *reply) {
	m_lastReply = reply;
	m_eventLoop.quit();
}

QNetworkReply* SyncHttpRequest::_waitForReply() {
	QNetworkReply *rc = 0;

	m_eventLoop.exec();

	if (m_lastReply) {
		rc = m_lastReply;
		m_lastReply = 0;
	}

	return rc;
}

QNetworkReply* SyncHttpRequest::deleteResource(const QNetworkRequest &request) {
	m_http.deleteResource(request);
	return _waitForReply();
}

QNetworkReply* SyncHttpRequest::get(const QNetworkRequest &request) {
	m_http.get(request);
	return _waitForReply();
}

QNetworkReply* SyncHttpRequest::post(const QNetworkRequest &request, const QByteArray &data) {
	m_http.post(request, data);
	return _waitForReply();
}

QNetworkReply* SyncHttpRequest::post(const QNetworkRequest &request, QIODevice *data) {
	m_http.post(request, data);
	return _waitForReply();
}

QNetworkReply* SyncHttpRequest::put(const QNetworkRequest &request, const QByteArray &data) {
	m_http.put(request, data);
	return _waitForReply();
}

QNetworkReply* SyncHttpRequest::put(const QNetworkRequest &request, QIODevice *data) {
	m_http.put(request, data);
	return _waitForReply();
}
