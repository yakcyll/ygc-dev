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
		BoardPage ^ bp;
	public:
		EditorPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		void OrientHandler(Object ^ s, SizeChangedEventArgs ^ sce);
	};
}
