// meshconverter.cpp
// FbxTool.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "meshconverter.h"

#include "../fbxtool.h"
#include "meshexporter.h"

#include <common/log.h>

#include <set>

MeshConverter::MeshConverter(FbxScene *scene, Arguments *arguments)
    : AssetConverter(scene, arguments)
{
}

bool MeshConverter::convert()
{
    class Comp
    {
    public:
        bool operator()(const std::pair<FbxMesh *, FbxNode *> &a,
                        const std::pair<FbxMesh *, FbxNode *> &b) const
        {
            return a.first < b.first;
        }
    };

    std::set<std::pair<FbxMesh *, FbxNode *>, Comp> meshes;
    
    //
    // Collect meshes
    //
    FbxNode *node = firstNode();
    while (node != NULL)
    {
        // Only output visible nodes. (3DsMax only uses Show property)
        if (node->GetVisibility() && node->Show.Get())
        {
            // Find all meshes connected to this node.
            int count = node->GetNodeAttributeCount();
            for (int i = 0; i < count; ++i)
            {
                FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex(i);
                if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh)
                {
                    FbxMesh *mesh = reinterpret_cast<FbxMesh*>(attrib);
                    meshes.insert(std::make_pair(mesh, node));
                }
            }
        }
        node = nextNode();
    }

    //
    // Convert meshes to spitfire3d format and export. 
    //
    std::set<std::pair<FbxMesh *, FbxNode *>, Comp>::iterator it = meshes.begin();
    std::set<std::pair<FbxMesh *, FbxNode *>, Comp>::iterator ie = meshes.end();
    while (it != ie)
    {
        FbxMesh *mesh = (*it).first;
        FbxNode *node = (*it).second;

        exportMesh(node, mesh);
        
        ++it;
    }

    return true;
}

