/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkCameraObject_h_
#define _gkCameraObject_h_

#include "gkGameObject.h"
#include "gkSerialize.h"
#include "OgreCamera.h"


class gkCameraCallback;

// Game camera
class gkCamera : public gkGameObject
{
public:
    gkCamera(gkScene *scene, const gkString &name, gkObject::Loader *loader = 0);
    virtual ~gkCamera() {}

    // Property access
    GK_INLINE void setProperties(const gkCameraProperties &props)   {m_cameraProps = props;}
    GK_INLINE gkCameraProperties &getCameraProperties(void)         {return m_cameraProps;}


    void setClip(gkScalar start, gkScalar end);
    void setFov(const gkRadian &fov);
    void setFov(const gkDegree &fov);
    void setMainCamera(bool v);
    void makeCurrent(void);

    GK_INLINE Ogre::Camera *getCamera(void)         {return m_camera;}
    GK_INLINE const gkScalar &getClipStart(void)    {return m_cameraProps.clipstart;}
    GK_INLINE const gkScalar &getClipEnd(void)      {return m_cameraProps.clipend;}
    GK_INLINE const gkScalar &getFov(void)          {return m_cameraProps.fov;}
    GK_INLINE bool isMainCamera(void)               {return m_cameraProps.start;}

private:
    gkObject    *clone(const gkString &name);

    gkCameraProperties  m_cameraProps;
    Ogre::Camera        *m_camera;
    gkCameraCallback    *m_callback;

    virtual void loadImpl(void);
    virtual void unloadImpl(void);
};

#endif//_gkCameraObject_h_
