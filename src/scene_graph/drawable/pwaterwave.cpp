// pwaterwave.cpp
// Shader based water wave effects with reflection and refraction.
//
// Copyright 2012 - 2014 Future Interface . 
// This software is licensed under the terms of the MIT license.
//
// Hongwei Li lihw81@gmail.com
//

#include "pwaterwave.h"

#include <math.h>

#include <Paper3D/pcamera.h>
#include <Paper3D/pscene.h>
#include <Paper3D/pgeometryplane.h>
#include <Paper3D/pframebuffer.h>
#include <Paper3D/pmaterial.h>
#include <Paper3D/pmaterialparameter.h>
#include <Paper3D/presourcemanager.h>
#include <Paper3D/prenderstate.h>
#include <Paper3D/prenderer.h>
#include <Paper3D/prendertarget.h>
#include <Paper3D/prenderpass.h>
#include <Paper3D/prenderqueue.h>
#include <Paper3D/pmesh.h>

#include <PFoundation/pcontext.h>
#include <PFoundation/pvector4.h>
#include <PFoundation/pplane.h>
#include <PFoundation/pxmlelement.h>
#include <PFoundation/pglframebuffer.h>


P_OBJECT_DEFINE(PWaterWave);

// FIXME: use brief version of variable names to make this string
// shorter.
static const pchar *WATERWAVE_PMT = 
	"<material>\n"
	"  <shader>\n"
	"    <vs>\n"
	"      uniform mat4 MVP;\n"
	"      uniform vec4 BumpTranslation;\n"
	"      uniform vec4 BumpScale;\n"
	"\n"
	"      attribute mediump vec3 position;\n"
	"      attribute mediump vec2 texcoord0;\n"
	"\n"
	"      varying mediump vec2 BumpCoord0;\n"
	"      varying mediump vec2 BumpCoord1;\n"
	"      varying mediump vec3 WorldPosition;\n"
	"\n"
	"      void main()\n"
	"      {\n"
	"          gl_Position = MVP * vec4(position, 1.0);\n"
	"          BumpCoord0 = texcoord0 * BumpScale.xy + BumpTranslation.xy;\n"
	"          BumpCoord1 = texcoord0 * BumpScale.zw + BumpTranslation.zw;\n"
	"          WorldPosition = position;\n"
	"      }\n"
	"    </vs>\n"
	"    <fs>\n"
	"      precision mediump float;\n"
	"\n"
	"      uniform int Reflection;\n"
	"      uniform sampler2D ReflectionMap;\n"
	"      uniform sampler2D NormalMap;\n"
	"      uniform sampler2D ColorMap;\n"
	"      uniform vec2 invViewport;\n"
	"      uniform mediump float ReflectionDistortion;\n"
	"      uniform vec3 CameraWorldPosition;\n"
	"\n"
	"      varying mediump vec2 BumpCoord0;\n"
	"      varying mediump vec2 BumpCoord1;\n"
	"      varying mediump vec3 WorldPosition;\n"
	"      \n"
	"      void main()\n"
	"      {\n"
	"           mediump vec2 coord = gl_FragCoord.xy * invViewport;\n"
	"\n"
	"           // get bumped normal.\n"
	"           mediump vec3 normal = vec3(0, 0, 0);\n"
	"           normal += texture2D(NormalMap, BumpCoord0).rgb;\n"
	"           normal += texture2D(NormalMap, BumpCoord1).rgb;\n"
	"           normal = normalize(normal - vec3(1.0));\n"
	"\n"
	"           // get view direction.\n"
	"           mediump vec3 ViewDir = (CameraWorldPosition - WorldPosition).xyz;\n"
	"           mediump vec3 viewDir = normalize(ViewDir); \n"
	"\n"
	"           mediump float fresnelFac = max(dot(viewDir, normal), 0.0);\n"
	"\n"
	"           // apply reflection distortion.\n"
	"           coord.xy += normal.xy * ReflectionDistortion * vec2(1.0, 0.1); // times viewDir.y to reduce the noise at horizon\n"
	"   \n"
	"           mediump vec4 waterColor = texture2D(ColorMap, vec2(fresnelFac, fresnelFac)); \n"
	"           mediump vec4 mixedColor = waterColor;\n"
    "           waterColor.a = 1.0;\n"
        // FIXME: reflection and refraction switching on/off will be realized by conditional compiling of the
        // shader rather than dyanmic branching as is now.
	"           if (Reflection == 1)\n"
	"           {\n"
    "               mediump vec4 reflectionColor = texture2D(ReflectionMap, coord.xy);\n"
	"               mixedColor.rgb = mix(waterColor.rgb, reflectionColor.rgb, waterColor.a);\n"
	"               mixedColor.a = 1.0;\n"
	"           }\n"
	"   \n"
    "           gl_FragColor = mixedColor;\n"
	"      }\n"
	"    </fs>\n"
	"  </shader>\n"
	"  <parameters>\n"
	"    <parameter name=\"mvp\" uniform=\"MVP\" source=\"transform\"/>\n"
	"    <parameter name=\"camera-world-position\" uniform=\"CameraWorldPosition\" source=\"transform\"/>\n"
	"    <parameter name=\"inversed-viewport\" uniform=\"invViewport\" source=\"camera\"/>\n"
	"    <parameter name=\"reflection-distortion\" uniform=\"ReflectionDistortion\" source=\"material\"/>\n"
	"    <parameter name=\"wave-scaling\" uniform=\"BumpScale\" source=\"material\"/>\n"
	"    <parameter name=\"wave-translation\" uniform=\"BumpTranslation\" source=\"material\"/>\n"
	"    <parameter name=\"reflection\" uniform=\"Reflection\" source=\"material\"/>\n"
	"    <parameter name=\"reflection-map\" uniform=\"ReflectionMap\" source=\"material\"/>\n"
	"    <parameter name=\"color-map\" uniform=\"ColorMap\" source=\"material\"/>\n"
	"    <parameter name=\"normal-map\" uniform=\"NormalMap\" source=\"material\"/>\n"
	"  </parameters>\n"
	"</material>\n";