bool MeshConverter::exportMesh(FbxNode *node, FbxMesh *mesh)
{
    FbxString meshName = getMeshName(node, mesh);

    int polygonCount = mesh->GetPolygonCount();
    for (int i = 0; i < polygonCount; ++i)
    {
        int polygonSize = mesh->GetPolygonSize(i);
        if (polygonSize != 3)
        {
			FBXSDK_printf("Warning: %s is not a triangle mesh.\n", meshName.Buffer());

			return false;
        }
    }

    //
    // Create vertex format and validate the FBX mesh.
    //
    VertexFormat vf;
    vf.addAttrib(VertexFormat::POSITION, 3, VertexFormat::FLOAT);

    // Normals
    int normalCount = mesh->GetElementNormalCount();
    if (normalCount > 1)
    {
        FBXSDK_printf("This mesh contains multiple normals. Currently we support only one.\n");
        return false;
    }
    else if (normalCount == 1)
    {
        if (m_arguments->vertexAttributes.Find('n') != -1 ||
            m_arguments->vertexAttributes.Find('a') != -1 )
        {
            vf.addAttrib(VertexFormat::NORMAL, 3, VertexFormat::FLOAT);
        }
    }

    // UVs 
    int uvCount = mesh->GetElementUVCount();
    if (uvCount >= 1)
    {
        if (uvCount > 4)
        {
            FBXSDK_printf("We only support up to 4 UVs per vertex.\n");
            return false;
        }

        if (m_arguments->vertexAttributes.Find('u') != -1 ||
            m_arguments->vertexAttributes.Find('a') != -1 )
        {
            for (int i = 0; i < uvCount; ++i)
            {
                vf.addAttrib(VertexFormat::AttribEnum(VertexFormat::TEXCOORD0 + i), 2, VertexFormat::FLOAT);
            }   
        }
    }

    // Colors
    int colorCount = mesh->GetElementVertexColorCount();
    if (colorCount > 1)
    {
        FBXSDK_printf("This mesh contains multiple vertex colors. Currently we support only one.\n");
    }
    else if (colorCount == 1)
    {
        if (m_arguments->vertexAttributes.Find('c') != -1 ||
            m_arguments->vertexAttributes.Find('a') != -1 )
        {
            vf.addAttrib(VertexFormat::COLOR, 4, VertexFormat::FLOAT);
        }
    }

        
    // Tangents & binormals
    int tangentCount = mesh->GetElementTangentCount();
    int binormalCount = mesh->GetElementBinormalCount();

    if ((tangentCount == 0 || binormalCount == 0) && 
        (m_arguments->vertexAttributes.Find('t') != -1 ||
         m_arguments->vertexAttributes.Find('b') != -1 ||
         m_arguments->vertexAttributes.Find('a') != -1))
    {
        mesh->GenerateTangentsDataForAllUVSets(true);
    }

    tangentCount = mesh->GetElementTangentCount();
    binormalCount = mesh->GetElementBinormalCount();

    if (tangentCount > 1)
    {
        FBXSDK_printf("This mesh contains multiple vertex tangents. Currently we support only one.\n");
    }
    else if (tangentCount == 1)
    {
        if (m_arguments->vertexAttributes.Find('t') != -1 ||
            m_arguments->vertexAttributes.Find('a') != -1)
        {
            vf.addAttrib(VertexFormat::TANGENT, 3, VertexFormat::FLOAT);
        }
    }
    
    if (binormalCount > 1)
    {
        FBXSDK_printf("This mesh contains multiple vertex binormals. Currently we support only one.\n");
    }
    else if (binormalCount == 1)
    {
        if (m_arguments->vertexAttributes.Find('b') != -1 ||
            m_arguments->vertexAttributes.Find('a') != -1)
        {
            vf.addAttrib(VertexFormat::BINORMAL, 3, VertexFormat::FLOAT);
        }
    }

    //
    // Build the mesh
    //
    Mesh myMesh;
    convertSplitted(mesh, vf, &myMesh);

    //
    // If the node contains the geometry transform, we should do the transformation.
    // See http://forums.autodesk.com/t5/FBX-SDK/GetNodeGlobalTransform-transforms-are-offset-incorrect/td-p/4260507
    FbxVector4 t = node->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 s = node->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxVector4 r = node->GetGeometricRotation(FbxNode::eSourcePivot);

    FbxAMatrix offset(t, r, s);
    myMesh.transform(offset);

    
    // 
    // Find the bounding box
    //
    for (FbxUInt32 i = 0; i < myMesh.numberOfVertices(); ++i)
    {
        Vertex v = myMesh.vertex(i);
        float *position = v.getAttrib(VertexFormat::POSITION);
        myMesh.box()[0] = std::min(myMesh.box()[0], position[0]);
        myMesh.box()[1] = std::min(myMesh.box()[1], position[1]);
        myMesh.box()[2] = std::min(myMesh.box()[2], position[2]);
        myMesh.box()[3] = std::max(myMesh.box()[3], position[0]);
        myMesh.box()[4] = std::max(myMesh.box()[4], position[1]);
        myMesh.box()[5] = std::max(myMesh.box()[5], position[2]);
    }
        

    //
    // Create a file stream
    //
    if (m_arguments->meshFormat == "obj")
    {
        FbxString outputFileName = meshName.Lower() + FbxString(".obj");
        FbxString path = FbxPathUtils::Bind(m_arguments->outputFolder, outputFileName.Buffer());

        MeshExporter exporter;
        if (!exporter.exportOBJ(path.Buffer(), &myMesh))
        {
            return false;
        }
                
        logInfo("%s f:%d v:%d", meshName.Buffer(), myMesh.numberOfIndices() / 3, myMesh.numberOfVertices());
    }
    else if (m_arguments->meshFormat == "pmh")
    {
        FbxString outputFileName = meshName.Lower() + FbxString(".pmh");

        FbxString path = FbxPathUtils::Bind(m_arguments->outputFolder, outputFileName.Buffer());

        MeshExporter exporter;
        if (!exporter.exportPMH(path.Buffer(), &myMesh))
        {
            return false;
        }

        logInfo("%s f:%d v:%d", meshName.Buffer(), myMesh.numberOfIndices() / 3, myMesh.numberOfVertices());
    }
    else
    {
        FBXSDK_printf("Unsupported mesh format.\n");
    }

    return true;
}
            

    
void MeshConverter::convertSplitted(FbxMesh *mesh, 
                                    const VertexFormat &vertexFormat,
                                    Mesh *converted)
{
    FbxUInt32 numVertices = mesh->GetPolygonCount() * 3;
    FbxUInt32 numIndices  = mesh->GetPolygonCount() * 3;

    Mesh init;
    init.initialize(vertexFormat, numVertices, numIndices);

    for (int i = 0; i < mesh->GetPolygonCount(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int index = i * 3 + j;
            
            int controlPointIndex = mesh->GetPolygonVertex(i, j);
            FbxVector4 p = mesh->GetControlPoints()[controlPointIndex];
            init.vertex(index).setAttrib(VertexFormat::POSITION, p); 
            
            if (vertexFormat.hasAttrib(VertexFormat::TANGENT))
            {
                FbxVector4 tangent = getTangent(mesh, index);
                init.vertex(index).setAttrib(VertexFormat::TANGENT, tangent);
            }
            if (vertexFormat.hasAttrib(VertexFormat::BINORMAL))
            {
                FbxVector4 binormal  = getBinormal(mesh, index);
                init.vertex(index).setAttrib(VertexFormat::BINORMAL, binormal);
            }
            if (vertexFormat.hasAttrib(VertexFormat::NORMAL))
            {
                FbxVector4 normal  = getNormal(mesh, index);
                init.vertex(index).setAttrib(VertexFormat::NORMAL, normal);
            }
            if (vertexFormat.hasAttrib(VertexFormat::TEXCOORD0))
            {
                FbxVector2 uv0 = getUV(mesh, 0, index);
                init.vertex(index).setAttrib(VertexFormat::TEXCOORD0, uv0);
            }
            if (vertexFormat.hasAttrib(VertexFormat::TEXCOORD1))
            {
                FbxVector2 uv1 = getUV(mesh, 1, index);
                init.vertex(index).setAttrib(VertexFormat::TEXCOORD1, uv1);
            }
            if (vertexFormat.hasAttrib(VertexFormat::TEXCOORD2))
            {
                FbxVector2 uv2 = getUV(mesh, 2, index);
                init.vertex(index).setAttrib(VertexFormat::TEXCOORD2, uv2);
            }
            if (vertexFormat.hasAttrib(VertexFormat::TEXCOORD3))
            {
                FbxVector2 uv3 = getUV(mesh, 3, index);
                init.vertex(index).setAttrib(VertexFormat::TEXCOORD3, uv3);
            }
            if (vertexFormat.hasAttrib(VertexFormat::COLOR))
            {
                FbxVector4 color     = getColor(mesh, index);
                init.vertex(index).setAttrib(VertexFormat::COLOR, color);
            }

            init.index(index) = index; 
        }
    }

    //
    // Compress the mesh by removing duplicated vertices.
    //
    int k = 0;
    int vertexSize = init.vertexFormat()->vertexSize();
    FbxUInt8 *tmpVertData = new FbxUInt8 [vertexSize * init.numberOfVertices()];
    for (FbxUInt32 i = 0; i < init.numberOfVertices(); ++i)
    {
        bool found = false;

        Vertex v = init.vertex(i);
        for (int j = 0; j < k; ++j)
        {
            if (memcmp(v.data(), &tmpVertData[j * vertexSize], vertexSize) == 0)
            {
                found = true;
                init.indices()[i] = j;
                break;
            }
        }
        
        if (!found)
        {
            memcpy(&tmpVertData[k * vertexSize], v.data(), vertexSize);
            init.indices()[i] = k;
            k++;
        }
    }

    //k = init.numberOfVertices();

    converted->initialize(vertexFormat, k, init.numberOfIndices()); 
    memcpy(converted->vertices(), tmpVertData, vertexSize * k);
    memcpy(converted->indices(), init.indices(), sizeof(FbxUInt16) * init.numberOfIndices());

    //FBXSDK_printf("(verts=%d, faces=%d) reduces to (verts=%d, faces=%d) after compression.\n", 
    //        init.numberOfVertices(), init.numberOfIndices() / 3, 
    //        converted->numberOfVertices(), converted->numberOfIndices() / 3);

}

