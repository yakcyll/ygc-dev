//
// BoardPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "BoardPage.g.h"

using namespace Windows::Foundation;

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class BoardPage sealed
	{
	internal:
		Canvas^ boardGrid;
		ygcMatch^ currentMatch;

		void InitMatch();
		void InitBoardGrid();
		void DrawBoardGrid();

	public:
		BoardPage();
		void debug(Canvas^ c, Point tapPoint);

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	};
}
