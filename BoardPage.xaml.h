//
// BoardPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "BoardPage.g.h"

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class BoardPage sealed
	{
	public:
		Grid^ boardGrid;
		ygcMatch^ currentMatch;

		BoardPage();

		void InitMatch();
		void InitBoardGrid();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	};
}
