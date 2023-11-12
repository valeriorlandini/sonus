/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#endif

using namespace c74::min;

class mbox : public object<mbox>
{
public:
	MIN_DESCRIPTION {"Message box generator"};
	MIN_TAGS {"utility"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in {this, "(bang) Show message box"};

    message<> m_bang
	{
		this,
		"bang",
		"Display the message box",
        MIN_FUNCTION
		{
			#ifdef _WIN32
			MessageBox(NULL, message_text_.c_str(), message_title_.c_str(), MB_OK);
			#endif

			#ifdef __APPLE__
    		NSAlert *alert = [[NSAlert alloc] init];
    		[alert setMessageText:message_title_.c_str()];
    		[alert setInformativeText:message_text_.c_str()];
    		[alert runModal];
    		#endif

			return {};
		}
    };

	attribute<symbol, threadsafe::no> text
	{
        this,
        "text",
		"",
		description {"Message to display inside the box."},
		setter
		{
			MIN_FUNCTION
			{
				message_text_ = std::string(args[0]);
				return args;
			}
		}
    };

	attribute<symbol, threadsafe::no> title
	{
        this,
        "title",
		"",
		description {"Title to display on the box."},
		setter
		{
			MIN_FUNCTION
			{
				message_title_ = std::string(args[0]);
				return args;
			}
		}
    };

	private:
	std::string message_text_;
	std::string message_title_;
};

MIN_EXTERNAL(mbox);
