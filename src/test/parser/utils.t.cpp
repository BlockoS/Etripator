#include <unittest++/UnitTest++.h>
#include <etripator/parser/utils.h>

SUITE(Parser)
{
    TEST(Chomp)
    {
        struct
        {
	    const char* str;
	    size_t start, end;
        } expected[] =
        {
    	    {"   test", 3, 6 },
	    {"\n\t  \t012\tabcd  \r", 5, 12},
	    {"_", 0, 0},
	    {"+-_]", 0, 3} 
        };
        for(size_t i=0; i<(sizeof(expected)/sizeof(expected[0])); i++)
	{
	    size_t start, end;
	    etripator::parser::chomp(expected[i].str, start, end);
            CHECK_EQUAL(expected[i].start, start);
            CHECK_EQUAL(expected[i].end, end);
	}
    }

    TEST(ParseSizeOK)
    {
        struct
	{
	    const char* name;
	    size_t value;
	} expected[] = 
	{
	    {"1",       1},
            {"  4  ",   4},  
	    {" 9017  ", 9017},
	    {"    07b", 7},
            {"081200B", 649600},
	    {"1024Kb ", 1048576},
	    {"511KB  ", 4186112},
	    {"    03M", 3145728}
	};

	for(size_t i=0; i<(sizeof(expected)/sizeof(expected[0])); i++)
	{
	    size_t value;
	    bool ret = etripator::parser::parseSize(expected[i].name, value);
            CHECK(ret);
            CHECK_EQUAL(expected[i].value, value);
	}
    }

    TEST(ParseSizeFAIL)
    {
        const char* name[] = {
	    "B",
	    "0137KKMb",
	    "129b98Mb",
	    " 9 b",
            "98!",
            "     K128b " 
	};

	for(size_t i=0; i<(sizeof(name)/sizeof(name[0])); i++)
	{
	    size_t value;
	    bool ret = etripator::parser::parseSize(name[i], value);
            CHECK_EQUAL(false, ret);
	}
    }
}
