/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): xat
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
#include "gkDebugFps.h"
#include "gkLogger.h"
#include "gkWindowSystem.h"
#include "gkEngine.h"
#include "gkStats.h"

#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreOverlayContainer.h"
#include "OgreFont.h"
#include "OgreFontManager.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreRenderWindow.h"
#include "OgreRenderTarget.h"

#define PROP_SIZE 14

// ----------------------------------------------------------------------------
gkDebugFps::gkDebugFps()
    :   m_isInit(false), m_isShown(false),
        m_over(0), m_cont(0), m_key(0), m_val(0)
{
    m_keys = "", 
    m_keys += "FPS:\n";
    m_keys += "Swap:\n";
    m_keys += "Physics:\n";
    m_keys += "LogicBricks:\n";
    m_keys += "LogicNodes:\n";
    m_keys += "Sound:\n";
    m_keys += "DBVT:\n";
    m_keys += "Rasterizer:\n";
    m_keys += "Scenegraph:\n";
    m_keys += "Network:\n";
    m_keys += "Services:\n";
    m_keys += "Overhead:\n";
    m_keys += "Outside:\n";
}

// ----------------------------------------------------------------------------
gkDebugFps::~gkDebugFps()
{
}

// ----------------------------------------------------------------------------
void gkDebugFps::initialize(void)
{
    if (m_isInit)
        return;

    try {
        // always initialize after gkDebugScreen!


        Ogre::OverlayManager &mgr = Ogre::OverlayManager::getSingleton();
        m_over  = mgr.create("<gkBuiltin/gkDebugFps>");
        m_key   = mgr.createOverlayElement("TextArea", "<gkBuiltin/gkDebugFps/Keys>");
        m_val   = mgr.createOverlayElement("TextArea", "<gkBuiltin/gkDebugFps/Vals>");
        m_cont  = (Ogre::OverlayContainer*)mgr.createOverlayElement("Panel", "<gkBuiltin/gkDebugFps/Containter1>");
       

        m_cont->setMetricsMode(Ogre::GMM_PIXELS);
        m_cont->setVerticalAlignment(Ogre::GVA_TOP);
        m_cont->setHorizontalAlignment(Ogre::GHA_RIGHT);
        m_cont->setLeft(-14*PROP_SIZE);

        m_key->setMetricsMode(Ogre::GMM_PIXELS);
        m_key->setVerticalAlignment(Ogre::GVA_TOP);
        m_key->setHorizontalAlignment(Ogre::GHA_LEFT);

        m_val->setMetricsMode(Ogre::GMM_PIXELS);
        m_val->setVerticalAlignment(Ogre::GVA_TOP);
        m_val->setHorizontalAlignment(Ogre::GHA_LEFT);
        m_val->setLeft(6*PROP_SIZE);

        Ogre::TextAreaOverlayElement *textArea;
        
        textArea = static_cast<Ogre::TextAreaOverlayElement*>(m_key);
        textArea->setFontName("<gkBuiltin/Font>");
        textArea->setCharHeight(PROP_SIZE);
        textArea->setColour(Ogre::ColourValue::White);

        textArea = static_cast<Ogre::TextAreaOverlayElement*>(m_val);
        textArea->setFontName("<gkBuiltin/Font>");
        textArea->setCharHeight(PROP_SIZE);
        textArea->setColour(Ogre::ColourValue::White);



        m_over->setZOrder(500);
        m_cont->addChild(m_key);
        m_cont->addChild(m_val);
        m_over->add2D(m_cont);
    }
    catch (Ogre::Exception & e)
    {
        gkPrintf("%s", e.getDescription().c_str());
        return;
    }

    m_isInit = true;
    
}

// ----------------------------------------------------------------------------
void gkDebugFps::show(bool v)
{
    if (m_over != 0 && m_isShown != v)
    {
        m_isShown = v;
        if (m_isShown)
            m_over->show();
        else
            m_over->hide();
    }
}

// ----------------------------------------------------------------------------
void gkDebugFps::draw(void)
{
    if (!m_over || !m_key || !m_val)
        return;

    Ogre::RenderWindow *window= gkWindowSystem::getSingleton().getMainWindow();
    const Ogre::RenderTarget::FrameStats& ogrestats = window->getStatistics();

    float fps = ogrestats.lastFPS;
    float swap = 1000/fps;
    float phys = gkStats::getSingleton().getPhysicsMicroSeconds()/1000.0f;
    float logicb = gkStats::getSingleton().getLogicBricksMicroSeconds()/1000.0f;
    float logicn = gkStats::getSingleton().getLogicNodesMicroSeconds()/1000.0f;
    float sound = gkStats::getSingleton().getSoundMicroSeconds()/1000.0f;
    float dbvt = gkStats::getSingleton().getDbvtMicroSeconds()/1000.0f;

    gkString vals="";

    vals += Ogre::StringConverter::toString(fps) + '\n';
    vals += Ogre::StringConverter::toString(swap, 6, 6, ' ', std::ios::fixed) + "ms 100%\n";
    
    vals += Ogre::StringConverter::toString(phys, 6, 6, ' ', std::ios::fixed) + "ms ";
    vals += Ogre::StringConverter::toString( int(100*phys/swap), 3 ) + "%\n";
    
    vals += Ogre::StringConverter::toString(logicb, 6, 6, ' ', std::ios::fixed) + "ms ";
    vals += Ogre::StringConverter::toString( int(100*logicb/swap), 3 ) + "%\n";
    
    vals += Ogre::StringConverter::toString(logicn, 6, 6, ' ', std::ios::fixed) + "ms ";
    vals += Ogre::StringConverter::toString( int(100*logicn/swap), 3 ) + "%\n";
    
    vals += Ogre::StringConverter::toString(sound, 6, 6, ' ', std::ios::fixed) + "ms ";
    vals += Ogre::StringConverter::toString( int(100*sound/swap), 3 ) + "%\n";
    
    vals += Ogre::StringConverter::toString(dbvt, 6, 6, ' ', std::ios::fixed) + "ms ";
    vals += Ogre::StringConverter::toString( int(100*dbvt/swap), 3 ) + "%\n";
    
    vals += "TODO\n";
    vals += "TODO\n";
    vals += "TODO\n";
    vals += "TODO\n";
    vals += "TODO\n";
    vals += "TODO\n";

    if (!m_keys.empty() && !vals.empty())
    {
        m_key->setCaption(m_keys);
        m_val->setCaption(vals);
    }
}
