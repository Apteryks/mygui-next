/*!
	@file
	@author		Albert Semenov
	@date		10/2009
*/

#include <cstring>
#include "MyGUI_Ogre2RTTexture.h"
#include "MyGUI_Ogre2RenderManager.h"

#if OGRE_VERSION_MINOR > 1
#include "OgreTextureGpu.h"
#endif

namespace MyGUI
{

#if OGRE_VERSION_MINOR < 2
	Ogre2RTTexture::Ogre2RTTexture(Ogre::TexturePtr _texture) :
		mViewport(nullptr),
		mSaveViewport(nullptr),
#else
	Ogre2RTTexture::Ogre2RTTexture(Ogre::TextureGpu* _texture) :
		mRenderPassDesc(nullptr),
		mSaveRenderPassDesc(nullptr),
#endif
		mTexture(_texture)
	{
		mProjectMatrix = Ogre::Matrix4::IDENTITY;
		Ogre::Root* root = Ogre::Root::getSingletonPtr();
		if (root != nullptr)
		{
			Ogre::RenderSystem* system = root->getRenderSystem();
			if (system != nullptr)
			{
				size_t width = mTexture->getWidth();
				size_t height = mTexture->getHeight();

				mRenderTargetInfo.maximumDepth = system->getMaximumDepthInputValue();
				mRenderTargetInfo.hOffset = system->getHorizontalTexelOffset() / float(width);
				mRenderTargetInfo.vOffset = system->getVerticalTexelOffset() / float(height);
				mRenderTargetInfo.aspectCoef = float(height) / float(width);
				mRenderTargetInfo.pixScaleX = 1.0f / float(width);
				mRenderTargetInfo.pixScaleY = 1.0f / float(height);
			}

#if OGRE_VERSION_MINOR < 2
			if (mTexture->getBuffer()->getRenderTarget()->requiresTextureFlipping())
#else
			if (mTexture->requiresTextureFlipping())
#endif
			{
				mProjectMatrix[1][0] = -mProjectMatrix[1][0];
				mProjectMatrix[1][1] = -mProjectMatrix[1][1];
				mProjectMatrix[1][2] = -mProjectMatrix[1][2];
				mProjectMatrix[1][3] = -mProjectMatrix[1][3];
			}
		}
	}

	Ogre2RTTexture::~Ogre2RTTexture()
	{
	}

	void Ogre2RTTexture::begin()
	{
#if OGRE_VERSION_MINOR < 2
		Ogre::RenderTexture* rtt = mTexture->getBuffer()->getRenderTarget();

		if (mViewport == nullptr)
		{
			mViewport = rtt->addViewport();
			mViewport->setOverlaysEnabled(false);
		}

		Ogre::RenderSystem* system = Ogre::Root::getSingleton().getRenderSystem();
		system->_setProjectionMatrix(mProjectMatrix);
		mSaveViewport = system->_getViewport();
		system->_setViewport(mViewport);
		system->clearFrameBuffer(Ogre::FBT_COLOUR, Ogre::ColourValue::ZERO);
#else
		Ogre::RenderSystem* system = Ogre::Root::getSingleton().getRenderSystem();
		if (mRenderPassDesc == nullptr)
		{
			mRenderPassDesc = system->createRenderPassDescriptor();
			mRenderPassDesc->mColour[0].texture = mTexture;
			mRenderPassDesc->mColour[0].loadAction = Ogre::LoadAction::Clear;
			mRenderPassDesc->mColour[0].storeAction = Ogre::StoreAction::StoreOrResolve;
			mRenderPassDesc->mColour[0].clearColour = Ogre::ColourValue::ZERO;
			mRenderPassDesc->mColour[0].resolveTexture = mTexture;
			mRenderPassDesc->mDepth.texture = system->getDepthBufferFor(mTexture, mTexture->getDepthBufferPoolId(), mTexture->getPreferDepthTexture(), mTexture->getDesiredDepthBufferFormat());
			mRenderPassDesc->mDepth.loadAction = Ogre::LoadAction::Clear;
			mRenderPassDesc->mDepth.storeAction = Ogre::StoreAction::DontCare;
			mRenderPassDesc->mStencil.texture = nullptr;
			mRenderPassDesc->mStencil.loadAction = Ogre::LoadAction::Clear;
			mRenderPassDesc->mStencil.storeAction = Ogre::StoreAction::DontCare;
			mRenderPassDesc->entriesModified(Ogre::RenderPassDescriptor::All);
		}
		// no need to save RenderPassDesc
		//mSaveRenderPassDesc = system->getCurrentPassDescriptor();
		Ogre::Vector4 viewportSize(0, 0, 1, 1);
		Ogre::Vector4 scissors(0, 0, 1, 1);
		system->beginRenderPassDescriptor(mRenderPassDesc, mTexture, 0, &viewportSize, &scissors, 1, false, false);
#endif
	}

	void Ogre2RTTexture::end()
	{
		Ogre::RenderSystem* system = Ogre::Root::getSingleton().getRenderSystem();
#if OGRE_VERSION_MINOR < 2
		system->_setViewport(mSaveViewport);
#else
		// no need to restore RenderPassDesc
#endif
#if OGRE_VERSION >= MYGUI_DEFINE_VERSION(1, 7, 0) && OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
		Ogre::OrientationMode orient = mSaveViewport->getOrientationMode();
		system->_setProjectionMatrix(Ogre::Matrix4::IDENTITY * Ogre::Quaternion(Ogre::Degree(orient * 90.f), Ogre::Vector3::UNIT_Z));
#else
		system->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
#endif
	}

	void Ogre2RTTexture::doRender(IVertexBuffer* _buffer, ITexture* _texture, size_t _count)
	{
		Ogre2RenderManager::getInstance().doRender(_buffer, _texture, _count);
	}

} // namespace MyGUI
