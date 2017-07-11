#include "ProductionItem.h"

using namespace UAlbertaBot;

ProductionItem::ProductionItem(const MetaType & unit)
{
	_unit = unit;
	_assigned = false;
}