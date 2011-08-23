#ifndef COUCHOUTPUT_H
#define COUCHOUTPUT_H

#include "Settings.h"

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

	void init() {
		checkTapUser();
		queryExistingObjects();
	}

	void checkTapUser() {
		fprintf(stderr, "Checking if the UID specified in the config.ini exists and is valid...");
		QNetworkAccessManager http;
		QNetworkRequest request;
		request.setUrl(QUrl(Settings::getCouchBaseUrl().toString().append("/"+Settings::getBotUID())));
		request.setRawHeader("User-Agent", "OsmImport");
		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_queryRequestFinished(QNetworkReply*)));
		http.get(request);

		int rc = m_eventLoop.exec();
		if (!rc) {
			fprintf(stderr, "done\n");
		}
		else {
			fprintf(stderr, "failed (code %i)!\n", rc);
			exit(rc);
		}
	}

	void queryExistingObjects() {
		fprintf(stderr, "Querying OSM-IDs that already exist in the CouchDB...");
		QNetworkAccessManager http;
		QNetworkRequest request;
		request.setUrl(QUrl(Settings::getCouchBaseUrl().toString().append("/_design/osm/_view/all")));
		request.setRawHeader("User-Agent", "OsmImport");
		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_queryRequestFinished(QNetworkReply*)));
		http.get(request);

		int rc = m_eventLoop.exec();
		if (!rc) {
			fprintf(stderr, "done\n");
		}
		else {
			fprintf(stderr, "failed (code %i)!\n", rc);
			exit(rc);
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
		QNetworkRequest request(QUrl(Settings::getCouchBaseUrl().toString().append("/_bulk_docs")));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		connect(&http, SIGNAL(finished(QNetworkReply*)), this, SLOT(_storeRequestFinished(QNetworkReply*)));

		m_outputBuffer.append("\n]\n}\n");
		http.post(request, m_outputBuffer);

		int rc = m_eventLoop.exec();
		if (!rc) {
			fprintf(stderr, "done\n");
		}
		else {
			fprintf(stderr, "failed (code %i)!\n", rc);
		}
		return rc == 0;
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

	void _userRequestFinished(QNetworkReply *reply) {
		int rc;

		if (reply->error() != QNetworkReply::NoError) {
			m_eventLoop.exit(reply->error());
			return;
		}

		QJson::Parser parser;
		QVariantMap userObject = parser.parse(reply->readAll()).toMap();
		if (userObject.contains("docType") && userObject["docType"] == "user") {
			rc = 0;
		}
		else {
			rc = 1;
		}

		delete reply;
		m_eventLoop.exit(rc);
	}
};

#endif // COUCHOUTPUT_H
