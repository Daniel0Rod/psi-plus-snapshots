/*
 * loader.h - plugin
 * Copyright (C) 2010  Evgeny Khryukin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef LOADER_H
#define LOADER_H

#include <QNetworkReply>

class QNetworkAccessManager;

class Loader : public QObject
{
	Q_OBJECT
public:
	Loader(const QString& id, QObject *p);
	~Loader();
	void start(const QString& url);
	void setProxy(const QString& host, int port, const QString& user = QString(), const QString& pass = QString());

private slots:
	void onRequestFinish(QNetworkReply*);

signals:
	void error(const QString&);
	void data(const QString&, const QByteArray&);

private:
	QNetworkAccessManager* manager_;
	QString id_;
};

#endif // LOADER_H
