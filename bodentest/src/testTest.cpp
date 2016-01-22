#include <bdn/init.h>
#include <bdn/test.h>

using namespace bdn;


TEST_CASE("checkEquality")
{
	class Comparer
	{
	public:
		Comparer(int val)
		{
			_val = val;
		}

		bool invertEqual=false;

		bool operator==(const Comparer& o) const
		{
			bool result = (_val==o._val);
			if(invertEqual)
				return !result;
			else
				return result;
		}

		bool invertNotEqual=false;

		bool operator!=(const Comparer& o) const
		{
			bool result = (_val!=o._val);
			if(invertNotEqual)
				return !result;
			else
				return result;
		}
		
		int _val;
	};

	Comparer a(0);
	Comparer b(0);
	Comparer c(1);

	REQUIRE( checkEquality(a, b, true) );
	REQUIRE( checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( checkEquality(a, c, false) );
	REQUIRE( checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
		
	// results are inconsistent => should always get false
	a.invertEqual = true;
	REQUIRE( !checkEquality(a, b, true) );
	REQUIRE( !checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( !checkEquality(a, c, false) );
	REQUIRE( !checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
	a.invertEqual = false;

	a.invertNotEqual = true;
	REQUIRE( !checkEquality(a, b, true) );
	REQUIRE( !checkEquality(b, a, true) );
	REQUIRE( !checkEquality(a, b, false) );
	REQUIRE( !checkEquality(b, a, false) );

	REQUIRE( !checkEquality(a, c, false) );
	REQUIRE( !checkEquality(c, a, false) );
	REQUIRE( !checkEquality(a, c, true) );
	REQUIRE( !checkEquality(c, a, true) );
	a.invertNotEqual = false;
	
}


