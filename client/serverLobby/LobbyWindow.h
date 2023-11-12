/*
 * LobbyWindow.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "../gui/InterfaceObjectConfigurable.h"
#include "../windows/CWindowObject.h"

#include "../../lib/network/NetworkClient.h"

class LobbyWindow;

class LobbyWidget : public InterfaceObjectConfigurable
{
	LobbyWindow * window;
public:
	LobbyWidget(LobbyWindow * window);

	std::shared_ptr<CTextInput> getMessageInput();
};

class LobbyClient : public NetworkClient
{
	LobbyWindow * window;

	void onPacketReceived(const std::vector<uint8_t> & message) override;
	void onConnectionFailed(const std::string & errorMessage) override;
	void onDisconnected() override;

public:
	explicit LobbyClient(LobbyWindow * window);

	void sendMessage(const JsonNode & data);
};

class LobbyWindow : public CWindowObject
{
	std::shared_ptr<LobbyWidget> widget;
	std::shared_ptr<LobbyClient> connection;

	void tick(uint32_t msPassed);

public:
	LobbyWindow();

	void doSendChatMessage();

	void onGameChatMessage(std::string sender, std::string message, std::string when);
};
