#include "HolaApp.h"
#include <iostream>
using namespace Ogre;

// ocultar el panel y poner luz ambiente
void HolaApp::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt){
	scnMgr->setAmbientLight(ColourValue(1, 1, 1));
	scnMgr->getEntity("eFondo")->setVisible(false);
};
// restablecer los cambios
void HolaApp::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt){
	scnMgr->setAmbientLight(ColourValue(0, 0, 0));
	scnMgr->getEntity("eFondo")->setVisible(true);
};

void HolaApp::frameRendered(const FrameEvent &  evt)
{
  //trayMgr->frameRendered(evt);
	for (int i = 0; i < vecObjMan.size(); ++i){
		vecObjMan[i]->frameRendered(evt);
	}
}

bool HolaApp::keyPressed(const OgreBites::KeyboardEvent& evt)
{
  if (evt.keysym.sym == SDLK_ESCAPE)
  {
    mRoot->queueEndRendering();
  }
 
  return true;
}

bool HolaApp::mousePressed(const OgreBites::MouseButtonEvent & evt)
{
	
	rayScnQuery->setRay(cam->getCameraToViewportRay(
		evt.x / (Real)mWindow->getViewport(0)->getActualWidth(),
		evt.y / (Real)cam->getViewport()->getActualHeight()));
	// coordenadas normalizadas en [0,1]
	RaySceneQueryResult& qryResult = rayScnQuery->execute();
	RaySceneQueryResult::iterator it = qryResult.begin();
	if (it != qryResult.end()) {
		//if (it->movable->getName() == "SinbadMan"){
		it->movable->getParentSceneNode()->translate(10, 10, 10);
		++it;
	UserControl* pCtrl = any_cast<UserControl*>(it->movable->getUserObjectBindings().getUserAny());
	pCtrl->getControl()->mousePicking(evt);
	}
  //if (trayMgr->mousePressed(evt)) return true;

  return true;
}

bool HolaApp::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
  //trayMgr->mouseMoved(evt);
  
  return true;
}

void HolaApp::setupInput()
{
  // do not forget to call the base first
  MyApplicationContext::setupInput(); 
  // register for input events
  addInputListener(this);
}

void HolaApp::shutdown()
{
 
  scnMgr->removeRenderQueueListener(mOverlaySystem);
  delete trayMgr;  trayMgr = nullptr;
  delete sinBadMgr; sinBadMgr = nullptr;
  for (int i = 0; i< vecObjMan.size(); ++i) {
	  delete vecObjMan[i];
  }
  // do not forget to call the base 
  MyApplicationContext::shutdown();
}

void HolaApp::setup(void)
{
  // do not forget to call the base first
  MyApplicationContext::setup();

  scnMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

  // register our scene with the RTSS
  mShaderGenerator->addSceneManager(scnMgr);

  scnMgr->addRenderQueueListener(mOverlaySystem);

  trayMgr = new OgreBites::TrayManager("TrayGUISystem", mWindow);
  trayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
  //A�adimos el objeto a la lista de observdores
  addInputListener(trayMgr);

  setupScene();
}

void HolaApp::setupScene(void)
{
  // without light we would just get a black screen    
  Light* light = scnMgr->createLight("Light");
  light->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z); // !!! opngl <-> direcci�n a la fuente de luz
  lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  lightNode->setPosition(0, 0, 100);
  lightNode->attachObject(light);

  // also need to tell where we are
  camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(0, 0, 100);
  camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_WORLD);

  // create the camera
  cam = scnMgr->createCamera("Cam");
  cam->setNearClipDistance(1); 
  cam->setFarClipDistance(10000);
  cam->setAutoAspectRatio(true);
  //cam->setPolygonMode(Ogre::PM_WIREFRAME);  // en material
  camNode->attachObject(cam);
  
  camMan = new OgreBites::CameraMan(camNode);
  camMan->setStyle(OgreBites::CS_ORBIT);
  addInputListener(camMan);


  // and tell it to render into the main window
  Viewport* vp = getRenderWindow()->addViewport(cam);
  //vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1));

  Ogre::SceneNode* nodeSinbad = scnMgr->getRootSceneNode()->createChildSceneNode("nSinbadMan");
  sinBadMgr = new SinbadMan(nodeSinbad);
  vecObjMan.push_back(sinBadMgr);
  addInputListener(sinBadMgr);


  Ogre::SceneNode* nodePlane = scnMgr->getRootSceneNode()->createChildSceneNode("nPlane");
  planeMgr = new ReflejoMan(nodeSinbad);
  vecObjMan.push_back(planeMgr);

 //RAYO
	rayScnQuery = scnMgr->createRayQuery(Ray());
	rayScnQuery->setSortByDistance(true);
}