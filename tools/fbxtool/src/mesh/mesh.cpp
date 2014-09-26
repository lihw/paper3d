// mesh.cpp
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "mesh.h"

#include <assert.h>
#include <stdlib.h>

//
// class VertexFormat
//
bool VertexFormat::hasAttrib(AttribEnum attrib) const
{
    int numAttribs = m_attributes.size();
    for (int i = 0; i < numAttribs; ++i)
    {
        if (m_attributes[i].attrib == attrib)
        {
            return true;
        }
    }

    return false;
}

int VertexFormat::vertexSize() const
{
    const int typeSize[] =
    {
        sizeof(float),
        sizeof(FbxUInt32),
        sizeof(FbxInt32),
        sizeof(bool),
    };
    int numAttribs = m_attributes.size();
    int size = 0;
    for (int i = 0; i < numAttribs; ++i)
    {
        size += m_attributes[i].size * typeSize[m_attributes[i].type];
    }

    return size;
}

void VertexFormat::addAttrib(AttribEnum attrib, int size, TypeEnum type)
{
    if (!hasAttrib(attrib))
    {
        int oldVertexSize = vertexSize();

        char *names[] =
        {
            "position",
            "tangent",
            "binormal",
            "normal",
            "texcoord0",
            "texcoord1",
            "texcoord2",
            "texcoord3",
            "color",
        };
        m_attributes.push_back(Attribute());
        m_attributes.back().attrib = attrib;
        m_attributes.back().size   = size;
        m_attributes.back().type   = type;
        m_attributes.back().name   = FbxString(names[attrib]);
        m_attributes.back().offset = oldVertexSize;
    }
}
    
FbxUInt32 VertexFormat::getAttribOffset(AttribEnum attrib) const
{
    int numAttribs = m_attributes.size();
    for (int i = 0; i < numAttribs; ++i)
    {
        if (m_attributes[i].attrib == attrib)
        {
            return m_attributes[i].offset;
        }
    }

    return 0xffffffff;
}

//
// class Vertex
//
Vertex::Vertex(FbxUInt8* data, VertexFormat *vertexFormat)
{
    assert(data != NULL);
    assert(vertexFormat != NULL);

    m_data = data;
    m_vertexFormat = vertexFormat;
}

Vertex::~Vertex()
{
}

void Vertex::setAttrib(VertexFormat::AttribEnum attrib, const FbxVector4 &value)
{
    if (m_vertexFormat->hasAttrib(attrib))
    {
        float *p = getAttrib(attrib);
        p[0] = (float)value.mData[0];
        p[1] = (float)value.mData[1];
        p[2] = (float)value.mData[2];

        if (attrib == VertexFormat::COLOR)
        {
            p[3] = (float)value.mData[3];
        }
    }
}

void Vertex::setAttrib(VertexFormat::AttribEnum attrib, const FbxVector2 &value)
{
    if (m_vertexFormat->hasAttrib(attrib))
    {
        float *p = getAttrib(attrib);
        p[0] = (float)value.mData[0];
        p[1] = (float)value.mData[1];
    }
}

bool Vertex::isAttribValid(VertexFormat::AttribEnum attrib)
{
    if (m_vertexFormat->hasAttrib(attrib))
    {
        FbxUInt32 *p = (FbxUInt32 *)getAttrib(attrib);
        switch (attrib)
        {
            case VertexFormat::POSITION:
            case VertexFormat::NORMAL:
            case VertexFormat::BINORMAL:
            case VertexFormat::TANGENT:
                return p[0] != 0xffffffff ||
                       p[1] != 0xffffffff ||
                       p[2] != 0xffffffff;
                break;
            case VertexFormat::TEXCOORD0:
            case VertexFormat::TEXCOORD1:
            case VertexFormat::TEXCOORD2:
            case VertexFormat::TEXCOORD3:
                return p[0] != 0xffffffff ||
                       p[1] != 0xffffffff;
                break;
            case VertexFormat::COLOR:
                return p[0] != 0xffffffff ||
                       p[1] != 0xffffffff ||
                       p[2] != 0xffffffff ||
                       p[3] != 0xffffffff;
                break;
        }
    }

    return false;
}

void Vertex::setAttrib(VertexFormat::AttribEnum attrib, const VertexAttribValue &value)
{
    if (m_vertexFormat->hasAttrib(attrib))
    {
        FbxUInt32 *p = (FbxUInt32 *)getAttrib(attrib);
        switch (attrib)
        {
            case VertexFormat::POSITION:
            case VertexFormat::NORMAL:
            case VertexFormat::BINORMAL:
            case VertexFormat::TANGENT:
            case VertexFormat::COLOR:
                setAttrib(attrib, value.vec4);
                break;
            case VertexFormat::TEXCOORD0:
            case VertexFormat::TEXCOORD1:
            case VertexFormat::TEXCOORD2:
            case VertexFormat::TEXCOORD3:
                setAttrib(attrib, value.vec2);
                break;
        }
    }
}

