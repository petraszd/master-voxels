#include <config.h>
#include <cubeapp.h>
#include <cube.h>
#include <textureutil.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreOverlayManager.h>
#include <OgreRenderTexture.h>
#include <OgreTexture.h>
#include <OgreSubEntity.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreMaterialManager.h>
#include <OgreCompositorManager.h>
#include <OgreStringConverter.h>
#include <OgreRay.h>
#include <OgreString.h>
#include <QColor>
#include <QDebug>

#define RTT_TEXTURE_NAME "RttTexture"
#define RTT_TEXTURE_INDEX 0
#define VOL_TEXTURE_INDEX 1
#define DYN_TEXTURE_NAME "DymTransferTexture"
#define DYN_TEXTURE_INDEX 2

using namespace Ogre;

CubeApp::CubeApp() :
    OgreApp(), m_rttListener(0), m_renderTex(0),
    m_sMgr(0), m_camNode(0)
{
    m_shaderFactor = 1.0;
    m_shaderAlpha = 1.0;
    m_shaderUseLight = true;
    m_dir = Vector3::ZERO;
    m_rotateAround = false;
}

CubeApp::~CubeApp()
{
    m_renderTex->removeAllListeners();
    delete m_rttListener;
}

void CubeApp::customInit()
{
    createSceneManager();
    createCamera();
    createScene();
    showOverlays();
    createColorsTexure();
    createRtt();
    createTransferTexture();
}

void CubeApp::createSceneManager()
{
    m_sMgr = m_root->createSceneManager(
            ST_GENERIC, "Main Scene Manager");
}

void CubeApp::createCamera()
{
    Camera *cam = m_sMgr->createCamera("Main Camera");
    cam->setNearClipDistance(0.001);
    cam->setFarClipDistance(100.0);
    Viewport *vp = m_window->addViewport(cam);
    vp->setBackgroundColour(ColourValue(0.4, 0.4, 0.4));
    Real ratio = vp->getActualWidth() /
        Real(vp->getActualHeight());
    cam->setAspectRatio(ratio);

    cam->setPosition(Vector3::ZERO);
    cam->lookAt(Vector3::UNIT_Z);

    m_camNode = m_sMgr->getRootSceneNode()
        ->createChildSceneNode("Cam Node");
    m_camNode->setPosition(0.0, 0.0, -3.0);
    m_camNode->attachObject(cam);
}

void CubeApp::createScene()
{
    SceneNode *rootNode = m_sMgr->getRootSceneNode();
    m_cubesNode = rootNode->createChildSceneNode("Cubes Node");

    SceneNode *node = m_cubesNode->createChildSceneNode("DirCube Node");
    m_visibleCubes = m_cubesNode->createChildSceneNode("Visible Cubes Node");

    m_dirCube = m_sMgr->createEntity("DirCube", "cube.mesh");
    m_dirCube->setMaterialName("Cube/Colors");

    node->attachObject(m_dirCube);

    for(unsigned int i = 0; i < m_cubesNode->numChildren(); ++i) {
        // Makes Cubes center as World center
        m_cubesNode->getChild(i)->translate(-0.5, -0.5, -0.5);
    }

    //Light *light = m_sMgr->createLight("Main Light");
    //light->setType(Light::LT_POINT);
    //light->setDiffuseColour(1.0, 1.0, 1.0);
    //light->setPosition(4.0, 0.0, -3.0);

    //qDebug("%f, %f, %f", light->getPosition().x, light->getPosition().y, light->getPosition().z);
    //m_camNode->attachObject(light);
    //qDebug("%f, %f, %f", light->getPosition().x, light->getPosition().y, light->getPosition().z);
}

