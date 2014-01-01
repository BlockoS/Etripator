#ifndef COMMAND_LINE_H11
#define COMMAND_LINE_H

#include "config.h"
#include <string>

namespace Etripator   {
namespace CommandLine {
	//! Command line argument parser.
	class Parser
	{
		public:
			//! Default constructor.
			Parser();
			//! Destructor.
			~Parser();

			//! Parse configuration to build the list of supported arguments.
			//! It also pases the argv string and verify it complies with the
			//! passed configuration.
			//! \param [in] argc             Argument count.
			//! \param [in] argv             Arguments string.
			//! \param [in] configuration    Argument configuration.
			// (todo)
			//! \code{.cpp}
			//! std::string cfg = "[h help usage|    |Display this message]"
			//!                   "[count c     |1   |Count option        ]"
			//!                   "[t threshold |0.5 |Threshold value     ]";
			//! \endcode
			//! \return (todo)
			bool Parse(int argc, char** argv, std::string const& configuration);
			//! Display general description and argument description.
			//! \param [in] general    General description.
			void Usage(std::string const& general);
			//! (todo)
			//! \param [in] arg    (todo)
			//! \return (todo)
			bool Contains(std::string const& arg) const;
			//! (todo)
			//! \param [in] arg    (todo)
			//! \return (todo)
			template <typename T>
			T Get(std::string const& arg) const; // [todo] ...

		private:
			class Impl;
			Impl* _impl;
	};

} // CommandLine
} // Etripator

#endif // COMMAND_LINE_H