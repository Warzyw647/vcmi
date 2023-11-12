/*
 * LobbyWindow.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"
#include "LobbyWindow.h"

#include "../gui/CGuiHandler.h"
#include "../gui/WindowHandler.h"
#include "../widgets/TextControls.h"

#include "../windows/InfoWindows.h"

LobbyClient::LobbyClient(LobbyWindow * window)
	: window(window)
{

}

void LobbyClient::onPacketReceived(const std::vector<uint8_t> & message)
{

}

void LobbyClient::onConnectionFailed(const std::string & errorMessage)
{
	GH.windows().popWindows(1);
	CInfoWindow::showInfoDialog("Failed to connect to game lobby!\n" + errorMessage, {});
}

void LobbyClient::onDisconnected()
{
	GH.windows().popWindows(1);
	CInfoWindow::showInfoDialog("Connection to game lobby was lost!", {});
}

void LobbyClient::sendMessage(const JsonNode & data)
{
	std::string payloadString = data.toJson(true);

	// FIXME: find better approach
	uint8_t * payloadBegin = reinterpret_cast<uint8_t*>(payloadString.data());
	uint8_t * payloadEnd = payloadBegin + payloadString.size();

	std::vector<uint8_t> payloadBuffer(payloadBegin, payloadEnd);

	sendPacket(payloadBuffer);
}

LobbyWidget::LobbyWidget(LobbyWindow * window)
	: window(window)
{
	addCallback("closeWindow", [](int) { GH.windows().popWindows(1); });
	addCallback("sendMessage", [this](int) { this->window->doSendChatMessage(); });

	const JsonNode config(JsonPath::builtin("config/widgets/lobbyWindow.json"));
	build(config);
}

std::shared_ptr<CTextInput> LobbyWidget::getMessageInput()
{
	return widget<CTextInput>("messageInput");
}

LobbyWindow::LobbyWindow():
	CWindowObject(BORDERED)
{
	OBJ_CONSTRUCTION_CAPTURING_ALL_NO_DISPOSE;
	widget = std::make_shared<LobbyWidget>(this);
	pos = widget->pos;
	center();
	connection = std::make_shared<LobbyClient>(this);

	connection->start("127.0.0.1", 30303);

	addUsedEvents(TIME);
}

void LobbyWindow::tick(uint32_t msPassed)
{
	connection->poll();
}

void LobbyWindow::doSendChatMessage()
{
	std::string messageText = widget->getMessageInput()->getText();

	JsonNode toSend;
	toSend["type"].String() = "sendChatMessage";
	toSend["messageText"].String() = messageText;

	connection->sendMessage(toSend);

	widget->getMessageInput()->setText("");
}
