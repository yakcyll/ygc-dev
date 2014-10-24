//
// BoardPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "BoardPage.g.h"

using namespace Windows::Foundation;
using namespace Windows::Devices::Sensors;

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class BoardPage sealed
	{
	private:
		static const double ScorePanelWidth;
		double sideMargin;
		static double AppSpaceWidth;
		static double AppSpaceHeight;
	internal:
		Canvas^ boardGrid;
		ygcMatch^ currentMatch;

		SimpleOrientationSensor^ sos;

		void InitMatch();
		void InitUI();
		void InitBoardGrid();
		void InitScrollViewer();
		void InitScorePanels();
		void DrawBoardGrid();
		void InitHandlers();

		void UpdateScrollBoardViewScroll(Object^, SizeChangedEventArgs^);

	public:
		BoardPage();
		void debug(Canvas^ c, Point tapPoint);

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object^ sender, SizeChangedEventArgs^ sce);
	};
}
