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
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=390556

EditorPage::EditorPage() : historyModeEnabled(false), moveId(0), noPlayers(2), mixedStonesEnabled(true), 
						   turn(ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK)), historyTurn(ref new Go19::Go19StoneColor())
{
	InitializeComponent();

	bp = ref new BoardPage(this);
	LayoutRoot = (Canvas ^)this->Content;

	scoreTBs = ref new Array<TextBlock^>(noPlayers);
	playerScores = ref new Array<uint16_t>(noPlayers);

	InitBoard();
	InitPanels();
	InitInputHandler();

	UpdateIcons();

	SizeChanged += ref new SizeChangedEventHandler(this, &EditorPage::OrientHandler);
}

void EditorPage::InitBoard()
{
	bp->currentMatch = ref new Go19::Go19Match(13, 13);

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
	ToolPanel = ref new StackPanel();
	CPInd = ref new StackPanel();
	CPCount = ref new StackPanel();
	TPMode = ref new StackPanel();
	TPHistory = ref new StackPanel();
	CPIndCont = ref new Grid();
	TPModeCont = ref new Grid();
	TPHistoryCont = ref new Grid();
	CPCountBorder = ref new Border();
	CPIndBorder = ref new Border();
	TPModeBorder = ref new Border();
	TPHistoryBorder = ref new Border();


	// top panel
	bp->LayoutRoot->Children->Append(CounterPanel);
	bp->LayoutRoot->SetTop(CounterPanel, 0.0);
	bp->LayoutRoot->SetLeft(CounterPanel, 0.0);
	bp->LayoutRoot->SetZIndex(CounterPanel, 2);

	CounterPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

	SolidColorBrush ^ scb = ref new SolidColorBrush(Windows::UI::Colors::Black);
	scb->Opacity = 0.5;
	CounterPanel->Background = scb;

	CounterPanel->Children->Append(CPCountBorder);
	CPCountBorder->Padding = 10.0;
	CPCountBorder->Child = CPCount;

	CounterPanel->Children->Append(CPIndCont);
	CPIndCont->RowDefinitions->Append(ref new RowDefinition());
	CPIndCont->Children->Append(CPIndBorder);
	CPIndCont->SetRow(CPIndBorder, 0);
	CPIndCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);
	CPIndBorder->Padding = 15.0;
	CPIndBorder->Child = CPInd;
	CPInd->Background = ref new SolidColorBrush(Windows::UI::Colors::Black);
	CPInd->Background->Opacity = 0.5;

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
		CPCount->Children->Append(e1);
		CPCount->Children->Append(s1);
	}

	turnIndicator = ref new Image();
	turnIndicator->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[turn]));
	turnIndicator->Width = 30.0;
	turnIndicator->Height = 30.0;

	CPInd->Children->Append(turnIndicator);


	//bottom panel
	bp->LayoutRoot->Children->Append(ToolPanel);
	bp->LayoutRoot->SetZIndex(ToolPanel, 2);

	ToolPanel->Background = scb;
	ToolPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

	ToolPanel->Children->Append(TPHistoryCont);
	TPHistoryCont->RowDefinitions->Append(ref new RowDefinition());
	TPHistoryCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);
	TPHistoryCont->Children->Append(TPHistory);
	TPHistoryCont->SetRow(TPHistory, 0);
	TPHistory->FlowDirection = Windows::UI::Xaml::FlowDirection::LeftToRight;

	ToolPanel->Children->Append(TPModeCont);
	TPModeCont->RowDefinitions->Append(ref new RowDefinition());
	TPModeCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);
	TPModeCont->Children->Append(TPMode);
	TPModeCont->SetRow(TPMode, 0);
	TPMode->FlowDirection = Windows::UI::Xaml::FlowDirection::LeftToRight;

	//init content
	stoneBrush = ref new Image();
	stoneBrush->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[0]));
	stoneBrush->Width = bp->PanelWidth;
	stoneBrush->Height = bp->PanelWidth;

	stoneBrush->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e){
		if (historyModeEnabled)
			return;
		if (mixedStonesEnabled)
			mixedStonesEnabled = false;
		else {
			turn->increment();
			if (*turn == 1)
				mixedStonesEnabled = true;
		}
		UpdateIcons();
	});

	editMode = ref new Image();
	editMode->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultModeIcons[0]));
	editMode->Width = bp->PanelWidth;
	editMode->Height = bp->PanelWidth;

	editMode->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e){
		if (historyModeEnabled) {
			RewindHistory();
			*turn = *historyTurn;
		}
		else
			*historyTurn = *turn;

		historyModeEnabled = !historyModeEnabled;
		UpdateIcons();
	});

	clearBoard = ref new Image();
	clearBoard->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultAdditionalIcons[0]));
	clearBoard->Width = bp->PanelWidth;
	clearBoard->Height = bp->PanelWidth;

	clearBoard->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e){
		if (historyModeEnabled)
			return;

		bp->ClearBoard();
		for (auto i = 0; i < noPlayers; ++i) {
			playerScores[i] = 0;
			scoreTBs[i]->Text = playerScores[i].ToString();
		}
		*turn = 1;
		UpdateIcons();
	});

	TPMode->Children->Append(stoneBrush);
	TPMode->Children->Append(editMode);
	TPMode->Children->Append(clearBoard);

	hRewind = ref new Image();
	hRewind->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[0]));
	hRewind->Width = bp->PanelWidth;
	hRewind->Height = bp->PanelWidth;

	hRewind->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ te){
		if (historyModeEnabled && moveId != 0) {
			RewindHistory();
			*turn = *historyTurn;
			UpdateIcons();
		}
	});

	hPrev = ref new Image();
	hPrev->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[1]));
	hPrev->Width = bp->PanelWidth;
	hPrev->Height = bp->PanelWidth;

	hPrev->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ te){
		if (!historyModeEnabled || moveId == 0)
			return;

		--moveId;
		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, false);

		*turn = turn->previous();
		UpdateIcons();
	});
	
	hNext = ref new Image();
	hNext->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[2]));
	hNext->Width = bp->PanelWidth;
	hNext->Height = bp->PanelWidth;

	hNext->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e){
		if (!historyModeEnabled || moveId == bp->currentMatch->moveHistory->Size)
			return;

		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, true);
		++moveId;

		turn->increment();
		UpdateIcons();
	});

	TPHistory->Children->Append(hRewind);
	TPHistory->Children->Append(hPrev);
	TPHistory->Children->Append(hNext);
}

