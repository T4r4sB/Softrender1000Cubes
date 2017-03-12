#pragma once

namespace tblib
{
	template <typename B, typename D>
	class is_base_of
	{
		typedef char yes;
		typedef yes no[2];

		yes& test_base (B*);
		no& test_base (...);

	public:
		static bool const value = (sizeof(test_base((D*)(NULL))) == sizeof(yes));
	};
};