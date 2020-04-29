#pragma once

#include "DataDefine.h"
#include "EventBase.hpp"

class EventOrder:public EventBase
{
public:
    using EventBase::EventBase;

    virutal void execute() override;

};

class EventUpdate:public EventBase
{
public:
    using EventBase::EventBase;

    virutal void execute() override;
};

class EventTimer:public EventBase
{
public:
    using EventBase::EventBase;

    virutal void execute() override;

};