void MeshConverter::convertShared(FbxMesh *mesh, 
                                  const VertexFormat &vertexFormat, 
                                  Mesh *converted)
{
    FbxUInt32 numVertices = mesh->GetControlPointsCount();
    FbxUInt32 numIndices  = mesh->GetPolygonCount() * 3;
    
    converted->initialize(vertexFormat, numVertices, numIndices);

    for (int i = 0; i < mesh->GetControlPointsCount(); ++i)
    {
        FbxVector4 p = mesh->GetControlPoints()[i];
        converted->vertex(i).setAttrib(VertexFormat::POSITION, p);
    }

    // First pass. Fill the vertices. For each vertex, we insert attributes
    // when they are empty.
    for (int i = 0; i < mesh->GetPolygonCount(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int index = i * 3 + j;
            
            int vertexId = mesh->GetPolygonVertex(i, j);

            FbxVector4 tangent   = getTangent(mesh, index);
            FbxVector4 binormal  = getBinormal(mesh, index);
            FbxVector4 normal    = getNormal(mesh, index);
            FbxVector2 uv0       = getUV(mesh, 0, index);
            FbxVector2 uv1       = getUV(mesh, 1, index);
            FbxVector2 uv2       = getUV(mesh, 2, index);
            FbxVector2 uv3       = getUV(mesh, 3, index);
            FbxVector4 color     = getColor(mesh, index);

            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::TANGENT))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::TANGENT, tangent);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::BINORMAL))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::BINORMAL, binormal);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::NORMAL))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::NORMAL, normal);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::TEXCOORD0))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::TEXCOORD0, uv0);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::TEXCOORD1))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::TEXCOORD1, uv1);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::TEXCOORD2))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::TEXCOORD2, uv2);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::TEXCOORD3))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::TEXCOORD3, uv3);
            }
            if (!converted->vertex(vertexId).isAttribValid(VertexFormat::COLOR))
            {
                converted->vertex(vertexId).setAttrib(VertexFormat::COLOR, color);
            }
        }
    }
    
    // Fill the control point splitted lists with their first element in each list
    // to be the original control point id.
    std::vector<std::list<int> > ctrlPointLists(mesh->GetControlPointsCount());
    for (int i = 0; i < mesh->GetControlPointsCount(); ++i)
    {
        ctrlPointLists[i].push_back(i);
    }

    // Second pass. Split the vertex when either of its properties have
    // two more values and create indices.
    for (int i = 0; i < mesh->GetPolygonCount(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int index = i * 3 + j;

            int vertexId = mesh->GetPolygonVertex(i, j);

            bool split = false;

            for (int attrib = VertexFormat::TANGENT; attrib <= VertexFormat::COLOR; ++attrib)
            {
                VertexAttribValue attribValue = getVertexAttrib(mesh, attrib, index);
                VertexFormat::AttribEnum attrib0 = VertexFormat::AttribEnum(attrib);
                if (vertexFormat.hasAttrib(VertexFormat::AttribEnum(attrib0)))
                {
                    if (split)
                    {
                        converted->vertex(vertexId).setAttrib(attrib0, attribValue);
                    }
                    else
                    {
                        split = true;

                        // Check if there is any previously visited control points that
                        // has the same attribute value with the current one. If so, 
                        // we no need to split the control points.
                        std::list<int>::iterator it = ctrlPointLists[vertexId].begin();
                        std::list<int>::iterator ie = ctrlPointLists[vertexId].end();
                        while (it != ie)
                        {
                            if (converted->vertex(*it).isAttribEqual(attrib0, attribValue))
                            {
                                split = false;
                                vertexId = *it;
                                break;
                            }

                            ++it;
                        }

                        if (split)
                        {
                            int oldVertexId = vertexId;
                            vertexId = converted->duplicateVertex(vertexId);
                            ctrlPointLists[oldVertexId].push_back(vertexId);
                            converted->vertex(vertexId).setAttrib(attrib0, attribValue);
                        }
                    }
                }
            }
            
            converted->index(index) = vertexId;
        }
    }
}
    

