
#pragma once

// std
#include <string>
#include <sstream>


namespace cgra {

	namespace detail {
		inline void to_stream(std::ostream &) {}

		template <typename ArgT0, typename ...ArgTs>
		inline void to_stream(std::ostream &out, const ArgT0 &arg, const ArgTs &... args) {
			out << arg;
			to_stream(out, args...);
		}
	}


	// helper function to return a string from given arguments
	// requires every argument has an overload for the '<<' operator
	template <typename ...ArgTs>
	inline std::string stringf(const ArgTs &... args) {
		ostringstream oss;
		detail::to_stream(oss, args...);
		return oss.str();
	}

}