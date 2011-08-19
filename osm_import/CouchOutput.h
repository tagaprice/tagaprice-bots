#ifndef COUCHOUTPUT_H
#define COUCHOUTPUT_H

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSet>

#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <cstdio>

class CouchOutput: public QObject {
	Q_OBJECT
private:
	QSet<QString> m_existingIds;
	QEventLoop m_eventLoop;

	QVariantList m_entityList;

public:
	CouchOutput() {
	}

	void queryExistingObjects() {
		QNetworkAccessManager http;
		QNetworkRequest request;
		request.setUrl(QUrl("http://127.0.0.1:5984/tagaprice/_design/osm/_view/all"));
		request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_queryRequestFinished(QNetworkReply*)));
		http.get(request);

		m_eventLoop.exec();
	}

	void addObject(const QVariant &entity) {
		QVariantMap osmMap = entity.toMap()["osm"].toMap();
		QString id = osmMap["_id"].toString();
		if (!m_existingIds.contains(id)) {
			m_entityList.append(entity);
		}
		//else qDebug("Skipping %s", id.toLocal8Bit().constData());
	}

	void print() {
		QJson::Serializer serializer;
		printf("%s\n", serializer.serialize(m_entityList).constData());
	}

private slots:
	void _queryRequestFinished(QNetworkReply *reply) {
		QByteArray json = reply->readAll();
		QJson::Parser parser;
		QVariantMap responseData = parser.parse(json).toMap();

		if (responseData.contains("rows")) {
			QVariantList list = responseData["rows"].toList();
			foreach (QVariant entry, list) {
				QVariantMap entryMap = entry.toMap();
				m_existingIds.insert(entryMap["key"].toString());
			}
		}

		m_eventLoop.quit();
	}

};

#endif // COUCHOUTPUT_H
