//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <ppl.h>

using namespace TransactionalWriteIssue;

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

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace concurrency;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

	statusLabel->Text = L"No file selected";
	pathLabel->Text = L"<none>";
	writeFileButton->IsEnabled = false;
}

static Platform::Array<uint8_t>^ makeData() {
	// 256 MB
	static Platform::Array<uint8_t>^ data = nullptr;
	if (!data) {
		data = ref new Platform::Array<uint8_t>(256 * 1024 * 1024);
		for (uint32_t i = 0; i < data->Length; i++) {
			data[i] = i % 256;
		}
	}

	return data;
}

void TransactionalWriteIssue::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	writeFileButton->IsEnabled = false;
	statusLabel->Text = L"Writing...";
	create_task(m_pickedFile->OpenTransactedWriteAsync()).then([](StorageStreamTransaction^ transaction) {
		auto stream = transaction->Stream;
		DataWriter^ writer = ref new DataWriter(stream);
		writer->WriteBytes(makeData());
		return create_task(writer->StoreAsync()).then([writer](uint32_t size) {
			return writer->FlushAsync();
		}).then([transaction](bool result) {
			return transaction->CommitAsync();
		}).then([transaction] {
			delete transaction;
		});
	}, task_continuation_context::use_arbitrary()).then([this] {
		statusLabel->Text = L"Idle";
		writeFileButton->IsEnabled = true;
	}, task_continuation_context::use_current());
}


void TransactionalWriteIssue::MainPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto picker = ref new FileSavePicker();
	picker->SuggestedStartLocation = PickerLocationId::Desktop;
	auto fileTypes = ref new Platform::Collections::Vector<String^>();
	fileTypes->Append(L".dat");
	picker->FileTypeChoices->Insert(L"Data File", fileTypes);

	create_task(picker->PickSaveFileAsync()).then([this](StorageFile^ pickedFile) {
		if (!pickedFile) {
			return;
		}

		m_pickedFile = pickedFile;
		pathLabel->Text = pickedFile->Path;
		statusLabel->Text = "Idle";
		writeFileButton->IsEnabled = true;
	});
}
