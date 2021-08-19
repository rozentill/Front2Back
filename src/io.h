#pragma once
#include <vector>
#include <filesystem>

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	v.clear();
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

std::vector<std::string> get_directories(const std::string& s)
{
	std::vector<std::string> v;
	std::vector<std::string> r;
	v.clear();
	r.clear();
	for (auto& p : std::experimental::filesystem::directory_iterator(s))
		if (p.status().type() == std::experimental::filesystem::file_type::directory)
		{
			SplitString(p.path().string(), v, "\\");
			r.push_back(v.back());
		}
	return r;
}


std::vector<std::string> get_all_files(const std::string& s)
{
	std::vector<std::string> v;
	std::vector<std::string> r;
	v.clear();
	r.clear();
	for (auto& p : std::experimental::filesystem::directory_iterator(s))
	{
		SplitString(p.path().string(), v, "\\");
		r.push_back(v.back());
	}
	return r;
}

