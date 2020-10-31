#pragma once

class XmlInfo {
public:
	static const std::wstring GetConfigFile();
	static const std::string ConfigFile();

	explicit XmlInfo(const std::wstring& filename = XmlInfo::GetConfigFile());
	const std::string Target() const;

private:
	winrt::hstring _target;
};
