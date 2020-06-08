//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace TransactionalWriteIssue
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		Windows::Storage::StorageFile^ m_pickedFile;

		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
