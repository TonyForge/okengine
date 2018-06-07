#include "ISpaceScreenGUI.h"

Zoner::ISpaceScreenGUI* Zoner::ISpaceScreenGUI::_instance = nullptr;

Zoner::ISpaceScreenGUI & Zoner::ISpaceScreenGUI::o()
{
	return *_instance;
}
