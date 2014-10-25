//
// BoardPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "BoardPage.h"

using namespace ygc;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Devices::Sensors;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;

BoardPage::BoardPage(Page^ rootPage)
{
	LayoutRoot = ref new Canvas();
	rootPage->Content = LayoutRoot;
	
	AppSpaceWidth = 0.0;
	AppSpaceHeight = 0.0;
}

void BoardPage::InitUI()
{
	AppSpaceWidth = Window::Current->Bounds.Width;
	AppSpaceHeight = Window::Current->Bounds.Height - 32 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
	PanelWidth = 60.0;

	SideMargin = AppSpaceWidth / (currentMatch->board->sBoardWidth + 1); // TODO: Can we have it bound to height in a nicer fashion?

	InitBoardGrid();
	InitScrollViewer();
	DrawBoardGrid();
}

void BoardPage::InitBoardGrid()
{
	//init board
	boardGrid = ref new Canvas();

	boardGrid->Background = ref new SolidColorBrush(Windows::UI::Colors::DarkGreen);
	boardGrid->RenderTransform = ref new RotateTransform();
	boardGrid->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	boardGrid->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	boardGrid->Margin = Thickness(0);
}

void BoardPage::InitScrollViewer()
{
	//init view
	ScrollBoardView = ref new ScrollViewer();
	
	ScrollBoardView->ZoomMode = Windows::UI::Xaml::Controls::ZoomMode::Enabled;
	ScrollBoardView->HorizontalScrollBarVisibility = Windows::UI::Xaml::Controls::ScrollBarVisibility::Hidden;
	ScrollBoardView->VerticalScrollBarVisibility = Windows::UI::Xaml::Controls::ScrollBarVisibility::Hidden;
	ScrollBoardView->HorizontalContentAlignment = Windows::UI::Xaml::HorizontalAlignment::Center; 
	ScrollBoardView->VerticalContentAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	ScrollBoardView->MinZoomFactor = 1.0;
	ScrollBoardView->MaxZoomFactor = 3.0;
	ScrollBoardView->Background = ref new SolidColorBrush(Windows::UI::Colors::DarkOrange);

	ScrollBoardView->Content = boardGrid;
	ScrollBoardView->Width = Window::Current->Bounds.Width;
	ScrollBoardView->Height = Window::Current->Bounds.Height;
	LayoutRoot->SetZIndex(ScrollBoardView, 1);

	ScrollBoardView->RenderTransform = ref new RotateTransform();
	((RotateTransform^)ScrollBoardView->RenderTransform)->CenterX = ScrollBoardView->Width / 2.0f;
	((RotateTransform^)ScrollBoardView->RenderTransform)->CenterY = ScrollBoardView->Height / 2.0f;

	ScrollBoardView->SizeChanged += ref new SizeChangedEventHandler(this, &BoardPage::UpdateScrollBoardViewScroll);

	LayoutRoot->Children->Append(ScrollBoardView);
}

void BoardPage::UpdateScrollBoardViewScroll(Object^ s, SizeChangedEventArgs^ e)
{
	double sw = ScrollBoardView->ScrollableWidth;
	double sh = ScrollBoardView->ScrollableHeight;
	ScrollBoardView->ChangeView(sw / 2.0, sh / 2.0, 1.0f);
}

void BoardPage::DrawBoardGrid()
{
	uint16_t bgWidth = currentMatch->board->sBoardWidth;
	uint16_t bgHeight = currentMatch->board->sBoardHeight;

	for (auto i = 0; i < bgWidth; ++i) {
		Shapes::Line^ vertLine = ref new Shapes::Line();
		vertLine->Stroke = ref new SolidColorBrush(defaultAppSettings::defaultGridColor);

		vertLine->X1 = (1 + i) * SideMargin;
		vertLine->X2 = vertLine->X1;
		vertLine->Y1 = SideMargin;
		vertLine->Y2 = SideMargin * bgHeight;

		boardGrid->Children->Append(vertLine);
	}
	for (auto j = 0; j < bgHeight; ++j) {
		Shapes::Line^ horiLine = ref new Shapes::Line();
		horiLine->Stroke = ref new SolidColorBrush(defaultAppSettings::defaultGridColor);

		horiLine->X1 = SideMargin;
		horiLine->X2 = SideMargin * bgWidth;
		horiLine->Y1 = (1 + j) * SideMargin;
		horiLine->Y2 = horiLine->Y1;

		boardGrid->Children->Append(horiLine);
	}
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void BoardPage::BoardOnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter

	/*switch (Settings.OrientationLock)
	{
	case OrientationLock.Rotate:
		this->SupportedOrientations = SupportedPageOrientation.PortraitOrLandscape;
		AppSpaceWidth = Window::Current->Bounds.Width;
		AppSpaceHeight = Window::Current->Bounds.Height - 32;
		break;
	case OrientationLock.HorizontalLock:
		this.SupportedOrientations = SupportedPageOrientation.Landscape;
		break;
	case OrientationLock.VerticalLock:
		this.SupportedOrientations = SupportedPageOrientation.Portrait;
		AppSpaceWidth = Window::Current->Bounds.Width;
		AppSpaceHeight = Window::Current->Bounds.Height - 32;
		break;
	}*/

	AppSpaceWidth = Window::Current->Bounds.Width;
	AppSpaceHeight = Window::Current->Bounds.Height - 32.0 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;

	//Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->HideAsync();

	boardGrid->Width = AppSpaceWidth;
	boardGrid->Height = AppSpaceWidth * (currentMatch->board->sBoardHeight + 1) / double(currentMatch->board->sBoardWidth + 1);

	((RotateTransform^)boardGrid->RenderTransform)->CenterX = boardGrid->Width / 2.0f;
	((RotateTransform^)boardGrid->RenderTransform)->CenterY = boardGrid->Height / 2.0f;

	boardGrid->Background = ref new SolidColorBrush(Windows::UI::Colors::DarkBlue);

	// TODO: If your application contains multiple pages, ensure that you are
	// handling the hardware Back button by registering for the
	// Windows::Phone::UI::Input::HardwareButtons.BackPressed event.
	// If you are using the NavigationHelper provided by some templates,
	// this event is handled for you.
}

void BoardPage::BoardOnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	//Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->ShowAsync();
}

void BoardPage::BoardOrientHandler(Object^ sender, SizeChangedEventArgs^ sce)
{
	boardGrid->Background = ref new SolidColorBrush(Windows::UI::Colors::DimGray);

	if (Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->Orientation == Windows::UI::ViewManagement::ApplicationViewOrientation::Portrait) {
		AppSpaceWidth = Window::Current->Bounds.Width;
		AppSpaceHeight = Window::Current->Bounds.Height - 32 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
		SideMargin = AppSpaceWidth / (currentMatch->board->sBoardWidth + 1); // TODO: Can we bound it to height in a nice fashion?

		((RotateTransform^)boardGrid->RenderTransform)->Angle = 0.0f;
		
	} else {
		AppSpaceWidth = Window::Current->Bounds.Width - 72 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
		AppSpaceHeight = Window::Current->Bounds.Height;
		SideMargin = AppSpaceHeight / (currentMatch->board->sBoardWidth + 1); // TODO: Can we bound it to height in a nice fashion?

		((RotateTransform^)boardGrid->RenderTransform)->Angle = 270.0f;
	}
	ScrollBoardView->Height = AppSpaceHeight;
	ScrollBoardView->Width = AppSpaceWidth;
}