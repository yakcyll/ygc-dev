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
		ygcMatch ^ currentMatch;

		void InitUI();
		void InitScrollViewer();
		void InitBoardGrid();
		void DrawBoardGrid();

		void UpdateScrollBoardViewScroll(Object^, SizeChangedEventArgs^);

		BoardPage(Page^);

		virtual void BoardOnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		virtual void BoardOnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void BoardOrientHandler(Object^ sender, SizeChangedEventArgs^ sce);
	};
}
