#pragma once

#include <string>
#include <Windows.h>

class GetCommandOutput
{
public:
	static std::string CMD(std::wstring command, bool convert_to_utf8 = true);
};

std::string GetCommandOutput::CMD(std::wstring command, bool convert_to_utf8)
{
	HANDLE pipe_read = INVALID_HANDLE_VALUE, pipe_write = INVALID_HANDLE_VALUE;
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = nullptr;
	attr.bInheritHandle = true;
	CreatePipe(&pipe_read, &pipe_write, &attr, 0);

	STARTUPINFOW startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	startup_info.dwFlags = STARTF_USESTDHANDLES;
	startup_info.hStdInput = nullptr;
	startup_info.hStdOutput = pipe_write;
	startup_info.hStdError = pipe_write;

	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof(process_info));

	BOOL process_success = CreateProcessW(nullptr, command.data(), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &startup_info, &process_info);
	CloseHandle(pipe_write);
	char buffer[1000];
	std::string output = "";
	while (true)
	{
		DWORD n = 0;
		BOOL success = ReadFile(pipe_read, buffer, 1000, &n, nullptr);
		if (!success || n == 0)
		{
			break;
		}
		output.append(buffer, n);
	}
	CloseHandle(pipe_read);
	CloseHandle(process_info.hProcess);
	CloseHandle(process_info.hThread);

	if (!convert_to_utf8)
	{
		return output;
	}
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, output.c_str(), -1, nullptr, 0);
		std::wstring temp_result(length, L'\0');
		MultiByteToWideChar(CP_ACP, 0, output.c_str(), -1, &temp_result[0], length);

		length = WideCharToMultiByte(CP_UTF8, 0, temp_result.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string result(length, '\0');
		WideCharToMultiByte(CP_UTF8, 0, temp_result.c_str(), -1, &result[0], length, nullptr, nullptr);
		return result;
	}
}