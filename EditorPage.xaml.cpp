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

EditorPage::EditorPage() : historyModeEnabled(false), moveId(0), checkPointId(0), noPlayers(2), mixedStonesEnabled(true), 
						   historyTurn(ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK))
{
	InitializeComponent();

	bp = ref new BoardPage(this);
	LayoutRoot = (Canvas ^)this->Content;

	scoreTBs = ref new Array<TextBlock^>(noPlayers);
	playerScores = ref new Array<uint16_t>(noPlayers);

	InitBoard();
	turn = bp->currentMatch->turn;

	InitPanels();
	InitInputHandler();

	UpdateIcons();

	SizeChanged += ref new SizeChangedEventHandler(this, &EditorPage::OrientHandler);
}

void EditorPage::InitBoard()
{
	bp->currentMatch = ref new Go19::Go19Match(13, 13); // <--

	for (uint16_t i = 0; i < bp->currentMatch->board->sBoardWidth; ++i)
		for (uint16_t j = 0; j < bp->currentMatch->board->sBoardHeight; ++j)
			bp->currentMatch->board->SetAt(Point(i, j), Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY));

	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK), ygcPlayerInputType::SCREEN, "Player1"));
	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::WHITE), ygcPlayerInputType::SCREEN, "Player2"));

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

	stoneBrush->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e) {
		if (mixedStonesEnabled) {
			mixedStonesEnabled = false;
			*turn = 1;
		}
		else {
			turn->increment();
			if (*turn == 1)
				mixedStonesEnabled = true;
		}
		UpdateIcons();
	});

	checkPoint = ref new Image();
	checkPoint->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultModeIcons[0]));
	checkPoint->Width = bp->PanelWidth;
	checkPoint->Height = bp->PanelWidth;

	checkPoint->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e) {
		if (historyModeEnabled) {
			RewindHistory(true);
			*turn = *historyTurn;
			for (auto i = 0; i < noPlayers; ++i)
				scoreTBs[i]->Text = playerScores[i].ToString();
			checkPointId = 0;
			*historyTurn = 1;
		}
		else {
			checkPointId = moveId;
			*historyTurn = *turn;
		}
			
		historyModeEnabled = !historyModeEnabled;
		UpdateIcons();
	});

	clearBoard = ref new Image();
	clearBoard->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultAdditionalIcons[0]));
	clearBoard->Width = bp->PanelWidth;
	clearBoard->Height = bp->PanelWidth;

	clearBoard->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e) {
		bp->ClearBoard();
		for (auto i = 0; i < noPlayers; ++i) {
			playerScores[i] = 0;
			scoreTBs[i]->Text = playerScores[i].ToString();
		}
		*turn = 1;
		moveId = 0;
		checkPointId = 0;
		historyModeEnabled = false;

		UpdateIcons();
	});

	TPMode->Children->Append(stoneBrush);
	TPMode->Children->Append(checkPoint);
	TPMode->Children->Append(clearBoard);

	hRewind = ref new Image();
	hRewind->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[0]));
	hRewind->Width = bp->PanelWidth;
	hRewind->Height = bp->PanelWidth;

	hRewind->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ te) {
		if (moveId > checkPointId) {
			RewindHistory();
			*turn = *historyTurn;
			UpdateIcons();
		}
	});

	hPrev = ref new Image();
	hPrev->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[1]));
	hPrev->Width = bp->PanelWidth;
	hPrev->Height = bp->PanelWidth;

	hPrev->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ te) {
		if (moveId <= checkPointId)
			return;

		--moveId;
		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, false);

		bp->currentMatch->matchMoveBack();

		if (mixedStonesEnabled && (lastMove->stonesChanged.Size != 1 || lastMove->stonesChanged.GetAt(0)->status != ygcStoneStatus::FALLEN))
			*turn = turn->previous();

		UpdateIcons();
	});
	
	hNext = ref new Image();
	hNext->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditNavIcons[2]));
	hNext->Width = bp->PanelWidth;
	hNext->Height = bp->PanelWidth;

	hNext->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ e) {
		if (moveId == bp->currentMatch->moveHistory->Size)
			return;

		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, true);
		++moveId;
		
		bp->currentMatch->matchMoveForward();

		if (mixedStonesEnabled && (lastMove->stonesChanged.Size != 1 || lastMove->stonesChanged.GetAt(0)->status != ygcStoneStatus::FALLEN))
			turn->increment();

		UpdateIcons();
	});

	TPHistory->Children->Append(hRewind);
	TPHistory->Children->Append(hPrev);
	TPHistory->Children->Append(hNext);
}

