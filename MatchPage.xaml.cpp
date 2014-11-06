//
// MatchPage.xaml.cpp
// Implementation of the MatchPage class
//

#include "pch.h"
#include "MatchPage.xaml.h"

using namespace ygc;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=390556

MatchPage::MatchPage()
{
	InitializeComponent();

	bp = ref new BoardPage(this);
	LayoutRoot = (Canvas ^)this->Content;
	
	InitMatch();

	scoreTBs = ref new Array<TextBlock^>(bp->currentMatch->players->Size);

	InitScorePanels();
	InitHandlers();

	SizeChanged += ref new SizeChangedEventHandler(this, &MatchPage::OrientHandler);
	bp->currentMatch->board->fields->VectorChanged += ref new VectorChangedEventHandler<ygcStoneColor^>([this](Object^ sender, IVectorChangedEventArgs^ e) {
		if (e->CollectionChange == CollectionChange::ItemChanged && bp->currentMatch->board->fields->GetAt(e->Index) == 0)
			bp->RemoveStone(Point((float)(e->Index % bp->currentMatch->board->sBoardWidth), (float)(e->Index / (uint16_t)bp->currentMatch->board->sBoardWidth)));
	});
}

void MatchPage::InitMatch()
{
	bp->currentMatch = ref new Go19::Go19Match(9, 9); // <--

	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK), ygcPlayerInputType::SCREEN, "Player1"));
	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::WHITE), ygcPlayerInputType::SCREEN, "Player2"));

	for (uint16_t i = 0; i < bp->currentMatch->board->sBoardWidth; ++i)
		for (uint16_t j = 0; j < bp->currentMatch->board->sBoardHeight; ++j)
			bp->currentMatch->board->SetAt(Point(i, j), Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY));

	bp->InitUI();

}

