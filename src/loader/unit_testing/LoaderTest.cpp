/***************************************************************************/ /**
 * \file       LoaderTest.cc
 * OVERVIEW:   Provides the implementation for the LoaderTest class, which
 *              tests the BinaryFile and derived classes
 ******************************************************************************/

/*
 * $Revision$
 *
 * 05 Apr 02 - Mike: Created
 * 14 Jun 02 - Mike: Added windows test for calc.exe
 * 20 Jun 02 - Mike: Added test for microX86Dis
 * 08 Jul 02 - Mike: Test more exe file formats
 * 30 Sep 02 - Trent: split microDis tests
 * 30 Sep 02 - Trent: remove dependancy between microDis test1 and Elf loader
 * 05 Aug 05 - Mike: added borland test; check address of main (not just != NO_ADDRESS)
 */

#define HELLO_SPARC       baseDir.absoluteFilePath("tests/inputs/sparc/hello")
#define HELLO_PENTIUM     baseDir.absoluteFilePath("tests/inputs/pentium/hello")
#define HELLO_HPPA        baseDir.absoluteFilePath("tests/inputs/hppa/hello")
#define STARTER_PALM      baseDir.absoluteFilePath("tests/inputs/mc68328/Starter.prc")
#if 0 /* FIXME: these programs are proprietary */
#define CALC_WINDOWS      "tests/inputs/windows/calc.exe"
#define CALC_WINXP        "tests/inputs/windows/calcXP.exe"
#define CALC_WIN2000      "tests/inputs/windows/calc2000.exe"
#define LPQ_WINDOWS       "tests/inputs/windows/lpq.exe"
#endif
#define SWITCH_BORLAND    baseDir.absoluteFilePath("tests/inputs/windows/switch_borland.exe")
#ifdef _WIN32
#define ELFBINFILE        baseDir.absoluteFilePath("out/lib/ElfBinaryFile.dll")
#else
#define ELFBINFILE        baseDir.absoluteFilePath("out/lib/ElfBinaryFile.so")
#endif

#include "../microX86dis.c"
#include "LoaderTest.h"
#include "include/boomerang.h"
#include "include/IBinaryImage.h"
#include "include/log.h"

#include <QLibrary>
#include <QTextStream>
#include <QDir>
#include <QProcessEnvironment>
#include <QDebug>
#include <sstream>

