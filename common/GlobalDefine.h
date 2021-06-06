#pragma once

#define qstr QStringLiteral
#define UserConfigFileName "user"


#define SafeRelease(p) if (p) do{ delete p; p = nullptr; } while (0)