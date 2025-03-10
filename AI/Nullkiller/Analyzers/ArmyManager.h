/*
* ArmyManager.h, part of VCMI engine
*
* Authors: listed in file AUTHORS in main folder
*
* License: GNU General Public License v2.0 or later
* Full text of license available in license.txt file, in main folder
*
*/

#pragma once

#include "../AIUtility.h"

#include "../../../lib/GameConstants.h"
#include "../../../lib/GameLibrary.h"

namespace NKAI
{

class Nullkiller;

struct SlotInfo
{
	const CCreature * creature;
	int count;
	uint64_t power;
};

struct ArmyUpgradeInfo
{
	std::vector<SlotInfo> resultingArmy;
	int64_t upgradeValue = 0;
	TResources upgradeCost;

	void addArmyToBuy(std::vector<SlotInfo> army);
	void addArmyToGet(std::vector<SlotInfo> army);
};

class DLL_EXPORT IArmyManager //: public: IAbstractManager
{
public:
	virtual ~IArmyManager() = default;
	virtual void update() = 0;
	virtual ui64 howManyReinforcementsCanBuy(const CCreatureSet * target, const CGDwelling * source) const = 0;
	virtual	ui64 howManyReinforcementsCanBuy(
		const CCreatureSet * targetArmy,
		const CGDwelling * dwelling,
		const TResources & availableResources,
		uint8_t turn = 0) const = 0;

	virtual ui64 howManyReinforcementsCanGet(const CGHeroInstance * hero, const CCreatureSet * source) const = 0;
	virtual ui64 howManyReinforcementsCanGet(
		const IBonusBearer * armyCarrier,
		const CCreatureSet * target,
		const CCreatureSet * source,
		const TerrainId & armyTerrain) const = 0;

	virtual std::vector<SlotInfo> getBestArmy(const IBonusBearer * armyCarrier, const CCreatureSet * target, const CCreatureSet * source, const TerrainId & armyTerrain) const = 0;
	virtual std::vector<SlotInfo>::iterator getBestUnitForScout(std::vector<SlotInfo> & army, const TerrainId & armyTerrain) const = 0;
	virtual std::vector<SlotInfo> getSortedSlots(const CCreatureSet * target, const CCreatureSet * source) const = 0;
	virtual std::vector<SlotInfo> toSlotInfo(std::vector<creInfo> creatures) const = 0;

	virtual std::vector<creInfo> getArmyAvailableToBuy(const CCreatureSet * hero, const CGDwelling * dwelling) const = 0;
	virtual std::vector<creInfo> getArmyAvailableToBuy(
		const CCreatureSet * hero,
		const CGDwelling * dwelling,
		TResources availableRes,
		uint8_t turn = 0) const = 0;

	virtual uint64_t evaluateStackPower(const Creature * creature, int count) const = 0;
	virtual SlotInfo getTotalCreaturesAvailable(CreatureID creatureID) const = 0;
	virtual ArmyUpgradeInfo calculateCreaturesUpgrade(
		const CCreatureSet * army,
		const CGObjectInstance * upgrader,
		const TResources & availableResources) const = 0;
	virtual std::shared_ptr<CCreatureSet> getArmyAvailableToBuyAsCCreatureSet(const CGDwelling * dwelling, TResources availableRes) const = 0;
};

class StackUpgradeInfo;

class DLL_EXPORT ArmyManager : public IArmyManager
{
private:
	CPlayerSpecificInfoCallback * cb; //this is enough, but we downcast from CCallback
	const Nullkiller * ai;
	std::map<CreatureID, SlotInfo> totalArmy;

public:
	ArmyManager(CPlayerSpecificInfoCallback * CB, const Nullkiller * ai): cb(CB), ai(ai) {}
	void update() override;

	ui64 howManyReinforcementsCanBuy(const CCreatureSet * target, const CGDwelling * source) const override;
	ui64 howManyReinforcementsCanBuy(
		const CCreatureSet * targetArmy,
		const CGDwelling * dwelling,
		const TResources & availableResources,
		uint8_t turn = 0) const override;

	ui64 howManyReinforcementsCanGet(const CGHeroInstance * hero, const CCreatureSet * source) const override;
	ui64 howManyReinforcementsCanGet(const IBonusBearer * armyCarrier, const CCreatureSet * target, const CCreatureSet * source, const TerrainId & armyTerrain) const override;
	std::vector<SlotInfo> getBestArmy(const IBonusBearer * armyCarrier, const CCreatureSet * target, const CCreatureSet * source, const TerrainId & armyTerrain) const override;
	std::vector<SlotInfo>::iterator getBestUnitForScout(std::vector<SlotInfo> & army, const TerrainId & armyTerrain) const override;
	std::vector<SlotInfo> getSortedSlots(const CCreatureSet * target, const CCreatureSet * source) const override;
	std::vector<SlotInfo> toSlotInfo(std::vector<creInfo> creatures) const override;

	std::vector<creInfo> getArmyAvailableToBuy(const CCreatureSet * hero, const CGDwelling * dwelling) const override;
	std::vector<creInfo> getArmyAvailableToBuy(
		const CCreatureSet * hero,
		const CGDwelling * dwelling,
		TResources availableRes,
		uint8_t turn = 0) const override;

	std::shared_ptr<CCreatureSet> getArmyAvailableToBuyAsCCreatureSet(const CGDwelling * dwelling, TResources availableRes) const override;
	uint64_t evaluateStackPower(const Creature * creature, int count) const override;
	SlotInfo getTotalCreaturesAvailable(CreatureID creatureID) const override;
	ArmyUpgradeInfo calculateCreaturesUpgrade(
		const CCreatureSet * army, 
		const CGObjectInstance * upgrader,
		const TResources & availableResources) const override;

private:
	std::vector<SlotInfo> convertToSlots(const CCreatureSet * army) const;
	std::vector<StackUpgradeInfo> getPossibleUpgrades(const CCreatureSet * army, const CGObjectInstance * upgrader) const;
	std::vector<StackUpgradeInfo> getHillFortUpgrades(const CCreatureSet * army) const;
	std::vector<StackUpgradeInfo> getDwellingUpgrades(const CCreatureSet * army, const CGDwelling * dwelling) const;
};

}