PWaterWave::PWaterWave(const pchar *name, PNode *parent, pbool reflection, pbool refraction)
    : PDrawable(name, parent)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)

    P_OBJECT_REGISTER_PROPERTY("wavescaling", &m_waveScaling);
    P_OBJECT_REGISTER_PROPERTY("wavevelocity", &m_waveVelocity);
    P_OBJECT_REGISTER_PROPERTY("reflectiondistortion", &m_reflectionDistortion);

    initialize(reflection, refraction);
}

PWaterWave::PWaterWave(const pchar *name, PScene *scene, pbool reflection, pbool refraction)
    : PDrawable(name, scene)
{
    P_OBJECT_INITIALIZE_PROPERTY(PDrawable)
   
    P_OBJECT_REGISTER_PROPERTY("wavescaling", &m_waveScaling);
    P_OBJECT_REGISTER_PROPERTY("wavevelocity", &m_waveVelocity);
    P_OBJECT_REGISTER_PROPERTY("reflectiondistortion", &m_reflectionDistortion);


    initialize(reflection, refraction);
}

PWaterWave::~PWaterWave()
{
    P_OBJECT_UNINITIALIZE_PROPERTY();

    m_reflectionFB->release();

    PDELETE(m_plane);
}

void PWaterWave::initialize(pbool reflection, pbool refraction)
{
    m_reflection = reflection;
    m_refraction = refraction;

    m_reflectionCamera = PNEW(PCamera("reflection", this));
    m_reflectionCamera->setFixed(true);
    
    m_refractionCamera = PNEW(PCamera("refraction", this));
    m_refractionCamera->setFixed(true);

    // set geometry and material.
    PResourceManager *resourceManager = scene()->context()->module<PResourceManager>("resource-manager");
    setGeometry(PNEW(PGeometryPlane(resourceManager)));
    setMaterial(PNEW(PMaterial("internal/water-wave.pmt", WATERWAVE_PMT, false, resourceManager)));

    // framebuffer.
    m_reflectionPass = PNEW(PRenderPass("water-wave", scene()));

    const puint32 *rect = scene()->context()->rect();
    if (m_reflection)
    {
        m_reflectionFB = resourceManager->createFrameBuffer(P_NULL,
                                                            rect[2],
                                                            rect[3],
                                                            P_GLTEXTURE_FORMAT_RGBA8888,
                                                            P_GLTEXTURE_FORMAT_DEPTH_COMPONENT16);
        m_reflectionFB->retain();

        material()->parameter("reflection-map") = m_reflectionFB->colorBuffer();
        m_reflectionPass->target()->setFrameBuffer(m_reflectionFB);
    }
    material()->parameter("reflection") = (pint32)m_reflection;

    // TODO: enable reflection or/and refraction with shader conditional compiling.

    // Initialize uniforms.
    m_plane = PNEW(PPlane(0, 1, 0, 0));
    m_waveScaling           = pVector4(4.4f, 4.4f, 6.4f, 6.4f);
    m_waveVelocity          = pVector4(0.0748f, 0.0095f, -0.0748f, -0.0095f);  
    m_waveTranslation       = pVector4(0.5f, 0.5f, -0.5f, -0.5f); 
    m_reflectionDistortion  = 0.1f;
    m_colorTexture          = P_NULL;
    m_normalTexture         = P_NULL;

    material()->parameter("reflection-distortion") = m_reflectionDistortion.toFloat();
    material()->parameter("wave-scaling") = m_waveScaling.toVector4();
    material()->parameter("wave-translation") = m_waveTranslation;
}

