//
// BoardPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

using namespace Windows::Foundation;

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class BoardPage sealed
	{
	internal:
		double PanelWidth;
		double SideMargin;
		double AppSpaceWidth;
		double AppSpaceHeight;

		ScrollViewer ^ ScrollBoardView;
		Canvas ^ LayoutRoot, ^ boardGrid;
		Border ^ boardGridBorder;
		Vector<Image^>^ stonesOnCanvas;
		Vector<Point>^ stonesCoordinates;
		ygcMatch ^ currentMatch;

		void InitUI();
		void InitScrollViewer();
		void InitBoardGrid();
		void DrawBoardGrid();

		void AddStone(Point, ygcStoneColor);
		bool RemoveStone(Point);
		void ClearBoard();

		BoardPage(Page^);

		virtual void BoardOnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		virtual void BoardOnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void BoardOrientHandler(Object^ sender, SizeChangedEventArgs^ sce);
	};
}