FbxVector4 MeshConverter::getNormal(FbxMesh *mesh, int index)
{
    FbxGeometryElementNormal *normal = mesh->GetElementNormal();

    FbxVector4 normal0;
    switch (normal->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
            normal0 = normal->GetDirectArray().GetAt(index);
            break;
        case FbxGeometryElement::eIndexToDirect:
            {
                int id = normal->GetIndexArray().GetAt(index);
                normal0 = normal->GetDirectArray().GetAt(id);
            }
            break;
        default:
            break; // other reference modes not shown here!
    }

    return normal0;
}

FbxVector4 MeshConverter::getBinormal(FbxMesh *mesh, int index)
{
    FbxGeometryElementBinormal *binormal = mesh->GetElementBinormal();

    FbxVector4 binormal0;
    switch (binormal->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
            binormal0 = binormal->GetDirectArray().GetAt(index);
            break;
        case FbxGeometryElement::eIndexToDirect:
            {
                int id = binormal->GetIndexArray().GetAt(index);
                binormal0 = binormal->GetDirectArray().GetAt(id);
            }
            break;
        default:
            break; // other reference modes not shown here!
    }

    return binormal0;
}

FbxVector4 MeshConverter::getTangent(FbxMesh *mesh, int index)
{
    FbxGeometryElementTangent *tangent = mesh->GetElementTangent();

    FbxVector4 tangent0;
    switch (tangent->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
            tangent0 = tangent->GetDirectArray().GetAt(index);
            break;
        case FbxGeometryElement::eIndexToDirect:
            {
                int id = tangent->GetIndexArray().GetAt(index);
                tangent0 = tangent->GetDirectArray().GetAt(id);
            }
            break;
        default:
            break; // other reference modes not shown here!
    }

    return tangent0;
}

