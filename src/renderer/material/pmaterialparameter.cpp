// pshaderparameter.cpp
// Materials' uniform value.
// 
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com and Hongwei Li lihw81@gmail.com
//

#include "pmaterialparameter.h"

#include <Paper3D/ptexture.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/pmaterialresource.h>
#include <Paper3D/presourcemanager.h>

#include <PFoundation/pconststring.h>

#include <PFoundation/pglshader.h>
#include <PFoundation/pgltexture.h>
#include <PFoundation/passert.h>


PMaterialParameter::PMaterialParameter(const pchar *name, const pchar *uniformName, 
    PGlShaderUniformTypeEnum type, puint32 size)
    : PAbstractRenderParameter(name, uniformName)
    , m_size(size)
{
    puint32 dataTypeSizes[] = 
    {
        4, // P_GLSHADERUNIFORM_FLOAT,
        8, // P_GLSHADERUNIFORM_FLOAT2,
        12, // P_GLSHADERUNIFORM_FLOAT3,
        16, // P_GLSHADERUNIFORM_FLOAT4,
        4, // P_GLSHADERUNIFORM_INT,
        8, // P_GLSHADERUNIFORM_INT2,
        12, // P_GLSHADERUNIFORM_INT3,
        16, // P_GLSHADERUNIFORM_INT4,
        4, // P_GLSHADERUNIFORM_BOOL,
        8, // P_GLSHADERUNIFORM_BOOL2,
        12, // P_GLSHADERUNIFORM_BOOL3,
        16, // P_GLSHADERUNIFORM_BOOL4,
        16, // P_GLSHADERUNIFORM_FLOATMATRIX2X2,
        36, // P_GLSHADERUNIFORM_FLOATMATRIX3X3,
        64, // P_GLSHADERUNIFORM_FLOATMATRIX4X4,
        4, // P_GLSHADERUNIFORM_SAMPLER2D,
        4, // P_GLSHADERUNIFORM_SAMPLERCUBE,
    };
    
    if ((type != P_GLSHADERUNIFORM_UNKNOWN &&
         type != P_GLSHADERUNIFORM_FLOAT &&
         type != P_GLSHADERUNIFORM_INT &&
         type != P_GLSHADERUNIFORM_BOOL &&
         type != P_GLSHADERUNIFORM_SAMPLER2D &&
         type != P_GLSHADERUNIFORM_SAMPLERCUBE) ||
        m_size > 1)
    {
        m_value.v = PNEWARRAY(puint8, dataTypeSizes[type] * m_size);
        pmemset(m_value.v, 0, dataTypeSizes[type] * m_size);
    }
    else
    {
        m_value.i = 0;
    }
        
    m_uniformType = type;

    if (m_uniformType == P_GLSHADERUNIFORM_UNKNOWN)
    {
        m_upload = P_NULL;
    }
    else if (m_size > 1)
    {
        UploadFuncPointer functions[] =
        {
            &PMaterialParameter::uploadFloatArray,
            &PMaterialParameter::uploadFloat2Array,
            &PMaterialParameter::uploadFloat3Array,
            &PMaterialParameter::uploadFloat4Array,
            &PMaterialParameter::uploadIntArray,
            &PMaterialParameter::uploadInt2Array,
            &PMaterialParameter::uploadInt3Array,
            &PMaterialParameter::uploadInt4Array,
            &PMaterialParameter::uploadIntArray,
            &PMaterialParameter::uploadInt2Array,
            &PMaterialParameter::uploadInt3Array,
            &PMaterialParameter::uploadInt4Array,
            &PMaterialParameter::uploadMatrix2x2Array,
            &PMaterialParameter::uploadMatrix3x3Array,
            &PMaterialParameter::uploadMatrix4x4Array,
        };

        m_upload = functions[m_uniformType];
    }
    else
    {
        UploadFuncPointer functions[] =
        {
            &PMaterialParameter::uploadFloat,
            &PMaterialParameter::uploadFloat2,
            &PMaterialParameter::uploadFloat3,
            &PMaterialParameter::uploadFloat4,
            &PMaterialParameter::uploadInt,
            &PMaterialParameter::uploadInt2,
            &PMaterialParameter::uploadInt3,
            &PMaterialParameter::uploadInt4,
            &PMaterialParameter::uploadInt,
            &PMaterialParameter::uploadInt2,
            &PMaterialParameter::uploadInt3,
            &PMaterialParameter::uploadInt4,
            &PMaterialParameter::uploadMatrix2x2,
            &PMaterialParameter::uploadMatrix3x3,
            &PMaterialParameter::uploadMatrix4x4,
            &PMaterialParameter::uploadTexture,
            &PMaterialParameter::uploadTexture,
        };

        m_upload = functions[m_uniformType];
    }
}
    
