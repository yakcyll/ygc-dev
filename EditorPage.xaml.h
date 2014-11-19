//
// EditorPage.xaml.h
// Declaration of the EditorPage class
//

#pragma once

#include "BoardPage.h"
#include "EditorPage.g.h"

namespace ygc
{
	public ref class EditorPage sealed
	{
	internal:
		StackPanel ^ CounterPanel, ^ ToolPanel, ^ FilePanel, ^ BottomStack, ^ CPCount, ^CPInd, ^ TPMode, ^ TPHistory;
		Grid ^ CPIndCont, ^ TPModeCont, ^ TPHistoryCont;
		Border ^ CPCountBorder, ^CPIndBorder, ^ TPModeBorder, ^TPHistoryBorder;
		Canvas ^ LayoutRoot;
		ScrollViewer ^ BottomPanel;

		Image ^ turnIndicator, ^ stoneBrush, ^ checkPoint, ^ clearBoard, ^ hRewind, ^ hPrev, ^ hNext, ^loadFile;

		Array<TextBlock^>^ scoreTBs;
		Array<uint16_t>^ playerScores;
		uint16_t moveId, checkPointId, noPlayers;
		ygcStoneColor ^ turn, ^ historyTurn;
		bool historyModeEnabled;
		bool mixedStonesEnabled;

		BoardPage ^ bp;

		void InitBoard();
		void InitPanels();
		void InitInputHandler();

		void UpdateIcons();
		void ClearBoard();
		bool LoadBoard(Platform::String^);
		void UpdateBoard(ygcMove ^, bool);
		void RewindHistory(bool = false);

		void FilePickerContinuationActivated(Windows::ApplicationModel::Core::CoreApplicationView ^s, Windows::ApplicationModel::Activation::IActivatedEventArgs ^e);
		Windows::Foundation::EventRegistrationToken fpcert;
		Windows::ApplicationModel::Activation::FileActivatedEventArgs^ fileEventArgs;

	public:
		EditorPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ s, SizeChangedEventArgs ^ sce);
	};
}
