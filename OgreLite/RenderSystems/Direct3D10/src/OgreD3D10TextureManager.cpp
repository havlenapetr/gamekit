/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgreD3D10TextureManager.h"
#include "OgreD3D10Texture.h"
#include "OgreRoot.h"
#include "OgreD3D10RenderSystem.h"
#include "OgreD3D10Device.h"

namespace Ogre 
{
	//---------------------------------------------------------------------
	D3D10TextureManager::D3D10TextureManager( D3D10Device & device ) : TextureManager(), mDevice (device)
	{
		if( mDevice.isNull())
			OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS, "Invalid Direct3DDevice passed", "D3D10TextureManager::D3D10TextureManager" );
		// register with group manager
		ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
	}
	//---------------------------------------------------------------------
	D3D10TextureManager::~D3D10TextureManager()
	{
		// unregister with group manager
		ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);

	}
	//---------------------------------------------------------------------
	Resource* D3D10TextureManager::createImpl(const String& name, 
		ResourceHandle handle, const String& group, bool isManual, 
		ManualResourceLoader* loader, const NameValuePairList* createParams)
	{
		return new D3D10Texture(this, name, handle, group, isManual, loader, mDevice); 
	}
	//---------------------------------------------------------------------
	void D3D10TextureManager::releaseDefaultPoolResources(void)
	{
		size_t count = 0;

		ResourceMap::iterator r, rend;
		rend = mResources.end();
		for (r = mResources.begin(); r != rend; ++r)
		{
			D3D10TexturePtr t = r->second;
			if (t->releaseIfDefaultPool())
				count++;
		}
		LogManager::getSingleton().logMessage("D3D10TextureManager released:");
		LogManager::getSingleton().logMessage(
			StringConverter::toString(count) + " unmanaged textures");
	}
	//---------------------------------------------------------------------
	void D3D10TextureManager::recreateDefaultPoolResources(void)
	{
		size_t count = 0;

		ResourceMap::iterator r, rend;
		rend = mResources.end();
		for (r = mResources.begin(); r != rend; ++r)
		{
			D3D10TexturePtr t = r->second;
			if(t->recreateIfDefaultPool(mDevice))
				count++;
		}
		LogManager::getSingleton().logMessage("D3D10TextureManager recreated:");
		LogManager::getSingleton().logMessage(
			StringConverter::toString(count) + " unmanaged textures");
	}
	//---------------------------------------------------------------------
	PixelFormat D3D10TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D10Mappings::_getPF(D3D10Mappings::_getClosestSupportedPF(format));

		return D3D10Mappings::_getPF(d3dPF);
	}
	//---------------------------------------------------------------------
	bool D3D10TextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
		bool preciseFormatOnly)
	{
		if (!preciseFormatOnly)
			format = getNativeFormat(ttype, format, usage);

		D3D10RenderSystem* rs = static_cast<D3D10RenderSystem*>(
			Root::getSingleton().getRenderSystem());

		return rs->_checkTextureFilteringSupported(ttype, format, usage);
	}
	//---------------------------------------------------------------------
}
