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
#include <cstdlib>

class CouchOutput: public QObject {
	Q_OBJECT
private:
	QSet<QString> m_existingIds;
	QEventLoop m_eventLoop;
	bool m_bFirstDoc;
	QByteArray m_outputBuffer;

public:
	CouchOutput() {
		m_bFirstDoc = true;
		m_outputBuffer.append("{\n");
		m_outputBuffer.append("\"docs\": [\n");
	}

	void queryExistingObjects() {
		fprintf(stderr, "Querying OSM-IDs that already exist in the CouchDB...");
		QNetworkAccessManager http;
		QNetworkRequest request;
		request.setUrl(QUrl("http://127.0.0.1:5984/tagaprice/_design/osm/_view/all"));
		request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_queryRequestFinished(QNetworkReply*)));
		http.get(request);

		if (!m_eventLoop.exec()) {
			fprintf(stderr, "done\n");
		}
		else {
			// this doesn't get called yet
			fprintf(stderr, "failed!\n");
			exit(1);
		}
	}

	void addObject(const QVariant &entity) {
		QVariantMap osmMap = entity.toMap()["osm"].toMap();
		QString id = osmMap["_id"].toString();
		if (!m_existingIds.contains(id)) {
			if (!m_bFirstDoc) {
				m_outputBuffer.append(",\n");
			}
			else m_bFirstDoc = false;
			QJson::Serializer serializer;
			m_outputBuffer.append(serializer.serialize(entity).constData());
		}
		//else qDebug("Skipping %s", id.toLocal8Bit().constData());
	}

	bool save() {
		fprintf(stderr, "Storing the new OSM-Objects to the CouchDB...");
		QNetworkAccessManager http;
		QNetworkRequest request(QUrl("http://localhost:5984/tagaprice/_bulk_docs"));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_storeRequestFinished(QNetworkReply*)));

		m_outputBuffer.append("\n]\n}\n");
		http.post(request, m_outputBuffer);

		if (!m_eventLoop.exec()) {
			fprintf(stderr, "done\n");
			return true;
		}
		else {
			fprintf(stderr, "failed!\n");
			return false;
		}
	}

private slots:
	void _queryRequestFinished(QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError) {
			m_eventLoop.exit(reply->error());
			return;
		}
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

		delete reply;

		m_eventLoop.quit();
	}

	void _storeRequestFinished(QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError) {
			m_eventLoop.exit(reply->error());
			return;
		}

		delete reply;
		m_eventLoop.quit();
	}
};

#endif // COUCHOUTPUT_H
