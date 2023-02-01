#include "pch.h"
#include "HomeViewModel.h"
#if __has_include("HomeViewModel.g.cpp")
#include "HomeViewModel.g.cpp"
#endif
#include "AppSettings.h"
#include "MagService.h"
#include "Win32Utils.h"
#include "StrUtils.h"
#include "UpdateService.h"

namespace winrt::Magpie::App::implementation {

HomeViewModel::HomeViewModel() {
	MagService& magService = MagService::Get();

	_isRunningChangedRevoker = magService.IsRunningChanged(
		auto_revoke, { this, &HomeViewModel::_MagService_IsRunningChanged });
	_isCountingDownRevoker = magService.IsCountingDownChanged(
		auto_revoke, { this, &HomeViewModel::_MagService_IsCountingDownChanged });
	_countdownTickRevoker = magService.CountdownTick(
		auto_revoke, { this, &HomeViewModel::_MagService_CountdownTick });
	_wndToRestoreChangedRevoker = magService.WndToRestoreChanged(
		auto_revoke, { this, &HomeViewModel::_MagService_WndToRestoreChanged });

	UpdateService& updateService = UpdateService::Get();
	_showUpdateCard = updateService.IsShowOnHomePage();
	_isShowOnHomePageChangedRevoker = updateService.IsShowOnHomePageChanged(
		auto_revoke,
		[this](bool value) {
			if (value) {
				ShowUpdateCard(true);
			}
		}
	);
}

bool HomeViewModel::IsCountingDown() const noexcept {
	return MagService::Get().IsCountingDown();
}

float HomeViewModel::CountdownProgressRingValue() const noexcept {
	MagService& magService = MagService::Get();
	return magService.IsCountingDown() ? magService.CountdownLeft() / magService.TickingDownCount() : 1.0f;
}

hstring HomeViewModel::CountdownLabelText() const noexcept {
	MagService& magService = MagService::Get();
	return to_hstring((int)std::ceil(magService.CountdownLeft()));
}

hstring HomeViewModel::CountdownButtonText() const noexcept {
	MagService& magService = MagService::Get();
	if (magService.IsCountingDown()) {
		return L"取消";
	} else {
		return hstring(fmt::format(L"{} 秒后缩放", AppSettings::Get().DownCount()));
	}
}

bool HomeViewModel::IsNotRunning() const noexcept {
	return !MagService::Get().IsRunning();
}

void HomeViewModel::ToggleCountdown() const noexcept {
	MagService& magService = MagService::Get();
	if (magService.IsCountingDown()) {
		magService.StopCountdown();
	} else {
		magService.StartCountdown();
	}
}

uint32_t HomeViewModel::DownCount() const noexcept {
	return AppSettings::Get().DownCount();
}

void HomeViewModel::DownCount(uint32_t value) {
	AppSettings& settings = AppSettings::Get();

	if (settings.DownCount() == value) {
		return;
	}

	settings.DownCount(value);
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"DownCount"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownButtonText"));
}

bool HomeViewModel::IsAutoRestore() const noexcept {
	return AppSettings::Get().IsAutoRestore();
}

void HomeViewModel::IsAutoRestore(bool value) {
	AppSettings& settings = AppSettings::Get();

	if (settings.IsAutoRestore() == value) {
		return;
	}

	settings.IsAutoRestore(value);
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"IsAutoRestore"));
}

bool HomeViewModel::IsWndToRestore() const noexcept {
	return (bool)MagService::Get().WndToRestore();
}

void HomeViewModel::ActivateRestore() const noexcept {
	HWND wndToRestore = (HWND)MagService::Get().WndToRestore();
	if (wndToRestore) {
		Win32Utils::SetForegroundWindow(wndToRestore);
	}
}

void HomeViewModel::ClearRestore() const {
	MagService::Get().ClearWndToRestore();
}

hstring HomeViewModel::RestoreWndDesc() const noexcept {
	HWND wndToRestore = (HWND)MagService::Get().WndToRestore();
	if (!wndToRestore) {
		return L"";
	}

	std::wstring title(GetWindowTextLength(wndToRestore), L'\0');
	GetWindowText(wndToRestore, title.data(), (int)title.size() + 1);

	return hstring(StrUtils::ConcatW(L"当前窗口：", title.empty() ? L"<标题为空>" : title));
}

bool HomeViewModel::IsProcessElevated() const noexcept {
	return Win32Utils::IsProcessElevated();
}

void HomeViewModel::RestartAsElevated() const noexcept {
	Application::Current().as<App>().RestartAsElevated();
}

bool HomeViewModel::IsAlwaysRunAsElevated() const noexcept {
	return AppSettings::Get().IsAlwaysRunAsElevated();
}

void HomeViewModel::IsAlwaysRunAsElevated(bool value) noexcept {
	AppSettings::Get().IsAlwaysRunAsElevated(value);
}

inline void HomeViewModel::ShowUpdateCard(bool value) noexcept {
	_showUpdateCard = value;
	if (!value) {
		UpdateService::Get().IsShowOnHomePage(false);
	}
	
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"ShowUpdateCard"));
}

bool HomeViewModel::IsAutoCheckForUpdates() const noexcept {
	return AppSettings::Get().IsAutoCheckForUpdates();
}

void HomeViewModel::IsAutoCheckForUpdates(bool value) noexcept {
	AppSettings::Get().IsAutoCheckForUpdates(value);
}

void HomeViewModel::DownloadAndInstall() {
	UpdateService::Get().DownloadAndInstall();
	Application::Current().as<App>().MainPage().NavigateToAboutPage();
}

void HomeViewModel::ReleaseNotes() {
	std::wstring url = StrUtils::ConcatW(
		L"https://github.com/Blinue/Magpie/releases/tag/",
		UpdateService::Get().Tag()
	);
	Win32Utils::ShellOpen(url.c_str());
}

void HomeViewModel::RemindMeLater() {
	ShowUpdateCard(false);
}

void HomeViewModel::_MagService_IsCountingDownChanged(bool value) {
	if (!value) {
		_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownProgressRingValue"));
	}

	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownProgressRingValue"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownLabelText"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownButtonText"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"IsCountingDown"));
}

void HomeViewModel::_MagService_CountdownTick(float) {
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownProgressRingValue"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"CountdownLabelText"));
}

void HomeViewModel::_MagService_IsRunningChanged(bool) {
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"IsNotRunning"));
}

void HomeViewModel::_MagService_WndToRestoreChanged(HWND) {
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"IsWndToRestore"));
	_propertyChangedEvent(*this, PropertyChangedEventArgs(L"RestoreWndDesc"));
}

}
