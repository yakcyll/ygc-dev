//
// EditorPage.xaml.cpp
// Implementation of the EditorPage class
//

#include "pch.h"
#include "EditorPage.xaml.h"

using namespace ygc;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=390556

EditorPage::EditorPage()
{
	InitializeComponent();

	bp = ref new BoardPage(this);

	SizeChanged += ref new SizeChangedEventHandler(this, &EditorPage::OrientHandler);
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.
/// This parameter is typically used to configure the page.</param>
void EditorPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter

	bp->BoardOnNavigatedTo(e);

}

void EditorPage::OrientHandler(Object ^ s, SizeChangedEventArgs ^ sce)
{
	bp->BoardOrientHandler(s, sce);
}