PMaterialParameter::PMaterialParameter(const PMaterialParameter &other)
    : PAbstractRenderParameter(other.m_name.c_str(), other.m_uniformName.c_str())
    , m_size(other.m_size)
    , m_upload(other.m_upload)
{
    m_uniformType = other.m_uniformType;

    if (m_uniformType == P_GLSHADERUNIFORM_FLOAT ||
        m_uniformType == P_GLSHADERUNIFORM_INT ||
        m_uniformType == P_GLSHADERUNIFORM_BOOL ||
        m_uniformType == P_GLSHADERUNIFORM_SAMPLER2D ||
        m_uniformType == P_GLSHADERUNIFORM_SAMPLERCUBE)
    {
        m_value = other.m_value;
    }
    else 
    {
        puint32 dataTypeSizes[] = 
        {
            4, // P_GLSHADERUNIFORM_FLOAT,
            8, // P_GLSHADERUNIFORM_FLOAT2,
            12, // P_GLSHADERUNIFORM_FLOAT3,
            16, // P_GLSHADERUNIFORM_FLOAT4,
            4, // P_GLSHADERUNIFORM_INT,
            8, // P_GLSHADERUNIFORM_INT2,
            12, // P_GLSHADERUNIFORM_INT3,
            16, // P_GLSHADERUNIFORM_INT4,
            4, // P_GLSHADERUNIFORM_BOOL,
            8, // P_GLSHADERUNIFORM_BOOL2,
            12, // P_GLSHADERUNIFORM_BOOL3,
            16, // P_GLSHADERUNIFORM_BOOL4,
            16, // P_GLSHADERUNIFORM_FLOATMATRIX2X2,
            36, // P_GLSHADERUNIFORM_FLOATMATRIX3X3,
            64, // P_GLSHADERUNIFORM_FLOATMATRIX4X4,
            4, // P_GLSHADERUNIFORM_SAMPLER2D,
            4, // P_GLSHADERUNIFORM_SAMPLERCUBE,
        };
        
        m_value.v = PNEWARRAY(puint8, dataTypeSizes[m_uniformType] * m_size);
        pmemcpy(m_value.v, other.m_value.v, dataTypeSizes[m_uniformType] * m_size);
    }
}

PMaterialParameter::~PMaterialParameter()
{
    if (m_uniformType == P_GLSHADERUNIFORM_SAMPLER2D ||
        m_uniformType == P_GLSHADERUNIFORM_SAMPLERCUBE)
    {
        // FIXME: how about sampler array?
        if (m_value.t != P_NULL)
        {
            m_value.t->release();
        }
    }

    if (m_size > 1 || 
        m_uniformType == P_GLSHADERUNIFORM_FLOAT2 ||
        m_uniformType == P_GLSHADERUNIFORM_FLOAT3 ||
        m_uniformType == P_GLSHADERUNIFORM_FLOAT4 ||
        m_uniformType == P_GLSHADERUNIFORM_INT2 ||
        m_uniformType == P_GLSHADERUNIFORM_INT3 ||
        m_uniformType == P_GLSHADERUNIFORM_INT4 ||
        m_uniformType == P_GLSHADERUNIFORM_BOOL2 ||
        m_uniformType == P_GLSHADERUNIFORM_BOOL3 ||
        m_uniformType == P_GLSHADERUNIFORM_BOOL4 ||
        m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX2X2 ||
        m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX3X3 ||
        m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX4X4)
    {
        PDELETEARRAY(m_value.v);
    }
}
    
void PMaterialParameter::operator=(PTexture *value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_SAMPLERCUBE ||
            m_uniformType == P_GLSHADERUNIFORM_SAMPLER2D ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType != P_GLSHADERUNIFORM_SAMPLERCUBE &&
        m_uniformType != P_GLSHADERUNIFORM_SAMPLER2D)
    {
        PLOG_WARNING("%s is not a texture parameter", m_name.c_str());
        return ;
    }
    if (m_value.t != value)
    {
        if (m_value.t != P_NULL)
        {
            m_value.t->release();
        }
        m_value.t = value;
        // Increase the reference count of texture object.
        if (m_value.t != P_NULL)
        {
            m_value.t->retain();
        }
    }
}

void PMaterialParameter::operator=(pbool value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_BOOL ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_BOOL)
    {
        m_value.i = value? 1 : 0;
    }
}

