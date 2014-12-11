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
	etripator::Memory::Result res;

	dummy.resize(16);

	// Read
	res = memRO.read(11, 7, &dummy[0]);
	CHECK_EQUAL(true, res.access);

	res = memWO.read(11, 7, &dummy[0]);
	CHECK_EQUAL(false, res.access);

	res = memRW.read(11, 7, &dummy[0]);
	CHECK_EQUAL(true, res.access);

	// Write
	res = memRO.write(1, 3, &dummy[0]);
	CHECK_EQUAL(false, res.access);

	res = memWO.write(1, 3, &dummy[0]);
	CHECK_EQUAL(true, res.access);
	
	res = memRW.write(1, 3, &dummy[0]);
	CHECK_EQUAL(true, res.access);
    }

    TEST(Clamp)
    {
	etripator::Memory memRWClamp(1, 128, etripator::Memory::ReadWrite, etripator::Memory::Clamp);
	etripator::Memory::Result res;
	
	std::vector<uint8_t> source;
	source.resize(memRWClamp.size());
	for(size_t i=0; i<memRWClamp.size(); i++)
	{
	    source[i] = 128 - i;
	}
	res = memRWClamp.write(0, 128, &source[0]);
	CHECK_EQUAL(true, res.access);

	size_t i;
	std::vector<uint8_t> dest;
	dest.resize(128);
	res = memRWClamp.read(0, 128, &dest[0]);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(128, res.bytes);
	for(i=0; i<dest.size(); i++)
	{
	    if(source[i] != dest[i])
	    {
		CHECK(false);
		break;
	    }
	}

	size_t offset = 27;
	size_t sz = 412;

	dest.clear();
        dest.resize(sz);

	res = memRWClamp.read(offset, sz, &dest[0]);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(sz, res.bytes);
	CHECK_EQUAL(sz-source.size()+offset, res.overflow);
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
	res = memRWClamp.read(offset, sz, &dest[0]);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(sz, res.bytes);
	CHECK_EQUAL(sz, res.overflow);
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
	etripator::Memory memROMirror(1, 32, etripator::Memory::ReadOnly, etripator::Memory::Repeat);
	std::vector<uint8_t> data;
	data.resize(32);
	for(size_t i=0; i<32; i++)
	{
	    data[i] = 32 - i;
	}
	memROMirror.flash(data.size(), &data[0]);
	
	etripator::Memory::Result res;
	std::vector<uint8_t> dummy;
	dummy.resize(64);
	size_t offset = 12;
	size_t size   = 64;
	res = memROMirror.read(offset, size, &dummy[0]);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(size, res.bytes);
	CHECK_EQUAL(size-(memROMirror.size()-offset), res.overflow);
	for(size_t i=0; i<dummy.size(); i++)
	{
	    if((32 - (i+offset)%32) != dummy[i])
	    {
		CHECK(false);
		break;
	    }
	}

	dummy.clear();
	dummy.resize(512);
	offset = 101;
	size   = 512;
	res = memROMirror.read(offset, size, &dummy[0]);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(size, res.bytes);
	CHECK_EQUAL(size, res.overflow);
	for(size_t i=0; i<dummy.size(); i++)
	{
	    if((32 - (i+offset)%32) != dummy[i])
	    {
		CHECK(false);
		break;
	    }
	}

	etripator::Memory memRWMirror(1, 160, etripator::Memory::ReadWrite, etripator::Memory::Repeat);
	offset = 92;
	size   = 128;
	res = memRWMirror.write(offset, size, &data[0]);
	memRWMirror.dump(dummy);
	CHECK_EQUAL(true, res.access);
	CHECK_EQUAL(size, res.bytes);
	CHECK_EQUAL(size-(memRWMirror.size()-offset), res.overflow);	
	for(size_t i=0; i<size; i++)
	{
	    if(data[i] != dummy[(i+offset)%dummy.size()])
	    {
		CHECK(false);
		break;
	    }
	}

	// [todo]
    }
}
