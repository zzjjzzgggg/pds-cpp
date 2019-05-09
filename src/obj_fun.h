/**
 * Copyright (C) by J.Z. 2019-03-25 16:37
 * Distributed under terms of the MIT license.
 */

#ifndef __OBJ_FUN_H__
#define __OBJ_FUN_H__

#include "stdafx.h"

class ObjFun {
public:
    virtual ~ObjFun() {}

    virtual double getVal(const int) const = 0;
    virtual double getVal(const std::vector<int>&) const = 0;
    virtual double getGain(const int, const std::vector<int>&) const = 0;

}; /* ObjFun */

#endif /* __OBJ_FUN_H__ */