void PMaterialParameter::operator=(pint32 value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_INT ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_INT)
    {
        m_value.i = value;
    }
}

void PMaterialParameter::operator=(pfloat32 value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOAT ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOAT)
    {
        m_value.f = value;
    }
}

void PMaterialParameter::operator=(const PVector2 &value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOAT2 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOAT2)
    {
        pmemcpy(m_value.v, &value, sizeof(PVector2));
    }
}

void PMaterialParameter::operator=(const PVector3 &value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOAT3 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOAT3)
    {
        pmemcpy(m_value.v, &value, sizeof(PVector3));
    }
}

void PMaterialParameter::operator=(const PVector4 &value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOAT4 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOAT4)
    {
        pmemcpy(m_value.v, &value, sizeof(PVector4));
    }
}

void PMaterialParameter::operator=(const PMatrix3x3 &value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX3X3 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX3X3)
    {
        pmemcpy(m_value.v, &value, sizeof(PMatrix3x3));
    }
}

void PMaterialParameter::operator=(const PMatrix4x4 &value)
{
    PASSERT(m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX4X4 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX4X4)
    {
        pmemcpy(m_value.v, &value, sizeof(PMatrix4x4));
    }
}

void PMaterialParameter::operator=(const PColorRGBA &value)
{
    operator=(value.toVector4());
}

void PMaterialParameter::operator=(const pbool *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_BOOL ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_BOOL)
    {
        pint32 *value = (pint32 *)m_value.v;
        for (puint32 i = 0; i < m_size; ++i)
        {
            m_value.v[i] = value[i]? 1 : 0;
        }
    }
}

void PMaterialParameter::operator=(const pint32 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_INT ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_INT)
    {
        pmemcpy(m_value.v, value, sizeof(pint32) * m_size);
    }
}

void PMaterialParameter::operator=(const pfloat32 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT)
    {
        pmemcpy(m_value.v, value, sizeof(pfloat32) * m_size);
    }
}

void PMaterialParameter::operator=(const PVector2 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT2 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT2)
    {
        pmemcpy(m_value.v, value, sizeof(PVector2) * m_size);
    }
}

void PMaterialParameter::operator=(const PVector3 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT3 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT3)
    {
        pmemcpy(m_value.v, value, sizeof(PVector3) * m_size);
    }
}

void PMaterialParameter::operator=(const PVector4 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT4 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOAT4)
    {
        pmemcpy(m_value.v, value, sizeof(PVector4) * m_size);
    }
}

void PMaterialParameter::operator=(const PMatrix3x3 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX3X3 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX3X3)
    {
        pmemcpy(m_value.v, value, sizeof(PMatrix3x3) * m_size);
    }
}

void PMaterialParameter::operator=(const PMatrix4x4 *value)
{
    PASSERT(m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX4X4 ||
            m_uniformType == P_GLSHADERUNIFORM_UNKNOWN);
    if (m_size >= 1 && m_uniformType == P_GLSHADERUNIFORM_FLOATMATRIX4X4)
    {
        pmemcpy(m_value.v, value, sizeof(PMatrix4x4) * m_size);
    }
}

void PMaterialParameter::upload(PMaterialResource *materialResource, PRenderState *renderState, puint32 &numTextures)
{
    if (m_uniformType == P_GLSHADERUNIFORM_SAMPLER2D ||
        m_uniformType == P_GLSHADERUNIFORM_SAMPLERCUBE)
    {
#if defined P_DEBUG
        if (m_value.t == P_NULL)
        {
            PLOG_DEBUG("Invalid texture parameter in material %s.", materialResource->id());
        }
#endif
        PASSERT(m_value.t != P_NULL);
        renderState->useTexture(m_value.t, numTextures);
        ++numTextures;
    }

    // The location information of material parameter in instance is the index
    // to the uniform in material resource.  It is because material resource
    // may be discarded and resumed, the uniform location of each parameter may
    // change.
    pint32 location = materialResource->materialParameter(m_uniformLocation)->uniformLocation();
    (this->*m_upload)(location, materialResource->shader());
}

void PMaterialParameter::uploadInt(pint32 location, PGlShader *shader)
{
    shader->uniform(location, m_value.i);
}

void PMaterialParameter::uploadInt2(pint32 location, PGlShader *shader)
{
    PASSERT_NOTIMPLEMENTED();
}

void PMaterialParameter::uploadInt3(pint32 location, PGlShader *shader)
{
    PASSERT_NOTIMPLEMENTED();
}

void PMaterialParameter::uploadInt4(pint32 location, PGlShader *shader)
{
    PASSERT_NOTIMPLEMENTED();
}

