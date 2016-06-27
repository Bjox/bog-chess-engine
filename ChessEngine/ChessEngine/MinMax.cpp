#include "MinMax.h"
#include "Validator.h"

// TODO: make minmax a class? make baseBoard static?
void MinMax::applyMinMax(Node *root)
{
	const unsigned int nc = root->fields.num_childs;

	// Recursive call
	for (unsigned int i = 0; i < nc; i++)
	{
		applyMinMax(root->childrenPtr[i]);
	}

	// Fetch values from children
	if (nc > 0)
	{
		short best = root->childrenPtr[0]->value;
		short childVal;

		// White -> max
		if (root->getColor() == Board::WHITE)
		{
			best = root->childrenPtr[0]->value;
			for (unsigned int i = 0; i < nc; i++)
			{
				childVal = root->childrenPtr[i]->value;
				if (childVal > best)
				{
					best = childVal;
				}
			}
		}
		// Black -> min
		else
		{
			for (unsigned int i = 0; i < nc; i++)
			{
				childVal = root->childrenPtr[i]->value;
				if (childVal < best)
				{
					best = childVal;
				}
			}
		}

		root->value = best;
	}
}
