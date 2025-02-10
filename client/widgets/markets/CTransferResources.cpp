/*
 * CTransferResources.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"
#include "CTransferResources.h"

#include "../../GameEngine.h"
#include "../../gui/Shortcut.h"
#include "../../widgets/Buttons.h"
#include "../../widgets/TextControls.h"

#include "../../CPlayerInterface.h"

#include "../../../CCallback.h"

#include "../../../lib/texts/CGeneralTextHandler.h"
#include "../../../lib/mapObjects/IMarket.h"
#include "../../../lib/texts/MetaString.h"
#include "../../../lib/VCMI_Lib.h"

CTransferResources::CTransferResources(const IMarket * market, const CGHeroInstance * hero)
	: CMarketBase(market, hero)
	, CResourcesSelling([this](const std::shared_ptr<CTradeableItem> & heroSlot){CTransferResources::onSlotClickPressed(heroSlot, bidTradePanel);})
	, CMarketSlider([this](int newVal){CMarketSlider::onOfferSliderMoved(newVal);})
	, CMarketTraderText(Point(28, 48))
{
	OBJECT_CONSTRUCTION;

	labels.emplace_back(std::make_shared<CLabel>(titlePos.x, titlePos.y, FONT_BIG, ETextAlignment::CENTER, Colors::YELLOW, VLC->generaltexth->allTexts[158]));
	labels.emplace_back(std::make_shared<CLabel>(445, 56, FONT_SMALL, ETextAlignment::CENTER, Colors::WHITE, VLC->generaltexth->allTexts[169]));
	deal = std::make_shared<CButton>(dealButtonPosWithSlider, AnimationPath::builtin("TPMRKB.DEF"),
		VLC->generaltexth->zelp[595], [this](){CTransferResources::makeDeal();}, EShortcut::MARKET_DEAL);

	// Player's resources
	assert(bidTradePanel);
	bidTradePanel->moveTo(pos.topLeft() + Point(40, 183));

	// Players panel
	offerTradePanel = std::make_shared<PlayersPanel>([this](const std::shared_ptr<CTradeableItem> & heroSlot)
		{
			CTransferResources::onSlotClickPressed(heroSlot, offerTradePanel);
		});
	offerTradePanel->moveTo(pos.topLeft() + Point(333, 84));

	CMarketBase::update();
	CTransferResources::deselect();
}

void CTransferResources::deselect()
{
	CMarketBase::deselect();
	CMarketSlider::deselect();
	CMarketTraderText::deselect();
}

void CTransferResources::makeDeal()
{
	if(auto toTrade = offerSlider->getValue(); toTrade != 0)
	{
		LOCPLINT->cb->trade(market->getObjInstanceID(), EMarketMode::RESOURCE_PLAYER, GameResID(bidTradePanel->getHighlightedItemId()),
			PlayerColor(offerTradePanel->getHighlightedItemId()), toTrade, hero);
		CMarketTraderText::makeDeal();
		deselect();
	}
}

CMarketBase::MarketShowcasesParams CTransferResources::getShowcasesParams() const
{
	if(bidTradePanel->isHighlighted() && offerTradePanel->isHighlighted())
		return MarketShowcasesParams
		{
			ShowcaseParams {std::to_string(offerSlider->getValue()), bidTradePanel->getHighlightedItemId()},
			ShowcaseParams {VLC->generaltexth->capColors[offerTradePanel->getHighlightedItemId()], offerTradePanel->getHighlightedItemId()}
		};
	else
		return MarketShowcasesParams {std::nullopt, std::nullopt};
}

void CTransferResources::highlightingChanged()
{
	if(bidTradePanel->isHighlighted() && offerTradePanel->isHighlighted())
	{
		offerSlider->setAmount(LOCPLINT->cb->getResourceAmount(GameResID(bidTradePanel->getHighlightedItemId())));
		offerSlider->scrollTo(0);
		offerSlider->block(false);
		maxAmount->block(false);
		deal->block(!LOCPLINT->makingTurn);
	}
	CMarketBase::highlightingChanged();
	CMarketTraderText::highlightingChanged();
}

std::string CTransferResources::getTraderText()
{
	if(bidTradePanel->isHighlighted() && offerTradePanel->isHighlighted())
	{
		MetaString message = MetaString::createFromTextID("core.genrltxt.165");
		message.replaceName(GameResID(bidTradePanel->getHighlightedItemId()));
		message.replaceName(PlayerColor(offerTradePanel->getHighlightedItemId()));
		return message.toString();
	}
	else
	{
		return madeTransaction ? VLC->generaltexth->allTexts[166] : VLC->generaltexth->allTexts[167];
	}
}
