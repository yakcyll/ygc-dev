//
// BoardPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "BoardPage.xaml.h"

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

const double BoardPage::ScorePanelWidth = 60.0;
double BoardPage::AppSpaceWidth = 0;
double BoardPage::AppSpaceHeight = 0;

BoardPage::BoardPage()
{
	InitializeComponent();
	InitMatch();
	InitUI();
	DrawBoardGrid();
	InitHandlers();

	SizeChanged += ref new SizeChangedEventHandler(this, &BoardPage::OrientHandler);
}

void BoardPage::InitMatch()
{
	currentMatch = ref new Go19::Go19Match();

	currentMatch->players->Append(ref new ygcPlayer(currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK), ygcPlayerInputType::SCREEN, "Player1"));
	currentMatch->players->Append(ref new ygcPlayer(currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::WHITE), ygcPlayerInputType::SCREEN, "Player2"));

	for (auto i = 0; i < currentMatch->board->sBoardWidth; ++i) {
		for (auto j = 0; j < currentMatch->board->sBoardHeight; ++j) {

			ygcField^ field = ref new ygcField();
			field->takenBy = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY);
			field->x = i;
			field->y = j;

			currentMatch->board->getAt(i, j) = field;
		}
	}

}

void BoardPage::InitUI()
{
	AppSpaceWidth = Window::Current->Bounds.Width;
	AppSpaceHeight = Window::Current->Bounds.Height - 32 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;

	sideMargin = AppSpaceWidth / (currentMatch->board->sBoardWidth + 1); // TODO: Can we bound it to height in a nice fashion?

	InitScorePanels();
	InitBoardGrid();
	InitScrollViewer();
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
	ScrollBoardView->Content = boardGrid;
	ScrollBoardView->Width = Window::Current->Bounds.Width;
	ScrollBoardView->Height = Window::Current->Bounds.Height;
	LayoutRoot->SetZIndex(ScrollBoardView, 1);

	ScrollBoardView->RenderTransform = ref new RotateTransform();
	((RotateTransform^)ScrollBoardView->RenderTransform)->CenterX = ScrollBoardView->Width / 2.0f;
	((RotateTransform^)ScrollBoardView->RenderTransform)->CenterY = ScrollBoardView->Height / 2.0f;

	ScrollBoardView->SizeChanged += ref new SizeChangedEventHandler(this, &BoardPage::UpdateScrollBoardViewScroll);
}

void BoardPage::UpdateScrollBoardViewScroll(Object^ s, SizeChangedEventArgs^ e)
{
	double sw = ScrollBoardView->ScrollableWidth;
	double sh = ScrollBoardView->ScrollableHeight;
	ScrollBoardView->ChangeView(sw / 2.0, sh / 2.0, 1.0f);
}

void BoardPage::InitScorePanels()
{
	//init score panels
	Shapes::Ellipse^ e1 = ref new Shapes::Ellipse();
	e1->Width = 30.0;
	e1->Height = 30.0;
	e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[currentMatch->players->GetAt(0)->color]);
	e1->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
	e1->StrokeThickness = 2.0f;
	e1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

	TextBlock^ s1 = ref new TextBlock();
	s1->Name = "Player1Score";
	s1->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
	s1->Text = "4";
	s1->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
	s1->FontSize = 20.0;
	s1->FontWeight = Windows::UI::Text::FontWeights::Bold;
	s1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	s1->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	s1->Margin = Thickness(10);

	PlayerPanelCont->Children->Append(s1);
	PlayerPanelCont->Children->Append(e1);

	Shapes::Ellipse^ e2 = ref new Shapes::Ellipse();
	e2->Width = 30.0;
	e2->Height = 30.0;
	e2->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[currentMatch->players->GetAt(1)->color]);
	e2->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
	e2->StrokeThickness = 2.0f;
	e2->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

	TextBlock^ s2 = ref new TextBlock();
	s2->Name = "Player2Score";
	s2->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
	s2->Text = "4";
	s2->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
	s2->FontSize = 20.0;
	s2->FontWeight = Windows::UI::Text::FontWeights::Bold;
	s2->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	s2->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	s2->Margin = Thickness(10);

	OppPanelCont->Children->Append(e2);
	OppPanelCont->Children->Append(s2);

	for (unsigned int i = 2; i < currentMatch->players->Size; ++i) {
		Border^ b1 = ref new Border(); // TODO: Test?
		b1->Width = 2.0;
		b1->BorderThickness = Thickness(1);
		b1->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::DimGray);
		b1->Background = ref new SolidColorBrush(Windows::UI::Colors::DimGray);

		e1 = ref new Shapes::Ellipse();
		e1->Width = 30.0;
		e1->Height = 30.0;
		e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[currentMatch->players->GetAt(i)->color]);
		e1->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
		e1->StrokeThickness = 2.0f;

		s1 = ref new TextBlock();
		s1->Name = "Player" + (i + 1).ToString() + "Score";
		s1->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
		s1->Text = "0";
		s1->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
		s1->FontWeight = Windows::UI::Text::FontWeights::Bold;
		s1->FontSize = 20.0;
		s1->Margin = Thickness(10);

		OppPanelCont->Children->Append(b1);
		OppPanelCont->Children->Append(e1);
		OppPanelCont->Children->Append(s1);
	}
}

