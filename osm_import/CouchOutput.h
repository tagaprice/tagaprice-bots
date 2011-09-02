#ifndef COUCHOUTPUT_H
#define COUCHOUTPUT_H

#include "Settings.h"
#include "SyncHttpRequest.h"

#include <QEventLoop>
#include <QSet>

#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <cstdio>
#include <cstdlib>

class CouchOutput: public QObject {
	Q_OBJECT
private:
	QSet<QString> m_existingIds;
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
		SyncHttpRequest http;
		QNetworkRequest request;
		request.setUrl(QUrl(Settings::getCouchBaseUrl().toString().append("/"+Settings::getBotUID())));
		request.setRawHeader("User-Agent", "OsmImport");
		QNetworkReply *reply = http.get(request);

		if (reply->error() != QNetworkReply::NoError) {
			fprintf(stderr, "failed (%s)!\n", reply->errorString().toLocal8Bit().constData());
			exit(1);
		}

		QJson::Parser parser;
		QVariantMap userObject = parser.parse(reply->readAll()).toMap();
		if (userObject.contains("docType") && userObject["docType"] == "user") {
			fprintf(stderr, "done\n");
		}
		else {
			fprintf(stderr, "failed (not a TaP-user)");
		}

		delete reply;
	}

	void queryExistingObjects() {
		fprintf(stderr, "Querying OSM-IDs that already exist in the CouchDB...");
		SyncHttpRequest http;
		QNetworkRequest request;
		request.setUrl(QUrl(Settings::getCouchBaseUrl().toString().append("/_design/osm/_view/all")));
		request.setRawHeader("User-Agent", "OsmImport");
		QNetworkReply *reply = http.get(request);

		if (reply->error() != QNetworkReply::NoError) {
			fprintf(stderr, "failed (%s)!\n", reply->errorString().toLocal8Bit().constData());
			exit(1);

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
		fprintf(stderr, "done\n");
	}

	bool addObject(const QVariant &entity) {
		QVariantMap osmMap = entity.toMap()["osm"].toMap();
		QString id = osmMap["_id"].toString();
		bool rc = false;

		if (!m_existingIds.contains(id)) {
			if (!m_bFirstDoc) {
				m_outputBuffer.append(",\n");
			}
			else m_bFirstDoc = false;
			QJson::Serializer serializer;
			QByteArray serialized = serializer.serialize(entity).constData();
			m_outputBuffer.append(serialized);
			rc = true;
		}
		return rc;
	}

	bool save() {
		fprintf(stderr, "Storing the new OSM-Objects to the CouchDB...");
		SyncHttpRequest http;
		QNetworkRequest request(QUrl(Settings::getCouchBaseUrl().toString().append("/_bulk_docs")));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

		m_outputBuffer.append("\n]\n}\n");
		QNetworkReply *reply = http.post(request, m_outputBuffer);

		if (reply->error() != QNetworkReply::NoError) {
			fprintf(stderr, "failed (%s)!\n", reply->errorString().toLocal8Bit().constData());
			return false;
		}

		delete reply;

		fprintf(stderr, "done\n");
		return true;
	}

	static bool hasCategory(const QString &categoryId) {
		SyncHttpRequest http;
		QString baseUrl = QUrl(Settings::getCouchBaseUrl().toString().append("/"+categoryId)).toString();
		//qDebug("QueryUrl: %s", baseUrl.toLocal8Bit().constData());
		QNetworkRequest request(QUrl(Settings::getCouchBaseUrl().toString().append("/"+categoryId)));
		QNetworkReply *reply = http.get(request);

		if (reply->error() == QNetworkReply::ContentNotFoundError) {
			return false;
		}
		else if (reply->error() != QNetworkReply::NoError) {
			return false;
		}

		QJson::Parser parser;
		QVariantMap categoryObject = parser.parse(reply->readAll()).toMap();
		if (categoryObject.contains("docType") && categoryObject["docType"] == "shopCategory") {
			// all went well
		}
		else {
			delete reply;
			return false;
		}

		delete reply;
		return true;
	}

private slots:
};

#endif // COUCHOUTPUT_H
