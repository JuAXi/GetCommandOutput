#include <print>
#include "GetCommandOutput.h"

using namespace std;

int main(int argc, char const *argv[])
{
	wstring cmd = L"powershell.exe /c ls";
	string result = GetCommandOutput::CMD(cmd);
	println("{}", result);
	system("pause");
	return 0;
}