void BoardPage::DrawBoardGrid()
{
	uint16_t bgWidth = currentMatch->board->sBoardWidth;
	uint16_t bgHeight = currentMatch->board->sBoardHeight;

	for (auto i = 0; i < bgWidth; ++i) {
		Shapes::Line^ vertLine = ref new Shapes::Line();
		vertLine->Stroke = ref new SolidColorBrush(defaultAppSettings::defaultGridColor);

		vertLine->X1 = (1 + i) * sideMargin;
		vertLine->X2 = vertLine->X1;
		vertLine->Y1 = sideMargin;
		vertLine->Y2 = sideMargin * bgHeight;

		boardGrid->Children->Append(vertLine);
	}
	for (auto j = 0; j < bgHeight; ++j) {
		Shapes::Line^ horiLine = ref new Shapes::Line();
		horiLine->Stroke = ref new SolidColorBrush(defaultAppSettings::defaultGridColor);

		horiLine->X1 = sideMargin;
		horiLine->X2 = sideMargin * bgWidth;
		horiLine->Y1 = (1 + j) * sideMargin;
		horiLine->Y2 = horiLine->Y1;

		boardGrid->Children->Append(horiLine);
	}
}

void BoardPage::InitHandlers()
{
	for (unsigned int k = 0; k < currentMatch->players->Size; ++k) {
		//if (currentMatch->players->GetAt(k)->inputHandler->ypiType == ygcPlayerInputType::SCREEN) {
			ygcPlayer^ pp = this->currentMatch->players->GetAt(k);
			boardGrid->Tapped += ref new TappedEventHandler([this, pp](Object^ s, RoutedEventArgs^ e){

				this->debug((Canvas^)s, ((TappedRoutedEventArgs^)e)->GetPosition((Canvas^)s));

				if (pp->inputHandler->handleInput(s, e)) {

					Canvas^ c = (Canvas^)s;
					TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;

					Shapes::Ellipse^ stone = ref new Shapes::Ellipse();
					stone->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[this->currentMatch->turn->previous()]);
					stone->Width = 0.9 * sideMargin;
					stone->Height = stone->Width;
					stone->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
					stone->StrokeThickness = 2.0f;

					boardGrid->Children->Append(stone);
					boardGrid->SetTop(stone, ::floor((te->GetPosition(c).Y - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f);
					boardGrid->SetLeft(stone, ::floor((te->GetPosition(c).X - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f);
					//boardGrid->SetZIndex(stone, 2);
				}
			});
		//}
	}
}

void BoardPage::debug(Canvas^ c, Point tapPoint) {
	static unsigned int responsive = 0;
	((TextBox^)LayoutRoot->FindName("TurnTextBox"))->Text = ActualWidth.ToString();
	((TextBox^)LayoutRoot->FindName("ResponsiveTextBox"))->Text = ActualHeight.ToString();
	((TextBox^)LayoutRoot->FindName("TapXTextBox"))->Text = (::floor((tapPoint.X - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f).ToString() + " / " + uint16_t((tapPoint.X - sideMargin / 2.0f) / sideMargin);
	((TextBox^)LayoutRoot->FindName("TapYTextBox"))->Text = (::floor((tapPoint.Y - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f).ToString() + " / " + uint16_t((tapPoint.Y - sideMargin / 2.0f) / sideMargin);
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void BoardPage::OnNavigatedTo(NavigationEventArgs^ e)
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

	LayoutRoot->SetTop(OppPanel, 0.0);
	LayoutRoot->SetLeft(OppPanel, 0.0);

	// TODO: If your application contains multiple pages, ensure that you are
	// handling the hardware Back button by registering for the
	// Windows::Phone::UI::Input::HardwareButtons.BackPressed event.
	// If you are using the NavigationHelper provided by some templates,
	// this event is handled for you.
}

void BoardPage::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	//Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->ShowAsync();
}

void BoardPage::OrientHandler(Object^ sender, SizeChangedEventArgs^ sce)
{
	boardGrid->Background = ref new SolidColorBrush(Windows::UI::Colors::DimGray);

	if (Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->Orientation == Windows::UI::ViewManagement::ApplicationViewOrientation::Portrait) {
		AppSpaceWidth = Window::Current->Bounds.Width;
		AppSpaceHeight = Window::Current->Bounds.Height - 32 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;

		((RotateTransform^)boardGrid->RenderTransform)->Angle = 0.0f;

		OppPanel->Orientation = Orientation::Horizontal;
		PlayerPanel->Orientation = Orientation::Horizontal;
		OppPanelCont->Orientation = Orientation::Horizontal;
		PlayerPanelCont->Orientation = Orientation::Horizontal;

		OppPanel->Height = ScorePanelWidth;
		OppPanel->Width = AppSpaceWidth;
		PlayerPanel->Height = ScorePanelWidth;
		PlayerPanel->Width = AppSpaceWidth;

		LayoutRoot->SetTop(PlayerPanel, AppSpaceHeight - PlayerPanel->Height);
		LayoutRoot->SetLeft(PlayerPanel, 0.0);
		
	} else {
		AppSpaceWidth = Window::Current->Bounds.Width - 72 / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
		AppSpaceHeight = Window::Current->Bounds.Height;

		((RotateTransform^)boardGrid->RenderTransform)->Angle = 270.0f;

		OppPanel->Orientation = Orientation::Vertical;
		PlayerPanel->Orientation = Orientation::Vertical;
		OppPanelCont->Orientation = Orientation::Vertical;
		PlayerPanelCont->Orientation = Orientation::Vertical;

		OppPanel->Height = AppSpaceHeight;
		OppPanel->Width = ScorePanelWidth;
		PlayerPanel->Height = AppSpaceHeight;
		PlayerPanel->Width = ScorePanelWidth;

		LayoutRoot->SetTop(PlayerPanel, 0.0);
		LayoutRoot->SetLeft(PlayerPanel, AppSpaceWidth - PlayerPanel->Width);

	}
	ScrollBoardView->Height = AppSpaceHeight;
	ScrollBoardView->Width = AppSpaceWidth;
}