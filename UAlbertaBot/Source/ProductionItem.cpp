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

ProductionItem& ProductionItem::operator=(const ProductionItem & item)
{
	if (this == &item) return *this;
	this->_unit = item._unit;
	this->_assigned = item._assigned;
	return *this;
}