// meshexporter.cpp
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "meshexporter.h"

#include "mesh.h"
#include <common/filestream.h>
#include <common/version.h>
#include <common/pmhmodel.h>
#include <common/log.h>

bool MeshExporter::exportOBJ(const char *filename, Mesh *mesh)
{
    //
    // Open the stream.
    //
    FileStream stream(filename);
    if (!stream.openToWrite())
    {
        logError("Fail to create %s.", filename);
        return false;
    }

    try
    {
        VertexFormat *vf = mesh->vertexFormat();
        for (FbxUInt32 i = 0; i < mesh->numberOfVertices(); ++i)
        {
            Vertex v = mesh->vertex(i);
            float *attribData = NULL;
            for (FbxUInt32 j = 0; j < vf->numberOfAttribs(); ++j)
            {
                switch (vf->attrib(j).attrib)
                {
                    case VertexFormat::POSITION:
                        attribData = v.getAttrib(VertexFormat::POSITION);
                        stream.printf("v %f %f %f\n", attribData[0], attribData[1], attribData[2]);
                        break;
                    case VertexFormat::NORMAL:
                        attribData = v.getAttrib(VertexFormat::NORMAL);
                        stream.printf("vn %f %f %f\n", attribData[0], attribData[1], attribData[2]);
                        break;
                    case VertexFormat::TEXCOORD0:
                        attribData = v.getAttrib(VertexFormat::TEXCOORD0);
                        stream.printf("vt %f %f\n", attribData[0], attribData[1]);
                        break;
                    default:
                        break;
                }
            }    
        }

        FbxUInt16 *id = mesh->indices();

        if (vf->hasAttrib(VertexFormat::NORMAL) &&
            vf->hasAttrib(VertexFormat::TEXCOORD0))
        {
            for (FbxUInt32 i = 0; i < mesh->numberOfIndices(); i += 3)
            {
                stream.printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                    id[0] + 1, id[0] + 1, id[0] + 1, 
                    id[1] + 1, id[1] + 1, id[1] + 1, 
                    id[2] + 1, id[2] + 1, id[2] + 1);
                id += 3;
            }
        }
        else if (!vf->hasAttrib(VertexFormat::NORMAL) &&
                 vf->hasAttrib(VertexFormat::TEXCOORD0))
        {
            for (FbxUInt32 i = 0; i < mesh->numberOfIndices(); i += 3)
            {
                stream.printf("f %d//%d %d//%d %d//%d\n", 
                    id[0] + 1, id[0] + 1, id[0] + 1, 
                    id[2] + 1, id[2] + 1, id[2] + 1);
                id += 3;
            }
        }
        else if (vf->hasAttrib(VertexFormat::NORMAL) &&
                 !vf->hasAttrib(VertexFormat::TEXCOORD0))
        {
            for (FbxUInt32 i = 0; i < mesh->numberOfIndices(); i += 3)
            {
                stream.printf("f %d/%d/ %d/%d/ %d/%d/\n", 
                    id[0] + 1, id[0] + 1, id[0] + 1, 
                    id[2] + 1, id[2] + 1, id[2] + 1);
                id += 3;
            }
        }
        else
        {
            for (FbxUInt32 i = 0; i < mesh->numberOfIndices(); i += 3)
            {
                stream.printf("f %d %d %d\n", 
                    id[2] + 1, id[2] + 1, id[2] + 1);
                id += 3;
            }
        }
    }
    catch (FbxString &message)
    {
        logError("Fail to write to %s.", filename);
        logError("error message \"%s\".", message.Buffer());
        return false;
    }

    //
    // Close the stream.
    //
    stream.close();

    return true;
}

bool MeshExporter::exportPMH(const char *filename, Mesh *mesh)
{
    PmhModel model;
    
    model.numVertices = mesh->numberOfVertices();
    model.numIndices  = mesh->numberOfIndices();

	VertexFormat *vf = mesh->vertexFormat();

	unsigned int vertexSize = vf->vertexSize();
    model.indices  = new unsigned short [model.numIndices];
    unsigned char *vertData = new unsigned char [model.numVertices * vertexSize];
	model.vertices = (float *)vertData;

    memcpy(model.indices, mesh->indices(), model.numIndices * sizeof(unsigned short));
    memcpy(model.vertices , mesh->vertices(), model.numVertices * vertexSize);

    model.bb[0][0] = mesh->box()[0];
    model.bb[0][1] = mesh->box()[1];
    model.bb[0][2] = mesh->box()[2];
    model.bb[1][0] = mesh->box()[3];
    model.bb[1][1] = mesh->box()[4];
    model.bb[1][2] = mesh->box()[5];
	
    model.numAttribs = vf->numberOfAttribs();

    for (FbxUInt32 i = 0; i < vf->numberOfAttribs(); ++i)
    {
        char *type = NULL;
        if (vf->attrib(i).type == VertexFormat::FLOAT)
        {
            type = "float32";
        }
        else if (vf->attrib(i).type == VertexFormat::INT32)
        {
            type = "int32";
        }
        else if (vf->attrib(i).type == VertexFormat::UINT32)
        {
            type = "uint32";
        }

        model.attribs[i].name = vf->attrib(i).name.Buffer(); 
        model.attribs[i].type = type;
        model.attribs[i].size = vf->attrib(i).size;
    }

    model.vertexSize = vf->vertexSize();

    return model.write(filename);
}
    
