#pragma once
#include "HomeViewModel.g.h"
#include "WinRTUtils.h"

namespace winrt::Magpie::App::implementation {

struct HomeViewModel : HomeViewModelT<HomeViewModel> {
	HomeViewModel();

	event_token PropertyChanged(PropertyChangedEventHandler const& handler) {
		return _propertyChangedEvent.add(handler);
	}

	void PropertyChanged(event_token const& token) noexcept {
		_propertyChangedEvent.remove(token);
	}

	bool IsCountingDown() const noexcept;

	float CountdownProgressRingValue() const noexcept;

	hstring CountdownLabelText() const noexcept;

	hstring CountdownButtonText() const noexcept;

	bool IsNotRunning() const noexcept;

	void ToggleCountdown() const noexcept;

	uint32_t DownCount() const noexcept;
	void DownCount(uint32_t value);

	bool IsAutoRestore() const noexcept;
	void IsAutoRestore(bool value);

	bool IsWndToRestore() const noexcept;

	void ActivateRestore() const noexcept;

	void ClearRestore() const;

	hstring RestoreWndDesc() const noexcept;

	bool IsProcessElevated() const noexcept;

	void RestartAsElevated() const noexcept;

	bool IsAlwaysRunAsElevated() const noexcept;
	void IsAlwaysRunAsElevated(bool value) noexcept;

	bool ShowUpdateCard() const noexcept {
		return _showUpdateCard;
	}

	void ShowUpdateCard(bool value) noexcept;

	bool IsAutoCheckForUpdates() const noexcept;
	void IsAutoCheckForUpdates(bool value) noexcept;

	void DownloadAndInstall();

	void ReleaseNotes();

	void RemindMeLater();
private:
	void _MagService_IsCountingDownChanged(bool value);

	void _MagService_CountdownTick(float);

	void _MagService_IsRunningChanged(bool);

	void _MagService_WndToRestoreChanged(HWND);

	event<PropertyChangedEventHandler> _propertyChangedEvent;

	WinRTUtils::EventRevoker _isCountingDownRevoker;
	WinRTUtils::EventRevoker _countdownTickRevoker;
	WinRTUtils::EventRevoker _isRunningChangedRevoker;
	WinRTUtils::EventRevoker _wndToRestoreChangedRevoker;
	WinRTUtils::EventRevoker _isShowOnHomePageChangedRevoker;

	bool _showUpdateCard = false;
};

}

namespace winrt::Magpie::App::factory_implementation {

struct HomeViewModel : HomeViewModelT<HomeViewModel, implementation::HomeViewModel> {
};

}
