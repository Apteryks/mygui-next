/*!
	@file
	@author		Albert Semenov
	@date		04/2009
*/

#include <cstring>
#include "MyGUI_Ogre2Texture.h"
#include "MyGUI_DataManager.h"
#include "MyGUI_Ogre2RenderManager.h"
#include "MyGUI_Ogre2Diagnostic.h"
#include "MyGUI_Ogre2RTTexture.h"
#include <Ogre.h>

#include "MyGUI_LastHeader.h"

#if OGRE_VERSION_MINOR > 1
#include "OgreTextureGpu.h"
#include "OgreTextureGpuManager.h"
#include "OgrePixelFormatGpu.h"
#include "OgrePixelFormatGpuUtils.h"
#include "OgreStagingTexture.h"
#endif

namespace MyGUI
{

	Ogre2Texture::Ogre2Texture(const std::string& _name, const std::string& _group) :
		mName(_name),
		mGroup(_group),
		mNumElemBytes(0),
#if OGRE_VERSION_MINOR == 1
		mPixelFormat(Ogre::PF_UNKNOWN),
#else
		mPixelFormat(Ogre::PixelFormatGpu::PFG_UNKNOWN),
#endif
		mListener(nullptr),
		mRenderTarget(nullptr)
	{
		mTmpData.data = nullptr;

		mDataBlock = HLMS_BLOCKS.createUnlitDataBlock(_name);
	}

	Ogre2Texture::~Ogre2Texture()
	{
		destroy();
	}

	const std::string& Ogre2Texture::getName() const
	{
		return mName;
	}

	void Ogre2Texture::saveToFile(const std::string& _filename)
	{
		Ogre::uchar* readrefdata = static_cast<Ogre::uchar*>(lock(TextureUsage::Read));

#if OGRE_VERSION_MINOR == 1
		Ogre::Image img;
		img = img.loadDynamicImage(readrefdata, mTexture->getWidth(), mTexture->getHeight(), mTexture->getFormat());
		img.save(_filename);
#endif

		unlock();
	}

	void Ogre2Texture::setInvalidateListener(ITextureInvalidateListener* _listener)
	{
		mListener = _listener;
	}

	void Ogre2Texture::destroy()
	{
		if (mTmpData.data != nullptr)
		{
			delete [] (uint8*)mTmpData.data;
			mTmpData.data = nullptr;
		}

		if (mRenderTarget != nullptr)
		{
			delete mRenderTarget;
			mRenderTarget = nullptr;
		}

#if OGRE_VERSION_MINOR == 1
		if (!mTexture.isNull())
		{
			Ogre::TextureManager::getSingleton().remove(mTexture->getName());
			mTexture.setNull();
		}
#else
		if (mTexture)
		{
			Ogre::TextureGpuManager *textureMgr = Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager();
			textureMgr->destroyTexture(mTexture);
			mTexture = nullptr;
		}
#endif
	}

	int Ogre2Texture::getWidth()
	{
		return (int)mTexture->getWidth();
	}

	int Ogre2Texture::getHeight()
	{
		return (int)mTexture->getHeight();
	}

	void* Ogre2Texture::lock(TextureUsage _access)
	{
#if OGRE_VERSION_MINOR == 1
		if (_access == TextureUsage::Write)
		{
			return mTexture->getBuffer()->lock(Ogre::v1::HardwareBuffer::HBL_DISCARD);
		}

		// здесь проверить режим создания, и возможно так залочить без пиксель бокса

		// для чтения копируем в пиксель бокс
		if (mTmpData.data != nullptr)
		{
			delete [] (uint8*)mTmpData.data;
			mTmpData.data = nullptr;
		}

		mTmpData = Ogre::PixelBox(mTexture->getWidth(), mTexture->getHeight(), mTexture->getDepth(), mTexture->getFormat());
		mTmpData.data = new uint8[mTexture->getBuffer()->getSizeInBytes()];

		mTexture->getBuffer()->blitToMemory(mTmpData);

		return mTmpData.data;
#else
		if (_access == TextureUsage::Write) {
			const uint32 rowAlignment = 4u;
			const size_t dataSize = Ogre::PixelFormatGpuUtils::getSizeBytes(mTexture->getWidth(), mTexture->getHeight(), 1u, 1u,
				mTexture->getPixelFormat(),
				rowAlignment);
			uint8 *imageData = reinterpret_cast<uint8*>(OGRE_MALLOC_SIMD(dataSize, Ogre::MEMCATEGORY_RESOURCE));
			mTmpData.data = imageData;
			return mTmpData.data;
		}
		else {
			// FIXME
			return nullptr;
		}
#endif
	}

