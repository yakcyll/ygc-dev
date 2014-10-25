//
// EditorPage.xaml.h
// Declaration of the EditorPage class
//

#pragma once

#include "BoardPage.h"
#include "EditorPage.g.h"

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class EditorPage sealed
	{
	internal:
		StackPanel ^ CounterPanel, ^ CPCont, ^ TPMode, ^ TPHistory;
		Grid ^ ToolPanel;
		Border ^ CounterPanelBorder, ^ TPModeBorder, ^TPHistoryBorder;
		Canvas ^ LayoutRoot;

		Array<TextBlock^>^ scoreTBs;
		Array<uint16_t>^ playerScores;
		uint16_t moveId, noPlayers;
		ygcStoneColor ^ turn;
		bool historyModeEnabled;
		bool mixedStonesEnabled;

		BoardPage ^ bp;

		void InitBoard();
		void InitPanels();
		void InitInputHandler();

		void RewindHistory();

	public:
		EditorPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ s, SizeChangedEventArgs ^ sce);
	};
}
