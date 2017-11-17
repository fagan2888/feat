/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2009 Soeren Sonnenburg
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#include "ml/shogun/lib/config.h"

#include <stdlib.h>
#include <string.h>

#include "ml/shogun/io/SGIO.h"
#include "ml/shogun/lib/Signal.h"
#include "ml/shogun/base/init.h"

using namespace shogun;

int CSignal::signals[NUMTRAPPEDSIGS]={SIGINT, SIGURG};
struct sigaction CSignal::oldsigaction[NUMTRAPPEDSIGS];
bool CSignal::active=false;
bool CSignal::cancel_computation=false;
bool CSignal::cancel_immediately=false;

CSignal::CSignal()
: CSGObject()
{
}

CSignal::~CSignal()
{
	if (!unset_handler())
		SG_PRINT("error uninitalizing signal handler\n")
}

void CSignal::handler(int signal)
{
	if (signal == SIGINT)
	{
		SG_SPRINT("\nImmediately return to prompt / Prematurely finish computations / Do nothing (I/P/D)? ")
		char answer=fgetc(stdin);

		if (answer == 'I')
		{
			unset_handler();
			set_cancel(true);
			if (sg_print_error)
				sg_print_error(stdout, "sg stopped by SIGINT\n");
		}
		else if (answer == 'P')
			set_cancel();
		else
			SG_SPRINT("Continuing...\n")
	}
	else if (signal == SIGURG)
		set_cancel();
	else
		SG_SPRINT("unknown signal %d received\n", signal)
}

bool CSignal::set_handler()
{
	if (!active)
	{
		struct sigaction act;
		sigset_t st;

		sigemptyset(&st);
		for (int32_t i=0; i<NUMTRAPPEDSIGS; i++)
			sigaddset(&st, signals[i]);

#if !(defined(__INTERIX) || defined(__MINGW64__) || defined(_MSC_VER) || defined(__MINGW32__))
		act.sa_sigaction=NULL; //just in case
#endif
		act.sa_handler=CSignal::handler;
		act.sa_mask = st;
		act.sa_flags = 0;

		for (int32_t i=0; i<NUMTRAPPEDSIGS; i++)
		{
			if (sigaction(signals[i], &act, &oldsigaction[i]))
			{
				SG_SPRINT("Error trapping signals!\n")
				for (int32_t j=i-1; j>=0; j--)
					sigaction(signals[i], &oldsigaction[i], NULL);

				clear();
				return false;
			}
		}

		active=true;
		return true;
	}
	else
		return false;
}

bool CSignal::unset_handler()
{
	if (active)
	{
		bool result=true;

		for (int32_t i=0; i<NUMTRAPPEDSIGS; i++)
		{
			if (sigaction(signals[i], &oldsigaction[i], NULL))
			{
				SG_SPRINT("error uninitalizing signal handler for signal %d\n", signals[i])
				result=false;
			}
		}

		if (result)
			clear();

		return result;
	}
	else
		return false;
}

void CSignal::clear_cancel()
{
	cancel_computation=false;
	cancel_immediately=false;
}

void CSignal::set_cancel(bool immediately)
{
	cancel_computation=true;

	if (immediately)
		cancel_immediately=true;
}

void CSignal::clear()
{
	clear_cancel();
	active=false;
	memset(&CSignal::oldsigaction, 0, sizeof(CSignal::oldsigaction));
}

#if defined(__MINGW64__) || defined(_MSC_VER) || defined(__MINGW32__)
#define SIGBAD(signo) ( (signo) <=0 || (signo) >=NSIG)
Sigfunc *handlers[NSIG]={0};

int sigaddset(sigset_t *set, int signo)
{
	if (SIGBAD(signo)) {
		errno = EINVAL;
		return -1;
	}
	*set |= 1 << (signo-1);
	return 0;
}

int sigaction(int signo, const struct sigaction *act, struct sigaction *oact)
{
	if (SIGBAD(signo)) {
		errno = EINVAL;
		return -1;
	}

	if(oact){
			oact->sa_handler = handlers[signo];
			oact->sa_mask = 0;
			oact->sa_flags =0;
	}
	if (act)
		handlers[signo]=act->sa_handler;

	return 0;
}
#endif