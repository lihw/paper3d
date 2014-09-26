// sceneconverter.cpp
// FBXConverter.
//
// Copyright 2012 - 2014 Future Interface. 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "sceneconverter.h"

#include "../fbxtool.h"
#include "scene.h"

#include <map>
#include <list>

SceneConverter::SceneConverter(FbxScene *scene, Arguments *arguments)
    : AssetConverter(scene, arguments)
{
    m_numDrawables = 0;
    m_numLights = 0;
    m_numCameras = 0;
}

bool SceneConverter::convert()
{
    //
    // Construt the scene
    //
    FbxNode *node = m_scene->GetRootNode();
    std::list<FbxNode *> nodes;
    std::map<FbxNode *, SceneNode *> fbxNode2SceneNodes;
    Scene scene;

    nodes.push_back(node);
    SceneNode *sceneNode = makeSceneNode(node);
    scene.addNode(sceneNode);
    fbxNode2SceneNodes.insert(std::make_pair(node, sceneNode));

    while (!nodes.empty())
    {
        FbxNode *ret = nodes.front();
        nodes.pop_front();

        for (int i = 0; i < ret->GetChildCount(); i++)
        {
            FbxNode *child = ret->GetChild(i);
            
            // Only output visible nodes.
            if (child->GetVisibility() && child->Show.Get())
            {
                SceneNode *sceneNode = makeSceneNode(child);
                if (sceneNode != NULL)
                {
                    if (sceneNode->type == "camera")
                    {
                        // The first camera will be the main camera of the scene
                        scene.setCamera(sceneNode);
                    }
                    scene.addNode(sceneNode, fbxNode2SceneNodes[ret]);
                    fbxNode2SceneNodes.insert(std::make_pair(child, sceneNode));
                }
            
                nodes.push_back(child);
            }
        }
    }

    // Create a camera if it is not included in FBX. The camera is evaluated
    // using the bounding box of all visible nodes.
    if (m_numCameras == 0)
    {
        FbxVector4 rootBboxMin;
        FbxVector4 rootBboxMax;
        FbxVector4 rootBboxCenter;

        rootBboxMin = FbxVector4(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
        rootBboxMax = FbxVector4(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
    
        FbxNode *node = m_scene->GetRootNode();

        nodes.push_back(node);

        while (!nodes.empty())
        {
            FbxNode *ret = nodes.front();
            nodes.pop_front();

            for (int i = 0; i < ret->GetChildCount(); i++)
            {
                FbxNode *child = ret->GetChild(i);
                
                nodes.push_back(child);
            }
                
            if (ret->GetChildCount() == 0 &&
                ret->GetVisibility() && 
                ret->Show.Get() &&
                ret->GetMesh() != NULL)
            {
                FbxVector4 bboxMin;
                FbxVector4 bboxMax;
                FbxVector4 bboxCenter;

                ret->EvaluateGlobalBoundingBoxMinMaxCenter(bboxMin, bboxMax, bboxCenter);

                rootBboxMin[0] = std::min(rootBboxMin[0], bboxMin[0]);
                rootBboxMin[1] = std::min(rootBboxMin[1], bboxMin[1]);
                rootBboxMin[2] = std::min(rootBboxMin[2], bboxMin[2]);
                
                rootBboxMax[0] = std::max(rootBboxMax[0], bboxMax[0]);
                rootBboxMax[1] = std::max(rootBboxMax[1], bboxMax[1]);
                rootBboxMax[2] = std::max(rootBboxMax[2], bboxMax[2]);
            }
        }

        rootBboxCenter = (rootBboxMin + rootBboxMax) / 2;
        FbxVector4 rootBboxSize = rootBboxMax - rootBboxMin;
            
        SceneNode *sceneNode = new SceneNode();

        sceneNode->type = FbxString("camera");
        sceneNode->attributes.push_back(std::make_pair(FbxString("name"), FbxString("camera")));
        sceneNode->attributes.push_back(std::make_pair(FbxString("fixed"), FbxString("true")));

        double diag = sqrt(rootBboxSize[0] * rootBboxSize[0] + 
                           rootBboxSize[1] * rootBboxSize[1] + 
                           rootBboxSize[2] * rootBboxSize[2]) * 0.5;
            
        double eye = diag / tan(15.0 * FBXSDK_PI_DIV_180);

        double position[3];
        double up[3];
        double znear;
        double zfar;
            
        znear = eye - diag - 1.0f;
        zfar  = eye + diag + 1.0f;

        if (rootBboxSize[0] <= rootBboxSize[1] && rootBboxSize[0] <= rootBboxSize[2])
        {
            position[0] = eye + rootBboxCenter[0]; 
            position[1] = rootBboxCenter[1];
            position[2] = rootBboxCenter[2];

            up[0] = 0;
            up[1] = 1;
            up[2] = 0;
        }
        else if (rootBboxSize[1] <= rootBboxSize[0] && rootBboxSize[1] <= rootBboxSize[2])
        {
            position[0] = rootBboxCenter[0]; 
            position[1] = eye + rootBboxCenter[1];
            position[2] = rootBboxCenter[2];

            up[0] = 0;
            up[1] = 0;
            up[2] = 1;
        }
        else
        {
            position[0] = rootBboxCenter[0]; 
            position[1] = rootBboxCenter[1];
            position[2] = eye + rootBboxCenter[2];

            up[0] = 0;
            up[1] = 1;
            up[2] = 0;
        }

        char lookat[1024];
        char perspective[1024];
            
        FBXSDK_sprintf(lookat, 1024, "eye:%8.5f,%8.5f,%8.5f,center:%8.5f,%8.5f,%8.5f,up:%8.5f,%8.5f,%8.5f",
                    (float)position[0], (float)position[1], (float)position[2],
                    (float)rootBboxCenter[0], (float)rootBboxCenter[1], (float)rootBboxCenter[2],
                    (float)up[0], (float)up[1], (float)up[2]);
        sceneNode->attributes.push_back(std::make_pair(FbxString("lookat"), FbxString(lookat)));
            
        FBXSDK_sprintf(perspective, 1024, "perspective,fov:%8.5f,aspect:-1,znear:%8.5f,zfar:%8.5f",
                    30.0f, (float)znear, (float)zfar);
        sceneNode->attributes.push_back(std::make_pair(FbxString("projection"), FbxString(perspective)));
                    
        scene.setCamera(sceneNode);
        scene.addNode(sceneNode, scene.root());
    }

    //
    // Output the file.
    //
    //FbxString outputFilename = FbxPathUtils::GetFileName(m_arguments->FBXFileName.Buffer()).Lower();
    //outputFilename.FindAndReplace(".fbx", ".psc");
    FbxString outputFilename("scene.psc");
    FbxString path = FbxPathUtils::Bind(m_arguments->outputFolder, outputFilename.Buffer());
    bool ret = scene.output(path.Buffer());

    if (!ret)
    {
        FBXSDK_printf("Exporting failed!\n\n");
    }

    return ret;
}
    
SceneNode *SceneConverter::makeSceneNode(FbxNode *node)
{
    SceneNode *sceneNode = new SceneNode();

    if (node->GetParent() == NULL) // The root
    {
        // Type
        sceneNode->type = FbxString("root");
        
        // Name
        sceneNode->attributes.push_back(std::make_pair(FbxString("name"), node->GetName()));
        
        // Transformation
        FbxAMatrix m = node->EvaluateGlobalTransform();

        const FbxVector4 translation = m.GetT();
        const FbxVector4 rotation    = m.GetR();
        const FbxVector4 scaling     = m.GetS();
        
        char buffer[1024];
        FBXSDK_sprintf(buffer, 1024, "s:%8.5f,%8.5f,%8.5f,r:%8.5f,%8.5f,%8.5f,t:%8.5f,%8.5f,%8.5f",
            (float)scaling[0], (float)scaling[1], (float)scaling[2],
            (float)rotation[0], (float)rotation[1], (float)rotation[2],
            (float)translation[0], (float)translation[1], (float)translation[2]);

        sceneNode->attributes.push_back(std::make_pair(FbxString("transform"), FbxString(buffer)));
    }
    else
    {
        FbxCamera *camera = node->GetCamera();
        if (camera != NULL)
        {
            sceneNode->type = FbxString("camera");
        
            sceneNode->attributes.push_back(std::make_pair(FbxString("name"), node->GetName()));

            sceneNode->attributes.push_back(std::make_pair(FbxString("fixed"), FbxString("true")));

            FbxVector4 position = camera->EvaluatePosition();
            FbxVector4 center = camera->EvaluateLookAtPosition();
            // FIXME: seems EvaluateUpDirection doesn't give correct result as it 
            // is affected by its parent nodes' tranforms however we attach camera
            // to the root in paper3d's scene.
            // FbxVector4 up = camera->EvaluateUpDirection(position, center);
            FbxDouble3 up = camera->UpVector.Get();

            char buffer[1024];
            FBXSDK_sprintf(buffer, 1024, "eye:%8.5f,%8.5f,%8.5f,center:%8.5f,%8.5f,%8.5f,up:%8.5f,%8.5f,%8.5f",
                    (float)position[0], (float)position[1], (float)position[2],
                    (float)center[0], (float)center[1], (float)center[2],
                    (float)up[0], (float)up[1], (float)up[2]);
            sceneNode->attributes.push_back(std::make_pair(FbxString("lookat"), FbxString(buffer)));

            float nearZ = (float)camera->GetNearPlane();
            float farZ = (float)camera->GetFarPlane();

            if (camera->ProjectionType.Get() == FbxCamera::ePerspective)
            {
                FbxCamera::EAspectRatioMode lCamAspectRatioMode = camera->GetAspectRatioMode();
                double lAspectX = camera->AspectWidth.Get();
                double lAspectY = camera->AspectHeight.Get();
                double lAspectRatio = 1.333333;
                switch( lCamAspectRatioMode)
                {
                    case FbxCamera::eWindowSize:
                        lAspectRatio = lAspectX / lAspectY;
                        break;
                    case FbxCamera::eFixedRatio:
                        lAspectRatio = lAspectX;

                        break;
                    case FbxCamera::eFixedResolution:
                        lAspectRatio = lAspectX / lAspectY * camera->GetPixelRatio();
                        break;
                    case FbxCamera::eFixedWidth:
                        lAspectRatio = camera->GetPixelRatio() / lAspectY;
                        break;
                    case FbxCamera::eFixedHeight:
                        lAspectRatio = camera->GetPixelRatio() * lAspectX;
                        break;
                    default:
                        break;

                }

                //get the aperture ratio
                double lFilmHeight = camera->GetApertureHeight();
                double lFilmWidth = camera->GetApertureWidth() * camera->GetSqueezeRatio();
                //here we use Height : Width
                double lApertureRatio = lFilmHeight / lFilmWidth;

                //change the aspect ratio to Height : Width
                lAspectRatio = 1 / lAspectRatio;
                //revise the aspect ratio and aperture ratio
                FbxCamera::EGateFit lCameraGateFit = camera->GateFit.Get();
                switch( lCameraGateFit )
                {

                    case FbxCamera::eFitFill:
                        if( lApertureRatio > lAspectRatio)  // the same as eHORIZONTAL_FIT
                        {
                            lFilmHeight = lFilmWidth * lAspectRatio;
                            camera->SetApertureHeight( lFilmHeight);
                            lApertureRatio = lFilmHeight / lFilmWidth;
                        }
                        else if( lApertureRatio < lAspectRatio) //the same as eVERTICAL_FIT
                        {
                            lFilmWidth = lFilmHeight / lAspectRatio;
                            camera->SetApertureWidth( lFilmWidth);
                            lApertureRatio = lFilmHeight / lFilmWidth;
                        }
                        break;
                    case FbxCamera::eFitVertical:
                        lFilmWidth = lFilmHeight / lAspectRatio;
                        camera->SetApertureWidth( lFilmWidth);
                        lApertureRatio = lFilmHeight / lFilmWidth;
                        break;
                    case FbxCamera::eFitHorizontal:
                        lFilmHeight = lFilmWidth * lAspectRatio;
                        camera->SetApertureHeight( lFilmHeight);
                        lApertureRatio = lFilmHeight / lFilmWidth;
                        break;
                    case FbxCamera::eFitStretch:
                        lAspectRatio = lApertureRatio;
                        break;
                    case FbxCamera::eFitOverscan:
                        if( lFilmWidth > lFilmHeight)
                        {
                            lFilmHeight = lFilmWidth * lAspectRatio;
                        }
                        else
                        {
                            lFilmWidth = lFilmHeight / lAspectRatio;
                        }
                        lApertureRatio = lFilmHeight / lFilmWidth;
                        break;
                    case FbxCamera::eFitNone:
                    default:
                        break;
                }
                //change the aspect ratio to Width : Height
                lAspectRatio = 1 / lAspectRatio;

#define HFOV2VFOV(h, ar) (2.0 * atan((ar) * tan( (h * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectY / aspectX
#define VFOV2HFOV(v, ar) (2.0 * atan((ar) * tan( (v * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectX / aspectY
                double lFieldOfViewX = 0.0;
                double lFieldOfViewY = 0.0;
                if (camera->GetApertureMode() == FbxCamera::eVertical)
                {
                    lFieldOfViewY = camera->FieldOfView.Get();
                    lFieldOfViewX = VFOV2HFOV( lFieldOfViewY, 1 / lApertureRatio);
                }
                else if (camera->GetApertureMode() == FbxCamera::eHorizontal)
                {
                    lFieldOfViewX = camera->FieldOfView.Get(); //get HFOV
                    lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
                }
                else if (camera->GetApertureMode() == FbxCamera::eFocalLength)
                {
                    lFieldOfViewX = camera->ComputeFieldOfView(camera->FocalLength.Get());    //get HFOV
                    lFieldOfViewY = HFOV2VFOV( lFieldOfViewX, lApertureRatio);
                }
                else if (camera->GetApertureMode() == FbxCamera::eHorizAndVert) 
                {
                    lFieldOfViewX = camera->FieldOfViewX.Get();
                    lFieldOfViewY = camera->FieldOfViewY.Get();
                }
#undef HFOV2VFOV
#undef VFOV2HFOV

                FBXSDK_sprintf(buffer, 1024, "perspective,fov:%8.5f,aspect:-1,znear:%8.5f,zfar:%8.5f",
                        (float)lFieldOfViewY, nearZ, farZ);
                sceneNode->attributes.push_back(std::make_pair(FbxString("projection"), FbxString(buffer)));

            }
            else
            {
                FBXSDK_sprintf(buffer, 1024, "orthogonal,aspect:-1,znear:%8.5f,zfar:%8.5f", nearZ, farZ);
                sceneNode->attributes.push_back(std::make_pair(FbxString("projection"), FbxString(buffer)));
            }

                
            m_numCameras++;
        }
        else
        {
            FbxLight *light = node->GetLight();
            if (light != NULL)
            {
                m_numLights++;

                FBX_ASSERT(!"Not implemented!");
            }
            else
            {
                // Type
                sceneNode->type = FbxString("drawable");

                // Name
                sceneNode->attributes.push_back(std::make_pair(FbxString("name"), node->GetName()));

                // Transformation
                FbxAMatrix m = node->EvaluateLocalTransform();
                //FbxAMatrix m1 = node->EvaluateGlobalTransform();


                const FbxVector4 translation = m.GetT();
                const FbxVector4 rotation    = m.GetR();
                const FbxVector4 scaling     = m.GetS();

                float s[3], r[3], t[3];
                t[0] = (float)translation.mData[0];
                t[1] = (float)translation.mData[1];
                t[2] = (float)translation.mData[2];

                r[0] = (float)(rotation[0] * FBXSDK_DEG_TO_RAD);
                r[1] = (float)(rotation[1] * FBXSDK_DEG_TO_RAD);
                r[2] = (float)(rotation[2] * FBXSDK_DEG_TO_RAD);

                s[0] = (float)scaling[0];
                s[1] = (float)scaling[1];
                s[2] = (float)scaling[2];

                //const FbxVector4 translation1 = m1.GetT();
                //const FbxVector4 rotation1    = m1.GetR();
                //const FbxVector4 scaling1     = m1.GetS();


                char buffer[1024];
                FBXSDK_sprintf(buffer, 1024, "s:%8.5f,%8.5f,%8.5f,r:%8.5f,%8.5f,%8.5f,t:%8.5f,%8.5f,%8.5f",
                        s[0], s[1], s[2], r[0], r[1], r[2], t[0], t[1], t[2]);

                sceneNode->attributes.push_back(std::make_pair(FbxString("transform"), FbxString(buffer)));

                // Mesh
                //if (node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
                //{
                //    FBXSDK_printf("fuck");
                //}

                static FbxUInt32 i = 0;
                if (node->GetMesh() == NULL)
                {
                    sceneNode->type = FbxString("node");
                }
                else
                {
                    const char *meshName = node->GetMesh()->GetName();
                    if (meshName == NULL || meshName[0] == 0)
                    {
                        meshName = node->GetName();
                    }

                    FbxString prefix;
                    if (meshName == NULL || meshName[0] == 0)
                    {
                        prefix = FbxString("mesh_") + FbxString(int(i++));
                    }
                    else
                    {
                        prefix = FbxString(meshName);
                    }
                    sceneNode->geometry = prefix + FbxString(".") + m_arguments->meshFormat;

                    // Material
                    FbxSurfaceMaterial *material = node->GetMaterial(0); 

                    if (material != NULL)
                    {
                        // This only gets the material of type sDiffuse, you probably need to
                        // traverse all Standard Material Property by its name to get all
                        // possible textures.
                        FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

                        // Check if it's layeredtextures
                        int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

                        if (prop.GetSrcObjectCount<FbxTexture>() > 0)
                        {
                            FbxFileTexture *lTex = prop.GetSrcObject<FbxFileTexture>(0);
                            FbxString filename = FbxPathUtils::GetFileName(lTex->GetFileName());
                            sceneNode->texture = filename.Lower();

                            m_textures.push_back(FbxString(lTex->GetFileName()));
                        }

                        // root node is not counted as a drawable.
                        m_numDrawables++;
                    }
                }
            }
        }
    }
    
    return sceneNode;
}
    