	void Ogre2Texture::unlock()
	{
#if OGRE_VERSION_MINOR == 1
		if (mTexture->getBuffer()->isLocked())
		{
			mTexture->getBuffer()->unlock();
		}
		else if (mTmpData.data != nullptr)
		{
			delete [] (uint8*)mTmpData.data;
			mTmpData.data = nullptr;
		}
#else
		Ogre::TextureGpuManager *textureManager = Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager();

		if (mTmpData.data != nullptr)
		{
			// write
			uint8 *imageData = reinterpret_cast<uint8*>(mTmpData.data);
			const size_t bytesPerRow = mTexture->_getSysRamCopyBytesPerRow(0);

			mTexture->_transitionTo(Ogre::GpuResidency::Resident, imageData);
			mTexture->_setNextResidencyStatus(Ogre::GpuResidency::Resident);

			Ogre::StagingTexture *stagingTexture = textureManager->getStagingTexture(mTexture->getWidth(), mTexture->getHeight(),
				1u, 1u,
				mTexture->getPixelFormat());
			stagingTexture->startMapRegion();
			Ogre::TextureBox texBox = stagingTexture->mapRegion(mTexture->getWidth(), mTexture->getHeight(), 1u, 1u,
				mTexture->getPixelFormat());
			texBox.copyFrom(imageData, mTexture->getWidth(), mTexture->getHeight(), bytesPerRow);
			stagingTexture->stopMapRegion();
			stagingTexture->upload(texBox, mTexture, 0, 0, 0, true);
			textureManager->removeStagingTexture(stagingTexture);
			stagingTexture = 0;

			OGRE_FREE_SIMD(imageData, Ogre::MEMCATEGORY_RESOURCE);
			mTmpData.data = nullptr;
			mTexture->notifyDataIsReady();
		}
#endif
	}

	bool Ogre2Texture::isLocked()
	{
#if OGRE_VERSION_MINOR == 1
		return mTexture->getBuffer()->isLocked();
#else
		// FIXME
		return false;
#endif
	}



#if OGRE_VERSION_MINOR > 1
	Ogre::PixelFormatGpu Ogre2Texture::convertFormat(PixelFormat _format)
	{
		if (_format == PixelFormat::L8) return Ogre::PixelFormatGpu::PFG_R8_UNORM;
		else if (_format == PixelFormat::L8A8) return Ogre::PixelFormatGpu::PFG_RG8_UNORM;
		else if (_format == PixelFormat::R8G8B8) return Ogre::PixelFormatGpu::PFG_RGBA8_UNORM;
		else if (_format == PixelFormat::R8G8B8A8) return Ogre::PixelFormatGpu::PFG_RGBA8_UNORM;

		return Ogre::PixelFormatGpu::PFG_UNKNOWN;
	}
#else
	Ogre::PixelFormat Ogre2Texture::convertFormat(PixelFormat _format)
	{
		if (_format == PixelFormat::L8) return Ogre::PF_BYTE_L;
		else if (_format == PixelFormat::L8A8) return Ogre::PF_BYTE_LA;
		else if (_format == PixelFormat::R8G8B8) return Ogre::PF_R8G8B8;
		else if (_format == PixelFormat::R8G8B8A8) return Ogre::PF_A8R8G8B8;

		return Ogre::PF_UNKNOWN;
	}
#endif

