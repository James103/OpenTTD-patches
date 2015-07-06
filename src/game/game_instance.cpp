/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file game_instance.cpp Implementation of GameInstance. */

#include "../stdafx.h"
#include "../error.h"

#include "../script/convert.hpp"

#include "../script/script_storage.hpp"
#include "../ai/ai_gui.hpp"
#include "game_config.hpp"
#include "game_info.hpp"
#include "game_instance.hpp"
#include "game_text.hpp"
#include "game.hpp"

#include "../script/api/script_controller.hpp"

#include "../script/api/game/game.hpp.sq"


GameInstance::GameInstance() :
	ScriptInstance("GS")
{}

void GameInstance::Initialize (const GameInfo *info)
{
	this->versionAPI = info->GetAPIVersion();

	/* Register the GameController */
	SQGSController_Register(this->engine);

	ScriptInstance::Initialize (info, OWNER_DEITY);
}

void GameInstance::RegisterAPI()
{
	ScriptInstance::RegisterAPI();

	/* Register all classes */
	SQGS_Register (this->engine);

	RegisterGameTranslation(this->engine);

	if (!this->LoadCompatibilityScripts(this->versionAPI, GAME_DIR)) this->Died();
}

int GameInstance::GetSetting(const char *name)
{
	return GameConfig::GetConfig()->GetSetting(name);
}

ScriptInfo *GameInstance::FindLibrary(const char *library, int version)
{
	return (ScriptInfo *)Game::FindLibrary(library, version);
}

void GameInstance::Died()
{
	ScriptInstance::Died();

	ShowAIDebugWindow(OWNER_DEITY);

	const GameInfo *info = Game::GetInfo();
	if (info != NULL) {
		ShowErrorMessage(STR_ERROR_AI_PLEASE_REPORT_CRASH, INVALID_STRING_ID, WL_WARNING);

		if (info->GetURL() != NULL) {
			ScriptLog::Info("Please report the error to the following URL:");
			ScriptLog::Info(info->GetURL());
		}
	}
}

/**
 * DoCommand callback function for all commands executed by Game Scripts.
 * @param result The result of the command.
 */
void CcGame (const CommandCost &result)
{
	Game::GetGameInstance()->DoCommandCallback (result);
	Game::GetGameInstance()->Continue();
}

CommandSource GameInstance::GetCommandSource()
{
	return CMDSRC_GS;
}
