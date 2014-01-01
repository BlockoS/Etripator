#include <vector>
#include <array>
#include "commandLine.h"

// [debug::begin]
#include <iostream>
// [debug::end]

namespace Etripator   {
namespace CommandLine {

// Command line parser implementation.
class Parser::Impl
{
	public:
		// CTOR
		Impl();
		// DTOR
		~Impl();
		// Parse configuration to build the list of supported arguments.
		// It also pases the argv string and verify it complies with the
		// passed configuration.
		bool Parse(int argc, char** argv, std::string const& configuration);
		// Check if the configuration contains a given argument.
		bool Contains(std::string const& arg) const;
		// Parser configuration string.
		bool ParseConfiguration(std::string const& configuration);
	private:
		// Argument description.
		struct Argument
		{
			std::string description;
			std::string defaultValue;
			std::vector<std::string> names;

			typedef void (Argument::*SetFunc)(std::string const& in, size_t start, size_t end);

			Argument();
			Argument(Argument const& arg);
			~Argument();

			Argument& operator= (Argument const& arg);

			void SetDescription (std::string const& in, size_t start, size_t end);
			void SetDefaultValue(std::string const& in, size_t start, size_t end);
			void SetNames       (std::string const& in, size_t start, size_t end);
			
			bool Parse(std::string const& in, size_t start, size_t end);

			void Clear();
		};

		std::vector<Argument> _args;
};

bool Parser::Impl::ParseConfiguration(std::string const& configuration)
{
	bool ret;
	size_t start, end;

	Parser::Impl::Argument currentArg;

	_args.clear();
	for(start=0; ;)
	{
		currentArg.Clear();

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

		ret = currentArg.Parse(configuration, start, end);
		// [todo] ret

		_args.push_back(currentArg);
	}

	// At this point we must have at least one configuration.
	if(_args.empty())
	{
		// [todo]
		return false;
	}
	return true;
}

bool Parser::Impl::Argument::Parse(std::string const& in, size_t start, size_t end)
{
	static const std::array<Argument::SetFunc, 3> set = 
	{ 
		&Parser::Impl::Argument::SetNames,
		&Parser::Impl::Argument::SetDefaultValue,
		&Parser::Impl::Argument::SetDescription
	};

	size_t i, current, previous;
	for(i=0, previous=start; (previous < end) && (i < 3); i++, previous=current+1)
	{
		current = in.find_first_of('|', previous);
		if((current > end) || (std::string::npos == current))
		{
			current = end;
		}
		(this->*set[i])(in, previous, current);
	}

	return (set.size() != i);
}

Parser::Impl::Argument::Argument()
{}
Parser::Impl::Argument::Argument(Parser::Impl::Argument const& arg)
	: description(arg.description)
	, defaultValue(arg.defaultValue)
	, names(arg.names)
{}
Parser::Impl::Argument::~Argument()
{}
Parser::Impl::Argument& Parser::Impl::Argument::operator= (Parser::Impl::Argument const& arg)
{
	description  = arg.description;
	defaultValue = arg.defaultValue;
	names        = arg.names;
	return *this;
}

void Parser::Impl::Argument::SetDescription (std::string const& in, size_t start, size_t end)
{
	size_t len = ((end < in.size()) ? end : in.size()) - start;
	description = in.substr(start, len);
}

void Parser::Impl::Argument::SetDefaultValue(std::string const& in, size_t start, size_t end)
{
	size_t len = ((end < in.size()) ? end : in.size()) - start;
	defaultValue = in.substr(start, len);	
}

void Parser::Impl::Argument::SetNames(std::string const& in, size_t start, size_t end)
{
	size_t current, next;

	names.clear();
	for(current=start; (std::string::npos!=current) && (current < end);)
	{
		current = in.find_first_not_of(" \t", current, end-current);
		if((std::string::npos!=current) && (current < end))
		{
			next = in.find_first_of(" \t", current, end-current);
			if((std::string::npos == next) || (next > end))
			{
				next = end;
			}
			names.push_back(in.substr(current, next-current));
			current=next;
		}
	}
}

void Parser::Impl::Argument::Clear()
{
	description.clear();
	defaultValue.clear();
	for(size_t i=0; i<names.size(); i++)
	{
		names[i].clear();
	}
	names.clear();
}

// CTOR
Parser::Impl::Impl()
{}
// DTOR	
Parser::Impl::~Impl()
{}
// Parse configuration to build the list of supported arguments.
// It also pases the argv string and verify it complies with the
// passed configuration.
bool Parser::Impl::Parse(int argc, char** argv, std::string const& configuration)
{
	bool ret;

	ret = ParseConfiguration(configuration);
	if(ret)
	{
		// [todo] parse argv
	}
	return ret;
}
// Check if the configuration contains a given argument.
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
	return _impl->Parse(argc, argv, configuration);
}

//! Display general description and argument description.
//! \param [in] general    General description.
void Parser::Usage(std::string const& general)
{
	std::cout << general << std::endl;
	// [todo]
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