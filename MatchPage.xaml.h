//
// MatchPage.xaml.h
// Declaration of the MatchPage class
//

#pragma once

#include "BoardPage.h"
#include "MatchPage.g.h"

namespace ygc
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MatchPage sealed 
	{
	internal:
		StackPanel ^ OppPanel, ^ PlayerPanel, ^ OppPanelCont, ^ PlayerPanelCont;
		Border ^ OppPanelBorder, ^ PlayerPanelBorder;

		BoardPage ^ bp;

	public:
		MatchPage();

		void InitScorePanels();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ sender, SizeChangedEventArgs ^ sce);
	};
}
