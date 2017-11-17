/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2009 Soeren Sonnenburg
 * Written (W) 1999-2008 Gunnar Raetsch
 * Written (W) 2011 Heiko Strathmann
 * Copyright (C) 1999-2009 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _REAL_LABELS__H__
#define _REAL_LABELS__H__

#include "ml/shogun/lib/config.h"

#include "ml/shogun/lib/common.h"
#include "ml/shogun/io/File.h"
#include "ml/shogun/labels/LabelTypes.h"
#include "ml/shogun/labels/DenseLabels.h"
#include "ml/shogun/features/SubsetStack.h"

namespace shogun
{
	class CFile;
	class CRegressionLabels;
	class CDenseLabels;

/** @brief Real Labels are real-valued labels
 *
 * They are used for regression problems and as outputs of classifiers.
 *
 * valid values for labels are all real-valued numbers.
 */
class CRegressionLabels : public CDenseLabels
{
	public:
		/** default constructor */
		CRegressionLabels();

		/** constructor
		 *
		 * @param num_labels number of labels
		 */
		CRegressionLabels(int32_t num_labels);

		/** constructor
		 *
		 * @param src labels to set
		 */
		CRegressionLabels(const SGVector<float64_t> src);

		/** constructor
		 *
		 * @param loader File object via which to load data
		 */
		CRegressionLabels(CFile* loader);

		/** get label type
		 *
		 * @return label type real
		 */
		virtual ELabelType get_label_type() const;

		/** @return object name */
		virtual const char* get_name() const { return "RegressionLabels"; }

#ifndef SWIG // SWIG should skip this part
		virtual CLabels* shallow_subset_copy();
#endif
};
}
#endif
