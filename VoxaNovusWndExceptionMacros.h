#pragma once

#define VNWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define VNWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())
#define VNWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__,__FILE__,GetLastError())