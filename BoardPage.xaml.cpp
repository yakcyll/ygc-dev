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
	InitBoardGrid();
}

void BoardPage::InitMatch()
{
	currentMatch = ref new ygcMatch();

	currentMatch->players->Append(ref new ygcPlayer(ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK), ygcPlayerInputType::SCREEN, "Player1"));
	currentMatch->players->Append(ref new ygcPlayer(ref new Go19::Go19StoneColor(Go19::Go19StoneColor::WHITE), ygcPlayerInputType::SCREEN, "Player2"));

}

void BoardPage::InitBoardGrid()
{
	boardGrid = ref new Grid();

	for (auto i = 0; i < currentMatch->board->sBoardWidth; ++i) {
		boardGrid->ColumnDefinitions->Append(ref new ColumnDefinition());
		boardGrid->ColumnDefinitions->GetAt(i)->Width = ref new GridLength(1, GridUnitType::Star);
	}
	for (auto j = 0; j < currentMatch->board->sBoardHeight; ++j) {
		boardGrid->RowDefinitions->Append(ref new RowDefinition());
		boardGrid->RowDefinitions->GetAt(j)->Height = ref new GridLength(1, GridUnitType::Star);
	}
	
	for (auto i = 0; i < currentMatch->board->sBoardWidth; ++i) {
		for (auto j = 0; j < currentMatch->board->sBoardHeight; ++j) {

			ygcField^ field = ref new ygcField();
			field->takenBy = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY);
			field->x = i;
			field->y = j;

			boardGrid->Children->Append(field);
			boardGrid->SetColumn(field, i);
			boardGrid->SetRow(field, j);

			field->Tapped += ref new TappedEventHandler(
				[this, &i, &j](Object^ s, TappedRoutedEventArgs e) {
				PlayerInput::ScreenInput^ si = (PlayerInput::ScreenInput^)this->currentMatch->players->GetAt(this->currentMatch->turn->stoneColor)->inputHandler;
					si->save
				}
			);

			currentMatch->board->fields[i][j] = field;
		}
	}
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
