/*
* ygcDefaultSettings.h
* default app settings
*/

#pragma once

namespace ygc {

	namespace defaultAppSettings {

		Platform::String^ defaultBoardBackgroundURI = "ms-appx://Assets/app_assets/ygc-tile-splash.png";
		Platform::String^ const defaultStones[3] = { "", "ms-appx://Assets/board_assets/ygc-stone-black.png", "ms-appx://Assets/board_assets/ygc-stone-white.png" };
		Windows::UI::Color const defaultStoneColors[3] = { Windows::UI::Colors::Transparent, Windows::UI::Colors::Black, Windows::UI::Colors::White };
		Windows::UI::Color const defaultGridColor = Windows::UI::Colors::White;

	};

};