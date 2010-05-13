/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-03-18
 * Description : Storage container for database connection parameters.
 *
 * Copyright (C) 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010 by Holger F�rster <hamsi2k at freenet dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */
/*
#ifndef DATABASEPARAMETERS_DEBUG
#define DATABASEPARAMETERS_DEBUG
#endif
*/

#include "databaseparameters.h"

// Qt includes

#include <QDir>
#include <QFile>

// KDE includes

#include <kcodecs.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kstandarddirs.h>

namespace Digikam
{

static const char *configGroupDatabase = "Database Settings";
static const char *configInternalDatabaseServer = "Internal Database Server";
static const char *configDatabaseType = "Database Type";
static const char *configDatabaseName = "Database Name";
static const char *configDatabaseNameThumbnails = "Database Name Thumbnails";
static const char *configDatabaseHostName = "Database Hostname";
static const char *configDatabasePort = "Database Port";
static const char *configDatabaseUsername = "Database Username";
static const char *configDatabasePassword = "Database Password";
static const char *configDatabaseConnectOptions = "Database Connectoptions";
// legacy
static const char *configDatabaseFilePathEntry = "Database File Path";
static const char *configAlbumPathEntry = "Album Path";

DatabaseParameters::DatabaseParameters()
                  : port(-1)
{
}

DatabaseParameters::DatabaseParameters(const QString& type,
                                       const QString& databaseName,
                                       const QString& connectOptions,
                                       const QString& hostName,
                                       int port,
                                       bool internalServer,
                                       const QString& userName,
                                       const QString& password,
                                       const QString& databaseNameThumbnails)
                  : databaseType(type), databaseName(databaseName),
                    connectOptions(connectOptions), hostName(hostName),
                    port(port), internalServer(internalServer), userName(userName),
                    password(password), databaseNameThumbnails(databaseNameThumbnails)
{
}

DatabaseParameters::DatabaseParameters(const KUrl& url)
                  : port(-1), internalServer(false)
{
    databaseType   = url.queryItem("databaseType");
    databaseName   = url.queryItem("databaseName");
    connectOptions = url.queryItem("connectOptions");
    hostName       = url.queryItem("hostName");
    QString queryPort = url.queryItem("port");
    if (!queryPort.isNull())
        port = queryPort.toInt();
    QString queryServer = url.queryItem("internalServer");
    if (!queryServer.isNull())
        internalServer = (queryServer == "true");
    userName       = url.queryItem("userName");
    password       = url.queryItem("password");
}

bool DatabaseParameters::operator==(const DatabaseParameters& other) const
{
    return databaseType   == other.databaseType &&
           databaseName   == other.databaseName &&
           connectOptions == other.connectOptions &&
           hostName       == other.hostName &&
           port           == other.port &&
           internalServer == other.internalServer &&
           userName       == other.userName &&
           password       == other.password;
}

bool DatabaseParameters::operator!=(const DatabaseParameters& other) const
{
    return !operator==(other);
}

bool DatabaseParameters::isValid() const
{
    if (isSQLite())
        return !databaseName.isEmpty();
    return false;
}

bool DatabaseParameters::isSQLite() const
{
    return databaseType == "QSQLITE";
}

bool DatabaseParameters::isMySQL() const
{
    return databaseType == "QMYSQL";
}

QString DatabaseParameters::SQLiteDatabaseFile() const
{
    if (isSQLite())
        return databaseName;
    return QString();
}

QByteArray DatabaseParameters::hash() const
{
    KMD5 md5;
    md5.update(databaseType.toUtf8());
    md5.update(databaseName.toUtf8());
    md5.update(connectOptions.toUtf8());
    md5.update(hostName.toUtf8());
    md5.update((const char *)&port, sizeof(int));
    md5.update(userName.toUtf8());
    md5.update(password.toUtf8());
    return md5.hexDigest();
}

DatabaseParameters DatabaseParameters::parametersFromConfig(KSharedConfig::Ptr config, const QString& configGroup)
{
    DatabaseParameters parameters;
    parameters.readFromConfig(config, configGroup);
    return parameters;
}

void DatabaseParameters::readFromConfig(KSharedConfig::Ptr config, const QString& configGroup)
{
    KConfigGroup group;
    if (configGroup.isNull())
        group = config->group(configGroupDatabase);
    else
        group = config->group(configGroup);

    databaseType             = group.readEntry(configDatabaseType, QString());
    databaseName             = group.readEntry(configDatabaseName, QString());
    databaseNameThumbnails   = group.readEntry(configDatabaseNameThumbnails, QString());
    hostName                 = group.readEntry(configDatabaseHostName, QString());
    port                     = group.readEntry(configDatabasePort, -1);
    userName                 = group.readEntry(configDatabaseUsername, QString());
    password                 = group.readEntry(configDatabasePassword, QString());
    connectOptions           = group.readEntry(configDatabaseConnectOptions, QString());
    internalServer           = group.readEntry(configInternalDatabaseServer, false);
                               //TODO When using mysql as default set default value to <true>

    QString orgDatabaseName = databaseName;
    databaseName = getDatabaseFilePath(orgDatabaseName);
    databaseNameThumbnails = getThumbsDatabaseFilePath(orgDatabaseName);
}

QString DatabaseParameters::getDatabaseFilePath(const QString &dbPath) const
{
    QString result = dbPath;
    if (isSQLite())
    {
        QFileInfo fileInfo(result);
        if (fileInfo.isDir())
        {
            result = QDir::cleanPath(fileInfo.filePath() + QDir::separator() + "digikam4.db");
        }
    }
    return result;
}

QString DatabaseParameters::getThumbsDatabaseFilePath(const QString &thumbsDbPath) const
{
    QString result = thumbsDbPath;
    if (isSQLite())
    {
        QFileInfo fileInfo(result);
        if (fileInfo.isDir())
        {
            result = QDir::cleanPath(fileInfo.filePath() + QDir::separator() + "thumbnails-digikam.db");
        }
    }
    return result;
}

void DatabaseParameters::legacyAndDefaultChecks(const QString& suggestedPath, KSharedConfig::Ptr config)
{
    // Additional semantic checks for the database section.
    // If the internal server should be started, then the connection options must be reset
    if (internalServer)
    {
        const QString miscDir = KStandardDirs::locateLocal("data", "digikam/db_misc");
        databaseType= "QMYSQL";
        databaseName = "digikam";
        internalServer = false;
        databaseNameThumbnails = "digikam";
        hostName = QString();
        port = -1;
        userName = "root";
        password = QString();
        connectOptions = QString::fromLatin1("UNIX_SOCKET=%1/mysql.socket").arg(miscDir);
    }

    if (databaseType.isEmpty())
    {
        // Empty 1.3 config: migration from older versions
        KConfigGroup group  = config->group("Album Settings");

        QString databaseFilePath;

        // 1.0 - 1.2 style database file path?
        if (group.hasKey(configDatabaseFilePathEntry))
            databaseFilePath = group.readEntry(configDatabaseFilePathEntry, QString());
        // <= 0.9 style album path entry?
        else if (group.hasKey(configAlbumPathEntry))
            databaseFilePath = group.readEntry(configAlbumPathEntry, QString());
        else if (!suggestedPath.isNull())
            databaseFilePath = suggestedPath;


        if (!databaseFilePath.isEmpty())
        {
            *this = parametersForSQLite(getDatabaseFilePath(databaseFilePath));
        }

        // Be aware that schema updating from  <= 0.9 requires reading the "Album Path", so dont remove it here
    }
}

void DatabaseParameters::removeLegacyConfig(KSharedConfig::Ptr config)
{
    KConfigGroup group  = config->group("Album Settings");
    if (group.hasKey(configDatabaseFilePathEntry))
    {
        group.deleteEntry(configDatabaseFilePathEntry);
    }

    if (group.hasKey(configAlbumPathEntry))
    {
        group.deleteEntry(configAlbumPathEntry);
    }
}

void DatabaseParameters::writeToConfig(KSharedConfig::Ptr config, const QString& configGroup) const
{
    KConfigGroup group;
    if (configGroup.isNull())
        group = config->group(configGroupDatabase);
    else
        group = config->group(configGroup);

    QString dbName = getDatabasePath(databaseName);
    QString dbNameThumbs = getThumbsDatabasePath(databaseNameThumbnails);

    group.writeEntry(configDatabaseType, databaseType);
    group.writeEntry(configDatabaseName, dbName);
    group.writeEntry(configDatabaseNameThumbnails, dbNameThumbs);
    group.writeEntry(configDatabaseHostName, hostName);
    group.writeEntry(configDatabasePort, port);
    group.writeEntry(configDatabaseUsername, userName);
    group.writeEntry(configDatabasePassword, password);
    group.writeEntry(configDatabaseConnectOptions, connectOptions);
    group.writeEntry(configInternalDatabaseServer, internalServer);
}

QString DatabaseParameters::getDatabasePath(const QString& dbFilePath) const
{
    QString result = dbFilePath;
    if (isSQLite())
    {
        if (result.endsWith("digikam4.db"))
            result.chop(QString("digikam4.db").length());
    }
    return result;
}

QString DatabaseParameters::getThumbsDatabasePath(const QString& thumbsDbFilePath) const
{
    QString result = thumbsDbFilePath;
    if (isSQLite())
    {
        if (result.endsWith("thumbnails-digikam.db"))
            result.chop(QString("thumbnails-digikam.db").length());
    }
    return result;
}

DatabaseParameters DatabaseParameters::defaultParameters(const QString databaseType)
{
    DatabaseParameters parameters;

    // only the database name is needed
    DatabaseConfigElement config = DatabaseConfigElement::element(databaseType);

    parameters.databaseType     = databaseType;
    parameters.databaseName     = config.databaseName;
    parameters.hostName         = config.hostName;
    parameters.userName         = config.userName;
    parameters.password         = config.password;
    parameters.port             = config.port.toInt();

    const QString miscDir     = KStandardDirs::locateLocal("data", "digikam/db_misc");
    QString connectOptions = config.connectOptions;
    connectOptions.replace(QString("$$DBMISCPATH$$"), miscDir);

    parameters.connectOptions   = connectOptions;

    kDebug(50003) << "ConnectOptions "<< parameters.connectOptions;
    return parameters;
}

DatabaseParameters DatabaseParameters::thumbnailParameters() const
{
    DatabaseParameters params = *this;
    params.databaseName = databaseNameThumbnails;
    return params;
}

DatabaseParameters DatabaseParameters::parametersForSQLite(const QString& databaseFile)
{
    // only the database name is needed
    DatabaseParameters params("QSQLITE", databaseFile);
    params.databaseNameThumbnails = params.getThumbsDatabaseFilePath(params.getDatabasePath(databaseFile));
    return params;
}

DatabaseParameters DatabaseParameters::parametersForSQLiteDefaultFile(const QString& directory)
{
    return parametersForSQLite(QDir::cleanPath(directory + QDir::separator() + "digikam4.db"));
}

void DatabaseParameters::insertInUrl(KUrl& url) const
{
    removeFromUrl(url);

    url.addQueryItem("databaseType", databaseType);
    url.addQueryItem("databaseName", databaseName);
    if (!connectOptions.isNull())
        url.addQueryItem("connectOptions", connectOptions);
    if (!hostName.isNull())
        url.addQueryItem("hostName", hostName);
    if (port != -1)
        url.addQueryItem("port", QString::number(port));
    if (internalServer)
        url.addQueryItem("internalServer", "true");
    if (!userName.isNull())
        url.addQueryItem("userName", userName);
    if (!password.isNull())
        url.addQueryItem("password", password);
}

void DatabaseParameters::removeFromUrl(KUrl& url)
{
    url.removeQueryItem("databaseType");
    url.removeQueryItem("databaseName");
    url.removeQueryItem("connectOptions");
    url.removeQueryItem("hostName");
    url.removeQueryItem("port");
    url.removeQueryItem("internalServer");
    url.removeQueryItem("userName");
    url.removeQueryItem("password");
}

QDebug operator<<(QDebug dbg, const DatabaseParameters& p)
{
    dbg.nospace() << "DatabaseParameters: [ Type "
                  << p.databaseType << ", ";
    dbg.nospace() << "Name "
                  << p.databaseName << " ";
    dbg.nospace() << "(Thumbnails Name "
                  << p.databaseNameThumbnails << "); ";

    if (!p.connectOptions.isNull())
        dbg.nospace() << "ConnectOptions: "
                      << p.connectOptions << ", ";
    if (!p.hostName.isNull())
        dbg.nospace() << "Host Name and Port: "
                      << p.hostName << " " << p.port << "; ";
    if (p.internalServer)
        dbg.nospace() << "Using an Internal Server; ";
    if (!p.userName.isNull())
        dbg.nospace() << "Username and Password: "
                      << p.userName << ", " << p.password;

    dbg.nospace() << "] ";

    return dbg.space();
}

}  // namespace Digikam
