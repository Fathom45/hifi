//
//  ScriptCache.h
//  metavoxels
//
//  Created by Andrzej Kapolka on 2/4/14.
//  Copyright (c) 2014 High Fidelity, Inc. All rights reserved.
//

#ifndef __interface__ScriptCache__
#define __interface__ScriptCache__

#include <QHash>
#include <QNetworkRequest>
#include <QObject>
#include <QScriptProgram>
#include <QScriptValue>
#include <QSharedPointer>
#include <QVariantHash>
#include <QWeakPointer>

class QNetworkAccessManager;
class QNetworkReply;
class QScriptEngine;
class QUrl;

class NetworkProgram;
class NetworkValue;
class ParameterizedURL;

/// Maintains a cache of loaded scripts.
class ScriptCache : public QObject {
    Q_OBJECT

public:

    static ScriptCache* getInstance();

    ScriptCache();
    
    void setNetworkAccessManager(QNetworkAccessManager* manager) { _networkAccessManager = manager; }
    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; }
    
    void setEngine(QScriptEngine* engine);
    QScriptEngine* getEngine() const { return _engine; }
    
    /// Loads a script program from the specified URL.
    QSharedPointer<NetworkProgram> getProgram(const QUrl& url);

    /// Loads a script value from the specified URL.
    QSharedPointer<NetworkValue> getValue(const ParameterizedURL& url);

private:
    
    QNetworkAccessManager* _networkAccessManager;
    QScriptEngine* _engine;
    QHash<QUrl, QWeakPointer<NetworkProgram> > _networkPrograms;
    QHash<ParameterizedURL, QWeakPointer<NetworkValue> > _networkValues;
};

/// A program loaded from the network.
class NetworkProgram : public QObject {
    Q_OBJECT

public:

    NetworkProgram(ScriptCache* cache, const QUrl& url);
    ~NetworkProgram();
    
    ScriptCache* getCache() const { return _cache; }
    
    bool isLoaded() const { return !_program.isNull(); }
    
    const QScriptProgram& getProgram() const { return _program; }
    
private slots:
    
    void makeRequest();
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleReplyError(); 
        
private:
    
    ScriptCache* _cache;
    QNetworkRequest _request;
    QNetworkReply* _reply;
    int _attempts;
    QScriptProgram _program;
};

/// A value loaded from the network.
class NetworkValue {
public:
    
    NetworkValue(const QSharedPointer<NetworkProgram>& program, const QVariantHash& parameters);

    bool isLoaded() { return getValue().isValid(); }

    QScriptValue& getValue();

private:
    
    QSharedPointer<NetworkProgram> _program;
    QVariantHash _parameters;
    QScriptValue _value;
};

#endif /* defined(__interface__ScriptCache__) */
