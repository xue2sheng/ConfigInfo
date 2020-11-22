#pragma once

class XmlInfo {
public:
	static const std::wstring GetConfigFile();

	explicit XmlInfo(const std::wstring& filename = XmlInfo::GetConfigFile());
	const std::wstring Target() const;
	const int Port() const;

private:
	winrt::hstring _target;
	int _port;
};
