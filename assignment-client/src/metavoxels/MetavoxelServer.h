//
//  MetavoxelServer.h
//  assignment-client/src/metavoxels
//
//  Created by Andrzej Kapolka on 12/18/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_MetavoxelServer_h
#define hifi_MetavoxelServer_h

#include <QList>
#include <QTimer>

#include <ThreadedAssignment.h>

#include <Endpoint.h>

class MetavoxelEditMessage;
class MetavoxelSession;

/// Maintains a shared metavoxel system, accepting change requests and broadcasting updates.
class MetavoxelServer : public ThreadedAssignment {
    Q_OBJECT

public:
    
    MetavoxelServer(const QByteArray& packet);

    void applyEdit(const MetavoxelEditMessage& edit);

    const MetavoxelData& getData() const { return _data; }

    virtual void run();
    
    virtual void readPendingDatagrams();
    
private slots:

    void maybeAttachSession(const SharedNodePointer& node);
    void sendDeltas();    
    
private:
    
    QTimer _sendTimer;
    qint64 _lastSend;
    
    MetavoxelData _data;
};

/// Contains the state of a single client session.
class MetavoxelSession : public Endpoint {
    Q_OBJECT
    
public:
    
    MetavoxelSession(const SharedNodePointer& node, MetavoxelServer* server);

    virtual void update();

protected:

    virtual void handleMessage(const QVariant& message, Bitstream& in);
    
    virtual PacketRecord* maybeCreateSendRecord() const;

private slots:

    void handleMessage(const QVariant& message);
    void checkReliableDeltaReceived();
    
private:
    
    void sendPacketGroup(int alreadySent = 0);
    
    MetavoxelServer* _server;
    
    MetavoxelLOD _lod;
    
    ReliableChannel* _reliableDeltaChannel;
    int _reliableDeltaReceivedOffset;
    MetavoxelData _reliableDeltaData;
    MetavoxelLOD _reliableDeltaLOD;
    Bitstream::WriteMappings _reliableDeltaWriteMappings;
    int _reliableDeltaID;
};

#endif // hifi_MetavoxelServer_h
