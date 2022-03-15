// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2021 Intel Corporation. All rights reserved.
//
// Author: Shriram Shastry <malladi.sastry@linux.intel.com>
//
//

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <math.h>
#include <cmocka.h>
#include <string.h>

#include <sof/math/log.h>
#include <sof/audio/format.h>
#include <sof/string.h>
#include <sof/common.h>
#include "log2_tables.h"
/* 'Error[max] = 0.0000071279028671,THD = -102.9407645424143993 ' */
/* 'Error[max] = rms(log10() - double(log10_int32()))' */
/* 'THD = 20*log10(Error[max])' */
#define CMP_TOLERANCE 0.0000071279028671
/* Natural logarithm log10(X) reference table generated by matlab/Octave */
/* UQ4.28 */
static const double common_log10_ref_table[] = {
	0.0000000000000000, 7.6373246662453802, 7.9383546619093615, 8.1144459209650428,
	8.2393846575733427, 8.3362946705813989, 8.4154759166290241, 8.4824227062596371,
	8.5404146532373240, 8.5915671756847054, 8.6373246662453802, 8.6787173514036056,
	8.7165059122930053, 8.7512680193222625, 8.7834527026386606, 8.8134159259684335,
	8.8414446495269665, 8.8677735882125130, 8.8925971719048302, 8.9160782677250818,
	8.9383546624098908, 8.9595439614559940, 8.9797473475226131, 8.9990525026982162,
	9.0175359083740947, 9.0352646753178405, 9.0522980146012202, 9.0686884307751292,
	9.0844826979451199, 9.0997226644895282, 9.1144459212987297, 9.1286863604025754,
	9.1424746448781171, 9.1558386064266184, 9.1688035835820649, 9.1813927108816724,
	9.1936271672907388, 9.2055263908534872, 9.2171082633890631, 9.2283892737852433,
	9.2393846580738721, 9.2501085234534361, 9.2605739571199752, 9.2707931222905753,
	9.2807773431865943, 9.2905371804656394, 9.3000824983621975, 9.3094225246070792,
	9.3185659040380759, 9.3275207466824899, 9.3362946709818218, 9.3448948427358882,
	9.3533280102652014, 9.3616005362239267, 9.3697184264391105, 9.3776873561036460,
	9.3855126936091011, 9.3931995222691196, 9.4007526601535094, 9.4081766782268659,
	9.4154759169627091, 9.4226545015843630, 9.4297163562280168, 9.4366652161756566,
	9.4435046406985137, 9.4502380233502628, 9.4568686022422757, 9.4633994693944423,
	9.4698335793932600, 9.4761737574178788, 9.4824227066886628, 9.4885830153874373,
	9.4946571630937573, 9.5006475267772306, 9.5065563863821918, 9.5123859300375031,
	9.5181382589213257, 9.5238153918078847, 9.5294192693208828, 9.5349517579159713,
	9.5404146536127215, 9.5458096854947918, 9.5511385189953373, 9.5564027589832818,
	9.5616039526647825, 9.5667435923129869, 9.5718231178381536, 9.5768439193242560,
	9.5818073388505756, 9.5867146733402073, 9.5915671761296206, 9.5963660590064990,
	9.6011124940261787, 9.6058076152298781, 9.6104525202710605, 9.6150482719557271,
	9.6195958997020572, 9.6240964009244330, 9.6285507423464711, 9.6329598611462810};

/* testvector in Q32.0 */
static const uint32_t uv[100] = {
	1ULL,	 43383509ULL,	86767017ULL,   130150525ULL, 173534033ULL, 216917541ULL,
	260301049ULL, 303684557ULL,  347068065ULL,  390451573ULL, 433835081ULL, 477218589ULL,
	520602097ULL, 563985605ULL,  607369113ULL,  650752621ULL, 694136129ULL, 737519638ULL,
	780903146ULL, 824286654ULL,  867670162ULL,  911053670ULL, 954437178ULL, 997820686ULL,
	1041204194ULL, 1084587702ULL, 1127971210ULL, 1171354718ULL, 1214738226ULL, 1258121734ULL,
	1301505242ULL, 1344888750ULL, 1388272258ULL, 1431655766ULL, 1475039274ULL, 1518422782ULL,
	1561806290ULL, 1605189798ULL, 1648573306ULL, 1691956814ULL, 1735340322ULL, 1778723830ULL,
	1822107338ULL, 1865490846ULL, 1908874354ULL, 1952257862ULL, 1995641370ULL, 2039024878ULL,
	2082408386ULL, 2125791894ULL, 2169175403ULL, 2212558911ULL, 2255942419ULL, 2299325927ULL,
	2342709435ULL, 2386092943ULL, 2429476451ULL, 2472859959ULL, 2516243467ULL, 2559626975ULL,
	2603010483ULL, 2646393991ULL, 2689777499ULL, 2733161007ULL, 2776544515ULL, 2819928023ULL,
	2863311531ULL, 2906695039ULL, 2950078547ULL, 2993462055ULL, 3036845563ULL, 3080229071ULL,
	3123612579ULL, 3166996087ULL, 3210379595ULL, 3253763103ULL, 3297146611ULL, 3340530119ULL,
	3383913627ULL, 3427297135ULL, 3470680643ULL, 3514064151ULL, 3557447659ULL, 3600831168ULL,
	3644214676ULL, 3687598184ULL, 3730981692ULL, 3774365200ULL, 3817748708ULL, 3861132216ULL,
	3904515724ULL, 3947899232ULL, 3991282740ULL, 4034666248ULL, 4078049756ULL, 4121433264ULL,
	4164816772ULL, 4208200280ULL, 4251583788ULL, 4294967295ULL};

static void test_math_arithmetic_base10log_fixed(void **state)
{
	(void)state;

	double clogfxp;
	double diff;
	int i;

	for (i = 0; i < ARRAY_SIZE(common_log10_ref_table); i++) {
		clogfxp = log10_int32(uv[i]);
		diff = fabs(common_log10_ref_table[i] - (double)clogfxp / (1 << 28));

		if (diff > CMP_TOLERANCE) {
			printf("%s: diff for %.16f: val = %16d, log10() = %.16f\n", __func__, diff,
			       uv[i], clogfxp / (1 << 28));
			assert_true(diff <= CMP_TOLERANCE);
		}
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_math_arithmetic_base10log_fixed)
	};

	cmocka_set_message_output(CM_OUTPUT_TAP);

	return cmocka_run_group_tests(tests, NULL, NULL);
}