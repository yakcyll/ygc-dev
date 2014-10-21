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
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Media::Imaging;

BoardPage::BoardPage()
{
	InitializeComponent();
	InitMatch();
	DrawBoardGrid();
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

	InitBoardGrid();

}

void BoardPage::InitBoardGrid()
{
	boardGrid = ref new Canvas();

	if (currentMatch->board->sBoardWidth >= currentMatch->board->sBoardHeight) {
		boardGrid->Width = Window::Current->Bounds.Width;
		boardGrid->Height = Window::Current->Bounds.Width * (currentMatch->board->sBoardHeight + 1) / double(currentMatch->board->sBoardWidth + 1);
	}
	else {
		boardGrid->Width = (Window::Current->Bounds.Height - 200) * (currentMatch->board->sBoardWidth + 1) / double(currentMatch->board->sBoardHeight + 1);
		boardGrid->Height = Window::Current->Bounds.Height - 200;
	}

	boardGrid->Background = ref new SolidColorBrush(Windows::UI::Colors::DarkGreen);

	LayoutRoot->Children->Append(boardGrid);
	LayoutRoot->SetRow(boardGrid, 1);
}

void BoardPage::DrawBoardGrid()
{
	uint16_t bgWidth = currentMatch->board->sBoardWidth;
	uint16_t bgHeight = currentMatch->board->sBoardHeight;

	float sideMargin = (bgWidth >= bgHeight ? Window::Current->Bounds.Width / (bgWidth + 1) : (Window::Current->Bounds.Height - 200) / (bgHeight + 1)); // TODO: How to make this aspect-ratio-agnostic?

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

	for (unsigned int k = 0; k < currentMatch->players->Size; ++k) {
		//if (currentMatch->players->GetAt(k)->inputHandler->ypiType == ygcPlayerInputType::SCREEN)
		ygcPlayer^ pp = this->currentMatch->players->GetAt(k);
		boardGrid->Tapped += ref new TappedEventHandler([this, pp](Object^ s, RoutedEventArgs^ e){

			this->debug((Canvas^)s, ((TappedRoutedEventArgs^)e)->GetPosition((Canvas^)s));

			if (pp->inputHandler->handleInput(s, e)) {

				Canvas^ c = (Canvas^)s;
				TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;
				float sideMargin = float(c->Width > c->Height ? c->Width / float(this->currentMatch->board->sBoardWidth + 1) : c->Height / float(this->currentMatch->board->sBoardHeight + 1));

				Shapes::Ellipse^ stone = ref new Shapes::Ellipse();
				stone->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[this->currentMatch->turn->previous()]);
				stone->Width = 0.9 * sideMargin;
				stone->Height = stone->Width;

				boardGrid->Children->Append(stone);
				boardGrid->SetTop(stone, ::floorf((te->GetPosition(c).Y - sideMargin / 2.0f) / sideMargin) * sideMargin  + sideMargin / 2.0f);
				boardGrid->SetLeft(stone, ::floorf((te->GetPosition(c).X - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f);
				//boardGrid->SetZIndex(stone, 2);
			}
		});
	}
}

void BoardPage::debug(Canvas^ c, Point tapPoint) {
	static unsigned int responsive = 0;
	float sideMargin = float(c->Width > c->Height ? c->Width / float(this->currentMatch->board->sBoardWidth + 1) : c->Height / float(this->currentMatch->board->sBoardHeight + 1));
	((TextBox^)LayoutRoot->FindName("TurnTextBox"))->Text = int(this->currentMatch->turn).ToString();
	((TextBox^)LayoutRoot->FindName("ResponsiveTextBox"))->Text = (responsive++).ToString();
	((TextBox^)LayoutRoot->FindName("TapXTextBox"))->Text = (::floorf((tapPoint.X - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f).ToString() + " / " + uint16_t((tapPoint.X - sideMargin / 2.0f) / sideMargin);
	((TextBox^)LayoutRoot->FindName("TapYTextBox"))->Text = (::floorf((tapPoint.Y - sideMargin / 2.0f) / sideMargin) * sideMargin + sideMargin / 2.0f).ToString() + " / " + uint16_t((tapPoint.Y - sideMargin / 2.0f) / sideMargin);
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void BoardPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter

	// TODO: Prepare page for display here.

	// TODO: If your application contains multiple pages, ensure that you are
	// handling the hardware Back button by registering for the
	// Windows::Phone::UI::Input::HardwareButtons.BackPressed event.
	// If you are using the NavigationHelper provided by some templates,
	// this event is handled for you.
}