void MatchPage::InitScorePanels()
{
	//init score panels
	TopPanel = ref new StackPanel();
	BottomPanel = ref new StackPanel();
	OppPanel = ref new StackPanel();
	PlayerPanel = ref new StackPanel();
	PassPanel = ref new StackPanel();
	OppPanelCont = ref new Grid();
	PlayerPanelCont = ref new Grid();
	TurnIndCont = ref new Grid();
	PassCont = ref new Grid();
	OppPanelBorder = ref new Border();
	PlayerPanelBorder = ref new Border();

	LayoutRoot->Children->Append(TopPanel);
	LayoutRoot->Children->Append(BottomPanel);

	LayoutRoot->SetZIndex(TopPanel, 2);
	LayoutRoot->SetZIndex(BottomPanel, 2);

	LayoutRoot->SetTop(TopPanel, 0.0);
	LayoutRoot->SetLeft(TopPanel, 0.0);

	TopPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
	BottomPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

	SolidColorBrush ^ scb = ref new SolidColorBrush(Windows::UI::Colors::Black);
	scb->Opacity = 0.5;
	TopPanel->Background = scb;
	BottomPanel->Background = scb;

	TopPanel->Children->Append(TurnIndCont);
	TopPanel->Children->Append(OppPanelCont);
	BottomPanel->Children->Append(PassCont);
	BottomPanel->Children->Append(PlayerPanelCont);

	OppPanelCont->Children->Append(OppPanelBorder);
	OppPanelCont->RowDefinitions->Append(ref new RowDefinition());
	OppPanelCont->SetRow(OppPanelBorder, 0);
	OppPanelCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);
	PlayerPanelCont->Children->Append(PlayerPanelBorder);
	PlayerPanelCont->RowDefinitions->Append(ref new RowDefinition());
	PlayerPanelCont->SetRow(PlayerPanelBorder, 0);
	PlayerPanelCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);

	PassCont->Children->Append(PassPanel);
	PassCont->RowDefinitions->Append(ref new RowDefinition());
	PassCont->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);
	PassCont->SetRow(PassPanel, 0);

	OppPanelBorder->Padding = 10.0;
	PlayerPanelBorder->Padding = 10.0;

	OppPanelBorder->Child = OppPanel;
	PlayerPanelBorder->Child = PlayerPanel;

	//init score panels' content
	Shapes::Ellipse^ e1 = ref new Shapes::Ellipse();
	e1->Width = 30.0;
	e1->Height = 30.0;
	e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->players->GetAt(0)->color]);
	e1->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
	e1->StrokeThickness = 2.0f;
	e1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

	scoreTBs[0] = ref new TextBlock();
	TextBlock^ s1 = scoreTBs[0];
	s1->Name = "Player1Score";
	s1->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
	s1->Text = "0";
	s1->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
	s1->FontSize = 20.0;
	s1->FontWeight = Windows::UI::Text::FontWeights::Bold;
	s1->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	s1->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	s1->Margin = Thickness(10);

	PlayerPanel->Children->Append(s1);
	PlayerPanel->Children->Append(e1);

	Image^ PassButton = ref new Image();
	PassButton->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultActionIcons[1]));
	PassButton->Width = bp->PanelWidth;
	PassButton->Height = bp->PanelWidth;

	PassButton->Tapped += ref new TappedEventHandler([this](Object^ ss, TappedRoutedEventArgs ^te) {

		MessageDialog ^ msg = ref new MessageDialog("", "Are you sure you want to pass?");
		UICommand ^ passCommand = ref new UICommand("Pass");
		UICommand ^ cancelCommand = ref new UICommand("Cancel");
		msg->Commands->Append(passCommand);
		msg->Commands->Append(cancelCommand);
		msg->DefaultCommandIndex = 0;
		msg->CancelCommandIndex = 1;

		msg->ShowAsync()->Completed = ref new AsyncOperationCompletedHandler<IUICommand^>([this](IAsyncOperation<IUICommand^>^ res, AsyncStatus)
		{
			if (res->GetResults()->Label == "Pass") {
				bp->currentMatch->matchSkipTurn();
				UpdateIcons();
			}
		});
	});

	Image^ ResignButton = ref new Image();
	ResignButton->Source = ref new BitmapImage(ref new Uri(defaultAppSettings::defaultActionIcons[0]));
	ResignButton->Width = bp->PanelWidth;
	ResignButton->Height = bp->PanelWidth;

	ResignButton->Tapped += ref new TappedEventHandler([this](Object^ s, TappedRoutedEventArgs^ te) {
		MessageDialog ^ msg = ref new MessageDialog("", "Are you sure you want to resign?");
		UICommand ^ resignCommand = ref new UICommand("Resign");
		UICommand ^ cancelCommand = ref new UICommand("Cancel");
		msg->Commands->Append(resignCommand);
		msg->Commands->Append(cancelCommand);
		msg->DefaultCommandIndex = 0;
		msg->CancelCommandIndex = 1;

		msg->ShowAsync()->Completed = ref new AsyncOperationCompletedHandler<IUICommand^>([this](IAsyncOperation<IUICommand^>^ res, AsyncStatus)
		{
			if (res->GetResults()->Label == "Resign") {
				//EndMatch(); 
			}
		});
	});

	PassPanel->Children->Append(PassButton);
	PassPanel->Children->Append(ResignButton);

	Shapes::Ellipse^ e2 = ref new Shapes::Ellipse();
	e2->Width = 30.0;
	e2->Height = 30.0;
	e2->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->players->GetAt(1)->color]);
	e2->Stroke = ref new SolidColorBrush(Windows::UI::Colors::White);
	e2->StrokeThickness = 2.0f;
	e2->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

	scoreTBs[1] = ref new TextBlock();
	TextBlock^ s2 = scoreTBs[1];
	s2->Name = "Player2Score";
	s2->Foreground = ref new SolidColorBrush(Windows::UI::Colors::White);
	s2->Text = "0";
	s2->FontFamily = ref new Windows::UI::Xaml::Media::FontFamily("Arial");
	s2->FontSize = 20.0;
	s2->FontWeight = Windows::UI::Text::FontWeights::Bold;
	s2->VerticalAlignment = Windows::UI::Xaml::VerticalAlignment::Center;
	s2->HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
	s2->Margin = Thickness(10);

	OppPanel->Children->Append(e2);
	OppPanel->Children->Append(s2);

	for (unsigned int i = 2; i < bp->currentMatch->players->Size; ++i) {
		Border^ b1 = ref new Border(); // TODO: Test?
		b1->Width = 2.0;
		b1->BorderThickness = Thickness(1);
		b1->BorderBrush = ref new SolidColorBrush(Windows::UI::Colors::DimGray);
		b1->Background = ref new SolidColorBrush(Windows::UI::Colors::DimGray);

		e1 = ref new Shapes::Ellipse();
		e1->Width = 30.0;
		e1->Height = 30.0;
		e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->players->GetAt(i)->color]);
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

		OppPanel->Children->Append(b1);
		OppPanel->Children->Append(e1);
		OppPanel->Children->Append(s1);
	}
}

