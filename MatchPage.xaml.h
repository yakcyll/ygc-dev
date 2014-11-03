//
// MatchPage.xaml.h
// Declaration of the MatchPage class
//

#pragma once

#include "BoardPage.h"
#include "MatchPage.g.h"

namespace ygc
{
	public ref class MatchPage sealed 
	{
	internal:
		StackPanel ^ OppPanel, ^ PlayerPanel, ^ OppPanelCont, ^ PlayerPanelCont;
		Border ^ OppPanelBorder, ^ PlayerPanelBorder;
		Canvas ^ LayoutRoot;

		BoardPage ^ bp;

	public:
		MatchPage();

		void InitMatch();
		void InitScorePanels();
		void InitHandlers();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ sender, SizeChangedEventArgs ^ sce);
	};
}
