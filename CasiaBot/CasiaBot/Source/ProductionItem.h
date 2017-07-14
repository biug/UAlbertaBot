#pragma once

#include "MetaType.h"

namespace CasiaBot
{

	struct ProductionItem
	{

		MetaType	_unit;
		bool		_assigned;

	public:
		ProductionItem(const MetaType & unit);
		ProductionItem(const ProductionItem & item);
		ProductionItem &operator=(const ProductionItem & item);
	};
}