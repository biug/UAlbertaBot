#include "ProductionItem.h"

using namespace UAlbertaBot;

ProductionItem::ProductionItem(const MetaType & unit)
{
	_unit = unit;
	_assigned = false;
}

ProductionItem::ProductionItem(const ProductionItem & item)
{
	_unit = item._unit;
	_assigned = item._assigned;
}