void EditorPage::InitInputHandler()
{
	bp->boardGrid->Tapped += ref new TappedEventHandler([this](Object^ s, RoutedEventArgs^ e) {
		
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
		Point coord = Point(x, y);

		if (*board->GetAt(coord) == 0) {
			ygcMove ^ move;
			uint16_t playerIndex = (int)turn - 1;
			Vector<Point>^ changedStones = nullptr;

			if (!bp->currentMatch->matchMakeMove(coord))
				return;

			bp->AddStone(coord, turn->previous());

			if (!mixedStonesEnabled)
				*turn = turn->previous();

			++playerScores[playerIndex];
			scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();

			move = bp->currentMatch->moveHistory->GetAt(bp->currentMatch->moveHistory->Size - 1);
			for (unsigned i = 0; i < move->stonesChanged.Size; ++i) {
				ygcStoneChange^ ysc = move->stonesChanged.GetAt(i);
				if (ysc->status == ygcStoneStatus::FALLEN) {
					--playerScores[(int)ysc->whose - 1];
					bp->RemoveStone(ysc->coord);
				}
			}
			
			++moveId;

			UpdateIcons();

		}
		else {
			uint16_t playerIndex = (int)*bp->currentMatch->board->GetAt(coord) - 1;
			ygcMove^ move = ref new ygcMove();
			move->stonesChanged.Append(ref new ygcStoneChange(*bp->currentMatch->board->GetAt(Point(x, y)), ygcStoneStatus::FALLEN, Point(x, y)));
			bp->currentMatch->moveHistory->Append(move);
			++bp->currentMatch->moveId;
			++moveId;

			bp->RemoveStone(coord);
			*bp->currentMatch->board->GetAt(coord) = 0;

			--playerScores[playerIndex];
			scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();
		}

		hRewind->Opacity = 1.0;
		hPrev->Opacity = 1.0;

	});
}

void EditorPage::UpdateIcons()
{
	turnIndicator->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultEditIcons[turn]));
	checkPoint->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultModeIcons[(int)historyModeEnabled]));

	if (moveId == bp->currentMatch->moveHistory->Size)
		hNext->Opacity = 0.5;
	else
		hNext->Opacity = 1.0;
	if (moveId == checkPointId) {
		hRewind->Opacity = 0.5;
		hPrev->Opacity = 0.5;
	}
	else {
		hRewind->Opacity = 1.0;
		hPrev->Opacity = 1.0;
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
		uint16_t playerIndex = (int)i->Current->whose - 1;

		if ((i->Current->status == ygcStoneStatus::ADDED && !forward) || (i->Current->status == ygcStoneStatus::FALLEN && forward)) {
			//remove
			if (bp->RemoveStone(i->Current->coord))
				--playerScores[playerIndex];
		}
		else if ((i->Current->status == ygcStoneStatus::ADDED && forward) || (i->Current->status == ygcStoneStatus::FALLEN && !forward)) {
			// add
			bp->AddStone(i->Current->coord, i->Current->whose);
			++playerScores[playerIndex];
		}
		scoreTBs[playerIndex]->Text = playerScores[playerIndex].ToString();
	}
}

void EditorPage::RewindHistory(bool clear)
{
	while (moveId > checkPointId) {
		--moveId;
		ygcMove^ lastMove = bp->currentMatch->moveHistory->GetAt(moveId);
		UpdateBoard(lastMove, false);
		bp->currentMatch->matchMoveBack();
		if (clear) bp->currentMatch->moveHistory->RemoveAtEnd();
	}
	*turn = *historyTurn;
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