#pragma once

#include "MetaType.h"

namespace UAlbertaBot
{

	struct ProductionItem
	{

		MetaType	_unit;
		bool		_assigned;

	public:
		ProductionItem(const MetaType & unit);
	};
}