//
// MatchPage.xaml.h
// Declaration of the MatchPage class
//

#pragma once

#include "BoardPage.h"
#include "MatchPage.g.h"

using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;

namespace ygc
{
	public ref class MatchPage sealed 
	{
	internal:
		StackPanel ^ TopPanel, ^ BottomPanel, ^ OppPanel, ^ PlayerPanel, ^ PassPanel;
		Grid ^ PlayerPanelCont, ^ OppPanelCont, ^ TurnIndCont, ^ PassCont;
		Border ^ OppPanelBorder, ^ PlayerPanelBorder, ^ TurnIndBorder;
		Canvas ^ LayoutRoot;

		Image ^ turnIndicator;
		Array<TextBlock^>^ scoreTBs;

		BoardPage ^ bp;

		void InitMatch();
		void InitScorePanels();
		void InitHandlers();

		void UpdateIcons();

	public:
		MatchPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ sender, SizeChangedEventArgs ^ sce);
	};
}
