/*!
	@file
	@author		Albert Semenov
	@date		02/2008
	@module
*/
#include "MyGUI_Precompiled.h"
#include "MyGUI_PopupMenu.h"

namespace MyGUI
{

	PopupMenu::PopupMenu(WidgetStyle _style, const IntCoord& _coord, Align _align, const WidgetSkinInfoPtr _info, WidgetPtr _parent, ICroppedRectangle * _croppedParent, IWidgetCreator * _creator, const std::string & _name) :
		Base(_style, _coord, _align, _info, _parent, _croppedParent, _creator, _name)
	{
		mHideByLostKey = true;
		initialiseWidgetSkin(_info);
	}

	PopupMenu::~PopupMenu()
	{
		shutdownWidgetSkin();
	}

	void PopupMenu::baseChangeWidgetSkin(WidgetSkinInfoPtr _info)
	{
		shutdownWidgetSkin();
		Base::baseChangeWidgetSkin(_info);
		initialiseWidgetSkin(_info);
	}

	void PopupMenu::initialiseWidgetSkin(WidgetSkinInfoPtr _info)
	{
	}

	void PopupMenu::shutdownWidgetSkin()
	{
	}

} // namespace MyGUI
