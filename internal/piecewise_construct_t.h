#ifndef RSTL_PIECEWISE_CONSTRUCT_T_H
#define RSTL_PIECEWISE_CONSTRUCT_T_H

#pragma once

#include "config.h"

RSTL_NAMESPACE_BEGIN

struct piecewise_construct_t
{
	explicit piecewise_construct_t() = default;
};

constexpr piecewise_construct_t piecewise_construct = rstl::piecewise_construct_t();

RSTL_NAMESPACE_END

#endif //RSTL_PIECEWISE_CONSTRUCT_T_H
