#include "ChannelStruct.h"


FrameBitStream::FrameBitStream( const FrameBitStream & node )
{
    pData = node.pData;
    uLen = node.uLen;
    ullPts = node.ullPts;
}

FrameBitStream & FrameBitStream::operator = (const FrameBitStream & node)
{
    if (this != &node)
    {
        pData = node.pData;
        uLen = node.uLen;
        ullPts = node.ullPts;
    }

    return *this;
}


TSInParam::TSInParam(): nVCode(2), nACode(1), nServiceID(0), nVPID(48), nServiceType(0), nVStreamType(0), nAStreamType(0)
{ 
    vecAPID.clear(); 
}

TSInParam::TSInParam(const TSInParam & node)
{
    nVCode = node.nVCode;
    nACode = node.nACode;
    nServiceID = node.nServiceID;
    nVPID = node.nVPID;
    nServiceType = node.nServiceType;

    vecAPID.clear();
    for(auto it = node.vecAPID.begin(); it != node.vecAPID.end(); ++it)
    {
        vecAPID.push_back(*it);
    }

    nVStreamType = node.nVStreamType;
    nAStreamType = node.nAStreamType;
}

TSInParam &TSInParam::operator = (const TSInParam & node)
{
    if (this != &node)
    {
        nVCode = node.nVCode;
        nACode = node.nACode;
        nServiceID = node.nServiceID;
        nVPID = node.nVPID;
        nServiceType = node.nServiceType;

        vecAPID.clear();
        for(auto it = node.vecAPID.begin(); it != node.vecAPID.end(); ++it)
        {
            vecAPID.push_back(*it);
        }

        nVStreamType = node.nVStreamType;
        nAStreamType = node.nAStreamType;
    }

    return *this;
}

