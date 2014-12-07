#include <unittest++/UnitTest++.h>
#include <etripator/memory.h>

SUITE(Memory)
{
    TEST(Access)
    {
	etripator::Memory memRO(1, 32, etripator::Memory::ReadOnly,  etripator::Memory::Clamp);
	etripator::Memory memWO(1, 32, etripator::Memory::WriteOnly, etripator::Memory::Clamp);
	etripator::Memory memRW(1, 32, etripator::Memory::ReadWrite, etripator::Memory::Clamp);

	std::vector<uint8_t> dummy;
	bool ret;

	// Read
	ret = memRO.read(11, 7, dummy);
	CHECK_EQUAL(true, ret);

	ret = memWO.read(11, 7, dummy);
	CHECK_EQUAL(false, ret);

	ret = memRW.read(11, 7, dummy);
	CHECK_EQUAL(true, ret);

	// Write
	ret = memRO.write(1, 3, dummy);
	CHECK_EQUAL(false, ret);

	ret = memWO.write(1, 3, dummy);
	CHECK_EQUAL(true, ret);
	
	ret = memRW.write(1, 3, dummy);
	CHECK_EQUAL(true, ret);
    }

    TEST(Clamp)
    {
	etripator::Memory memROClamp(1, 128, etripator::Memory::ReadOnly, etripator::Memory::Clamp);
	bool ret;
	
	std::vector<uint8_t> source;
	source.resize(memROClamp.size());
	for(size_t i=0; i<memROClamp.size(); i++)
	{
	    source[i] = 128 - i;
	}
	ret = memROClamp.write(0, 128, source);
	CHECK_EQUAL(false, ret);

	size_t i;
	std::vector<uint8_t> dest;
	ret = memROClamp.read(0, 128, dest);
	CHECK_EQUAL(true, ret);
	CHECK_EQUAL(128, dest.size());
	for(i=0; i<dest.size(); i++)
	{
	    if(source[i] != dest[i])
	    {
		CHECK(false);
		break;
	    }
	}

	dest.clear();
	size_t offset = 27;
	size_t sz = 412;
        ret = memROClamp.read(offset, sz, dest);
	CHECK_EQUAL(true, ret);
	CHECK_EQUAL(sz, dest.size());
	for(i=0; i<(source.size()-offset); i++)
	{
	    if(source[offset+i] != dest[i])
	    {
		CHECK(false);
		break;
	    }
	}
	for(; i<sz; i++)
	{
	    if(0xff != dest[i])
	    {
		CHECK(false);
		break;
	    }
	}

	dest.clear();
	offset = 511;
	sz = 32;
	ret = memROClamp.read(offset, sz, dest);
	CHECK_EQUAL(true, ret);
	CHECK_EQUAL(sz, dest.size());
	for(i=0; i<sz; i++)
	{
	    if(0xff != dest[i])
	    {
		CHECK(false);
		break;
	    }
	}
    }

    TEST(Repeat)
    {
	// etripator::Memory memRWMirror(2, 32, etripator::Memory::ReadWrite, etripator::Memory::Repeat);
	// [todo]
    }
}