void EditorPage::InitInputHandler()
{
	bp->boardGrid->Tapped += ref new TappedEventHandler([this](Object^ s, RoutedEventArgs^ e){
		
		Canvas^ c = (Canvas^)s;
		TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;
		Point tapPoint = te->GetPosition(c);

		ygcBoard^ board = bp->currentMatch->board;
		float SideMargin = float(c->Width > c->Height ? c->Width / float(board->sBoardWidth + 1) : c->Height / float(board->sBoardHeight + 1));

		if (tapPoint.X < SideMargin / 2.0f || tapPoint.X > c->Width - SideMargin / 2.0f ||
			tapPoint.Y < SideMargin / 2.0f || tapPoint.Y > c->Height - SideMargin / 2.0f)
			return;

		uint16_t x = uint16_t(::round(tapPoint.X / SideMargin) - 1.0);
		uint16_t y = uint16_t(::round(tapPoint.Y / SideMargin) - 1.0);

		if (*board->GetAt(x,y)->takenBy == 0) {

			bp->AddStone(turn, x, y);

			if (historyModeEnabled) {
				if (bp->currentMatch->moveHistory->Size != moveId) {
					while (bp->currentMatch->moveHistory->Size != moveId)
						bp->currentMatch->moveHistory->RemoveAtEnd();
					UpdateIcons();
				}

				hRewind->Opacity = 1.0;
				hPrev->Opacity = 1.0;

				ygcMove ^ move = ref new ygcMove();
				move->stonesChanged.Append(ref new ygcStoneChange(ref new ygcStoneColor(*turn), ygcStoneStatus::ADDED, x, y));
				bp->currentMatch->moveHistory->Append(move);
				++moveId;
				
			}

			uint16_t playerIndex = (int)turn - 1;
			++playerScores[playerIndex];
			scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();

			if (historyModeEnabled || mixedStonesEnabled) {
				turn->increment();
				turnIndicator->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[turn]));
			}

		}
		else {
			if (historyModeEnabled)
				return;

			uint16_t playerIndex = (int)*bp->currentMatch->board->GetAt(x, y)->takenBy - 1;

			bp->RemoveStone(x, y);

			--playerScores[playerIndex];
			scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();
		}
	});
}

