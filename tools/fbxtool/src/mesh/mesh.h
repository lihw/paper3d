// mesh.h
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <fbxsdk.h>


class VertexFormat
{
public:
    enum AttribEnum
    {
        POSITION,
        TANGENT,
        BINORMAL,
        NORMAL,
        TEXCOORD0,
        TEXCOORD1,
        TEXCOORD2,
        TEXCOORD3,
        COLOR,
    };

    enum TypeEnum
    {
        FLOAT,
        INT32,
        UINT32,
        BOOL,
    };

    struct Attribute
    {
        AttribEnum attrib;
        FbxString name;
        int       size;
        TypeEnum  type;
        int       offset;
    };

    VertexFormat() {}
    ~VertexFormat() {}

    Attribute &attrib(int index)  { return m_attributes[index]; }
    FbxUInt32 numberOfAttribs() const { return m_attributes.size(); }

    bool hasAttrib(AttribEnum attrib) const;
    int vertexSize() const;
    void addAttrib(AttribEnum attrib, int size, TypeEnum type);
    FbxUInt32 getAttribOffset(AttribEnum attrib) const;

private:
    std::vector<Attribute> m_attributes;
};

struct VertexAttribValue
{
    FbxVector2 vec2;
    FbxVector4 vec4;
};

class Vertex
{
public:
    Vertex(FbxUInt8* data, VertexFormat *vertexFormat);
    ~Vertex();

    void setAttrib(VertexFormat::AttribEnum attrib, const FbxVector4 &value);
    void setAttrib(VertexFormat::AttribEnum attrib, const FbxVector2 &value);
    void setAttrib(VertexFormat::AttribEnum attrib, const VertexAttribValue &value);
    bool isAttribValid(VertexFormat::AttribEnum attrib);
    bool isAttribEqual(VertexFormat::AttribEnum attrib, const VertexAttribValue &value);

    float *getAttrib(VertexFormat::AttribEnum attrib);

    FbxUInt8 *data() const { return m_data; }
private:
    FbxUInt8     *m_data;
    VertexFormat *m_vertexFormat;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void initialize(const VertexFormat &vertexFormat, FbxUInt32 numVertices, FbxUInt32 numIndices); 

    float *vertices() { return (float*)&m_vertices[0]; }
    FbxUInt16 *indices() { return &m_indices[0]; }
    FbxUInt32 numberOfVertices();
    FbxUInt32 numberOfIndices();
    int duplicateVertex(int vertexId);
    Vertex vertex(int id);
    FbxUInt16 &index(int id);

    VertexFormat *vertexFormat() { return &m_vertexFormat; }
    FbxFloat* box() { return m_box; };

    void transform(const FbxAMatrix &matrix);

private:
    std::vector<FbxUInt8>    m_vertices;
    std::vector<FbxUInt16>   m_indices;
    FbxUInt32                m_numVertices;
    FbxUInt32                m_numIndices;
    VertexFormat             m_vertexFormat;
    FbxFloat                 m_box[6];
};



#endif // !MESH_H
