/*!
	@file
	@author		Albert Semenov
	@date		10/2009
*/

#ifndef MYGUI_OGRE_RTTEXTURE_H_
#define MYGUI_OGRE_RTTEXTURE_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_IRenderTarget.h"

#include <Ogre.h>

#include "MyGUI_LastHeader.h"

namespace MyGUI
{

	class Ogre2RTTexture :
		public IRenderTarget
	{
	public:
#if OGRE_VERSION_MINOR < 2 && OGRE_VERSION_MAJOR < 3
		Ogre2RTTexture(Ogre::TexturePtr _texture);
#else
		Ogre2RTTexture(Ogre::TextureGpu* _texture);
#endif
		virtual ~Ogre2RTTexture();

		virtual void begin();
		virtual void end();

		virtual void doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count);

		virtual const RenderTargetInfo& getInfo()
		{
			return mRenderTargetInfo;
		}

	private:
		RenderTargetInfo mRenderTargetInfo;
#if OGRE_VERSION_MINOR < 2 && OGRE_VERSION_MAJOR < 3
		Ogre::Viewport* mViewport;
		Ogre::Viewport* mSaveViewport;
		Ogre::TexturePtr mTexture;
#else
		Ogre::RenderPassDescriptor* mRenderPassDesc;
		Ogre::RenderPassDescriptor* mSaveRenderPassDesc;
		Ogre::TextureGpu* mTexture;
#endif
		Ogre::Matrix4 mProjectMatrix;
	};

} // namespace MyGUI

#endif // MYGUI_OGRE_RTTEXTURE_H_