	void Ogre2Texture::setFormat(PixelFormat _format)
	{
		mOriginalFormat = _format;
		mPixelFormat = convertFormat(_format);
		mNumElemBytes = 0;

		if (_format == PixelFormat::L8) mNumElemBytes = 1;
		else if (_format == PixelFormat::L8A8) mNumElemBytes = 2;
		else if (_format == PixelFormat::R8G8B8) mNumElemBytes = 3;
		else if (_format == PixelFormat::R8G8B8A8) mNumElemBytes = 4;
	}

	void Ogre2Texture::createManual(int _width, int _height, TextureUsage _usage, PixelFormat _format)
	{
		setFormat(_format);
		uint32 textureFlags = 0; // Ogre::TextureFlags::PrefersLoadingFromFileAsSRGB
		if (_usage.isValue(TextureUsage::RenderTarget)) {
			textureFlags |= Ogre::TextureFlags::RenderToTexture;
		}
		// TODO: Ogre::TextureFlags::Manual

		Ogre::TextureGpuManager *textureMgr = Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager();
		mTexture = textureMgr->createOrRetrieveTexture(
			mName,
			Ogre::GpuPageOutStrategy::Discard,
			textureFlags,
			Ogre::TextureTypes::Type2D,
			Ogre::ResourceGroupManager::
			AUTODETECT_RESOURCE_GROUP_NAME);

		if (mTexture) {
			mTexture->setResolution(_width, _height);
			mTexture->setPixelFormat(mPixelFormat);
		}

		if (_usage.isValue(TextureUsage::RenderTarget)) {
			textureFlags |= Ogre::TextureFlags::RenderToTexture;
			if (mTexture->getNextResidencyStatus() != Ogre::GpuResidency::Resident) {
				mTexture->_transitionTo(Ogre::GpuResidency::Resident, nullptr);
				mTexture->_setNextResidencyStatus(Ogre::GpuResidency::Resident);
			}
		}
		setDataBlockTexture(mTexture);
	}

	void Ogre2Texture::loadFromFile(const std::string& _filename)
	{

		Ogre::TextureGpuManager* manager = Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager();

#if OGRE_VERSION_MINOR > 1
		bool needLoadTexture = false;
#endif

		if ( !manager->findTextureNoThrow(_filename) )
		{
			DataManager& resourcer = DataManager::getInstance();
			if (!resourcer.isDataExist(_filename))
			{
				MYGUI_PLATFORM_LOG(Error, "Texture '" + _filename + "' not found, set default texture");
			}
			else
			{
#if OGRE_VERSION_MINOR == 1
				mTexture = manager->load(_filename, mGroup, Ogre::TEX_TYPE_2D, 0, 1.0f, false, Ogre::PF_UNKNOWN, true);
#else
				needLoadTexture = true;
#endif
			}
		}
		else
		{
#if OGRE_VERSION_MINOR == 1
			mTexture = manager->getByName(_filename);
#else
			needLoadTexture = true;
#endif
		}
#if OGRE_VERSION_MINOR > 1
		if (needLoadTexture) {
			Ogre::TextureGpuManager *textureMgr = Ogre::Root::getSingletonPtr()->getRenderSystem()->getTextureGpuManager();
			mTexture = textureMgr->createOrRetrieveTexture(
				_filename,
				Ogre::GpuPageOutStrategy::Discard,
				Ogre::TextureFlags::PrefersLoadingFromFileAsSRGB,
				Ogre::TextureTypes::Type2D,
				Ogre::ResourceGroupManager::
				AUTODETECT_RESOURCE_GROUP_NAME);
			if (mTexture) {
				if (mTexture->getNextResidencyStatus() != Ogre::GpuResidency::Resident)
					mTexture->scheduleTransitionTo(Ogre::GpuResidency::Resident);
				mTexture->waitForData();
			}
		}
#endif
		setFormatByOgreTexture();

		setDataBlockTexture(mTexture);
	}

