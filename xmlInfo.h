#pragma once

class XmlInfo {
public:
	static const std::wstring GetConfigFile();

	explicit XmlInfo(const std::wstring& filename = XmlInfo::GetConfigFile());
	const std::wstring Target() const;
	const int Port() const;
	const std::wstring Message() const;
	void Message(const std::wstring msg);
	const std::wstring URL() const;

private:
	winrt::hstring _target;
	int _port;
	std::wstring _message;
	std::wstring _url;
};
