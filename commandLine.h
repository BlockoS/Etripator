#ifndef COMMAND_LINE_H11
#define COMMAND_LINE_H

#include "config.h"
#include <string>

namespace Etripator   {
namespace CommandLine {

	class Parser
	{
		public:
			Parser();
			~Parser();

			bool Parse(int argc, char** argv, std::string const& configuration);
			bool Contains(std::string const& arg) const;
			
			template <typename T>
			T Get(std::string const& arg) const; // [todo] ...

		private:
			class Impl;
			Impl* _impl;
	};

} // CommandLine
} // Etripator

#endif // COMMAND_LINE_H