bool Vertex::isAttribEqual(VertexFormat::AttribEnum attrib, const VertexAttribValue &value)
{
    if (m_vertexFormat->hasAttrib(attrib))
    {
        float *p = getAttrib(attrib);
        switch (attrib)
        {
            case VertexFormat::POSITION:
            case VertexFormat::NORMAL:
            case VertexFormat::BINORMAL:
            case VertexFormat::TANGENT:
                return p[0] == value.vec4.mData[0] &&
                       p[1] == value.vec4.mData[1] &&
                       p[2] == value.vec4.mData[2];
                break;
            case VertexFormat::TEXCOORD0:
            case VertexFormat::TEXCOORD1:
            case VertexFormat::TEXCOORD2:
            case VertexFormat::TEXCOORD3:
                return p[0] == value.vec2.mData[0] &&
                       p[1] == value.vec2.mData[1];
                break;
            case VertexFormat::COLOR:
                return p[0] == value.vec4.mData[0] &&
                       p[1] == value.vec4.mData[1] &&
                       p[2] == value.vec4.mData[2] &&
                       p[3] == value.vec4.mData[3];
                break;
        }
    }

    return false;
}
    
float *Vertex::getAttrib(VertexFormat::AttribEnum attrib)
{
    FbxUInt32 offset = m_vertexFormat->getAttribOffset(attrib);
    if (offset != 0xffffffff)
    {
        return (float*)(&m_data[offset]);
    }

    return NULL;
}

//
// class Mesh
//
Mesh::Mesh()
{
    m_numVertices = 0;
    m_numIndices  = 0;

    m_box[0] = FLT_MAX;
    m_box[1] = FLT_MAX;
    m_box[2] = FLT_MAX;
    m_box[3] = -FLT_MAX;
    m_box[4] = -FLT_MAX;
    m_box[5] = -FLT_MAX;
}

Mesh::~Mesh()
{
}
    
void Mesh::transform(const FbxAMatrix &matrix)
{
    for (FbxUInt32 i = 0; i < m_numVertices; i++)
    {
        Vertex v = vertex(i);
        float *p = v.getAttrib(VertexFormat::POSITION);
        if (p != NULL)
        {
            FbxVector4 p0(p[0], p[1], p[2], 1.0f);

            FbxVector4 tp = matrix.MultT(p0);

            p[0] = (float)tp.mData[0];
            p[1] = (float)tp.mData[1];
            p[2] = (float)tp.mData[2];
        }

        float* n = v.getAttrib(VertexFormat::NORMAL);
        if (n != NULL)
        {
            FbxAMatrix normalMatrix = matrix.Inverse().Transpose();
            // Clear the matrix translation for normal multiplication.
            // http://forums.autodesk.com/t5/FBX-SDK/Matrix-Vector-multiplication/td-p/4245079
            normalMatrix.SetT(FbxVector4(0, 0, 0, 0));

            FbxVector4 n0(n[0], n[1], n[2], 1.0f);

            FbxVector4 tn = normalMatrix.MultT(n0);

            n[0] = (float)tn.mData[0];
            n[1] = (float)tn.mData[1];
            n[2] = (float)tn.mData[2];
        }
        
        float* b = v.getAttrib(VertexFormat::BINORMAL);
        if (b != NULL)
        {
            FbxAMatrix normalMatrix = matrix.Inverse().Transpose();

            FbxVector4 b0(b[0], b[1], b[2], 1.0f);

            FbxVector4 tb = normalMatrix.MultT(b0);

            b[0] = (float)tb.mData[0];
            b[1] = (float)tb.mData[1];
            b[2] = (float)tb.mData[2];
        }

        float* t = v.getAttrib(VertexFormat::TANGENT);
        if (t != NULL)
        {
            FbxAMatrix normalMatrix = matrix.Inverse().Transpose();

            FbxVector4 t0(t[0], t[1], t[2], 1.0f);

            FbxVector4 tt = normalMatrix.MultT(t0);

            t[0] = (float)tt.mData[0];
            t[1] = (float)tt.mData[1];
            t[2] = (float)tt.mData[2];
        }
    }
}
    
void Mesh::initialize(const VertexFormat &vertexFormat, FbxUInt32 numVertices, FbxUInt32 numIndices)
{
    m_vertexFormat = vertexFormat;
   
    m_numVertices  = numVertices;
    m_numIndices = numIndices;

    m_vertices.resize(numVertices * m_vertexFormat.vertexSize());
    m_indices.resize(numIndices);

    memset(&m_vertices[0], 0xff, m_vertices.size());
}

FbxUInt32 Mesh::numberOfVertices()
{
    return m_numVertices;
}

FbxUInt32 Mesh::numberOfIndices()
{
    return m_numIndices;
}

Vertex Mesh::vertex(int vertexId)
{
    int vertexSize = m_vertexFormat.vertexSize();
    return Vertex(&m_vertices[0] + vertexSize * vertexId, &m_vertexFormat);
}

FbxUInt16 &Mesh::index(int id)
{
    return m_indices[id];
}
    
int Mesh::duplicateVertex(int vertexId)
{
    // Add a new vertex
    int vertexSize = m_vertexFormat.vertexSize();
    int newVertexId = m_numVertices;
    m_numVertices += 1;
    m_vertices.resize(vertexSize * m_numVertices);

    // Whose data are copied from the specified one.
    FbxUInt8 *source = &m_vertices[vertexSize * vertexId];
    FbxUInt8 *destination = &m_vertices[vertexSize * newVertexId];

    memcpy(destination, source, vertexSize);

    return newVertexId;
}


