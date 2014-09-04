// prenderlightqueue.cpp
// The queue of lights to be used in the render queue.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "prenderlightqueue.h"

#include <Paper3D/pabstractlight.h>
#include <Paper3D/ppointlight.h>
#include <Paper3D/pdirectionallight.h>
#include <Paper3D/pspotlight.h>


PRenderLightQueue::PRenderLightQueue()
{
    clear();
}

PRenderLightQueue::~PRenderLightQueue()
{
}

void PRenderLightQueue::addLight(PAbstractLight *light)
{
    PASSERT(light != P_NULL);

    switch (light->type())
    {
        case P_LIGHT_POINT:
            if (m_lights[0] != P_NULL && m_lights[1] != P_NULL)
            {
                PLOG_WARNING("One scene can render up to two point lights.");
                return ;
            }
            if (m_lights[0] == P_NULL)
            {
                m_lights[0] = reinterpret_cast<PPointLight *>(light);
            }
            else if (m_lights[1] == P_NULL)
            {
                m_lights[1] = reinterpret_cast<PPointLight *>(light);
            }

            break;
        case P_LIGHT_DIRECTIONAL:
            if (m_lights[2] != P_NULL)
            {
                PLOG_WARNING("One scene can render only one directional light.");
                return ;
            }
            m_lights[2] = reinterpret_cast<PDirectionalLight *>(light);
            break;
        case P_LIGHT_SPOT:
            if (m_lights[3] != P_NULL)
            {
                PLOG_WARNING("One scene can render only one spot light.");
                return ;
            }
            m_lights[3] = reinterpret_cast<PSpotLight *>(light);
            break;
        default:
            PASSERT_NOTREACHABLE("Unknown light type.");
    }
}

void PRenderLightQueue::removeLight(PAbstractLight *light)
{
    if (m_lights[0] == light)
    {
        m_lights[0] = m_lights[1];
        m_lights[1] = P_NULL;
    }
    else if (m_lights[1] == light)
    {
        m_lights[1] = P_NULL;
    }
    else if (m_lights[2] == light)
    {
        m_lights[2] = P_NULL;
    }
    else if (m_lights[3] == light)
    {
        m_lights[3] = P_NULL;
    }
    else
    {
        PLOG_WARNING("Failed to find light %s in the queue.", light->name().c_str());
    }
}

void PRenderLightQueue::clear()
{
    m_lights[0] = m_lights[1] = m_lights[2] = m_lights[3] = P_NULL;
}

PAbstractLight *PRenderLightQueue::light(PLightTypeEnum type, puint32 index) const
{
    PASSERT((type == P_LIGHT_POINT && index < 2) ||
            (type == P_LIGHT_DIRECTIONAL && index < 1) ||
            (type == P_LIGHT_SPOT && index < 1));

    return type == P_LIGHT_POINT? m_lights[index] : m_lights[type];
}
    
