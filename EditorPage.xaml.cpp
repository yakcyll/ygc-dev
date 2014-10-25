//
// EditorPage.xaml.cpp
// Implementation of the EditorPage class
// TODO: Make it player-number agnostic
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

EditorPage::EditorPage() : historyModeEnabled(false), moveId(0), noPlayers(2), mixedStonesEnabled(true), turn(ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK))
{
	InitializeComponent();

	bp = ref new BoardPage(this);
	LayoutRoot = (Canvas ^)this->Content;

	scoreTBs = ref new Array<TextBlock^>(noPlayers);
	playerScores = ref new Array<uint16_t>(noPlayers);

	InitBoard();
	InitPanels();
	InitInputHandler();

	SizeChanged += ref new SizeChangedEventHandler(this, &EditorPage::OrientHandler);
}

void EditorPage::InitBoard()
{
	bp->currentMatch = ref new Go19::Go19Match();

	for (auto i = 0; i < bp->currentMatch->board->sBoardWidth; ++i) {
		for (auto j = 0; j < bp->currentMatch->board->sBoardHeight; ++j) {

			ygcField^ field = ref new ygcField();
			field->takenBy = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY);
			field->x = i;
			field->y = j;

			bp->currentMatch->board->GetAt(i, j) = field;
		}
	}

	bp->InitUI();
}

void EditorPage::InitPanels()
{
	CounterPanel = ref new StackPanel();
	CPCont = ref new StackPanel();
	TPMode = ref new StackPanel();
	TPHistory = ref new StackPanel();
	ToolPanel = ref new Grid();
	CounterPanelBorder = ref new Border();
	TPModeBorder = ref new Border();
	TPHistoryBorder = ref new Border();

	bp->LayoutRoot->Children->Append(CounterPanel);
	bp->LayoutRoot->SetTop(CounterPanel, 0.0);
	bp->LayoutRoot->SetLeft(CounterPanel, 0.0);
	bp->LayoutRoot->SetZIndex(CounterPanel, 2);

	CounterPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

	SolidColorBrush ^ scb = ref new SolidColorBrush(Windows::UI::Colors::Black);
	scb->Opacity = 0.5;
	CounterPanel->Background = scb;
	ToolPanel->Background = scb;

	CounterPanel->Children->Append(CounterPanelBorder);

	CounterPanelBorder->Padding = 10.0;

	CounterPanelBorder->Child = CPCont;


	//init content
	for (auto i = 0; i < noPlayers; ++i) {
		Shapes::Ellipse^ e1 = ref new Shapes::Ellipse();
		e1->Width = 30.0;
		e1->Height = 30.0;
		e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[i+1]);
		e1->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
		e1->StrokeThickness = 2.0f;
		e1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

		TextBlock^ s1 = ref new TextBlock();
		s1->Name = "Player" + (i + 1).ToString() + "Score";
		s1->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
		s1->Text = "0";
		s1->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
		s1->FontSize = 20.0;
		s1->FontWeight = Windows::UI::Text::FontWeights::Bold;
		s1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
		s1->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
		s1->Margin = Thickness(10.0);

		scoreTBs[i] = s1;
		CPCont->Children->Append(e1);
		CPCont->Children->Append(s1);
	}
}

void EditorPage::InitInputHandler()
{
	bp->boardGrid->Tapped += ref new TappedEventHandler([this](Object^ s, RoutedEventArgs^ e){
		
		Canvas^ c = (Canvas^)s;
		TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;
		Point tapPoint = te->GetPosition(c);

		ygcBoard^ board = this->bp->currentMatch->board;
		float SideMargin = float(c->Width > c->Height ? c->Width / float(board->sBoardWidth + 1) : c->Height / float(board->sBoardHeight + 1));

		if (tapPoint.X < SideMargin / 2.0f || tapPoint.X > c->Width - SideMargin / 2.0f ||
			tapPoint.Y < SideMargin / 2.0f || tapPoint.Y > c->Height - SideMargin / 2.0f)
			return;

		uint16_t x = uint16_t((tapPoint.X - SideMargin / 2.0f) / SideMargin);
		uint16_t y = uint16_t((tapPoint.Y - SideMargin / 2.0f) / SideMargin);

		if (*board->GetAt(x,y)->takenBy == 0) {
			Shapes::Ellipse^ stone = ref new Shapes::Ellipse();
			stone->Name = "Stone" + x.ToString() + "x" + y.ToString();
			stone->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[turn]);
			stone->Width = 0.9 * bp->SideMargin;
			stone->Height = stone->Width;
			stone->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
			stone->StrokeThickness = 2.0f;

			bp->boardGrid->Children->Append(stone);
			bp->boardGrid->SetTop(stone, ::floor((te->GetPosition(c).Y - bp->SideMargin / 2.0f) / bp->SideMargin) * bp->SideMargin + bp->SideMargin / 2.0f);
			bp->boardGrid->SetLeft(stone, ::floor((te->GetPosition(c).X - bp->SideMargin / 2.0f) / bp->SideMargin) * bp->SideMargin + bp->SideMargin / 2.0f);

			*board->GetAt(x, y)->takenBy = *turn;

			if (historyModeEnabled) {
				ygcMove ^ move = ref new ygcMove();
				move->stonesChanged.Append(ref new ygcStoneChange(this->turn, ygcStoneStatus::ADDED, x, y));
				bp->currentMatch->moveHistory->Append(move);
			}

			++playerScores[(int)turn - 1];
			scoreTBs[(int)turn - 1]->Text = playerScores[(int)turn - 1].ToString();

			if (historyModeEnabled || mixedStonesEnabled)
				turn->increment();

			moveId++;

		}
		else {
			if (historyModeEnabled)
				return;

			Shapes::Ellipse^ stone;
			uint32_t index;
			for (auto i = bp->boardGrid->Children->First(); i->HasCurrent; i->MoveNext()) {
				UIElement ^ child = i->Current;
				try {
					Shapes::Ellipse^ ell = dynamic_cast<Shapes::Ellipse^>(child);
					if (ell == nullptr) continue;
					if (ell->Name == "Stone" + x.ToString() + "x" + y.ToString()) {
						bp->boardGrid->Children->IndexOf(ell, &index);
						bp->boardGrid->Children->RemoveAt(index);
						uint16_t index = (int)*bp->currentMatch->board->GetAt(x, y)->takenBy - 1;
						--playerScores[index];
						scoreTBs[index]->Text = playerScores[index].ToString();
						*bp->currentMatch->board->GetAt(x, y)->takenBy = 0;
						break;
					}
				}
				catch (Exception^ ex) { /* ... */ }
			}
		}
	});
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

	if (Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->Orientation == Windows::UI::ViewManagement::ApplicationViewOrientation::Portrait) {
		CounterPanel->Orientation = Orientation::Horizontal;
		CPCont->Orientation = Orientation::Horizontal;

		CounterPanel->Height = bp->PanelWidth;
		CounterPanel->Width = Window::Current->Bounds.Width;
	}
	else {
		CounterPanel->Orientation = Orientation::Vertical;
		CPCont->Orientation = Orientation::Vertical;

		CounterPanel->Height = Window::Current->Bounds.Height;
		CounterPanel->Width = bp->PanelWidth;
	}
}