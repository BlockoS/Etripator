#include <vector>
#include <array>
#include "commandLine.h"

// [debug::begin]
#include <iostream>
// [debug::end]

// [todo] COMMENTS!

namespace Etripator   {
namespace CommandLine {

class Parser::Impl
{
	public:
		Impl();
		~Impl();
		
		bool Parse(int argc, char** argv, std::string const& configuration);
		bool Contains(std::string const& arg) const;
		
		bool ParseConfiguration(std::string const& configuration);
	private:
		struct Arguments
		{
			std::string description;
			std::string defaultValue;
			std::vector<std::string> names;

			typedef void (Arguments::*SetFunc)(std::string const& in, size_t start, size_t end);

			// [todo] CTOR and co?

			void SetDescription (std::string const& in, size_t start, size_t end);
			void SetDefaultValue(std::string const& in, size_t start, size_t end);
			void SetNames       (std::string const& in, size_t start, size_t end);
		};
};

Parser::Impl::Impl()
{}
		
Parser::Impl::~Impl()
{}

bool Parser::Impl::ParseConfiguration(std::string const& configuration)
{
	size_t start, end;
	size_t previous, separator, i;

	Parser::Impl::Arguments arg;

	std::array<Arguments::SetFunc, 3> set = 
	{ 
		&Parser::Impl::Arguments::SetNames,
		&Parser::Impl::Arguments::SetDefaultValue,
		&Parser::Impl::Arguments::SetDescription
	};

	for(start=0; ;)
	{
		start = configuration.find_first_of('[', start);
		if(std::string::npos == start)
		{
			// Ignore garbage if any.
			break;
		}
		++start;
		end   = configuration.find_first_of(']', start);
		if(std::string::npos == end)
		{
			// [todo] exception
			return false;
		}

		separator = previous = start;
		for(i=0; (previous < end) && (i < 3); i++)
		{
			separator = configuration.find_first_of('|', previous);
			if((separator > end) || (std::string::npos == separator))
			{
				separator = end;
			}
			(arg.*set[i])(configuration, previous, separator);

			previous = separator+1;
		}

		if(3 != i)
		{
			// [todo] throw exception
			return false;
		}
	}

	// At this point we must have at least one configuration.
	// [todo] if() { throw ARG! }
	return true;
}

void Parser::Impl::Arguments::SetDescription (std::string const& in, size_t start, size_t end)
{
	size_t len = ((end < in.size()) ? end : in.size()) - start;
	description = in.substr(start, len);
}

void Parser::Impl::Arguments::SetDefaultValue(std::string const& in, size_t start, size_t end)
{
	size_t len = ((end < in.size()) ? end : in.size()) - start;
	defaultValue = in.substr(start, len);	
}

void Parser::Impl::Arguments::SetNames(std::string const& in, size_t start, size_t end)
{
	size_t len = ((end < in.size()) ? end : in.size()) - start;
	// [todo] split by space/tab
	// [todo] then check for extra infos
	names.push_back(in.substr(start, len));
}

bool Parser::Impl::Parse(int argc, char** argv, std::string const& configuration)
{
	bool ret;

	ret = ParseConfiguration(configuration);

	// [todo]

	return ret;
}

bool Parser::Impl::Contains(std::string const& arg) const
{
	// [todo]
	return true;
}

Parser::Parser()
	: _impl(NULL)
{}

Parser::~Parser()
{}

bool Parser::Parse(int argc, char** argv, std::string const& configuration)
{
	// [todo] throw exceptions?
	if(NULL != _impl)
	{
		return false;
	}

	_impl = new Parser::Impl;
	_impl->Parse(argc, argv, configuration);

	return true;
}

bool Parser::Contains(std::string const& arg) const
{
	if(NULL == _impl)
	{
		// [todo] throw exception
	}

	return _impl->Contains(arg);
}

} // CommandLine
} // Etripator