FbxVector2 MeshConverter::getUV(FbxMesh *mesh, int id, int index)
{
    FbxGeometryElementUV *uv = mesh->GetElementUV(id);

    FbxVector2 uv0;
    switch (uv->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
            uv0 = uv->GetDirectArray().GetAt(index);
            break;
        case FbxGeometryElement::eIndexToDirect:
            {
                int id = uv->GetIndexArray().GetAt(index);
                uv0 = uv->GetDirectArray().GetAt(id);
            }
            break;
        default:
            break; // other reference modes not shown here!
    }

    // Autodesk's tools output vertical flipped UV.
    uv0.mData[1] = 1.0f - uv0.mData[1];

    return uv0;
}

FbxVector4 MeshConverter::getColor(FbxMesh *mesh, int index)
{
    FbxGeometryElementVertexColor *color = mesh->GetElementVertexColor();

    FbxColor color0;
    switch (color->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
            color0 = color->GetDirectArray().GetAt(index);
            break;
        case FbxGeometryElement::eIndexToDirect:
            {
                int id = color->GetIndexArray().GetAt(index);
                color0 = color->GetDirectArray().GetAt(id);
            }
            break;
        default:
            break; // other reference modes not shown here!
    }

    return FbxVector4(color0[0], color0[1], color0[2], color0[3]);
}
    
VertexAttribValue MeshConverter::getVertexAttrib(FbxMesh *mesh, int attrib, int index)
{
    VertexAttribValue ret;
    switch (attrib)
    {
        case VertexFormat::NORMAL:
            ret.vec4 = getNormal(mesh, index);
            break;
        case VertexFormat::BINORMAL:
            ret.vec4 = getBinormal(mesh, index);
            break;
        case VertexFormat::TANGENT:
            ret.vec4 = getTangent(mesh, index);
            break;
        case VertexFormat::COLOR:
            ret.vec4 = getColor(mesh, index);
            break;
        case VertexFormat::TEXCOORD0:
            ret.vec2 = getUV(mesh, 0, index);
            break;
        case VertexFormat::TEXCOORD1:
            ret.vec2 = getUV(mesh, 1, index);
            break;
        case VertexFormat::TEXCOORD2:
            ret.vec2 = getUV(mesh, 2, index);
            break;
        case VertexFormat::TEXCOORD3:
            ret.vec2 = getUV(mesh, 3, index);
            break;
        default:
            FBXSDK_printf("Unrecognized vertex attribute.\n");
            break;
    }

    return ret;
}

/*
bool MeshConverter::isControlPointSplitted(FbxLayerElement *element)
{
    bool ret = false;
    if (element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
    {
        switch (element->GetReferenceMode())
        {
            case FbxGeometryElement::eDirect:
                ret = true;
                break;
            case FbxGeometryElement::eIndexToDirect:
                ret = false;
                break;
            default:
               FBXSDK_printf("Unsupported the FBX element reference mode.\n");
                return false;
        }
    }

    return ret;
}
*/
    
FbxString MeshConverter::getMeshName(FbxNode *node, FbxMesh *mesh)
{
    static FbxUInt32 i = 0;
    FbxString prefix;
    const char *meshName = mesh->GetName();
    if (meshName == NULL || meshName[0] == 0)
    {
        meshName = node->GetName();
    }

    if (meshName == NULL || meshName[0] == 0)
    {
        return FbxString("mesh_") + FbxString(int(i++));
    }
        
    return FbxString(meshName);
}