	void Ogre2Texture::setFormatByOgreTexture()
	{
		mOriginalFormat = PixelFormat::Unknow;
		mNumElemBytes = 0;
#if OGRE_VERSION_MINOR == 1
		mPixelFormat = Ogre::PF_UNKNOWN;

		if (!mTexture.isNull())
		{
			mPixelFormat = mTexture->getFormat();

			if (mPixelFormat == Ogre::PF_BYTE_L)
			{
				mOriginalFormat = PixelFormat::L8;
				mNumElemBytes = 1;
			}
			else if (mPixelFormat == Ogre::PF_BYTE_LA)
			{
				mOriginalFormat = PixelFormat::L8A8;
				mNumElemBytes = 2;
			}
			else if (mPixelFormat == Ogre::PF_R8G8B8)
			{
				mOriginalFormat = PixelFormat::R8G8B8;
				mNumElemBytes = 3;
			}
			else if (mPixelFormat == Ogre::PF_A8R8G8B8)
			{
				mOriginalFormat = PixelFormat::R8G8B8A8;
				mNumElemBytes = 4;
			}
			else
			{
				mOriginalFormat = PixelFormat::Unknow;
				mNumElemBytes = Ogre::PixelUtil::getNumElemBytes(mPixelFormat);
			}
#else
		mPixelFormat = Ogre::PixelFormatGpu::PFG_UNKNOWN;
		if (mTexture)
		{
			mPixelFormat = mTexture->getPixelFormat();

			if (mPixelFormat == Ogre::PixelFormatGpu::PFG_R8_UNORM)
			{
				mOriginalFormat = PixelFormat::L8;
				mNumElemBytes = 1;
			}
			else if (mPixelFormat == Ogre::PixelFormatGpu::PFG_RG8_UNORM)
			{
				mOriginalFormat = PixelFormat::L8A8;
				mNumElemBytes = 2;
			}
			else if (mPixelFormat == Ogre::PixelFormatGpu::PFG_RGBA8_UNORM)
			{
				mOriginalFormat = PixelFormat::R8G8B8;
				mNumElemBytes = 3;
			}
			else if (mPixelFormat == Ogre::PixelFormatGpu::PFG_RGBA8_UNORM)
			{
				mOriginalFormat = PixelFormat::R8G8B8A8;
				mNumElemBytes = 4;
			}
			else
			{
				mOriginalFormat = PixelFormat::Unknow;
				mNumElemBytes = Ogre::PixelFormatGpuUtils::getBytesPerPixel(mPixelFormat);
			}

		}
#endif
	}

	void Ogre2Texture::loadResource(Ogre::Resource* resource)
	{
		if (mListener)
			mListener->textureInvalidate(this);
	}

	IRenderTarget* Ogre2Texture::getRenderTarget()
	{
		if (mRenderTarget == nullptr)
			mRenderTarget = new Ogre2RTTexture(mTexture);

		return mRenderTarget;
	}


#if OGRE_VERSION_MINOR > 1
	void Ogre2Texture::setDataBlockTexture(Ogre::TextureGpu* _value)
	{
		mDataBlock->setTexture(TEXTURE_UNIT_NUMBER, _value);
	}
#else
	void Ogre2Texture::setDataBlockTexture(Ogre::TexturePtr _value)
	{
		mDataBlock->setTexture(TEXTURE_UNIT_NUMBER, 0, _value);
	}
#endif

	Ogre::HlmsDatablock* Ogre2Texture::getDataBlock()
	{
		return mDataBlock;
	}

	const Ogre::uint8 Ogre2Texture::TEXTURE_UNIT_NUMBER = 0;

	const OgreHlmsBlocks Ogre2Texture::HLMS_BLOCKS;

} // namespace MyGUI
