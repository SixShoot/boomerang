#pragma region License
/*
 * This file is part of the Boomerang Decompiler.
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */
#pragma endregion License
#pragma once


#include "TestUtils.h"


class StatementSetTest : public BoomerangTest
{
    Q_OBJECT

private slots:
    void testInsert();
    void testRemove();
    void testContains();
    void testDefinesLoc();
    void testIsSubSetOf();
    void testMakeUnion();
    void testMakeIsect();
    void testMakeDiff();
};