Pass* CubeApp::createVisibleCube(
    const String &name,
    SceneNode *parentNode)
{
    String matName = name + " Material";

    m_sMgr->destroyEntity(name);
    Entity *cube = m_sMgr->createEntity(name, "cube.mesh");

    // Creates a material
    MaterialManager *man = MaterialManager::getSingletonPtr();

    man->remove(matName);
    MaterialPtr matPtr = man->create(matName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Technique *tech = matPtr->getTechnique(0);
    Pass *pass = tech->getPass(0);

    pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
    pass->setDepthWriteEnabled(false);
    pass->setVertexProgram("voxelVP");
    pass->setFragmentProgram("voxelFP");

    //pass->setAlphaToCoverageEnabled(true); // ??

    TextureUnitState *state;

    pass->createTextureUnitState();

    state = pass->createTextureUnitState();
    state->setTextureFiltering(TFO_ANISOTROPIC);
    state->setTextureBorderColour(ColourValue::ZERO);
    state->setTextureAddressingMode(TextureUnitState::TAM_BORDER);

    state = pass->createTextureUnitState();
    state->setTextureFiltering(TFO_TRILINEAR);
    state->setTextureAddressingMode(TextureUnitState::TAM_BORDER);
    state->setTextureBorderColour(ColourValue::ZERO);

    cube->setMaterialName(matName);

    parentNode->attachObject(cube);

    return pass;
}

void CubeApp::showOverlays()
{
    OverlayManager *mgr = OverlayManager::getSingletonPtr();
    mgr->getByName("CursorOverlays/Main")->show();
}

void CubeApp::createColorsTexure()
{
    TextureManager *mgr = TextureManager::getSingletonPtr();
    mgr->remove(RTT_TEXTURE_NAME);
    TexturePtr tex = mgr->createManual(RTT_TEXTURE_NAME,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            TEX_TYPE_2D,
            m_window->getWidth(),
            m_window->getHeight(),
            0,
            PF_R8G8B8,
            TU_RENDERTARGET);

    m_renderTex = tex->getBuffer()->getRenderTarget();
    Camera *cam = m_sMgr->getCamera("Main Camera");
    Viewport *vp = m_renderTex->addViewport(cam);
    vp->setClearEveryFrame(true);
    vp->setBackgroundColour(ColourValue::White);
    vp->setOverlaysEnabled(false);
    m_renderTex->setAutoUpdated(true);

    MaterialPtr mat = MaterialManager::getSingletonPtr()
        ->getByName("RTTMaterials/MainPanel");
    mat->getTechnique(0)->getPass(0)
        ->getTextureUnitState(RTT_TEXTURE_INDEX)
        ->setTextureName(RTT_TEXTURE_NAME);
}

void CubeApp::assignRTTTexture()
{
    MaterialPtr mat;
    for(unsigned int i = 0; i < m_visibleCubes->numChildren(); ++i) {
        SceneNode *node = (SceneNode*)m_visibleCubes->getChild(i);
        for (unsigned int j = 0; j < node->numAttachedObjects(); ++j) {
            Entity *ent = (Entity*)node->getAttachedObject(j);
            mat = ent->getSubEntity(0)->getMaterial();
            mat->getTechnique(0)->getPass(0)
                ->getTextureUnitState(RTT_TEXTURE_INDEX)
                ->setTextureName(RTT_TEXTURE_NAME);
        }
    }
}

void CubeApp::setCubesShaderValue(const String &name, Real value)
{
    MaterialPtr mat;
    GpuProgramParametersSharedPtr params;
    Entity *ent;
    SceneNode *node;
    for(unsigned int i = 0; i < m_visibleCubes->numChildren(); ++i) {
        node = (SceneNode*)m_visibleCubes->getChild(i);
        for (unsigned int j = 0; j < node->numAttachedObjects(); ++j) {
            ent = (Entity*)node->getAttachedObject(j);
            mat = ent->getSubEntity(0)->getMaterial();

            params = mat->getTechnique(0)->getPass(0)
                ->getFragmentProgramParameters();
            params->setNamedConstant(name, value);
            mat->getTechnique(0)->getPass(0)
                ->setFragmentProgramParameters(params);
        }
    }
}

void CubeApp::createTransferTexture()
{
    TextureManager *mgr = TextureManager::getSingletonPtr();
    m_transferTex = mgr->createManual(DYN_TEXTURE_NAME,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            TEX_TYPE_2D,
            Config::getUInt("transfer/size"), 1, 0,
            PF_R8G8B8A8);

    HardwarePixelBufferSharedPtr buffer = m_transferTex->getBuffer();
    buffer->lock(HardwareBuffer::HBL_NORMAL);
    Ogre::uint8 *data = static_cast<Ogre::uint8*> (buffer->getCurrentLock().data);
    for (unsigned int i = 0; i < Config::getUInt("transfer/size"); ++i) {
        data[i * 4 + 0] = 0;
        data[i * 4 + 1] = 0;
        data[i * 4 + 2] = 0;
        data[i * 4 + 3] = 0;
    }
    buffer->unlock();
}

void CubeApp::createRtt()
{
    m_rttListener = new RttListener(m_dirCube, m_visibleCubes);
    m_renderTex->addListener(m_rttListener);
}

void CubeApp::clearVisibleCubes()
{
    for(unsigned int i = 0; i < m_visibleCubes->numChildren(); ++i) {
        SceneNode *node = (SceneNode*)m_visibleCubes->getChild(i);
        for (unsigned int j = 0; j < node->numAttachedObjects(); ++j) {
            Entity *ent = (Entity*)node->getAttachedObject(j);

            TextureManager::getSingleton().remove(
                ent->getSubEntity(0)->getMaterial()
                   ->getTechnique(0)->getPass(0)
                   ->getTextureUnitState(VOL_TEXTURE_INDEX)
                   ->getTextureName()
            );
            MaterialManager::getSingleton().remove(
                ent->getSubEntity(0)->getMaterialName()
            );
            m_sMgr->destroyEntity(ent);
        }
        // TODO: not working
        //m_visibleCubes->removeAndDestroyChild(i);
    }
}

void CubeApp::open3DTexture(const QString &name)
{
    clearVisibleCubes();
    MultiTexture textures = TextureUtil::readMultiTexture(name.toStdString().c_str());

    SceneNode *node;
    unsigned int n = textures.dimension();
    float deltaSize = 1.0 / n;
    unsigned int number = 0;
    for(unsigned int z = 0; z < n; ++z) {
        for(unsigned int y = 0; y < n; ++y) {
            for(unsigned int x = 0; x < n; ++x) {
                if (!textures.hasTexture(x, y, z))
                    continue;

                node = m_visibleCubes->createChildSceneNode();
                node->scale(1.0 / n, 1.0 / n, 1.0 / n);
                node->translate(Real(x) / n, Real(y) / n, Real(z) / n);

                String name = "Cube " + StringConverter::toString(number++);
                Pass *pass = createVisibleCube(name, node);
                pass->getTextureUnitState(RTT_TEXTURE_INDEX)
                    ->setTextureName(RTT_TEXTURE_NAME);
                pass->getTextureUnitState(VOL_TEXTURE_INDEX)
                    ->setTextureName(textures.getName(x, y, z));
                pass->getTextureUnitState(DYN_TEXTURE_INDEX)
                    ->setTextureName(DYN_TEXTURE_NAME);

                GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
                params->setNamedConstant("textureDeltas", Vector3(deltaSize * x, deltaSize * y , deltaSize * z));
                pass->setFragmentProgramParameters(params);

            }
        }
    }
    setCubesShaderValue("useLight", m_shaderUseLight);
    setCubesShaderValue("alpha", m_shaderAlpha);
    setCubesShaderValue("diffuseFactor", m_shaderFactor);
    setCubesShaderValue("textureDimension", n);
}

void CubeApp::shoot()
{
    Camera *cam = m_sMgr->getCamera("Main Camera");
    Ogre::Ray ray = cam->getCameraToViewportRay(0.5, 0.5);

    for(unsigned int i = 0; i < m_visibleCubes->numChildren(); ++i) {
        SceneNode *node = (SceneNode*)m_visibleCubes->getChild(i);
        for (unsigned int j = 0; j < node->numAttachedObjects(); ++j) {
            Entity *ent = (Entity*)node->getAttachedObject(j);
            Cube cube(ent);
            cube.gap(ray);
        }
    }
}

void CubeApp::useLight(bool use)
{
    m_shaderUseLight = use;
    setCubesShaderValue("useLight", use);
}

void CubeApp::setAlpha(Ogre::Real alpha)
{
    m_shaderAlpha = alpha;
    setCubesShaderValue("alpha", alpha);
}

void CubeApp::toggleRotateAround()
{
    m_rotateAround = !m_rotateAround;
}

void CubeApp::setDiffuseFactor(Ogre::Real factor)
{
    m_shaderFactor = factor;
    setCubesShaderValue("diffuseFactor", factor);
}

void CubeApp::setTransferTexData(Ogre::uchar *passedData)
{
    if (!started())
        return;

    HardwarePixelBufferSharedPtr buffer = m_transferTex->getBuffer();
    buffer->lock(HardwareBuffer::HBL_NORMAL);
    Ogre::uchar *data = static_cast<Ogre::uchar*> (buffer->getCurrentLock().data);
    for (unsigned int i = 0; i < Config::getUInt("transfer/size"); ++i) {
        QRgb* rgba = (QRgb*) (&passedData[i * 4]);
        data[i * 4 + 0] = qBlue(*rgba); // TODO: find out why R <-> B
        data[i * 4 + 1] = qGreen(*rgba);
        data[i * 4 + 2] = qRed(*rgba);
        data[i * 4 + 3] = qAlpha(*rgba);
    }
    buffer->unlock();
}

void CubeApp::animate(int milisecs)
{
    m_camNode->translate(m_dir * milisecs *
            Config::getFloat("cubeapp/move_factor"),
            Node::TS_LOCAL);

    if (m_rotateAround) {
        m_cubesNode->yaw(Ogre::Degree(milisecs *
                Config::getFloat("cubeapp/anim_rotate_factor")));
    }
}

void CubeApp::downOn()
{
    m_dir.z = -1.0;
}

void CubeApp::downOff()
{
    if (m_dir.z == -1.0)
        m_dir.z = 0.0;
}

void CubeApp::upOn()
{
    m_dir.z = 1.0;
}

void CubeApp::upOff()
{
    if (m_dir.z == 1.0)
        m_dir.z = 0.0;
}

void CubeApp::leftOn()
{
    m_dir.x = 1.0;
}

void CubeApp::leftOff()
{
    if (m_dir.x == 1.0)
        m_dir.x = 0.0;
}

void CubeApp::rightOn()
{
    m_dir.x = -1.0;
}

void CubeApp::rightOff()
{
    if (m_dir.x == -1.0)
        m_dir.x = 0.0;
}

void CubeApp::mouse(int x, int y)
{
    float factor = Config::getFloat("cubeapp/rotate_factor");
    m_camNode->yaw(Degree(x * factor), Node::TS_WORLD);
    m_camNode->pitch(Degree(y * -factor), Node::TS_LOCAL);
}

void CubeApp::resized(unsigned int w, unsigned int h)
{
    m_renderTex->removeAllListeners();
    createColorsTexure();
    assignRTTTexture();
    m_renderTex->addListener(m_rttListener);
}