void PMaterialParameter::uploadFloat(pint32 location, PGlShader *shader)
{
    shader->uniform(location, m_value.f);
}

void PMaterialParameter::uploadFloat2(pint32 location, PGlShader *shader)
{
    PVector2 *v = reinterpret_cast<PVector2*>(m_value.v);
    shader->uniform(location, v->m_v[0], v->m_v[1]);
}

void PMaterialParameter::uploadFloat3(pint32 location, PGlShader *shader)
{
    PVector3 *v = reinterpret_cast<PVector3*>(m_value.v);
    shader->uniform(location, v->m_v[0], v->m_v[1], v->m_v[2]);
}

void PMaterialParameter::uploadFloat4(pint32 location, PGlShader *shader)
{
    PVector4 *v = reinterpret_cast<PVector4*>(m_value.v);
    shader->uniform(location, v->m_v[0], v->m_v[1], v->m_v[2], v->m_v[3]);
}

void PMaterialParameter::uploadMatrix2x2(pint32 location, PGlShader *shader)
{
    PASSERT_NOTIMPLEMENTED();
}

void PMaterialParameter::uploadMatrix3x3(pint32 location, PGlShader *shader)
{
    PMatrix3x3 *v = reinterpret_cast<PMatrix3x3*>(m_value.v);
    shader->uniformMatrix3fv(location, v->m_m, 1);
}

void PMaterialParameter::uploadMatrix4x4(pint32 location, PGlShader *shader)
{
    PMatrix4x4 *v = reinterpret_cast<PMatrix4x4*>(m_value.v);
    shader->uniformMatrix4fv(location, v->m_m, 1);
}

void PMaterialParameter::uploadIntArray(pint32 location, PGlShader *shader)
{
    pint32 *v = reinterpret_cast<pint32*>(m_value.v);
    shader->uniform1iv(location, v, m_size);
}

void PMaterialParameter::uploadInt2Array(pint32 location, PGlShader *shader)
{
    pint32 *v = reinterpret_cast<pint32*>(m_value.v);
    shader->uniform2iv(location, v, m_size);
}

void PMaterialParameter::uploadInt3Array(pint32 location, PGlShader *shader)
{
    pint32 *v = reinterpret_cast<pint32*>(m_value.v);
    shader->uniform3iv(location, v, m_size);
}

void PMaterialParameter::uploadInt4Array(pint32 location, PGlShader *shader)
{
    pint32 *v = reinterpret_cast<pint32*>(m_value.v);
    shader->uniform4iv(location, v, m_size);
}

void PMaterialParameter::uploadFloatArray(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32*>(m_value.v);
    shader->uniform1fv(location, v, m_size);
}

void PMaterialParameter::uploadFloat2Array(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32 *>(m_value.v);
    shader->uniform2fv(location, v, m_size);
}

void PMaterialParameter::uploadFloat3Array(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32 *>(m_value.v);
    shader->uniform3fv(location, v, m_size);
}

void PMaterialParameter::uploadFloat4Array(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32 *>(m_value.v);
    shader->uniform4fv(location, v, m_size);
}

void PMaterialParameter::uploadMatrix2x2Array(pint32 location, PGlShader *shader)
{
    PASSERT_NOTIMPLEMENTED();
}

void PMaterialParameter::uploadMatrix3x3Array(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32 *>(m_value.v);
    shader->uniformMatrix3fv(location, v, m_size);
}

void PMaterialParameter::uploadMatrix4x4Array(pint32 location, PGlShader *shader)
{
    pfloat32 *v = reinterpret_cast<pfloat32 *>(m_value.v);
    shader->uniformMatrix4fv(location, v, m_size);
}
    
void PMaterialParameter::uploadTexture(pint32 location, PGlShader *shader)
{
    // FIXME: es 2.0 supports up to 8 texture units, so when texture is NULL, we 
    // set the value of the uniform to the last image unit, which should be a
    // black image by default.
    puint32 textureUnit = 7;
    if (m_value.t != P_NULL)
    {
        textureUnit = m_value.t->textureObject()->textureUnit();
    }
    shader->uniform(location, (pint32)textureUnit);
}
    
void PMaterialParameter::upload(PGlShader              *shader,
                                PRenderTransform       *transform, 
                                PCamera                *camera, 
                                const PRenderLightQueue &lights)
{
    PASSERT_NOTREACHABLE("PMaterialParameter doesn't use this upload().");
}

pbool PMaterialParameter::unpackFloat(const pchar *text)
{
    if (pStringUnpackFloat(text, &m_value.f) != P_NULL)
    {
        return true;
    }
        
    return false;
}