void EditorPage::UpdateIcons()
{
	turnIndicator->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[turn]));
	editMode->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultModeIcons[(int)historyModeEnabled]));

	if (historyModeEnabled) {
		stoneBrush->Opacity = 0.5;
		clearBoard->Opacity = 0.5;

		//silly looking bit, but has to cover 0 == bp->currentMatch->moveHistory->Size
		if (moveId == bp->currentMatch->moveHistory->Size)
			hNext->Opacity = 0.5;
		else
			hNext->Opacity = 1.0;
		if (moveId == 0) {
			hRewind->Opacity = 0.5;
			hPrev->Opacity = 0.5;
		}
		else {
			hRewind->Opacity = 1.0;
			hPrev->Opacity = 1.0;
		}

	}
	else {
		stoneBrush->Opacity = 1.0;
		clearBoard->Opacity = 1.0;
		hRewind->Opacity = 0.5;
		hNext->Opacity = 0.5;
		hPrev->Opacity = 0.5;
	}

	if (mixedStonesEnabled)
		stoneBrush->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[0]));
	else
		stoneBrush->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[turn]));
	
}

void EditorPage::UpdateBoard(ygcMove^ lastMove, bool forward)
{
	// O(n*m)
	// TODO: find a less complex alternative
	for (auto i = lastMove->stonesChanged.First(); i->HasCurrent; i->MoveNext()) {
		uint16_t playerIndex = (int)*i->Current->whose - 1;

		if ((i->Current->status == ygcStoneStatus::ADDED && !forward) || (i->Current->status == ygcStoneStatus::FALLEN && forward)) {
			//remove
			bp->RemoveStone(i->Current->x, i->Current->y);
			--playerScores[playerIndex];
		}
		else if ((i->Current->status == ygcStoneStatus::ADDED && forward) || (i->Current->status == ygcStoneStatus::FALLEN && !forward)) {
			// add
			bp->AddStone(i->Current->whose, i->Current->x, i->Current->y);
			++playerScores[playerIndex];
		}
		scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();
	}
}

void EditorPage::RewindHistory()
{
	while (moveId) {
		--moveId;
		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, false);
	}

	bp->currentMatch->moveHistory->Clear();
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
		CPCount->Orientation = Orientation::Horizontal;

		CounterPanel->Height = bp->PanelWidth;
		CounterPanel->Width = bp->AppSpaceWidth;
		CPCountBorder->Width = (bp->AppSpaceWidth / 2);
		CPCountBorder->Height = bp->PanelWidth;
		CPIndCont->Width = (bp->AppSpaceWidth / 2);
		CPIndCont->Height = bp->PanelWidth;
		CPInd->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
		CPInd->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;

		ToolPanel->Height = bp->PanelWidth;
		ToolPanel->Width = bp->AppSpaceWidth;
		ToolPanel->Orientation = Orientation::Horizontal;

		bp->LayoutRoot->SetTop(ToolPanel, bp->AppSpaceHeight - bp->PanelWidth);
		bp->LayoutRoot->SetLeft(ToolPanel, 0.0);

		TPMode->Orientation = Orientation::Horizontal;
		TPHistory->Orientation = Orientation::Horizontal;
		TPModeCont->Width = (bp->AppSpaceWidth / 2);
		TPModeCont->Height = bp->PanelWidth;
		TPHistoryCont->Width = (bp->AppSpaceWidth / 2);
		TPHistoryCont->Height = bp->PanelWidth;

		TPMode->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
		TPMode->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Right;
		TPHistory->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
		TPHistory->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
	}
	else {
		CounterPanel->Orientation = Orientation::Vertical;
		CPCount->Orientation = Orientation::Vertical;

		CounterPanel->Height = bp->AppSpaceHeight;
		CounterPanel->Width = bp->PanelWidth;
		CPCountBorder->Width = bp->PanelWidth;
		CPCountBorder->Height = (bp->AppSpaceHeight / 2);
		CPIndCont->Width = bp->PanelWidth;
		CPIndCont->Height = (bp->AppSpaceHeight / 2); 
		CPInd->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Bottom;
		CPInd->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;

		ToolPanel->Height = bp->AppSpaceHeight;
		ToolPanel->Width = bp->PanelWidth;
		ToolPanel->Orientation = Orientation::Vertical;

		bp->LayoutRoot->SetTop(ToolPanel, 0.0);
		bp->LayoutRoot->SetLeft(ToolPanel, bp->AppSpaceWidth - bp->PanelWidth);

		TPMode->Orientation = Orientation::Vertical;
		TPHistory->Orientation = Orientation::Vertical;
		TPModeCont->Width = bp->PanelWidth;
		TPModeCont->Height = (bp->AppSpaceHeight / 2);
		TPHistoryCont->Width = bp->PanelWidth;
		TPHistoryCont->Height = (bp->AppSpaceHeight / 2);

		TPMode->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Bottom;
		TPMode->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
		TPHistory->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Top;
		TPHistory->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	}
}