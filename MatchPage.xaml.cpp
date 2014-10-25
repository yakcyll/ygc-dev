﻿//
// MatchPage.xaml.cpp
// Implementation of the MatchPage class
//

#include "pch.h"
#include "MatchPage.xaml.h"

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

MatchPage::MatchPage()
{
	InitializeComponent();

	bp = ref new BoardPage(this);
	LayoutRoot = (Canvas ^)this->Content;
	
	InitMatch();
	bp->InitUI();
	InitHandlers();
	InitScorePanels();
}

void MatchPage::InitMatch()
{
	bp->currentMatch = ref new Go19::Go19Match();

	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::BLACK), ygcPlayerInputType::SCREEN, "Player1"));
	bp->currentMatch->players->Append(ref new ygcPlayer(bp->currentMatch, ref new Go19::Go19StoneColor(Go19::Go19StoneColor::WHITE), ygcPlayerInputType::SCREEN, "Player2"));

	for (auto i = 0; i < bp->currentMatch->board->sBoardWidth; ++i) {
		for (auto j = 0; j < bp->currentMatch->board->sBoardHeight; ++j) {

			ygcField^ field = ref new ygcField();
			field->takenBy = ref new Go19::Go19StoneColor(Go19::Go19StoneColor::EMPTY);
			field->x = i;
			field->y = j;

			bp->currentMatch->board->GetAt(i, j) = field;
		}
	}

}

void MatchPage::InitHandlers()
{
	for (unsigned int k = 0; k < bp->currentMatch->players->Size; ++k) {
		if (bp->currentMatch->players->GetAt(k)->inputHandler->ypiType == ygcPlayerInputType::SCREEN) {
			ygcPlayer^ pp = bp->currentMatch->players->GetAt(k);
			bp->boardGrid->Tapped += ref new TappedEventHandler([this, pp](Object^ s, RoutedEventArgs^ e){

				if (pp->inputHandler->handleInput(s, e)) {

					Canvas^ c = (Canvas^)s;
					TappedRoutedEventArgs^ te = (TappedRoutedEventArgs^)e;

					Shapes::Ellipse^ stone = ref new Shapes::Ellipse();
					stone->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->turn->previous()]);
					stone->Width = 0.9 * bp->SideMargin;
					stone->Height = stone->Width;
					stone->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Black);
					stone->StrokeThickness = 2.0f;

					bp->boardGrid->Children->Append(stone);
					bp->boardGrid->SetTop(stone, ::floor((te->GetPosition(c).Y - bp->SideMargin / 2.0f) / bp->SideMargin) * bp->SideMargin + bp->SideMargin / 2.0f);
					bp->boardGrid->SetLeft(stone, ::floor((te->GetPosition(c).X - bp->SideMargin / 2.0f) / bp->SideMargin) * bp->SideMargin + bp->SideMargin / 2.0f);
				}
			});
		}
	}
}


void MatchPage::InitScorePanels()
{
	//init score panels
	OppPanel = ref new StackPanel();
	PlayerPanel = ref new StackPanel();
	OppPanelCont = ref new StackPanel();
	PlayerPanelCont = ref new StackPanel();
	OppPanelBorder = ref new Border();
	PlayerPanelBorder = ref new Border();

	bp->LayoutRoot->Children->Append(OppPanel);
	bp->LayoutRoot->Children->Append(PlayerPanel);

	bp->LayoutRoot->SetZIndex(OppPanel, 2);
	bp->LayoutRoot->SetZIndex(PlayerPanel, 2);

	OppPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
	PlayerPanel->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;

	SolidColorBrush ^ scb = ref new SolidColorBrush(Windows::UI::Colors::Black);
	scb->Opacity = 0.5;
	OppPanel->Background = scb;
	PlayerPanel->Background = scb;

	OppPanel->Children->Append(OppPanelBorder);
	PlayerPanel->Children->Append(PlayerPanelBorder);

	OppPanelBorder->Padding = 10.0;
	PlayerPanelBorder->Padding = 10.0;

	PlayerPanelBorder->RenderTransform = ref new RotateTransform();
	((RotateTransform^)PlayerPanelBorder->RenderTransform)->Angle = 180.0;
	PlayerPanelBorder->RenderTransformOrigin = Point(0.5, 0.5);

	OppPanelBorder->Child = OppPanelCont;
	PlayerPanelBorder->Child = PlayerPanelCont;

	//init score panels' content
	Shapes::Ellipse^ e1 = ref new Shapes::Ellipse();
	e1->Width = 30.0;
	e1->Height = 30.0;
	e1->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->players->GetAt(0)->color]);
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
	e2->Fill = ref new SolidColorBrush(defaultAppSettings::defaultStoneColors[bp->currentMatch->players->GetAt(1)->color]);
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

		OppPanelCont->Children->Append(b1);
		OppPanelCont->Children->Append(e1);
		OppPanelCont->Children->Append(s1);
	}
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

		OppPanel->Orientation = Orientation::Horizontal;
		PlayerPanel->Orientation = Orientation::Horizontal;
		OppPanelCont->Orientation = Orientation::Horizontal;
		PlayerPanelCont->Orientation = Orientation::Horizontal;

		OppPanel->Height = bp->PanelWidth;
		OppPanel->Width = bp->AppSpaceWidth;
		PlayerPanel->Height = bp->PanelWidth;
		PlayerPanel->Width = bp->AppSpaceWidth;

		LayoutRoot->SetTop(PlayerPanel, bp->AppSpaceHeight - PlayerPanel->Height);
		LayoutRoot->SetLeft(PlayerPanel, 0.0);
	}
	else {

		OppPanel->Orientation = Orientation::Vertical;
		PlayerPanel->Orientation = Orientation::Vertical;
		OppPanelCont->Orientation = Orientation::Vertical;
		PlayerPanelCont->Orientation = Orientation::Vertical;

		OppPanel->Height = bp->AppSpaceHeight;
		OppPanel->Width = bp->PanelWidth;
		PlayerPanel->Height = bp->AppSpaceHeight;
		PlayerPanel->Width = bp->PanelWidth;

		LayoutRoot->SetTop(PlayerPanel, 0.0);
		LayoutRoot->SetLeft(PlayerPanel, bp->AppSpaceWidth - PlayerPanel->Width);
	}
}