pbool PMaterialParameter::unpackFloat2(const pchar *text)
{
    const pchar *p = text;
    pfloat32 *v = (pfloat32 *)m_value.v;

    if ((p = pStringUnpackFloat(p, &v[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[1])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackFloat3(const pchar *text)
{
    const pchar *p = text;
    pfloat32 *v = (pfloat32 *)m_value.v;

    if ((p = pStringUnpackFloat(p, &v[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[1])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[2])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackFloat4(const pchar *text)
{
    pfloat32 *v = (pfloat32 *)m_value.v;
    const pchar *p = text;

    if ((p = pStringUnpackFloat(p, &v[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[1])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[2])) != P_NULL &&
        (p = pStringUnpackFloat(p, &v[3])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackInt(const pchar *text)
{
    if (pStringUnpackInt(text, &m_value.i) != P_NULL)
    {
        return true;
    }
        
    return false;
}

pbool PMaterialParameter::unpackInt2(const pchar *text)
{
    const pchar *p = text;
    pint32 *v = (pint32 *)m_value.v;

    if ((p = pStringUnpackInt(p, &v[0])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[1])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackInt3(const pchar *text)
{
    const pchar *p = text;
    pint32 *v = (pint32 *)m_value.v;

    if ((p = pStringUnpackInt(p, &v[0])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[1])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[2])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackInt4(const pchar *text)
{
    const pchar *p = text;
    pint32 *v = (pint32 *)m_value.v;

    if ((p = pStringUnpackInt(p, &v[0])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[1])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[2])) != P_NULL &&
        (p = pStringUnpackInt(p, &v[3])) != P_NULL)
    {
        return true;
    }

    return false;
}

pbool PMaterialParameter::unpackFloat2x2(const pchar *text)
{
    pfloat32 *m = (pfloat32 *)m_value.v;
    const pchar *p = text;

    if ((p = pStringUnpackFloat(p, &m[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[1])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[2])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[3])) != P_NULL)
    {
        return true;
    }
    return false;
}

pbool PMaterialParameter::unpackFloat3x3(const pchar *text)
{
    pfloat32 *m = (pfloat32 *)m_value.v;
    const pchar *p = text;

    if ((p = pStringUnpackFloat(p, &m[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[1])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[2])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[3])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[4])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[5])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[6])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[7])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[8])) != P_NULL)
    {
        return true;
    }
    return false;
}

pbool PMaterialParameter::unpackFloat4x4(const pchar *text)
{
    pfloat32 *m = (pfloat32 *)m_value.v;
    const pchar *p = text;

    if ((p = pStringUnpackFloat(p, &m[0])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[1])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[2])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[3])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[4])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[5])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[6])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[7])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[8])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[9])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[10])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[11])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[12])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[13])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[14])) != P_NULL &&
        (p = pStringUnpackFloat(p, &m[15])) != P_NULL)
    {
        return true;
    }
    return false;
}
    
pbool PMaterialParameter::unpack(const pchar *text, PResourceManager *resourceManager)
{
    PASSERTINFO(m_size == 1, "Can't support to unpack of an array of material parameters.");
    
    typedef pbool (PMaterialParameter::*UnpackFunc)(const pchar *text);

    UnpackFunc functions[] =
    {
        &PMaterialParameter::unpackFloat,
        &PMaterialParameter::unpackFloat2,
        &PMaterialParameter::unpackFloat3,
        &PMaterialParameter::unpackFloat4,
        &PMaterialParameter::unpackInt,
        &PMaterialParameter::unpackInt2,
        &PMaterialParameter::unpackInt3,
        &PMaterialParameter::unpackInt4,
        &PMaterialParameter::unpackInt,
        &PMaterialParameter::unpackInt2,
        &PMaterialParameter::unpackInt3,
        &PMaterialParameter::unpackInt4,
        &PMaterialParameter::unpackFloat2x2,
        &PMaterialParameter::unpackFloat3x3,
        &PMaterialParameter::unpackFloat4x4,
    };

    if (m_uniformType == P_GLSHADERUNIFORM_SAMPLER2D ||
        m_uniformType == P_GLSHADERUNIFORM_SAMPLERCUBE)
    {
        PTexture *texture = resourceManager->getTexture(text);
        PASSERT(texture != P_NULL);
        if (texture != P_NULL)
        {
            m_value.t = texture;
            if (m_value.t != P_NULL)
            {
                m_value.t->retain();
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    return (this->*functions[m_uniformType])(text);
}