void MatchPage::InitHandlers()
{
	for (unsigned int k = 0; k < bp->currentMatch->players->Size; ++k) {
		if (bp->currentMatch->players->GetAt(k)->inputHandler->ypiType == ygcPlayerInputType::SCREEN) {
			ygcPlayer^ pp = bp->currentMatch->players->GetAt(k);
			bp->boardGrid->Tapped += ref new TappedEventHandler([this, pp](Object^ s, TappedRoutedEventArgs^ te){
				
				Canvas^ c = (Canvas^)s;
				Point tapPoint = te->GetPosition(c);

				ygcBoard^ board = pp->activeMatch->board;
				float SideMargin = float(c->Width > c->Height ? c->Width / float(board->sBoardWidth + 1) : c->Height / float(board->sBoardHeight + 1));

				if (tapPoint.X < SideMargin / 2.0f || tapPoint.X > c->Width - SideMargin / 2.0f ||
					tapPoint.Y < SideMargin / 2.0f || tapPoint.Y > c->Height - SideMargin / 2.0f)
					return;

				uint16_t x = uint16_t(::round(tapPoint.X / SideMargin) - 1.0);
				uint16_t y = uint16_t(::round(tapPoint.Y / SideMargin) - 1.0);

				if (pp->inputHandler->handleInput(Point(x, y))) {
					Point coord = Point(::roundf(te->GetPosition(c).X / (float)bp->SideMargin) - 1.0f, ::roundf(te->GetPosition(c).Y / (float)bp->SideMargin) - 1.0f);
					ygcStoneColor^ pturn = ref new ygcStoneColor(pp->activeMatch->turn->previous());
					bp->AddStone(coord, *pturn);
					scoreTBs[(int)*pturn - 1]->Text = ((uint16_t)bp->currentMatch->players->GetAt((int)*pturn - 1)->score).ToString();
				}
			});
		}
	}
}

void MatchPage::UpdateIcons() 
{

}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.
/// This parameter is typically used to configure the page.</param>
void MatchPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter

	bp->BoardOnNavigatedTo(e);
}

void MatchPage::OrientHandler(Object ^ s, SizeChangedEventArgs ^ sce)
{
	bp->BoardOrientHandler(s, sce);

	if (Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->Orientation == Windows::UI::ViewManagement::ApplicationViewOrientation::Portrait) {

		TopPanel->Orientation = Orientation::Horizontal;
		BottomPanel->Orientation = Orientation::Horizontal;
		OppPanel->Orientation = Orientation::Horizontal;
		PlayerPanel->Orientation = Orientation::Horizontal;
		PassPanel->Orientation = Orientation::Horizontal;

		TopPanel->Height = bp->PanelWidth;
		TopPanel->Width = bp->AppSpaceWidth;
		BottomPanel->Height = bp->PanelWidth;
		BottomPanel->Width = bp->AppSpaceWidth;

		LayoutRoot->SetTop(BottomPanel, bp->AppSpaceHeight - BottomPanel->Height);
		LayoutRoot->SetLeft(BottomPanel, 0.0);

		PlayerPanelCont->Width = bp->AppSpaceWidth / 2;
		PlayerPanelCont->Height = bp->PanelWidth;
		PassCont->Width = bp->AppSpaceWidth / 2;
		PassCont->Height = bp->PanelWidth;

		PassPanel->VerticalAlignment = ::VerticalAlignment::Center;
		PassPanel->HorizontalAlignment = ::HorizontalAlignment::Left;
		PlayerPanel->VerticalAlignment = ::VerticalAlignment::Center;
		PlayerPanel->HorizontalAlignment = ::HorizontalAlignment::Right;

	}
	else {

		TopPanel->Orientation = Orientation::Vertical;
		BottomPanel->Orientation = Orientation::Vertical;
		OppPanel->Orientation = Orientation::Vertical;
		PlayerPanel->Orientation = Orientation::Vertical;
		PassPanel->Orientation = Orientation::Vertical;

		TopPanel->Height = bp->AppSpaceHeight;
		TopPanel->Width = bp->PanelWidth;
		BottomPanel->Height = bp->AppSpaceHeight;
		BottomPanel->Width = bp->PanelWidth;

		LayoutRoot->SetTop(BottomPanel, 0.0);
		LayoutRoot->SetLeft(BottomPanel, bp->AppSpaceWidth - BottomPanel->Width);

		PlayerPanelCont->Width = bp->PanelWidth;
		PlayerPanelCont->Height = bp->AppSpaceHeight / 2;
		PassCont->Width = bp->PanelWidth;
		PassCont->Height = bp->AppSpaceHeight / 2;

		PassPanel->VerticalAlignment = ::VerticalAlignment::Top;
		PassPanel->HorizontalAlignment = ::HorizontalAlignment::Center;
		PlayerPanel->VerticalAlignment = ::VerticalAlignment::Bottom;
		PlayerPanel->HorizontalAlignment = ::HorizontalAlignment::Center;

	}
}
