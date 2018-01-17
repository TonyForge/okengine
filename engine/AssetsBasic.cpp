#include "AssetsBasic.h"

ok::AssetsBasic* ok::AssetsBasic::basic_instance = nullptr;

ok::AssetsBasic & ok::AssetsBasic::instance()
{
	return *basic_instance;
}