void PWaterWave::update()
{
    pfloat32 dt = scene()->context()->clock().deltaTime() * 0.000075f;
    m_waveTranslation = pVector4(
        fmod(m_waveTranslation[0] + m_waveVelocity[0] * m_waveScaling[0] * dt, 1.0f),
        fmod(m_waveTranslation[1] + m_waveVelocity[1] * m_waveScaling[1] * dt, 1.0f),
        fmod(m_waveTranslation[2] + m_waveVelocity[2] * m_waveScaling[2] * dt, 1.0f),
        fmod(m_waveTranslation[3] + m_waveVelocity[3] * m_waveScaling[3] * dt, 1.0f));

    material()->parameter("wave-translation") = m_waveTranslation;
}

void PWaterWave::prepareRender(PRenderState *renderState)
{
    // Refraction.

    // -------------------------------------------------------------- 
    // Compute the reflected camera.
    // -------------------------------------------------------------- 
    PPlane plane = *m_plane;
    plane.transform(m_worldTransform);

    PCamera *mainCamera = scene()->mainCamera();
    m_reflectionCamera->copyOf(*mainCamera);
    //m_reflectionCamera->reflect(plane);
    // FIXME: oblique camera is buggy.
    //m_reflectionCamera->oblique(plane);

    // Prepare the reflection render pass.
    if (m_reflectionPass->camera() == P_NULL)
    {
        m_reflectionPass->setCamera(m_reflectionCamera);
        m_reflectionPass->queue()->addNodes(m_scene->root());
        m_reflectionPass->queue()->removeNodes(this);
    }

    // -------------------------------------------------------------- 
    // Setup the GL state
    // -------------------------------------------------------------- 
    renderState->renderStateObject()->setCull(P_GLCULL_FRONT);

    // -------------------------------------------------------------- 
    // Render
    // -------------------------------------------------------------- 
    m_reflectionPass->render(renderState);

    // -------------------------------------------------------------- 
    // Restore the GL state
    // -------------------------------------------------------------- 
    renderState->renderStateObject()->setCull(P_GLCULL_BACK);
}

void PWaterWave::setWaveScaling(const PVector4 &scaling)
{
    m_waveScaling = scaling;
    material()->parameter("wave-scaling") = m_waveScaling.toVector4();
}

void PWaterWave::setWaveVelocity(const PVector4 &velocity)
{
    m_waveVelocity = velocity;
}

void PWaterWave::setReflectionDistortion(pfloat32 distortion)
{
    m_reflectionDistortion = distortion;
    material()->parameter("reflection-distortion") = m_reflectionDistortion.toFloat();
}

void PWaterWave::setColorTexture(PTexture *texture)
{
    material()->parameter("color-map") = texture;
}

void PWaterWave::setNormalTexture(PTexture *texture)
{
    material()->parameter("normal-map") = texture;
}

void PWaterWave::setWaterPlane(const PVector4 &plane)
{
    m_plane->m_N[0] = plane[0];
    m_plane->m_N[1] = plane[1];
    m_plane->m_N[2] = plane[2];
    m_plane->m_D    = plane[3];
}

pbool PWaterWave::unpack(const PXmlElement* xmlElement)
{
    PASSERT(xmlElement->isValid());

    if (!PObject::unpack(xmlElement))
    {
        return false;
    }

    material()->parameter("wave-scaling") = m_waveScaling.toVector4();
    material()->parameter("relfection-distortion") = m_reflectionDistortion.toFloat();

    PResourceManager *resourceManager = scene()->context()->module<PResourceManager>("resource-manager");

    PXmlElement materialElement = xmlElement->child("material");

    PASSERT(materialElement.isValid());

    if (!materialElement.isValid())
    {
        PLOG_ERROR("Failed to find the material element to the PWaterWave in this xml node.");
        return false;
    }

    const pchar *colorTextureName = materialElement.attribute("colortexture");
    if (colorTextureName != P_NULL)
    {
        PTexture *colorTexture = resourceManager->getTexture(colorTextureName);
        PASSERT(colorTexture != P_NULL);
        setColorTexture(colorTexture);
    }
    else
    {
        PLOG_ERROR("Failed to find the color texture for PWaterWave in this xml node.");
        return false;
    }
    
    const pchar *normalTextureName = materialElement.attribute("normaltexture");
    if (normalTextureName != P_NULL)
    {
        PTexture *normalTexture = resourceManager->getTexture(normalTextureName);
        PASSERT(normalTexture != P_NULL);
        setNormalTexture(normalTexture);
    }
    else
    {
        PLOG_ERROR("Failed to find the normal texture for PWaterWave in this xml node.");
        return false;
    }
    
    PXmlElement geometryElement = xmlElement->child("geometry");
    if (geometryElement.isValid())
    {
        PAbstractGeometry *geometry = PAbstractGeometry::unpack(&geometryElement, resourceManager);
        if (geometry == P_NULL)
        {
            return false;
        }
        setGeometry(geometry);
    }

    return true;    
}

