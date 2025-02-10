/*
 * CCursorHandler.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"
#include "CursorHandler.h"

#include "CGuiHandler.h"
#include "FramerateManager.h"
#include "../renderSDL/CursorSoftware.h"
#include "../renderSDL/CursorHardware.h"
#include "../render/CAnimation.h"
#include "../render/IImage.h"
#include "../render/IScreenHandler.h"
#include "../render/IRenderHandler.h"

#include "../../lib/CConfigHandler.h"

std::unique_ptr<ICursor> CursorHandler::createCursor()
{
#if defined(VCMI_MOBILE)
	if (settings["general"]["userRelativePointer"].Bool())
		return std::make_unique<CursorSoftware>();
#endif

	if (settings["video"]["cursor"].String() == "hardware")
		return std::make_unique<CursorHardware>();

	assert(settings["video"]["cursor"].String() == "software");
	return std::make_unique<CursorSoftware>();
}

CursorHandler::CursorHandler()
	: cursor(createCursor())
	, frameTime(0.f)
	, showing(false)
	, pos(0,0)
{

	type = Cursor::Type::DEFAULT;
	dndObject = nullptr;

	cursors =
	{
		GH.renderHandler().loadAnimation(AnimationPath::builtin("CRADVNTR"), EImageBlitMode::COLORKEY),
		GH.renderHandler().loadAnimation(AnimationPath::builtin("CRCOMBAT"), EImageBlitMode::COLORKEY),
		GH.renderHandler().loadAnimation(AnimationPath::builtin("CRDEFLT"), EImageBlitMode::COLORKEY),
		GH.renderHandler().loadAnimation(AnimationPath::builtin("CRSPELL"), EImageBlitMode::COLORKEY)
	};

	set(Cursor::Map::POINTER);
	showType = dynamic_cast<CursorSoftware *>(cursor.get()) ? Cursor::ShowType::SOFTWARE : Cursor::ShowType::HARDWARE;
}

CursorHandler::~CursorHandler() = default;

void CursorHandler::changeGraphic(Cursor::Type type, size_t index)
{
	assert(dndObject == nullptr);

	if (type == this->type && index == this->frame)
		return;

	this->type = type;
	this->frame = index;

	cursor->setImage(getCurrentImage(), getPivotOffset());
}

void CursorHandler::set(Cursor::Default index)
{
	changeGraphic(Cursor::Type::DEFAULT, static_cast<size_t>(index));
}

void CursorHandler::set(Cursor::Map index)
{
	changeGraphic(Cursor::Type::ADVENTURE, static_cast<size_t>(index));
}

void CursorHandler::set(Cursor::Combat index)
{
	changeGraphic(Cursor::Type::COMBAT, static_cast<size_t>(index));
}

void CursorHandler::set(Cursor::Spellcast index)
{
	//Note: this is animated cursor, ignore specified frame and only change type
	changeGraphic(Cursor::Type::SPELLBOOK, frame);
}

void CursorHandler::dragAndDropCursor(std::shared_ptr<IImage> image)
{
	dndObject = image;
	cursor->setImage(getCurrentImage(), getPivotOffset());
}

void CursorHandler::dragAndDropCursor (const AnimationPath & path, size_t index)
{
	auto anim = GH.renderHandler().loadAnimation(path, EImageBlitMode::COLORKEY);
	dragAndDropCursor(anim->getImage(index));
}

void CursorHandler::cursorMove(const int & x, const int & y)
{
	pos.x = x;
	pos.y = y;

	cursor->setCursorPosition(pos);
}

Point CursorHandler::getPivotOffsetDefault(size_t index)
{
	return {0, 0};
}

Point CursorHandler::getPivotOffsetMap(size_t index)
{
	static const std::array<Point, 43> offsets = {{
		{  0,  0}, // POINTER          =  0,
		{  0,  0}, // HOURGLASS        =  1,
		{ 12, 10}, // HERO             =  2,
		{ 12, 12}, // TOWN             =  3,

		{ 15, 13}, // T1_MOVE          =  4,
		{ 13, 13}, // T1_ATTACK        =  5,
		{ 16, 32}, // T1_SAIL          =  6,
		{ 13, 20}, // T1_DISEMBARK     =  7,
		{  8,  9}, // T1_EXCHANGE      =  8,
		{ 14, 16}, // T1_VISIT         =  9,

		{ 15, 13}, // T2_MOVE          = 10,
		{ 13, 13}, // T2_ATTACK        = 11,
		{ 16, 32}, // T2_SAIL          = 12,
		{ 13, 20}, // T2_DISEMBARK     = 13,
		{  8,  9}, // T2_EXCHANGE      = 14,
		{ 14, 16}, // T2_VISIT         = 15,

		{ 15, 13}, // T3_MOVE          = 16,
		{ 13, 13}, // T3_ATTACK        = 17,
		{ 16, 32}, // T3_SAIL          = 18,
		{ 13, 20}, // T3_DISEMBARK     = 19,
		{  8,  9}, // T3_EXCHANGE      = 20,
		{ 14, 16}, // T3_VISIT         = 21,

		{ 15, 13}, // T4_MOVE          = 22,
		{ 13, 13}, // T4_ATTACK        = 23,
		{ 16, 32}, // T4_SAIL          = 24,
		{ 13, 20}, // T4_DISEMBARK     = 25,
		{  8,  9}, // T4_EXCHANGE      = 26,
		{ 14, 16}, // T4_VISIT         = 27,

		{ 16, 32}, // T1_SAIL_VISIT    = 28,
		{ 16, 32}, // T2_SAIL_VISIT    = 29,
		{ 16, 32}, // T3_SAIL_VISIT    = 30,
		{ 16, 32}, // T4_SAIL_VISIT    = 31,

		{  6,  1}, // SCROLL_NORTH     = 32,
		{ 16,  2}, // SCROLL_NORTHEAST = 33,
		{ 21,  6}, // SCROLL_EAST      = 34,
		{ 16, 16}, // SCROLL_SOUTHEAST = 35,
		{  6, 21}, // SCROLL_SOUTH     = 36,
		{  1, 16}, // SCROLL_SOUTHWEST = 37,
		{  1,  5}, // SCROLL_WEST      = 38,
		{  2,  1}, // SCROLL_NORTHWEST = 39,

		{  0,  0}, // POINTER_COPY     = 40,
		{ 14, 16}, // TELEPORT         = 41,
		{ 20, 20}, // SCUTTLE_BOAT     = 42
	}};

	assert(offsets.size() == size_t(Cursor::Map::COUNT)); //Invalid number of pivot offsets for cursor
	assert(index < offsets.size());
	return offsets[index] * GH.screenHandler().getScalingFactor();
}

Point CursorHandler::getPivotOffsetCombat(size_t index)
{
	static const std::array<Point, 20> offsets = {{
		{ 12, 12 }, // BLOCKED        = 0,
		{ 10, 14 }, // MOVE           = 1,
		{ 14, 14 }, // FLY            = 2,
		{ 12, 12 }, // SHOOT          = 3,
		{ 12, 12 }, // HERO           = 4,
		{  8, 12 }, // QUERY          = 5,
		{  0,  0 }, // POINTER        = 6,
		{ 21,  0 }, // HIT_NORTHEAST  = 7,
		{ 31,  5 }, // HIT_EAST       = 8,
		{ 21, 21 }, // HIT_SOUTHEAST  = 9,
		{  0, 21 }, // HIT_SOUTHWEST  = 10,
		{  0,  5 }, // HIT_WEST       = 11,
		{  0,  0 }, // HIT_NORTHWEST  = 12,
		{  6,  0 }, // HIT_NORTH      = 13,
		{  6, 31 }, // HIT_SOUTH      = 14,
		{ 14,  0 }, // SHOOT_PENALTY  = 15,
		{ 12, 12 }, // SHOOT_CATAPULT = 16,
		{ 12, 12 }, // HEAL           = 17,
		{ 12, 12 }, // SACRIFICE      = 18,
		{ 14, 20 }, // TELEPORT       = 19
	}};

	assert(offsets.size() == size_t(Cursor::Combat::COUNT)); //Invalid number of pivot offsets for cursor
	assert(index < offsets.size());
	return offsets[index] * GH.screenHandler().getScalingFactor();
}

Point CursorHandler::getPivotOffsetSpellcast()
{
	return Point(18, 28) * GH.screenHandler().getScalingFactor();
}

Point CursorHandler::getPivotOffset()
{
	if (dndObject)
		return dndObject->dimensions() / 2;

	switch (type) {
	case Cursor::Type::ADVENTURE: return getPivotOffsetMap(frame);
	case Cursor::Type::COMBAT:    return getPivotOffsetCombat(frame);
	case Cursor::Type::DEFAULT:   return getPivotOffsetDefault(frame);
	case Cursor::Type::SPELLBOOK: return getPivotOffsetSpellcast();
	};

	assert(0);
	return {0, 0};
}

std::shared_ptr<IImage> CursorHandler::getCurrentImage()
{
	if (dndObject)
		return dndObject;

	return cursors[static_cast<size_t>(type)]->getImage(frame);
}

void CursorHandler::updateSpellcastCursor()
{
	static const float frameDisplayDuration = 0.1f; // H3 uses 100 ms per frame

	frameTime += GH.framerate().getElapsedMilliseconds() / 1000.f;
	size_t newFrame = frame;

	while (frameTime >= frameDisplayDuration)
	{
		frameTime -= frameDisplayDuration;
		newFrame++;
	}

	auto & animation = cursors.at(static_cast<size_t>(type));

	while (newFrame >= animation->size())
		newFrame -= animation->size();

	changeGraphic(Cursor::Type::SPELLBOOK, newFrame);
}

void CursorHandler::render()
{
	cursor->render();
}

void CursorHandler::update()
{
	if(!showing)
		return;

	if (type == Cursor::Type::SPELLBOOK)
		updateSpellcastCursor();

	cursor->update();
}

void CursorHandler::hide()
{
	if (!showing)
		return;

	showing = false;
	cursor->setVisible(false);
}

void CursorHandler::show()
{
	if (showing)
		return;

	showing = true;
	cursor->setVisible(true);
}

Cursor::ShowType CursorHandler::getShowType() const
{
	return showType;
}

void CursorHandler::changeCursor(Cursor::ShowType newShowType)
{
	if(newShowType == showType)
		return;

	switch(newShowType)
	{
		case Cursor::ShowType::SOFTWARE:
			cursor.reset(new CursorSoftware());
			showType = Cursor::ShowType::SOFTWARE;
			cursor->setImage(getCurrentImage(), getPivotOffset());
			break;
		case Cursor::ShowType::HARDWARE:
			cursor.reset(new CursorHardware());
			showType = Cursor::ShowType::HARDWARE;
			cursor->setImage(getCurrentImage(), getPivotOffset());
			break;
	}
}

void CursorHandler::onScreenResize()
{
	cursor->setImage(getCurrentImage(), getPivotOffset());
}
