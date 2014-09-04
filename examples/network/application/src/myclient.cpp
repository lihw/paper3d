// myclient.cpp
// "Network" example
//
// Copyright 2012 - 2014 Future Interface. All rights reserved.
//
// Future Interface  lihw81@gmail.com
//

#include "myclient.h"
    
MyClient::MyClient(const pchar *address, puint16 port, pint32 timeout, PNetworkManager *networkManager)
    : PNetworkClient(address, port, timeout, networkManager)
{
}

MyClient::~MyClient()
{
}

void MyClient::onConnectFailed()
{
    PLOG_ERROR("Connecting to %s:%d failed.", m_address.c_str(), m_port);
}
    
void MyClient::onConnected()
{
    PLOG_INFO("Connecting to %s:%d succeed.", m_address.c_str(), m_port);
}
