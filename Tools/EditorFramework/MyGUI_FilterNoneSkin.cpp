/*!
	@file
	@author		Albert Semenov
	@date		08/2010
*/

#include "Precompiled.h"
#include "MyGUI_Precompiled.h"
#include "MyGUI_FilterNoneSkin.h"
#include "MyGUI_RenderItem.h"
#include "MyGUI_LayerManager.h"
#include "MyGUI_CommonStateInfo.h"

#if defined(MYGUI_OGRE_PLATFORM)
	#include <MyGUI_OgreRenderManager.h>
	#include <MyGUI_OgreTexture.h>
	#include <MyGUI_OgreVertexBuffer.h>
#elif defined(MYGUI_OPENGL_PLATFORM)
	#include <MyGUI_OpenGLRenderManager.h>
#elif defined(MYGUI_DIRECTX_PLATFORM)
	#include <MyGUI_DirectXRenderManager.h>
#elif defined(MYGUI_DIRECTX11_PLATFORM)
	#include <MyGUI_DirectX11RenderManager.h>
#elif defined(MYGUI_OPENGL3_PLATFORM)
	#include <MyGUI_OpenGL3RenderManager.h>
#endif

namespace MyGUI
{

#ifdef MYGUI_OGRE_PLATFORM
	FilterNone::FilterNone()
	{
		mSeparate = true;
	}
#else
	FilterNone::FilterNone() = default;
#endif

	void FilterNone::_setAlign(const IntSize& _oldsize)
	{
		mCurrentCoord.set(0, 0, mCroppedParent->getWidth(), mCroppedParent->getHeight());
		mAlign = Align::Stretch;

		Base::_setAlign(_oldsize);
	}

	void FilterNone::doManualRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count)
	{
#if defined(MYGUI_OGRE_PLATFORM)

	#if OGRE_COMPILER == OGRE_COMPILER_MSVC
		#define OGRE_IGNORE_DEPRECATED_BEGIN __pragma(warning(push)) __pragma(warning(disable : 4996))
		#define OGRE_IGNORE_DEPRECATED_END __pragma(warning(pop))
	#else
		#define OGRE_IGNORE_DEPRECATED_BEGIN \
			_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
		#define OGRE_IGNORE_DEPRECATED_END _Pragma("GCC diagnostic pop")
	#endif

		if (Ogre2RenderManager::getInstancePtr()->getManualRender())
			Ogre2RenderManager::getInstancePtr()->begin();

		Ogre2RenderManager::getInstancePtr()->setManualRender(true);

		if (_texture)
		{
			Ogre2Texture* texture = static_cast<Ogre2Texture*>(_texture);
			Ogre::TexturePtr texture_ptr = texture->getOgreTexture();
			if (texture_ptr)
			{
				Ogre2RenderManager::getInstancePtr()->getRenderSystem()->_setTexture(0, true, texture_ptr);
				OGRE_IGNORE_DEPRECATED_BEGIN
	#if OGRE_VERSION < MYGUI_DEFINE_VERSION(14, 0, 0)
				// TODO: use _setSampler
				Ogre2RenderManager::getInstancePtr()->getRenderSystem()->_setTextureUnitFiltering(
					0,
					Ogre::FO_NONE,
					Ogre::FO_NONE,
					Ogre::FO_NONE);
	#endif
				OGRE_IGNORE_DEPRECATED_END
			}
		}

		Ogre2VertexBuffer* buffer = static_cast<Ogre2VertexBuffer*>(_buffer);
		Ogre::RenderOperation* operation = buffer->getRenderOperation();
		operation->vertexData->vertexCount = _count;

		Ogre2RenderManager::getInstancePtr()->getRenderSystem()->_render(*operation);
#elif defined(MYGUI_OPENGL_PLATFORM)
		OpenGLRenderManager::getInstancePtr()->doRender(_buffer, _texture, _count);
#elif defined(MYGUI_DIRECTX_PLATFORM)
		DirectXRenderManager::getInstancePtr()->doRender(_buffer, _texture, _count);
#elif defined(MYGUI_DIRECTX11_PLATFORM)
		DirectX11RenderManager::getInstancePtr()->doRender(_buffer, _texture, _count);
#elif defined(MYGUI_OPENGL3_PLATFORM)
		OpenGL3RenderManager::getInstancePtr()->doRender(_buffer, _texture, _count);
#endif
	}

}
