/*
* ygcDefaultSettings.h
* default app settings
*/

#pragma once

namespace ygc {

	namespace defaultAppSettings {

		Platform::String^ defaultBoardBackgroundURI = "ms-appx:///Assets/app_assets/ygc-tile-splash.png";
		Platform::String^ const defaultStones[3] = { "", "ms-appx:///Assets/board_assets/ygc-stone-black.png", "ms-appx:///Assets/board_assets/ygc-stone-white.png" };
		Windows::UI::Color const defaultStoneColors[3] = { Windows::UI::Colors::Transparent, Windows::UI::Colors::Black, Windows::UI::Colors::White };
		Windows::UI::Color const defaultGridColor = Windows::UI::Colors::White;

		Platform::String^ const defaultEditIcons[3] = { "ms-appx:///Assets/board_assets/ygc-edit-stone-both.png", "ms-appx:///Assets/board_assets/ygc-edit-stone-black.png", "ms-appx:///Assets/board_assets/ygc-edit-stone-white.png" };
		Platform::String^ const defaultModeIcons[2] = { "ms-appx:///Assets/board_assets/appbar.edit.png", "ms-appx:///Assets/board_assets/appbar.control.play.png" };
		Platform::String^ const defaultEditNavIcons[2] = { "ms-appx:///Assets/board_assets/appbar.navigate.previous.png", "ms-appx:///Assets/board_assets/appbar.navigate.next.png" };

	};

};