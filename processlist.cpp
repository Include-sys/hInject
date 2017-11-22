#include "processlist.h"
#include <Windows.h>
#include <tlhelp32.h>

typedef DWORD(WINAPI *PFN_GETWINDOWTHREADPROCESSID)(HWND hWnd, LPDWORD lpdwProcessId);
typedef HWND(WINAPI *PFN_GETWINDOW)(HWND hWnd, UINT uCmd);
typedef BOOL(WINAPI *PFN_ISWINDOWVISIBLE)(HWND hWnd);
typedef int(WINAPI *PFN_GETWINDOWTEXTLENGTHW)(HWND hWnd);
typedef int(WINAPI *PFN_GETWINDOWTEXTW)(HWND hWnd, LPWSTR lpString, int nMaxCount);
typedef BOOL(WINAPI *PFN_ENUMWINDOWS)(WNDENUMPROC lpEnumFunc, LPARAM lParam);

namespace
{
struct callbackContext
{
  callbackContext(QProcList &l) : list(l) {}
  QProcList &list;

  PFN_GETWINDOWTHREADPROCESSID GetWindowThreadProcessId;
  PFN_GETWINDOW GetWindow;
  PFN_ISWINDOWVISIBLE IsWindowVisible;
  PFN_GETWINDOWTEXTLENGTHW GetWindowTextLengthW;
  PFN_GETWINDOWTEXTW GetWindowTextW;
  PFN_ENUMWINDOWS EnumWindows;
};
}

ProcessList::ProcessList()
{

}

static BOOL CALLBACK fillWindowTitles(HWND hwnd, LPARAM lp)
{
  callbackContext *ctx = (callbackContext *)lp;

  DWORD pid = 0;
  ctx->GetWindowThreadProcessId(hwnd, &pid);

  HWND parent = ctx->GetWindow(hwnd, GW_OWNER);

  if(parent != 0)
    return TRUE;

  if(!ctx->IsWindowVisible(hwnd))
    return TRUE;

  for(ProcessList &info : ctx->list)
  {
    if(info.pid() == (uint32_t)pid)
    {
      int len = ctx->GetWindowTextLengthW(hwnd);
      wchar_t *buf = new wchar_t[len + 1];
      ctx->GetWindowTextW(hwnd, buf, len + 1);
      buf[len] = 0;
      info.setWindowTitle(QString::fromStdWString(std::wstring(buf)));
      delete[] buf;
      return TRUE;
    }
  }

  return TRUE;
}

QProcList ProcessList::enumerate()
{
  QProcList ret;

  HANDLE h = NULL;
  PROCESSENTRY32 pe = {0};
  pe.dwSize = sizeof(PROCESSENTRY32);
  h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if(Process32First(h, &pe))
  {
    do
    {
      ProcessList info;
      info.setPid((uint32_t)pe.th32ProcessID);
      info.setName(QString::fromStdWString(std::wstring(pe.szExeFile)));

      ret.push_back(info);
    } while(Process32Next(h, &pe));
  }
  CloseHandle(h);

  HMODULE user32 = LoadLibraryA("user32.dll");

  if(user32)
  {
    callbackContext ctx(ret);

    ctx.GetWindowThreadProcessId =
        (PFN_GETWINDOWTHREADPROCESSID)GetProcAddress(user32, "GetWindowThreadProcessId");
    ctx.GetWindow = (PFN_GETWINDOW)GetProcAddress(user32, "GetWindow");
    ctx.IsWindowVisible = (PFN_ISWINDOWVISIBLE)GetProcAddress(user32, "IsWindowVisible");
    ctx.GetWindowTextLengthW =
        (PFN_GETWINDOWTEXTLENGTHW)GetProcAddress(user32, "GetWindowTextLengthW");
    ctx.GetWindowTextW = (PFN_GETWINDOWTEXTW)GetProcAddress(user32, "GetWindowTextW");
    ctx.EnumWindows = (PFN_ENUMWINDOWS)GetProcAddress(user32, "EnumWindows");

    if(ctx.GetWindowThreadProcessId && ctx.GetWindow && ctx.IsWindowVisible &&
       ctx.GetWindowTextLengthW && ctx.GetWindowTextW && ctx.EnumWindows)
    {
      ctx.EnumWindows(fillWindowTitles, (LPARAM)&ctx);
    }

    FreeLibrary(user32);
  }

  return ret;
}

uint32_t ProcessList::pid() const
{
  return m_pid;
}

void ProcessList::setPid(uint32_t pid)
{
  m_pid = pid;
}

const QString &ProcessList::name() const
{
  return m_name;
}

void ProcessList::setName(const QString &name)
{
  m_name = name;
}

const QString &ProcessList::windowTitle() const
{
  return m_title;
}

void ProcessList::setWindowTitle(const QString &title)
{
  m_title = title;
}

const QString &ProcessList::commandLine() const
{
  return m_cmdLine;
}

void ProcessList::setCommandLine(const QString &cmd)
{
  m_cmdLine = cmd;
}