static bool    logset = false;
static QString TEST_BASE;
static QDir    baseDir;
void LoaderTest::initTestCase()
{
	if (!logset) {
		TEST_BASE = QProcessEnvironment::systemEnvironment().value("BOOMERANG_TEST_BASE", "");
		baseDir   = QDir(TEST_BASE);

		if (TEST_BASE.isEmpty()) {
			qWarning() << "BOOMERANG_TEST_BASE environment variable not set, will assume '..', many test may fail";
			TEST_BASE = "..";
			baseDir   = QDir("..");
		}

		logset = true;
		Boomerang::get()->setProgPath(TEST_BASE);
		Boomerang::get()->setPluginPath(TEST_BASE + "/out");
		Boomerang::get()->setLogger(new NullLogger());
	}
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testSparcLoad
 * OVERVIEW:        Test loading the sparc hello world program
 ******************************************************************************/
void LoaderTest::testSparcLoad()
{
	QString     actual;
	QTextStream ost(&actual);

	// Load SPARC hello world
	BinaryFileFactory bff;
	QObject           *pBF = bff.Load(HELLO_SPARC);

	QVERIFY(pBF != nullptr);
	int n;
	const IBinarySection *si;
	IBinaryImage         *sect_iface = Boomerang::get()->getImage();
	QVERIFY(sect_iface != nullptr);

	n = sect_iface->GetNumSections();
	ost << "Number of sections = " << n << "\r\n\t";
	// Just use the first (real one) and last sections
	si = sect_iface->GetSectionInfo(1);
	ost << si->getName() << "\t";
	si = sect_iface->GetSectionInfo(n - 1);
	ost << si->getName();
	// Note: the string below needs to have embedded tabs. Edit with caution!
	QString expected("Number of sections = 28\r\n\t"
					 ".hash\t.stab.indexstr");
	QCOMPARE(actual, expected);
	bff.UnLoad();
	delete pBF;
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testPentiumLoad
 * OVERVIEW:        Test loading the pentium (Solaris) hello world program
 ******************************************************************************/
void LoaderTest::testPentiumLoad()
{
	QString     actual;
	QTextStream ost(&actual);

	// Load Pentium hello world
	BinaryFileFactory bff;
	QObject           *pBF = bff.Load(HELLO_PENTIUM);

	QVERIFY(pBF != nullptr);
	IBinaryImage *sect_iface = Boomerang::get()->getImage();
	QVERIFY(sect_iface != nullptr);
	int n;
	const IBinarySection *si;
	n = sect_iface->GetNumSections();
	ost << "Number of sections = " << n << "\r\n\t";
	si = sect_iface->GetSectionInfo(1);
	ost << si->getName() << "\t";
	si = sect_iface->GetSectionInfo(n - 1);
	ost << si->getName();
	// Note: the string below needs to have embedded tabs. Edit with caution!
	// (And slightly different string to the sparc test, e.g. rel vs rela)
	QString expected("Number of sections = 33\r\n\t"
					 ".note.ABI-tag\t.strtab");

	QCOMPARE(actual, expected);
	bff.UnLoad();
	delete pBF;
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testHppaLoad
 * OVERVIEW:        Test loading the sparc hello world program
 ******************************************************************************/
void LoaderTest::testHppaLoad()
{
	QString     actual;
	QTextStream ost(&actual);

	// Load HPPA hello world
	BinaryFileFactory bff;
	QObject           *pBF = bff.Load(HELLO_HPPA);

	QVERIFY(pBF != nullptr);
	IBinaryImage *sect_iface = Boomerang::get()->getImage();
	QVERIFY(sect_iface != nullptr);
	int n;
	const IBinarySection *si;
	n = sect_iface->GetNumSections();
	ost << "Number of sections = " << n << "\r\n";

	for (int i = 0; i < n; i++) {
		si = sect_iface->GetSectionInfo(i);
		ost << si->getName() << "\t";
	}

	// Note: the string below needs to have embedded tabs. Edit with caution!
	QString expected("Number of sections = 3\r\n"
					 "$TEXT$\t$DATA$\t$BSS$\t");
	QCOMPARE(actual, expected);
	bff.UnLoad();
	delete pBF;
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testPalmLoad
 * OVERVIEW:        Test loading the Palm 68328 Starter.prc program
 ******************************************************************************/
void LoaderTest::testPalmLoad()
{
	QString     actual;
	QTextStream ost(&actual);

	// Load Palm Starter.prc
	BinaryFileFactory bff;
	QObject           *pBF = bff.Load(STARTER_PALM);

	QVERIFY(pBF != nullptr);
	IBinaryImage *sect_iface = Boomerang::get()->getImage();
	QVERIFY(sect_iface != nullptr);
	int n;
	const IBinarySection *si;
	n = sect_iface->GetNumSections();
	ost << "Number of sections = " << n << "\r\n";

	for (int i = 0; i < n; i++) {
		si = sect_iface->GetSectionInfo(i);
		ost << si->getName() << "\t";
	}

	// Note: the string below needs to have embedded tabs. Edit with caution!
	QString expected("Number of sections = 8\r\n"
					 "code1\tMBAR1000\ttFRM1000\tTalt1001\t"
					 "data0\tcode0\ttAIN1000\ttver1000\t");
	QCOMPARE(actual, expected);
	bff.UnLoad();
	delete pBF;
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testWinLoad
 * OVERVIEW:        Test loading Windows programs
 ******************************************************************************/
void LoaderTest::testWinLoad()
{
#if 0 /* FIXME: these tests should use non-proprietary programs */
	// Load Windows program calc.exe
	BinaryFileFactory bff;
	BinaryFile        *pBF = bff.Load(CALC_WINDOWS);
	QVERIFY(pBF != nullptr);
	int            n;
	IBinarySection *si;
	n = pBF->GetNumSections();
	ost << "Number of sections = " << std::dec << n << "\r\n";

	for (int i = 0; i < n; i++) {
		si = pBF->GetSectionInfo(i);
		ost << si->getName() << "\t";
	}

	// Note: the string below needs to have embedded tabs. Edit with caution!
	QString expected("Number of sections = 5\r\n"
					 ".text    .rdata    .data    .rsrc    .reloc    ");
	QString actual(ost.str());
	QCOMPARE(actual, expected);

	ADDRESS addr = pBF->GetMainEntryPoint();
	QVERIFY(addr != NO_ADDRESS);

	// Test symbol table (imports)
	const char *s = pBF->SymbolByAddress(0x1292060U);

	if (s == 0) {
		actual = "<not found>";
	}
	else{
		actual = s;
	}

	expected = "SetEvent";
	QCOMPARE(actual, expected);

	ADDRESS a            = pBF->GetAddressByName("SetEvent");
	ADDRESS expectedAddr = 0x1292060;
	QCOMPARE(a, expectedAddr);
	pBF->UnLoad();
	bff.UnLoad();

	// Test loading the "new style" exes, as found in winXP etc
	pBF = bff.Load(CALC_WINXP);
	QVERIFY(pBF != nullptr);
	addr = pBF->GetMainEntryPoint();
	std::ostringstream ost1;
	ost1 << std::hex << addr;
	actual   = ost1.str();
	expected = "1001f51";
	QCOMPARE(actual, expected);
	pBF->UnLoad();
	bff.UnLoad();

	// Test loading the calc.exe found in Windows 2000 (more NT based)
	pBF = bff.Load(CALC_WIN2000);
	QVERIFY(pBF != nullptr);
	expected = "1001680";
	addr     = pBF->GetMainEntryPoint();
	std::ostringstream ost2;
	ost2 << std::hex << addr;
	actual = ost2.str();
	QCOMPARE(actual, expected);
	pBF->UnLoad();
	bff.UnLoad();

	// Test loading the lpq.exe program - console mode PE file
	pBF = bff.Load(LPQ_WINDOWS);
	QVERIFY(pBF != nullptr);
	addr = pBF->GetMainEntryPoint();
	std::ostringstream ost3;
	ost3 << std::hex << addr;
	actual   = ost3.str();
	expected = "18c1000";
	QCOMPARE(actual, expected);
	pBF->UnLoad();
	bff.UnLoad();
#endif

	// Borland
	BinaryFileFactory bff;
	QObject           *pBF = bff.Load(SWITCH_BORLAND);
	QVERIFY(pBF != nullptr);
	LoaderInterface *ldr_iface = qobject_cast<LoaderInterface *>(pBF);
	QVERIFY(ldr_iface != nullptr);
	ADDRESS     addr = ldr_iface->GetMainEntryPoint();
	QString     actual;
	QTextStream ost4(&actual);
	ost4 << addr;
	QString expected("401150");
	QCOMPARE(actual, expected);
	bff.UnLoad();
}


/***************************************************************************/ /**
 * \fn        LoaderTest::testMicroDis
 * OVERVIEW:        Test the micro disassembler
 ******************************************************************************/


// The below lengths were derived from a quick and dirty program (called
// quick.c) which used the output from a disassembly to find the lengths.
// Best way to test, but of course this array is very dependent on the
// exact booked in test program
static char lengths[] =
{
	2, 2, 2, 1, 5, 2, 2, 5, 5, 3, 5, 2,               2,                5, 5, 5, 3, 4, 6, 1,                3, 1, 1, 5, 5, 5, 3,
	1, 5, 2, 5, 7, 1, 1, 1, 2, 1, 5, 1,               6,                2, 1, 1, 3, 6, 2, 2,                6, 3, 2, 6, 1, 5, 3,
	1, 1, 1, 1, 1, 1, 2, 1, 5, 1, 6, 3,               1,                1, 1, 1, 1, 1, 2, 1,                5, 1, 6, 6, 1, 6, 1,
	5, 3, 1, 1, 1, 2, 1, 5, 1, 6, 3, 1,               1, /* main */ 2,     3, 1, 5, 5, 3, 2,                2, 1, /* label */ 1,2, 1, 2, 1,
	1, 2, 2, 1, 1, 1, 3, 3, 1, 3, 2, 5,               2,                2, 2, 2, 2, 3, 2, 3,                2, 3, 3, 1, 1, 1, 1,
	1, 1, 2, 3, 1, 1, 3, 7, 3, 1, 1, 1,               3,                1, 2, 5, 2, 3, 3, 2,                2, 2, 3, 2, 6, 2, 5,
	2, 3, 3, 3, 2, 2, 3, 1, 1, 1, 1, 1,               1,                1, 1, 2, 3, 1, 1, 3,                3, 3, 3, 2, 2, 3, 1,
	2, 3, 3, 4, 3, 3, 3, 2, 2, 2, 2, 3,               2,                3, 3, 4, 3, 1, 2, 3,                1, 1, 1, 1, 1, 1, 1,
	1, 2, 3, 2, 3, 2, 1, 1, 1, 4, 2, 4,               2,                1, 2, 2, 3, 4, 2, 2,                1, 1, 1, 1, 1, 2, 3,
	1, 1, 1, 5, 1, 6, 3, 3, 2, 3, 2, 3,               3,                2, 3, 3, 2, 1, 1, 4,                2, 4, 2, 1, 1, 1, 3,
	5, 3, 3, 3, 2, 3, 3, 3, 2, 3, 2, 2,               3,                4, 2, 3, 2, 3, 3, 2,                3, 3, 2, 3, 1, 1, 1,
	1, 1, 1, 1, 1, 2, 3, 1, 1, 1, 5, 1,               6,                3, 3, 2, 2, 2, 7, 3,                2, 1, 1, 1, 2, 5, 3,
	3, 3, 3, 2, 2, 1, 3, 3, 5, 3, 3, 3,               3,                3, 3, 1, 5, 2, 7, 2,                3, 3, 3, 3, 3, 2, 2,
	2, 3, 2, 3, 3, 1, 1, 3, 3, 1, 3, 1,               1,                2, 5, 3, 3, 3, 2, 2,                3, 1, 3, 1, 3, 1, 1,
	3, 3, 5, 3, 3, 3, 2, 3, 3, 3, 1, 1,               1,                1, 1, 1, 1, 1, 1, 2,                3, 1, 1, 1, 5, 1, 6,
	6, 2, 2, 1, 3, 2, 1, 5, 3, 3, 2, 2,               3,                2, 3, 2, 2, 2, 2, 2,                1, 3, 2, 1, 1, 1, 2,
	3, 3, 2, 2, 3, 1, 3, 3, 2, 2, 3, 3,               3,                3, 2, 3, 2, 1, 1, 1,                3, 3, 3, 2, 3, 3, 2,
	2, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,               3,                1, 1, 1, 5, 1, 6, 3,                3, 5, 2, 3, 3, 3, 2,
	3, 6, 3, 5, 2, 1, 2, 2, 2, 3, 6, 5,               1,                2, 2, 2, 4, 2, 2, 5,                1, 1, 1, 3, 2, 3, 2,
	2, 2, 1, 5, 2, 2, 3, 4, 3, 2, 1, 3,               3,                6, 3, 5, 1, 2, 2, 2,                3, 3, 3, 3, 3, 2, 1,
	1, 1, 3, 7, 3, 5, 1, 1, 5, 2, 3, 3,               1,                1, 5, 2, 3, 3, 3, 1,                2, 3, 3, 2, 3, 1, 1,
	5, 2, 3, 2, 3, 1, 1, 1, 1, 1, 1, 1,               2,                3, 1, 1, 1, 5, 1, 6,                3, 3, 3, 3, 1, 3, 2,
	6, 3, 2, 5, 4, 2, 5, 1, 1, 2, 2, 5,               3,                3, 1, 3, 5, 3, 3, 4,                3, 3, 3, 5, 3, 7, 3,
	4, 5, 1, 1, 2, 2, 5, 3, 3, 3, 4, 5,               1,                5, 6, 2, 7, 2, 1, 1,                2, 2, 2, 2, 6, 2, 3,
	2, 2, 4, 3, 2, 2, 2, 1, 2, 6, 2, 3,               2,                2, 2, 3, 3, 2, 2, 2,                2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,               2,                2, 2, 2, 2, 2, 2, 2,                2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,               2,                2, 2, 2, 2, 2, 2, 2,                2, 2, 3, 2, 2, 6, 2,
	3, 3, 5, 1, 1, 3, 3, 2, 1, 3, 5, 1,               1,                1, 3, 3, 2, 3, 3, 5,                1, 2, 3, 2, 2, 3, 3,
	5, 1, 1, 1, 1, 3, 1, 3, 5, 3, 3, 1,               3,                5, 3, 3, 4, 3, 3, 3,                5, 3, 7, 3, 4, 5, 1,
	1, 1, 3, 1, 3, 5, 3, 3, 3, 5, 5, 1,               3,                1, 3, 5, 3, 3, 1, 3,                5, 3, 3, 3, 5, 3, 7,
	3, 4, 5, 1, 3, 1, 3, 5, 3, 3, 1, 3,               5,                3, 3, 3, 4, 3, 3, 5,                1, 3, 1, 3, 5, 3, 3,
	3, 4, 5, 1, 1, 3, 1, 3, 5, 3, 3, 3,               3,                5, 1, 1, 1, 2, 5, 2,                2, 3, 2, 1, 5, 2, 3,
	3, 2, 3, 3, 2, 2, 2, 1, 5, 2, 1, 5,               2,                7, 1, 3, 3, 5, 3, 7,                4, 3, 3, 2, 5, 2, 2,
	1, 1, 3, 1, 3, 5, 3, 3, 3, 3, 2, 1,               1,                5, 1, 1, 1, 3, 3, 1,                1, 1, 1, 1, 1, 1, 2,
	1, 5, 1, 6, 3, 3, 3, 7, 6, 7, 7, 6,               3,                6, 3, 1, 1, 1, 2, 1,                5, 1, 6, 3, 3, 3, 3,
	7, 6, 7, 6, 3, 6, 3, 1, 1, 1, 2, 1,               5,                1, 6, 3, 6, 7, 2, 1,                1, 2, 3, 2, 3, 2, 3,
	2, 3, 5, 2, 1, 3, 4, 2, 5, 1, 1, 3,               1,                1, 1, 1, 1, 1, 2, 6,                1, 1, 1, 5, 1, 6, 3,
	5, 6, 3, 2, 6, 3, 6, 1, 6, 5, 2, 3,               2,                6, 2, 2, 6, 6, 1, 5,                3, 4, 3, 6, 3, 6, 3,
	5, 2, 2, 2, 3, 2, 2, 6, 6, 6, 6, 1,               2,                6, 6, 1, 5, 2, 3, 2,                2, 6, 3, 3, 3, 2, 6,
	1, 1, 5, 2, 6, 3, 6, 2, 3, 6, 3, 6,               2,                2, 6, 6, 3, 6, 2, 6,                3, 1, 6, 1, 1, 5, 2,
	3, 2, 2, 3, 6, 1, 5, 2, 3, 6, 1, 1,               1,                1, /* label */ 1,2, 1, 2, 1, 1,                5, 1, 6, 6, 3, 4, 2,
	2, 2, 3, 3, 2, 3, 1, 1, 1, 1, 1, 1,               2,                1, 5, 1, 6, 3, 1, 1
};

// text segment of hello pentium
static unsigned char pent_hello_text[] =
{
	0x6a,  0x00, 0x6a,  0x00, 0x8b,  0xec, 0x52,  0xb8, 0x80,  0x87, 0x04,  0x08, 0x85,  0xc0, 0x74,  0x0d, 0x68,  0x80,
	0x87,  0x04, 0x08,  0xe8, 0x66,  0xff, 0xff,  0xff, 0x83,  0xc4, 0x04,  0xb8, 0x44,  0xa4, 0x04,  0x08, 0x85,  0xc0,
	0x74,  0x05, 0xe8,  0x55, 0xff,  0xff, 0xff,  0x68, 0xe0,  0x93, 0x04,  0x08, 0xe8,  0x4b, 0xff,  0xff, 0xff,  0x8b,
	0x45,  0x08, 0x8d,  0x54, 0x85,  0x10, 0x89,  0x15, 0x0c,  0xaa, 0x04,  0x08, 0x52,  0x8d, 0x55,  0x0c, 0x52,  0x50,
	0xe8,  0x53, 0x0b,  0x00, 0x00,  0xe8, 0x3e,  0xff, 0xff,  0xff, 0xe8,  0xb1, 0x00,  0x00, 0x00,  0x83, 0xc4,  0x0c,
	0x50,  0xe8, 0x40,  0xff, 0xff,  0xff, 0x6a,  0x00, 0xb8,  0x01, 0x00,  0x00, 0x00,  0x9a, 0x00,  0x00, 0x00,  0x00,
	0x07,  0x00, 0xf4,  0xc3, 0x55,  0x8b, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0x8b,  0x1b,
	0x00,  0x00, 0xeb,  0x0f, 0x90,  0x90, 0x8d,  0x50, 0x04,  0x89, 0x93,  0xc8, 0x00,  0x00, 0x00,  0x8b, 0x00,  0xff,
	0xd0,  0x8b, 0x83,  0xc8, 0x00,  0x00, 0x00,  0x83, 0x38,  0x00, 0x75,  0xe8, 0x8d,  0x83, 0xdc,  0x00, 0x00,  0x00,
	0x50,  0xe8, 0xe8,  0x08, 0x00,  0x00, 0x8b,  0x5d, 0xfc,  0xc9, 0xc3,  0x90, 0x90,  0x90, 0x55,  0x8b, 0xec,  0x53,
	0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0x4b,  0x1b, 0x00,  0x00, 0x8b,  0x5d, 0xfc,  0xc9, 0xc3,  0x90,
	0x90,  0x90, 0x55,  0x8b, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0x33,  0x1b, 0x00,  0x00,
	0x8d,  0x83, 0xdc,  0x05, 0x00,  0x00, 0x50,  0x8d, 0x83,  0xdc, 0x00,  0x00, 0x00,  0x50, 0xe8,  0x19, 0x08,  0x00,
	0x00,  0x8b, 0x5d,  0xfc, 0xc9,  0xc3, 0x55,  0x8b, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3,
	0x0b,  0x1b, 0x00,  0x00, 0x8b,  0x5d, 0xfc,  0xc9, 0xc3,  0x00, 0x00,  0x00, 0x55,  0x8b, 0xec,  0x68, 0xf8,  0x93,
	0x04,  0x08, 0xe8,  0x9b, 0xfe,  0xff, 0xff,  0x83, 0xc4,  0x04, 0x33,  0xc0, 0xeb,  0x00, 0xc9,  0xc3, 0x00,  0x00,
	0x55,  0x8b, 0xec,  0x57, 0x56,  0x33, 0xff,  0x8b, 0xf7,  0x90, 0x90,  0x90, 0x8b,  0x4d, 0x08,  0x0f, 0xb6,  0x11,
	0x41,  0x89, 0x4d,  0x08, 0x8b,  0xc2, 0x25,  0x7f, 0x00,  0x00, 0x00,  0x8b, 0xce,  0xd3, 0xe0,  0x0b, 0xf8,  0x84,
	0xd2,  0x7d, 0x05,  0x83, 0xc6,  0x07, 0xeb,  0xe0, 0x8b,  0x45, 0x0c,  0x89, 0x38,  0x8b, 0x45,  0x08, 0x8d,  0x65,
	0xf8,  0x5e, 0x5f,  0xc9, 0xc3,  0x90, 0x55,  0x8b, 0xec,  0x83, 0xec,  0x04, 0x57,  0x56, 0x8b,  0x7d, 0x08,  0xc7,
	0x45,  0xfc, 0x00,  0x00, 0x00,  0x00, 0x8b,  0x4d, 0xfc,  0x90, 0x90,  0x90, 0x0f,  0xb6, 0x37,  0x47, 0x8b,  0xc6,
	0x25,  0x7f, 0x00,  0x00, 0x00,  0xd3, 0xe0,  0x09, 0x45,  0xfc, 0x83,  0xc1, 0x07,  0x8b, 0xd6,  0x84, 0xd2,  0x7c,
	0xe7,  0x83, 0xf9,  0x1f, 0x77,  0x12, 0xf7,  0xc6, 0x40,  0x00, 0x00,  0x00, 0x74,  0x0a, 0xb8,  0xff, 0xff,  0xff,
	0xff,  0xd3, 0xe0,  0x09, 0x45,  0xfc, 0x8b,  0x45, 0x0c,  0x8b, 0x55,  0xfc, 0x89,  0x10, 0x8b,  0xc7, 0x8d,  0x65,
	0xf4,  0x5e, 0x5f,  0xc9, 0xc3,  0x90, 0x90,  0x90, 0x55,  0x8b, 0xec,  0x83, 0xec,  0x08, 0x57,  0x56, 0x8b,  0x55,
	0x0c,  0x8b, 0x45,  0x10, 0x8b,  0x75, 0x08,  0x89, 0x04,  0x96, 0x85,  0xd2, 0x74,  0x36, 0x8d,  0x0c, 0x96,  0x90,
	0x8b,  0x39, 0x89,  0x7d, 0xfc,  0x8b, 0x75,  0x08, 0x8b,  0x74, 0x96,  0xfc, 0x89,  0x75, 0xf8,  0x8b, 0x46,  0x08,
	0x8b,  0x77, 0x08,  0x2b, 0xf0,  0x8b, 0xc6,  0x85, 0xc0,  0x7d, 0x15,  0x8b, 0x7d,  0xf8, 0x89,  0x39, 0x8b,  0x7d,
	0xfc,  0x8b, 0x75,  0x08, 0x89,  0x7c, 0x96,  0xfc, 0x83,  0xc1, 0xfc,  0x4a, 0x75,  0xce, 0x8d,  0x65, 0xf0,  0x5e,
	0x5f,  0xc9, 0xc3,  0x90, 0x90,  0x90, 0x55,  0x8b, 0xec,  0x8b, 0x55,  0x08, 0x33,  0xc9, 0x83,  0x3a, 0x00,  0x74,
	0x1d,  0x90, 0x90,  0x90, 0x83,  0x7a, 0x04,  0x00, 0x74,  0x07, 0x83,  0x7a, 0x08,  0x00, 0x74,  0x01, 0x41,  0x8b,
	0xc2,  0x03, 0x02,  0x8d, 0x50,  0x04, 0x83,  0x78, 0x04,  0x00, 0x75,  0xe6, 0x8b,  0xc1, 0xc9,  0xc3, 0x90,  0x90,
	0x55,  0x8b, 0xec,  0x83, 0xec,  0x08, 0x57,  0x56, 0x53,  0xe8, 0x00,  0x00, 0x00,  0x00, 0x5b,  0x81, 0xc3,  0xb6,
	0x19,  0x00, 0x00,  0x8b, 0x75,  0x08, 0x8b,  0x55, 0x10,  0x8b, 0x3a,  0x8b, 0x4d,  0x14, 0x8b,  0x09, 0x89,  0x4d,
	0xfc,  0x8b, 0x55,  0x18, 0x8b,  0x12, 0x89,  0x55, 0xf8,  0x83, 0x3e,  0x00, 0x74,  0x3f, 0x90,  0x90, 0x83,  0x7e,
	0x04,  0x00, 0x74,  0x2a, 0x83,  0x7e, 0x08,  0x00, 0x74,  0x24, 0x56,  0x57, 0x47,  0xff, 0x75,  0x0c, 0xe8,  0x2d,
	0xff,  0xff, 0xff,  0x8b, 0x46,  0x08, 0x83,  0xc4, 0x0c,  0x39, 0x45,  0xfc, 0x76,  0x03, 0x89,  0x45, 0xfc,  0x03,
	0x46,  0x0c, 0x39,  0x45, 0xf8,  0x73, 0x03,  0x89, 0x45,  0xf8, 0x8b,  0xc6, 0x03,  0x06, 0x8d,  0x70, 0x04,  0x83,
	0x78,  0x04, 0x00,  0x75, 0xc3,  0x8b, 0x4d,  0x10, 0x89,  0x39, 0x8b,  0x4d, 0xfc,  0x8b, 0x55,  0x14, 0x89,  0x0a,
	0x8b,  0x4d, 0xf8,  0x8b, 0x55,  0x18, 0x89,  0x0a, 0x8d,  0x65, 0xec,  0x5b, 0x5e,  0x5f, 0xc9,  0xc3, 0x90,  0x90,
	0x55,  0x8b, 0xec,  0x83, 0xec,  0x10, 0x57,  0x56, 0x53,  0xe8, 0x00,  0x00, 0x00,  0x00, 0x5b,  0x81, 0xc3,  0x26,
	0x19,  0x00, 0x00,  0x8b, 0x55,  0x08, 0x8b,  0x42, 0x0c,  0x85, 0xc0,  0x74, 0x29,  0x8b, 0xf0,  0xc7, 0x45,  0xf4,
	0x00,  0x00, 0x00,  0x00, 0x83,  0x3e, 0x00,  0x74, 0x2c,  0x90, 0x90,  0x90, 0xff,  0x36, 0xe8,  0x09, 0xff,  0xff,
	0xff,  0x01, 0x45,  0xf4, 0x83,  0xc4, 0x04,  0x83, 0xc6,  0x04, 0x83,  0x3e, 0x00,  0x75, 0xeb,  0xeb, 0x12,  0x90,
	0x8b,  0x55, 0x08,  0xff, 0x72,  0x08, 0xe8,  0xed, 0xfe,  0xff, 0xff,  0x89, 0x45,  0xf4, 0x83,  0xc4, 0x04,  0x8b,
	0x45,  0xf4, 0x8b,  0x55, 0x08,  0x89, 0x42,  0x10, 0xc1,  0xe0, 0x02,  0x50, 0xe8,  0x85, 0xfc,  0xff, 0xff,  0x8b,
	0xf8,  0xc7, 0x45,  0xf8, 0xff,  0xff, 0xff,  0xff, 0x33,  0xc0, 0x89,  0x45, 0xfc,  0x89, 0x45,  0xf4, 0x83,  0xc4,
	0x04,  0x8b, 0x55,  0x08, 0x8b,  0x42, 0x0c,  0x85, 0xc0,  0x74, 0x2f,  0x8b, 0xf0,  0x83, 0x3e,  0x00, 0x74,  0x40,
	0x8d,  0x55, 0xfc,  0x89, 0x55,  0xf0, 0x90,  0x90, 0xff,  0x75, 0xf0,  0x8d, 0x45,  0xf8, 0x50,  0x8d, 0x45,  0xf4,
	0x50,  0x57, 0xff,  0x36, 0xe8,  0xc5, 0xfe,  0xff, 0xff,  0x83, 0xc4,  0x14, 0x83,  0xc6, 0x04,  0x83, 0x3e,  0x00,
	0x75,  0xe2, 0xeb,  0x18, 0x8d,  0x45, 0xfc,  0x50, 0x8d,  0x45, 0xf8,  0x50, 0x8d,  0x45, 0xf4,  0x50, 0x57,  0x8b,
	0x55,  0x08, 0xff,  0x72, 0x08,  0xe8, 0xa0,  0xfe, 0xff,  0xff, 0x8b,  0x55, 0x08,  0x89, 0x7a,  0x0c, 0x8b,  0x45,
	0xf8,  0x89, 0x02,  0x8b, 0x45,  0xfc, 0x89,  0x42, 0x04,  0x8d, 0x65,  0xe4, 0x5b,  0x5e, 0x5f,  0xc9, 0xc3,  0x90,
	0x90,  0x90, 0x55,  0x8b, 0xec,  0x83, 0xec,  0x08, 0x57,  0x56, 0x53,  0xe8, 0x00,  0x00, 0x00,  0x00, 0x5b,  0x81,
	0xc3,  0x3a, 0x18,  0x00, 0x00,  0x8b, 0xb3,  0xf4, 0x05,  0x00, 0x00,  0x85, 0xf6,  0x74, 0x74,  0x90, 0x83,  0x3e,
	0x00,  0x75, 0x09,  0x56, 0xe8,  0xe9, 0xfe,  0xff, 0xff,  0x83, 0xc4,  0x04, 0x8b,  0x4d, 0x08,  0x39, 0x0e,  0x77,
	0x05,  0x39, 0x4e,  0x04, 0x77,  0x07, 0x8b,  0x76, 0x14,  0x85, 0xf6,  0x75, 0xdf,  0x85, 0xf6,  0x75, 0x0b,  0xeb,
	0x4c,  0x90, 0x8b,  0x45, 0xf8,  0xeb, 0x48,  0x90, 0x90,  0x90, 0x33,  0xff, 0x8b,  0x4e, 0x10,  0x89, 0x4d,  0xfc,
	0x3b,  0xf9, 0x73,  0x37, 0x8b,  0x76, 0x0c,  0x90, 0x8b,  0x4d, 0xfc,  0x8d, 0x04,  0x39, 0x8b,  0xd0, 0xd1,  0xea,
	0x8b,  0x0c, 0x96,  0x89, 0x4d,  0xf8, 0x8b,  0x41, 0x08,  0x39, 0x45,  0x08, 0x73,  0x08, 0x89,  0x55, 0xfc,  0xeb,
	0x11,  0x90, 0x90,  0x90, 0x8b, 0x4du, 0xf8,  0x03, 0x41,  0x0c, 0x39,  0x45, 0x08,  0x76, 0xbd,  0x8d, 0x7a,  0x01,
	0x39,  0x7d, 0xfc, 0x77u, 0xcd,  0x33, 0xc0,  0x8d, 0x65,  0xec, 0x5b,  0x5e, 0x5f,  0xc9, 0xc3,  0x90, 0x90,  0x90,
	0x55, 0x8bu, 0xec,  0x83, 0xec,  0x18, 0x57,  0x56, 0x53,  0xe8, 0x00,  0x00, 0x00,  0x00, 0x5b,  0x81, 0xc3, 0x9au,
	0x17,  0x00, 0x00,  0x8b, 0x55,  0x08, 0x8b,  0x42, 0x04,  0x05, 0xfc,  0xff, 0xff,  0xff, 0x2b, 0xd0u, 0x8d,  0x72,
	0x09,  0x89, 0x75,  0xf4, 0x8b,  0x7d, 0x0c,  0x89, 0x37,  0x89, 0x75,  0xf8, 0x8d, 0x83u, 0xf9,  0xef, 0xff,  0xff,
	0x89,  0x45, 0xf0,  0xb9, 0x01,  0x00, 0x00,  0x00, 0x8b,  0xf8, 0xfc, 0xa8u, 0x00,  0xf3, 0xa6,  0x74, 0x25,  0x8b,
	0x75,  0xf4, 0x8d,  0xbb, 0xfa,  0xef, 0xff,  0xff, 0xb9, 0x03u, 0x00,  0x00, 0x00,  0xfc, 0xa8,  0x00, 0xf3,  0xa6,
	0x74,  0x10, 0x80,  0x7a, 0x09,  0x7a, 0x74, 0x0au, 0x33,  0xc0, 0xe9,  0xa6, 0x00,  0x00, 0x00,  0x90, 0x90,  0x90,
	0x8b,  0x45, 0x0c,  0x8b, 0x00, 0x89u, 0x45,  0xf0, 0x8b,  0xd0, 0x8b,  0xfa, 0x33,  0xc0, 0xfc,  0xb9, 0xff,  0xff,
	0xff,  0xff, 0xf2, 0xaeu, 0xf7,  0xd1, 0x89,  0x4d, 0xf8,  0x8d, 0x44,  0x0a, 0xff,  0x89, 0x45,  0xf4, 0x8b,  0xd0,
	0x42, 0x8bu, 0x75,  0xf0, 0x89,  0x75, 0xf8,  0x8d, 0xbb,  0xfa, 0xef,  0xff, 0xff,  0x89, 0x7d,  0xf0, 0xb9, 0x03u,
	0x00,  0x00, 0x00,  0xfc, 0xa8,  0x00, 0xf3,  0xa6, 0x75,  0x14, 0x8b,  0x40, 0x01,  0x8b, 0x75, 0x0cu, 0x89,  0x46,
	0x04,  0x8b, 0x55,  0xf4, 0x83,  0xc2, 0x05,  0xeb, 0x0d,  0x90, 0x90,  0x90, 0x8b, 0x7du, 0x0c,  0xc7, 0x47,  0x04,
	0x00,  0x00, 0x00,  0x00, 0x8b,  0x45, 0x0c,  0x05, 0x08,  0x00, 0x00, 0x00u, 0x50,  0x52, 0xe8,  0xeb, 0xfb,  0xff,
	0xff,  0x8b, 0xd0,  0x8b, 0x75,  0x0c, 0x83,  0xc6, 0x0c, 0x56u, 0x52,  0xe8, 0x18,  0xfc, 0xff,  0xff, 0x8b,  0xd0,
	0x0f,  0xb6, 0x02,  0x8b, 0x75,  0x0c, 0x89, 0x46u, 0x10,  0x42, 0x8b,  0x3e, 0x83,  0xc4, 0x10,  0x80, 0x3f,  0x7a,
	0x75,  0x0f, 0x8d,  0x45, 0xfc, 0x50u, 0x52,  0xe8, 0xbc,  0xfb, 0xff,  0xff, 0x8b,  0xd0, 0x03,  0x55, 0xfc,  0x8b,
	0xc2,  0x8d, 0x65, 0xdcu, 0x5b,  0x5e, 0x5f,  0xc9, 0xc3,  0x90, 0x55,  0x8b, 0xec,  0x83, 0xec,  0x10, 0x57,  0x56,
	0x53, 0xe8u, 0x00,  0x00, 0x00,  0x00, 0x5b,  0x81, 0xc3,  0x82, 0x16,  0x00, 0x00,  0x8b, 0x7d,  0x10, 0x8b, 0x55u,
	0x14,  0x8b, 0x4d,  0x08, 0x0f,  0xb6, 0x01,  0x41, 0x89,  0x4d, 0x08,  0x8b, 0xf0,  0x81, 0xe6, 0x40u, 0x00,  0x00,
	0x00,  0x89, 0x75,  0xf0, 0x74,  0x12, 0x25,  0x3f, 0x00,  0x00, 0x00,  0x0f, 0xaf, 0x47u, 0x08,  0x01, 0x02,  0xe9,
	0x42,  0x03, 0x00,  0x00, 0x90,  0x90, 0x84,  0xc0, 0x7d,  0x44, 0x25, 0x3fu, 0x00,  0x00, 0x00,  0x89, 0x45,  0xf8,
	0x8d,  0x45, 0xfc,  0x50, 0xff,  0x75, 0x08,  0xe8, 0x50, 0xfbu, 0xff,  0xff, 0x89,  0x45, 0x08,  0x8b, 0x45,  0xfc,
	0x0f,  0xaf, 0x47,  0x0c, 0x89,  0x45, 0xfc, 0x8bu, 0x45,  0xf8, 0x8b,  0x4d, 0x0c,  0xc6, 0x44,  0x08, 0x5c,  0x01,
	0x8b,  0x75, 0xf8,  0x8d, 0x14, 0xb5u, 0x00,  0x00, 0x00,  0x00, 0x8b,  0x45, 0xfc,  0x89, 0x44,  0x0a, 0x10,  0xe9,
	0xfa,  0x02, 0x00, 0x00u, 0x90,  0x90, 0xa8,  0xc0, 0x74,  0x18, 0x25,  0x3f, 0x00,  0x00, 0x00,  0x89, 0x45,  0xf8,
	0x8a, 0x4du, 0xf0,  0x8b, 0x75,  0x0c, 0x88,  0x4c, 0x30,  0x5c, 0xe9,  0xdd, 0x02,  0x00, 0x00,  0x90, 0x3d, 0x2eu,
	0x00,  0x00, 0x00,  0x0f, 0x87,  0xc9, 0x02,  0x00, 0x00,  0x8b, 0xcb,  0x2b, 0x8c,  0x83, 0x30, 0xeau, 0xff,  0xff,
	0xff,  0xe1, 0x90,  0x90, 0x0c,  0x13, 0x00,  0x00, 0x14,  0x15, 0x00,  0x00, 0x00, 0x15u, 0x00,  0x00, 0xec,  0x14,
	0x00,  0x00, 0xd8,  0x14, 0x00,  0x00, 0xc0,  0x14, 0x00,  0x00, 0x74, 0x14u, 0x00,  0x00, 0x0c,  0x13, 0x00,  0x00,
	0x0c,  0x13, 0x00,  0x00, 0x54,  0x14, 0x00,  0x00, 0xa0, 0x13u, 0x00,  0x00, 0x80,  0x13, 0x00,  0x00, 0x14,  0x14,
	0x00,  0x00, 0xe0,  0x13, 0x00,  0x00, 0xc0, 0x13u, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00,
	0x14,  0x13, 0x00,  0x00, 0x14, 0x13u, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13,
	0x00,  0x00, 0x14, 0x13u, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00,
	0x14, 0x13u, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14, 0x13u,
	0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14, 0x13u, 0x00,  0x00,
	0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14, 0x13u, 0x00,  0x00, 0x14,  0x13,
	0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00, 0x14, 0x13u, 0x00,  0x00, 0x14,  0x13, 0x00,  0x00,
	0x14,  0x13, 0x00,  0x00, 0x64,  0x13, 0x00,  0x00, 0x30, 0x13u, 0x00,  0x00, 0x8b,  0x75, 0x08,  0x8b, 0x06,  0x89,
	0x02,  0x83, 0xc6,  0x04, 0x89,  0x75, 0x08, 0xe9u, 0xf6,  0x01, 0x00,  0x00, 0x90,  0x90, 0x8b,  0x4d, 0x08,  0x0f,
	0xb6,  0x01, 0x01,  0x02, 0x41, 0x89u, 0x4d,  0x08, 0xe9,  0xe3, 0x01,  0x00, 0x00,  0x90, 0x90,  0x90, 0x8b,  0x75,
	0x08,  0x0f, 0xb7, 0x06u, 0x01,  0x02, 0x83,  0xc6, 0x02,  0x89, 0x75,  0x08, 0xe9,  0xcd, 0x01,  0x00, 0x00,  0x90,
	0x8b, 0x02u, 0x8b,  0x4d, 0x08,  0x03, 0x01,  0x89, 0x02,  0x83, 0xc1,  0x04, 0x89,  0x4d, 0x08,  0xe9, 0xb8, 0x01u,
	0x00,  0x00, 0x90,  0x90, 0x90,  0x90, 0x8d,  0x45, 0xf8,  0x50, 0xff,  0x75, 0x08,  0xe8, 0xd0, 0xf9u, 0xff,  0xff,
	0x89,  0x45, 0x08,  0x8d, 0x45,  0xfc, 0x50,  0xff, 0x75,  0x08, 0xe8,  0xc1, 0xf9, 0xffu, 0xff,  0x89, 0x45,  0x08,
	0x8b,  0x45, 0xfc,  0x0f, 0xaf,  0x47, 0x0c,  0x89, 0x45,  0xfc, 0x8b, 0x45u, 0xf8,  0x8b, 0x75,  0x0c, 0xc6,  0x44,
	0x30,  0x5c, 0x01,  0x8b, 0x4d,  0xf8, 0x8d,  0x14, 0x8d, 0x00u, 0x00,  0x00, 0x00,  0x8b, 0x45,  0xfc, 0x89,  0x44,
	0x32,  0x10, 0xe9,  0x6b, 0x01,  0x00, 0x00, 0x90u, 0x90,  0x90, 0x8d,  0x45, 0xf8,  0x50, 0xff,  0x75, 0x08,  0xe8,
	0x84,  0xf9, 0xff,  0xff, 0x89, 0x45u, 0x08,  0x8b, 0x45,  0xf8, 0x8b,  0x75, 0x0c,  0xc6, 0x44,  0x30, 0x5c,  0x00,
	0xe9,  0x49, 0x01, 0x00u, 0x00,  0x90, 0x8d,  0x45, 0xf8,  0x50, 0xff,  0x75, 0x08,  0xe8, 0x64,  0xf9, 0xff,  0xff,
	0x89, 0x45u, 0x08,  0x8d, 0x45,  0xf4, 0x50,  0xff, 0x75,  0x08, 0xe8,  0x55, 0xf9,  0xff, 0xff,  0x89, 0x45, 0x08u,
	0x8b,  0x45, 0xf8,  0x8b, 0x4d,  0x0c, 0xc6,  0x44, 0x08,  0x5c, 0x02,  0x8b, 0x75,  0xf8, 0x8d, 0x14u, 0xb5,  0x00,
	0x00,  0x00, 0x00,  0x8b, 0x45,  0xf4, 0x89,  0x44, 0x0a,  0x10, 0xe9,  0x09, 0x01, 0x00u, 0x00,  0x90, 0x8d,  0x45,
	0xf8,  0x50, 0xff,  0x75, 0x08,  0xe8, 0x24,  0xf9, 0xff,  0xff, 0x89, 0x45u, 0x08,  0x8d, 0x45,  0xfc, 0x50,  0xff,
	0x75,  0x08, 0xe8,  0x15, 0xf9,  0xff, 0xff,  0x89, 0x45, 0x08u, 0x8b,  0x45, 0xf8,  0x8b, 0x4d,  0x0c, 0x66,  0x89,
	0x41,  0x58, 0x8b,  0x45, 0xfc,  0x89, 0x41, 0x08u, 0xe9,  0xd5, 0x00,  0x00, 0x00,  0x90, 0x8d,  0x45, 0xf8,  0x50,
	0xff,  0x75, 0x08,  0xe8, 0xf0, 0xf8u, 0xff,  0xff, 0x89,  0x45, 0x08,  0x8b, 0x45,  0xf8, 0x8b,  0x75, 0x0c,  0x66,
	0x89,  0x46, 0x58, 0xe9u, 0xb6,  0x00, 0x00,  0x00, 0x90,  0x90, 0x8d,  0x45, 0xfc,  0x50, 0xff,  0x75, 0x08,  0xe8,
	0xd0, 0xf8u, 0xff,  0xff, 0x89,  0x45, 0x08,  0x8b, 0x45,  0xfc, 0x8b,  0x4d, 0x0c,  0x89, 0x41,  0x08, 0xe9, 0x97u,
	0x00,  0x00, 0x00,  0x90, 0x90,  0x90, 0x6a,  0x74, 0xe8,  0x55, 0xf7,  0xff, 0xff,  0x8b, 0xd0, 0x8bu, 0xfa,  0x8b,
	0x45,  0x0c, 0x8b,  0xf0, 0xfc,  0xb9, 0x1d,  0x00, 0x00,  0x00, 0xf3,  0xa5, 0x8b, 0x4du, 0x0c,  0x89, 0x51,  0x70,
	0xeb,  0x74, 0x8b,  0x75, 0x0c,  0x8b, 0x56,  0x70, 0x8b,  0xfe, 0x8b, 0xc2u, 0x8b,  0xf0, 0xfc,  0xb9, 0x1d,  0x00,
	0x00,  0x00, 0xf3,  0xa5, 0x52,  0xe8, 0x32,  0xf7, 0xff, 0xffu, 0xeb,  0x58, 0xc7,  0x45, 0xf8,  0x10, 0x00,  0x00,
	0x00,  0x90, 0x8b,  0x45, 0xf8,  0x8b, 0x4d, 0x0cu, 0xc6,  0x44, 0x08,  0x5c, 0x01,  0x8b, 0x45,  0xf8, 0x8d,  0x14,
	0x85,  0xc0, 0xff,  0xff, 0xff, 0x89u, 0x54,  0x81, 0x10,  0x8d, 0x70,  0x01, 0x89,  0x75, 0xf8,  0x8b, 0xc6,  0x3d,
	0x1f,  0x00, 0x00, 0x00u, 0x76,  0xd8, 0xeb,  0x26, 0x90,  0x90, 0x8d,  0x45, 0xfc,  0x50, 0xff,  0x75, 0x08,  0xe8,
	0x40, 0xf8u, 0xff,  0xff, 0x89,  0x45, 0x08,  0x8b, 0x45,  0xfc, 0x8b,  0x4d, 0x0c,  0x89, 0x41,  0x0c, 0xeb, 0x0au,
	0x90,  0x90, 0xe8,  0xeb, 0xf6,  0xff, 0xff,  0x90, 0x90,  0x90, 0x8b,  0x45, 0x08,  0x8d, 0x65, 0xe4u, 0x5b,  0x5e,
	0x5f,  0xc9, 0xc3,  0x90, 0x55,  0x8b, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00, 0x5bu, 0x81,  0xc3, 0xf7,  0x12,
	0x00,  0x00, 0x8b,  0x45, 0x08,  0x8b, 0x55,  0x0c, 0x89,  0x42, 0x08, 0xc7u, 0x42,  0x04, 0x00,  0x00, 0x00,  0x00,
	0xc7,  0x02, 0x00,  0x00, 0x00,  0x00, 0xc7,  0x42, 0x0c, 0x00u, 0x00,  0x00, 0x00,  0xc7, 0x42,  0x10, 0x00,  0x00,
	0x00,  0x00, 0x8b,  0x83, 0xf4,  0x05, 0x00, 0x00u, 0x89,  0x42, 0x14,  0x89, 0x93,  0xf4, 0x05,  0x00, 0x00,  0x8b,
	0x5d,  0xfc, 0xc9,  0xc3, 0x55, 0x8bu, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0xaf,  0x12,
	0x00,  0x00, 0x8b, 0x45u, 0x08,  0x8b, 0x55,  0x0c, 0x89,  0x42, 0x08,  0x89, 0x42,  0x0c, 0xc7,  0x42, 0x04,  0x00,
	0x00, 0x00u, 0x00,  0xc7, 0x02,  0x00, 0x00,  0x00, 0x00,  0xc7, 0x42,  0x10, 0x00,  0x00, 0x00,  0x00, 0x8b, 0x83u,
	0xf4,  0x05, 0x00,  0x00, 0x89,  0x42, 0x14,  0x89, 0x93,  0xf4, 0x05,  0x00, 0x00,  0x8b, 0x5d, 0xfcu, 0xc9,  0xc3,
	0x55,  0x8b, 0xec,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0x6b, 0x12u, 0x00,  0x00, 0x8b,  0x45,
	0x08,  0x8d, 0x8b,  0xf4, 0x05,  0x00, 0x00,  0x83, 0xbb,  0xf4, 0x05, 0x00u, 0x00,  0x00, 0x74,  0x27, 0x90,  0x90,
	0x8b,  0x11, 0x39,  0x42, 0x08,  0x75, 0x15,  0x8b, 0x42, 0x14u, 0x89,  0x01, 0x83,  0x3a, 0x00,  0x74, 0x1b,  0xff,
	0x72,  0x0c, 0xe8,  0x03, 0xf6,  0xff, 0xff, 0xebu, 0x11,  0x90, 0x8d,  0x4a, 0x14,  0x83, 0x7a,  0x14, 0x00,  0x75,
	0xdb,  0xe8, 0x02,  0xf6, 0xff, 0xffu, 0x90,  0x90, 0x8b,  0x5d, 0xfc,  0xc9, 0xc3,  0x90, 0x90,  0x90, 0x55,  0x8b,
	0xec,  0x81, 0xec, 0xa8u, 0x00,  0x00, 0x00,  0x57, 0x56,  0x53, 0xe8,  0x00, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3,
	0x0b, 0x12u, 0x00,  0x00, 0xff,  0x75, 0x08,  0xe8, 0xb4,  0xf9, 0xff,  0xff, 0x89,  0x85, 0x6c,  0xff, 0xff, 0xffu,
	0x83,  0xc4, 0x04,  0x85, 0xc0,  0x0f, 0x84,  0x2e, 0x01,  0x00, 0x00,  0x8d, 0x4d,  0xec, 0x89, 0x8du, 0x68,  0xff,
	0xff,  0xff, 0x51,  0xff, 0xb5,  0x6c, 0xff,  0xff, 0xff,  0xe8, 0x2e,  0xfa, 0xff, 0xffu, 0x8b,  0xf0, 0x83,  0xc4,
	0x08,  0x85, 0xf6,  0x0f, 0x84,  0x0c, 0x01,  0x00, 0x00,  0x6a, 0x74, 0x6au, 0x00,  0x8d, 0x8d,  0x78, 0xff,  0xff,
	0xff,  0x89, 0x8d,  0x64, 0xff,  0xff, 0xff,  0x51, 0xe8, 0x9fu, 0xf5,  0xff, 0xff,  0x8b, 0x45,  0xfc, 0x66,  0x89,
	0x45,  0xd2, 0x8b,  0x45, 0xf0,  0x89, 0x85, 0x7cu, 0xff,  0xff, 0xff,  0x83, 0xc4,  0x0c, 0x8b,  0x8d, 0x6c,  0xff,
	0xff,  0xff, 0x8b,  0x41, 0x04, 0x05u, 0xfc,  0xff, 0xff,  0xff, 0x2b,  0xc8, 0x8b,  0xc1, 0x03,  0x00, 0x8d,  0x78,
	0x04,  0x3b, 0xf7, 0x73u, 0x36,  0x8b, 0x8d,  0x68, 0xff,  0xff, 0xff,  0x89, 0x8d,  0x60, 0xff,  0xff, 0xff,  0x8b,
	0x8d, 0x64u, 0xff,  0xff, 0xff,  0x89, 0x8d,  0x5c, 0xff,  0xff, 0xff,  0x90, 0x6a,  0x00, 0xff,  0xb5, 0x60, 0xffu,
	0xff,  0xff, 0xff,  0xb5, 0x5c,  0xff, 0xff,  0xff, 0x56,  0xe8, 0xc8,  0xfa, 0xff,  0xff, 0x8b, 0xf0u, 0x83,  0xc4,
	0x10,  0x3b, 0xf7,  0x72, 0xe3,  0x8b, 0xb5,  0x6c, 0xff,  0xff, 0xff,  0x83, 0xc6, 0x10u, 0x8b,  0x45, 0xec,  0x80,
	0x38,  0x7a, 0x75,  0x18, 0x8d,  0x85, 0x74,  0xff, 0xff,  0xff, 0x50, 0x56u, 0xe8,  0x4d, 0xf6,  0xff, 0xff,  0x8b,
	0xf0,  0x03, 0xb5,  0x74, 0xff,  0xff, 0xff,  0x83, 0xc4, 0x08u, 0x8b,  0x85, 0x6c,  0xff, 0xff,  0xff, 0x03,  0x00,
	0x8d,  0x78, 0x04,  0x8b, 0x8d,  0x6c, 0xff, 0xffu, 0xff,  0x8b, 0x41,  0x08, 0x89,  0x85, 0x70,  0xff, 0xff,  0xff,
	0x3b,  0xf7, 0x73,  0x37, 0x8d, 0x8du, 0x70,  0xff, 0xff,  0xff, 0x89,  0x8d, 0x58,  0xff, 0xff,  0xff, 0x8b,  0x4d,
	0x08,  0x39, 0x8d, 0x70u, 0xff,  0xff, 0xff,  0x77, 0x20,  0xff, 0xb5,  0x58, 0xff,  0xff, 0xff,  0x8d, 0x45,  0xec,
	0x50, 0x8du, 0x85,  0x78, 0xff,  0xff, 0xff,  0x50, 0x56,  0xe8, 0x4a,  0xfa, 0xff,  0xff, 0x8b,  0xf0, 0x83, 0xc4u,
	0x10,  0x3b, 0xf7,  0x72, 0xd5,  0x8b, 0x7d,  0x0c, 0x8d,  0xb5, 0x78,  0xff, 0xff,  0xff, 0xfc, 0xb9u, 0x1c,  0x00,
	0x00,  0x00, 0xf3,  0xa5, 0x8b,  0x45, 0x0c,  0x8d, 0xa5,  0x4c, 0xff,  0xff, 0xff, 0x5bu, 0x5e,  0x5f, 0xc9,  0xc3,
	0x00,  0x00, 0x55,  0x8b, 0xec,  0x56, 0x53,  0xe8, 0x00,  0x00, 0x00, 0x00u, 0x5b,  0x81, 0xc3,  0xa6, 0x10,  0x00,
	0x00,  0x8d, 0x83,  0xd0, 0x00,  0x00, 0x00,  0x8d, 0x70, 0xfcu, 0x83,  0x78, 0xfc,  0xff, 0x74,  0x0c, 0x8b,  0x06,
	0xff,  0xd0, 0x83,  0xc6, 0xfc,  0x83, 0x3e, 0xffu, 0x75,  0xf4, 0x8d,  0x65, 0xf8,  0x5b, 0x5e,  0xc9, 0xc3,  0x90,
	0x55,  0x8b, 0xec,  0x53, 0xe8, 0x00u, 0x00,  0x00, 0x00,  0x5b, 0x81,  0xc3, 0x73,  0x10, 0x00,  0x00, 0x8b,  0x5d,
	0xfc,  0xc9, 0xc3
};

void LoaderTest::testMicroDis1()
{
	QString      deb;
	QTextStream  deb_str(&deb);
	unsigned int n         = sizeof(pent_hello_text);
	int          totalSize = 0;
	void         *p        = pent_hello_text;
	int          i         = 0;

	while (totalSize < (int)n) {
		int size = microX86Dis((uint8_t *)p);

		if (size >= 0x40) {
			deb_str << "Not handled instruction at offset 0x"
					<< ADDRESS::host_ptr(p) - ADDRESS::host_ptr(pent_hello_text) << '\n';
			qDebug() << deb;
			QVERIFY(size != 0x40);
			return;
		}

		int expected = lengths[i++];

		if (expected != size) {
			deb_str << "At offset 0x" << ADDRESS::host_ptr(p) - ADDRESS::host_ptr(pent_hello_text) << " ("
					<< (int)*((unsigned char *)p) << " " << (int)*((unsigned char *)p + 1) << " "
					<< (int)*((unsigned char *)p + 2) << " " << (int)*((unsigned char *)p + 3) << " "
					<< ") expected " << expected << ", actual " << size << '\n';
			qDebug() << deb;
			QCOMPARE(size, expected);
		}

		p          = (void *)((char *)p + size);
		totalSize += size;
	}

	QCOMPARE(totalSize, (int)n);
}


void LoaderTest::testMicroDis2()
{
	// Now a special test:
	// 8048910:  0f be 00            movsbl (%eax),%eax
	// 8048913:  0f bf 00            movswl (%eax),%eax

	unsigned char movsbl[3] = { 0x0f, 0xbe, 0x00 };
	unsigned char movswl[3] = { 0x0f, 0xbf, 0x00 };
	int           size      = microX86Dis(movsbl);

	QCOMPARE(size, 3);
	size = microX86Dis(movswl);
	QCOMPARE(size, 3);
}


typedef unsigned (*elfHashFcn)(const char *);
void LoaderTest::testElfHash()
{
	QLibrary z;

	z.setFileName(ELFBINFILE);
	bool opened = z.load();
	QVERIFY(opened);
	// Use the handle to find the "elf_hash" function
	elfHashFcn pFcn = (elfHashFcn)z.resolve("elf_hash");
	QVERIFY(pFcn);
	// Call the function with the string "main
	unsigned act = (*pFcn)("main");
	unsigned exp = 0x737fe;
	QCOMPARE(act, exp);
}


QTEST_MAIN(LoaderTest)
