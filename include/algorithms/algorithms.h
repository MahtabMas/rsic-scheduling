#pragma once

// Non-clairvoyant algorithms
#include "non_clairvoyant/next_fit.h"
#include "non_clairvoyant/modified_next_fit.h"
#include "non_clairvoyant/first_fit.h"
#include "non_clairvoyant/modified_first_fit.h"
#include "non_clairvoyant/best_fit.h"
#include "non_clairvoyant/worst_fit.h"
#include "non_clairvoyant/last_fit.h"
#include "non_clairvoyant/random_fit.h"
#include "non_clairvoyant/mtf.h"

// Clairvoyant algorithms
#include "clairvoyant/departure_strategy.h"
#include "clairvoyant/duration.h"
#include "clairvoyant/hybrid.h"
#include "clairvoyant/new_hybrid.h"
#include "clairvoyant/greedy.h"

// Combined algorithms
#include "combined/greedy_greedy.h"
#include "combined/greedy_hybrid.h"
#include "combined/greedy_duration.h"
#include "combined/new_greedy.h"

// Weakly clairvoyant algorithms
#include "combined/weakly_clairvoyant/ff_ff.h"
#include "combined/weakly_clairvoyant/ff_bf.h"
#include "combined/weakly_clairvoyant/bf_bf.h"
#include "combined/weakly_clairvoyant/bf_ff.h"

