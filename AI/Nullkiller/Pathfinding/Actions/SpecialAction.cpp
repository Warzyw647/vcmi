/*
* SpecialAction.cpp, part of VCMI engine
*
* Authors: listed in file AUTHORS in main folder
*
* License: GNU General Public License v2.0 or later
* Full text of license available in license.txt file, in main folder
*
*/

#include "StdInc.h"
#include "SpecialAction.h"
#include "../../AIGateway.h"
#include "../../Goals/CGoal.h"
#include "../../Goals/Invalid.h"

namespace NKAI
{

Goals::TSubgoal SpecialAction::decompose(const Nullkiller * ai, const CGHeroInstance * hero) const
{
	return Goals::sptr(Goals::Invalid());
}

void SpecialAction::execute(AIGateway * ai, const CGHeroInstance * hero) const
{
	throw cannotFulfillGoalException("Can not execute " + toString());
}

bool CompositeAction::canAct(const Nullkiller * ai, const AIPathNode * source) const
{
	for(auto part : parts)
	{
		if(!part->canAct(ai, source)) return false;
	}

	return true;
}

Goals::TSubgoal CompositeAction::decompose(const Nullkiller * ai, const CGHeroInstance * hero) const
{
	for(auto part : parts)
	{
		auto goal = part->decompose(ai, hero);

		if(!goal->invalid()) return goal;
	}

	return SpecialAction::decompose(ai, hero);
}

void CompositeAction::execute(AIGateway * ai, const CGHeroInstance * hero) const
{
	for(auto part : parts)
	{
		part->execute(ai, hero);
	}
}

void CompositeAction::applyOnDestination(
	const CGHeroInstance * hero,
	CDestinationNodeInfo & destination,
	const PathNodeInfo & source,
	AIPathNode * dstNode,
	const AIPathNode * srcNode) const
{
	for(auto part : parts)
	{
		part->applyOnDestination(hero, destination, source, dstNode, srcNode);
	}
}

std::string CompositeAction::toString() const
{
	std::string result = "";

	for(auto part : parts)
	{
		result += ", " + part->toString();
	}

	return result;
}

const CGObjectInstance * CompositeAction::targetObject() const
{
	if(parts.empty())
		return nullptr;

	return parts.front()->targetObject();
}

}
