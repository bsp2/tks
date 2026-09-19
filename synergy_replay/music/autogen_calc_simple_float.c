// auto-generated on 20260919-214904

//         name: cycle_bd sat b off=3548 num=4000
//  sample_rate: 44100
//    note_freq: 261.63
//      #frames: 4000
//        xfade: 100



static float var_x_cycle_bd_sat_b_0_bd = 0.0f;
static float var_wt_x_cycle_bd_sat_b_0_bd = 0.0f;
static float var_wt_y_cycle_bd_sat_b_0_bd = 0.0f;
static float var_wt_freq_cycle_bd_sat_b_0_bd = 0.0f;

static float tmp_f;
static float tmp2_f;
static short tmp_i;
static short tmp2_i;
#define loop(X)  for(unsigned int i = 0u; i < (X); i++)
#define clamp(a,b,c) (((a)<(b))?(b):(((a)>(c))?(c):(a)))

static inline float mathLerpf(float _a, float _b, float _t) { return _a + (_b - _a) * _t; }
static inline float mathClampf(float a, float b, float c) { return (((a)<(b))?(b):(((a)>(c))?(c):(a))); }
static inline float mathMinf(float a, float b) { return (a<b)?a:b; }
static inline float mathMaxf(float a, float b) { return (a>b)?a:b; }
static inline float mathAbsMaxf(float _x, float _y) { return ( ( (_x<0.0f)?-_x:_x)>((_y<0.0f)?-_y:_y)?_x:_y ); }
static inline float mathAbsMinf(float _x, float _y) { return ( ((_x<0.0f)?-_x:_x)<((_y<0.0f)?-_y:_y)?_x:_y ); }
static inline float frac(float _x) { return _x - ((int)_x); }

static inline float winLinear(const float *_s, float _index) {
   int idx = (int)_index;
   float r = _index - (float)idx;
   return mathLerpf(_s[idx], _s[idx+1], r);
}

extern float cycle_sine_tbl_f[16384];

extern short cycle_sine_tbl_i[16384];
extern float cycle_xfade_buf[4096];

static float ffrac_s(float _f) { int i; if(_f >= 0.0f) { i = (int)_f; return _f - (float)i; } else { i = (int)-_f; return 1.0f - (-_f - (float)i); } }

static short curve_0_cycle_bd_sat_b_0_bd[16] = {
2048, 554, 220, 100, 51, 35, 33, 37, 43, 48, 50, 49, 44, 34, 19, 0, };
static short curve_3_cycle_bd_sat_b_0_bd[32] = {
2048, 1746, 1533, 1375, 1255, 1163, 1093, 1040, 1002, 976, 960, 952, 952, 958, 969, 985, 1005, 1028, 1053, 1081, 1110, 1141, 1172, 1202, 1233, 1262, 1289, 1314, 1335, 1352, 1363, 1368, 
};
static short curve_4_cycle_bd_sat_b_0_bd[32] = {
-2023, -1883, -1744, -1605, -1467, -1330, -1193, -1058, -924, -792, -661, -533, -407, -283, -161, -39, 85, 219, 359, 502, 643, 783, 919, 1052, 1183, 1312, 1438, 1563, 1686, 1808, 1928, 2047, 
};

float tmp5_rmp_step__cycle_bd_sat_b_0_bd;
float tmp7_rmp_step__cycle_bd_sat_b_0_bd;
int tmp14_nos_seed1__cycle_bd_sat_b_0_bd;
int tmp15_nos_seed2__cycle_bd_sat_b_0_bd;
float tmp17_rmp_step__cycle_bd_sat_b_0_bd;

static void calc_cycle_bd_sat_b_0_bd(float *_d) {
   float tmp1_sin_phase = 0.0f;
   float tmp4_rmp_cur = 0.0f;
   float tmp6_rmp_cur = 0.0f;
   short tmp8;
   short tmp9;
   float tmp13_box_last = 0.0f;
   float tmp16_rmp_cur = 1.0f;
   float tmp2;
   // start=0 end=0.91 #frames=1380.1:
   // start=0 end=0.93 #frames=8093.53:
   // start=1 end=0 #frames=259.744:
   
   tmp5_rmp_step__cycle_bd_sat_b_0_bd = 0.000659374f;
   tmp7_rmp_step__cycle_bd_sat_b_0_bd = 0.000114907f;
   tmp14_nos_seed1__cycle_bd_sat_b_0_bd = 1033877898;
   tmp15_nos_seed2__cycle_bd_sat_b_0_bd = 1033877898;
   tmp17_rmp_step__cycle_bd_sat_b_0_bd = -0.00384995f;
   int smpIdx = 0;
   int outIdx = 0;
   int xfadeCaptureLeft = 100;
   int xfadeReadIdx = 0;
   float xfadeCur = 0.0f;
   int xfadeCaptureIdx = 0;
   loop(4000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=sin
      // ========
      
      // -- mod="sin" dstVar=out
      float tmp3;
      
      // ---- mod="sin" input "freq" seq 1/1
      
      // -- mod="rmp" dstVar=tmp3/*sin_freq*/
      tmp3/*sin_freq*/ = tmp4_rmp_cur;
      tmp4_rmp_cur = mathMinf(tmp4_rmp_cur+tmp5_rmp_step__cycle_bd_sat_b_0_bd, 0.91);
      
      // -- mod="lut" dstVar=tmp3/*sin_freq*/
      tmp3/*sin_freq*/ = curve_0_cycle_bd_sat_b_0_bd[((unsigned int)(tmp3/*sin_freq*/ * 16)) & 15] * (1.0f / 2048);
      
      // -- mod="$SIN_FREQ_RNG" dstVar=tmp3/*sin_freq*/
      tmp3/*sin_freq*/ *= 1.22303f;
      
      // -- mod="1" dstVar=tmp3/*sin_freq*/
      tmp3/*sin_freq*/ += 1.0f;
      tmp2/*sin_speed*/ = 0.00593265f * tmp3/*sin_freq*/;
      tmp3/*sin_tmp*/ = (tmp1_sin_phase);
      tmp3/*sin_tmp*/ = ffrac_s(tmp3/*sin_tmp*/);
      out = cycle_sine_tbl_f[(unsigned short)(16384 * tmp3/*sin_tmp*/)&16383u];
      tmp1_sin_phase = ffrac_s(tmp1_sin_phase + tmp2/*sin_speed*/);
      
      // -- mod="rmp" dstVar=out
      tmp2/*seq*/ = out;
      tmp2/*seq*/ = tmp6_rmp_cur;
      tmp6_rmp_cur = mathMinf(tmp6_rmp_cur+tmp7_rmp_step__cycle_bd_sat_b_0_bd, 0.93);
      
      // -- mod="lut" dstVar=tmp2/*seq*/
      float tmp10;
      float tmp11;
      float tmp12;
      tmp3/*lut_f*/ = (tmp2/*seq*/ * 32);
      tmp8/*lut_idx_a*/ = (int)tmp3/*lut_f*/;
      tmp10/*lut_frac*/ = tmp3/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp11/*lut_a*/ = curve_3_cycle_bd_sat_b_0_bd[( (unsigned int)tmp8/*lut_idx_a*/      ) & 31] * (1.0f / 2048);
      tmp12/*lut_b*/ = curve_3_cycle_bd_sat_b_0_bd[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 31] * (1.0f / 2048);
      tmp2/*seq*/ = tmp11/*lut_a*/ + (tmp12/*lut_b*/ - tmp11/*lut_a*/) * tmp10/*lut_frac*/;
      out *= tmp2/*seq*/;
      
      // -- mod="$SIN_DRIVE" dstVar=out
      out *= 2.00633f;
      
      // -- mod="0.25" dstVar=out
      out *= 0.25f;
      
      // -- mod="0.5" dstVar=out
      out += 0.5f;
      
      // -- mod="lut" dstVar=out
      tmp2/*lut_f*/ = (out * 32);
      tmp8/*lut_idx_a*/ = (int)tmp2/*lut_f*/;
      tmp3/*lut_frac*/ = tmp2/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp10/*lut_a*/ = curve_4_cycle_bd_sat_b_0_bd[( (unsigned int)tmp8/*lut_idx_a*/      ) & 31] * (1.0f / 2048);
      tmp11/*lut_b*/ = curve_4_cycle_bd_sat_b_0_bd[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 31] * (1.0f / 2048);
      out = tmp10/*lut_a*/ + (tmp11/*lut_b*/ - tmp10/*lut_a*/) * tmp3/*lut_frac*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      
      // -- mod="box" dstVar=out
      out = mathLerpf(tmp13_box_last, out, 0.750976f);
      tmp13_box_last = out;
      
      // -- mod="$SIN_AMP" dstVar=out
      out *= 0.856057f;
      
      
      // ========
      // ======== lane "out" modIdx=1 modString=nos
      // ========
      
      // -- mod="nos" dstVar=out
      tmp2/*seq*/ = out;
      tmp8/*nos_fb*/ = tmp14_nos_seed1__cycle_bd_sat_b_0_bd & 1;
      tmp14_nos_seed1__cycle_bd_sat_b_0_bd >>= 1;
      if(0 != tmp8/*nos_fb*/)
          tmp14_nos_seed1__cycle_bd_sat_b_0_bd ^= 0xB4BCD35C;
      tmp8/*nos_fb*/ = tmp15_nos_seed2__cycle_bd_sat_b_0_bd & 1;
      tmp15_nos_seed2__cycle_bd_sat_b_0_bd >>= 1;
      if(0 != tmp8/*nos_fb*/)
          tmp15_nos_seed2__cycle_bd_sat_b_0_bd ^= 0x7A5BC2E3;
      tmp2/*seq*/ = (((tmp14_nos_seed1__cycle_bd_sat_b_0_bd ^ tmp15_nos_seed2__cycle_bd_sat_b_0_bd) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="rmp" dstVar=tmp2/*seq*/
      tmp3/*seq*/ = tmp2/*seq*/;
      tmp3/*seq*/ = tmp16_rmp_cur;
      tmp16_rmp_cur = mathMaxf(tmp16_rmp_cur+tmp17_rmp_step__cycle_bd_sat_b_0_bd, 0);
      tmp2/*seq*/ *= tmp3/*seq*/;
      
      // -- mod="$NOS_LVL" dstVar=tmp2/*seq*/
      tmp2/*seq*/ *= 0.0230656f;
      out += tmp2/*seq*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      
      // ------ xfade (100 frames) -----
      if(outIdx >= 3400)
      {
         if(xfadeCaptureLeft > 0)
         {
            cycle_xfade_buf[xfadeCaptureIdx++] = out;
            xfadeCaptureLeft--;
         }
         else if(outIdx >= 3900)
         {
            out = mathLerpf(out, cycle_xfade_buf[xfadeReadIdx++], xfadeCur);
            xfadeCur += 0.01;
         }
      }
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: fm choir 1 cycle export 1b off=7548 num=24000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 24000



static float var_x_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_wt_x_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_wt_y_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_wt_freq_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_v_env_rmp_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_v_o3_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_v_o2_fm_choir_1_cycle_export_1b_0_ = 0.0f;
static float var_v_o1_fm_choir_1_cycle_export_1b_0_ = 0.0f;

static short curve_0_fm_choir_1_cycle_export_1b_0_[256] = {
2, 48, 93, 139, 185, 230, 276, 322, 368, 413, 459, 505, 550, 596, 642, 688, 733, 779, 825, 870, 916, 962, 1007, 1053, 1098, 1144, 1189, 1234, 1279, 1325, 1369, 1414, 
1457, 1498, 1537, 1571, 1603, 1633, 1665, 1699, 1734, 1769, 1801, 1831, 1857, 1879, 1897, 1912, 1927, 1945, 1965, 1982, 1996, 2006, 2014, 2020, 2025, 2028, 2031, 2034, 2038, 2042, 2045, 2046, 
2046, 2047, 2040, 2036, 2032, 2028, 2023, 2017, 2010, 2000, 1988, 1975, 1958, 1940, 1921, 1901, 1880, 1858, 1836, 1813, 1790, 1766, 1742, 1718, 1692, 1666, 1639, 1612, 1583, 1552, 1513, 1471, 
1428, 1384, 1340, 1296, 1251, 1207, 1162, 1117, 1072, 1027, 982, 937, 892, 847, 801, 756, 711, 666, 620, 575, 530, 484, 439, 393, 348, 303, 257, 212, 166, 121, 76, 30, 
-14, -60, -105, -151, -196, -241, -287, -332, -377, -423, -468, -513, -559, -604, -649, -694, -740, -785, -830, -875, -920, -965, -1010, -1055, -1099, -1144, -1188, -1233, -1277, -1321, -1364, -1407, 
-1450, -1490, -1530, -1567, -1604, -1638, -1672, -1704, -1734, -1763, -1791, -1817, -1842, -1865, -1887, -1907, -1927, -1944, -1960, -1975, -1989, -2001, -2012, -2021, -2029, -2036, -2041, -2045, -2048, -2049, -2049, -2047, 
-2045, -2043, -2040, -2038, -2035, -2032, -2029, -2025, -2019, -2007, -1990, -1974, -1958, -1944, -1928, -1911, -1893, -1874, -1855, -1834, -1812, -1788, -1764, -1738, -1711, -1682, -1652, -1621, -1587, -1552, -1515, -1476, 
-1434, -1392, -1348, -1304, -1259, -1213, -1168, -1122, -1075, -1029, -982, -935, -888, -841, -794, -747, -699, -652, -605, -557, -510, -462, -415, -368, -320, -273, -226, -179, -133, -87, -41, 2, 
};
static short curve_1_fm_choir_1_cycle_export_1b_0_[256] = {
2048, 2048, 2049, 2050, 2050, 2051, 2052, 2052, 2053, 2054, 2054, 2055, 2056, 2056, 2057, 2058, 2058, 2059, 2060, 2060, 2061, 2062, 2062, 2063, 2064, 2065, 2065, 2066, 2067, 2067, 2068, 2069, 
2069, 2070, 2071, 2071, 2072, 2073, 2073, 2074, 2075, 2075, 2076, 2077, 2077, 2078, 2079, 2079, 2080, 2081, 2082, 2082, 2083, 2084, 2084, 2085, 2086, 2086, 2087, 2088, 2088, 2089, 2090, 2090, 
2091, 2092, 2092, 2093, 2094, 2094, 2095, 2096, 2096, 2097, 2098, 2099, 2099, 2100, 2101, 2101, 2102, 2103, 2103, 2104, 2105, 2105, 2106, 2107, 2107, 2108, 2109, 2108, 2108, 2108, 2107, 2107, 
2107, 2106, 2106, 2105, 2105, 2105, 2104, 2104, 2104, 2103, 2103, 2102, 2102, 2102, 2101, 2101, 2101, 2100, 2100, 2100, 2099, 2099, 2098, 2098, 2098, 2097, 2097, 2097, 2096, 2096, 2095, 2095, 
2095, 2094, 2094, 2094, 2093, 2093, 2093, 2092, 2092, 2091, 2091, 2091, 2090, 2090, 2090, 2089, 2089, 2088, 2088, 2088, 2087, 2087, 2087, 2086, 2086, 2086, 2085, 2085, 2084, 2084, 2084, 2083, 
2083, 2083, 2082, 2082, 2081, 2081, 2081, 2080, 2080, 2080, 2079, 2079, 2079, 2078, 2078, 2077, 2077, 2077, 2076, 2076, 2076, 2075, 2075, 2074, 2074, 2074, 2073, 2073, 2073, 2072, 2072, 2072, 
2071, 2071, 2070, 2070, 2070, 2069, 2069, 2069, 2068, 2068, 2067, 2067, 2067, 2066, 2066, 2066, 2065, 2065, 2065, 2064, 2064, 2063, 2063, 2063, 2062, 2062, 2062, 2061, 2061, 2060, 2060, 2060, 
2059, 2059, 2059, 2058, 2058, 2058, 2057, 2057, 2056, 2056, 2056, 2055, 2055, 2055, 2054, 2054, 2053, 2053, 2053, 2052, 2052, 2052, 2051, 2051, 2051, 2050, 2050, 2049, 2049, 2049, 2048, 2048, 
};
static short curve_2_fm_choir_1_cycle_export_1b_0_[256] = {
0, 30, 60, 91, 121, 152, 182, 213, 243, 274, 304, 335, 365, 396, 426, 457, 487, 518, 548, 579, 609, 640, 671, 701, 732, 762, 793, 823, 854, 884, 915, 945, 
976, 1006, 1037, 1067, 1098, 1128, 1159, 1189, 1220, 1250, 1281, 1311, 1342, 1372, 1403, 1433, 1463, 1494, 1524, 1555, 1585, 1616, 1646, 1676, 1707, 1737, 1768, 1798, 1828, 1859, 1889, 1919, 
1949, 1979, 2010, 2039, 2034, 2030, 2026, 2022, 2018, 2014, 2010, 2006, 2003, 1999, 1996, 1992, 1988, 1985, 1981, 1978, 1975, 1971, 1968, 1964, 1961, 1957, 1954, 1951, 1947, 1944, 1941, 1937, 
1934, 1931, 1927, 1924, 1921, 1917, 1914, 1911, 1907, 1904, 1901, 1898, 1894, 1891, 1888, 1885, 1881, 1878, 1875, 1872, 1868, 1865, 1862, 1859, 1855, 1852, 1849, 1846, 1843, 1839, 1836, 1833, 
1830, 1827, 1823, 1820, 1817, 1814, 1811, 1807, 1804, 1801, 1798, 1795, 1791, 1788, 1785, 1782, 1779, 1776, 1772, 1769, 1766, 1763, 1760, 1756, 1753, 1750, 1747, 1744, 1741, 1737, 1734, 1731, 
1728, 1725, 1722, 1718, 1715, 1712, 1709, 1706, 1703, 1699, 1696, 1693, 1690, 1687, 1684, 1680, 1677, 1674, 1671, 1668, 1665, 1661, 1658, 1655, 1652, 1649, 1645, 1642, 1639, 1636, 1633, 1629, 
1626, 1623, 1620, 1616, 1613, 1610, 1606, 1603, 1599, 1595, 1591, 1588, 1584, 1580, 1577, 1573, 1570, 1567, 1563, 1560, 1557, 1554, 1550, 1547, 1544, 1541, 1538, 1535, 1532, 1529, 1526, 1522, 
1519, 1516, 1513, 1509, 1506, 1503, 1499, 1496, 1493, 1489, 1486, 1482, 1479, 1476, 1472, 1469, 1466, 1462, 1459, 1456, 1452, 1449, 1445, 1442, 1439, 1435, 1432, 1429, 1425, 1422, 1418, 1415, 
};
static short curve_3_fm_choir_1_cycle_export_1b_0_[256] = {
2, 48, 94, 140, 187, 233, 280, 326, 373, 420, 466, 513, 560, 606, 653, 699, 745, 792, 838, 884, 930, 975, 1021, 1066, 1111, 1156, 1200, 1244, 1288, 1331, 1373, 1414, 
1453, 1489, 1524, 1557, 1588, 1618, 1646, 1674, 1700, 1725, 1750, 1773, 1796, 1818, 1840, 1861, 1882, 1902, 1923, 1945, 1957, 1969, 1982, 1995, 2007, 2018, 2028, 2036, 2043, 2049, 2052, 2053, 
2051, 2047, 2041, 2036, 2031, 2026, 2020, 2014, 2008, 2001, 1993, 1984, 1974, 1964, 1952, 1939, 1925, 1910, 1894, 1876, 1856, 1834, 1810, 1784, 1755, 1723, 1687, 1647, 1601, 1548, 1494, 1442, 
1392, 1343, 1295, 1247, 1200, 1153, 1106, 1059, 1013, 967, 921, 875, 830, 784, 739, 693, 648, 603, 558, 513, 468, 423, 379, 334, 290, 245, 201, 156, 112, 68, 23, -20, 
-64, -108, -152, -196, -240, -284, -328, -372, -416, -459, -503, -547, -590, -634, -677, -721, -764, -808, -851, -894, -938, -981, -1024, -1067, -1110, -1153, -1196, -1238, -1281, -1324, -1366, -1408, 
-1450, -1491, -1531, -1571, -1609, -1647, -1683, -1718, -1751, -1781, -1809, -1833, -1854, -1873, -1890, -1909, -1929, -1946, -1960, -1971, -1981, -1990, -1997, -2004, -2009, -2014, -2018, -2021, -2024, -2025, -2026, -2025, 
-2024, -2022, -2019, -2015, -2011, -2006, -2001, -1995, -1989, -1982, -1975, -1967, -1958, -1943, -1921, -1901, -1881, -1861, -1840, -1819, -1798, -1775, -1752, -1727, -1702, -1675, -1647, -1617, -1585, -1552, -1515, -1476, 
-1434, -1392, -1348, -1304, -1259, -1214, -1168, -1123, -1077, -1030, -984, -937, -890, -843, -796, -749, -702, -655, -608, -560, -513, -466, -418, -371, -324, -276, -229, -182, -135, -89, -43, 2, 
};
static short curve_4_fm_choir_1_cycle_export_1b_0_[256] = {
2048, 2048, 2049, 2050, 2050, 2051, 2052, 2052, 2053, 2053, 2054, 2055, 2055, 2056, 2057, 2057, 2058, 2059, 2059, 2060, 2060, 2061, 2062, 2062, 2063, 2064, 2064, 2065, 2066, 2066, 2067, 2068, 
2068, 2069, 2070, 2070, 2071, 2072, 2072, 2073, 2074, 2074, 2075, 2076, 2076, 2077, 2077, 2078, 2079, 2079, 2080, 2081, 2081, 2082, 2083, 2083, 2084, 2085, 2085, 2086, 2087, 2087, 2088, 2088, 
2089, 2090, 2090, 2091, 2092, 2092, 2093, 2094, 2094, 2095, 2096, 2096, 2097, 2098, 2098, 2099, 2100, 2100, 2101, 2102, 2102, 2103, 2104, 2104, 2105, 2106, 2106, 2107, 2108, 2108, 2109, 2110, 
2110, 2109, 2109, 2109, 2108, 2108, 2107, 2107, 2107, 2106, 2106, 2105, 2105, 2105, 2104, 2104, 2103, 2103, 2103, 2102, 2102, 2102, 2101, 2101, 2101, 2100, 2100, 2099, 2099, 2099, 2098, 2098, 
2097, 2097, 2097, 2096, 2096, 2095, 2095, 2095, 2094, 2094, 2093, 2093, 2093, 2092, 2092, 2091, 2091, 2091, 2090, 2090, 2089, 2089, 2089, 2088, 2088, 2087, 2087, 2087, 2086, 2086, 2086, 2085, 
2085, 2084, 2084, 2084, 2083, 2083, 2082, 2082, 2082, 2081, 2081, 2080, 2080, 2080, 2079, 2079, 2078, 2078, 2078, 2077, 2077, 2077, 2076, 2076, 2075, 2075, 2075, 2074, 2074, 2073, 2073, 2073, 
2072, 2072, 2071, 2071, 2071, 2070, 2070, 2069, 2069, 2069, 2068, 2068, 2068, 2067, 2067, 2067, 2066, 2066, 2065, 2065, 2065, 2064, 2064, 2063, 2063, 2063, 2062, 2062, 2061, 2061, 2061, 2060, 
2060, 2060, 2059, 2059, 2058, 2058, 2058, 2057, 2057, 2056, 2056, 2056, 2055, 2055, 2054, 2054, 2054, 2053, 2053, 2053, 2052, 2052, 2051, 2051, 2051, 2050, 2050, 2049, 2049, 2049, 2048, 2048, 
};
static short curve_5_fm_choir_1_cycle_export_1b_0_[256] = {
0, 30, 61, 91, 122, 153, 184, 215, 247, 278, 309, 340, 372, 403, 434, 466, 497, 529, 560, 592, 624, 655, 687, 718, 750, 782, 813, 845, 877, 908, 940, 972, 
1003, 1035, 1067, 1099, 1130, 1162, 1194, 1226, 1257, 1289, 1321, 1352, 1384, 1416, 1448, 1479, 1511, 1542, 1574, 1606, 1637, 1669, 1700, 1732, 1763, 1794, 1826, 1857, 1888, 1919, 1949, 1979, 
2002, 2026, 2048, 2045, 2048, 2047, 2045, 2042, 2039, 2037, 2034, 2031, 2028, 2025, 2022, 2018, 2015, 2012, 2009, 2006, 2002, 1999, 1996, 1993, 1989, 1986, 1983, 1979, 1976, 1973, 1969, 1966, 
1963, 1959, 1956, 1952, 1949, 1946, 1942, 1939, 1935, 1932, 1929, 1925, 1922, 1918, 1915, 1911, 1908, 1905, 1901, 1898, 1894, 1891, 1887, 1884, 1880, 1877, 1873, 1870, 1867, 1863, 1860, 1856, 
1853, 1849, 1846, 1842, 1839, 1835, 1832, 1828, 1825, 1821, 1818, 1814, 1811, 1807, 1804, 1800, 1797, 1793, 1790, 1786, 1783, 1779, 1775, 1772, 1768, 1765, 1761, 1758, 1754, 1751, 1747, 1744, 
1740, 1737, 1733, 1730, 1726, 1723, 1719, 1715, 1712, 1708, 1705, 1701, 1698, 1694, 1691, 1687, 1684, 1680, 1677, 1673, 1669, 1666, 1662, 1659, 1655, 1652, 1648, 1645, 1641, 1638, 1634, 1631, 
1627, 1624, 1620, 1616, 1613, 1609, 1606, 1603, 1599, 1596, 1593, 1590, 1586, 1583, 1580, 1576, 1573, 1570, 1566, 1563, 1559, 1556, 1552, 1549, 1545, 1542, 1539, 1535, 1532, 1529, 1526, 1522, 
1519, 1516, 1512, 1509, 1506, 1502, 1499, 1496, 1492, 1489, 1486, 1482, 1479, 1476, 1472, 1469, 1466, 1462, 1459, 1455, 1452, 1449, 1445, 1442, 1439, 1435, 1432, 1429, 1425, 1422, 1418, 1415, 
};
static short curve_6_fm_choir_1_cycle_export_1b_0_[256] = {
-20, 49, 117, 185, 252, 318, 383, 446, 509, 571, 631, 691, 749, 806, 862, 917, 970, 1023, 1074, 1124, 1173, 1220, 1266, 1311, 1355, 1397, 1438, 1477, 1515, 1552, 1587, 1621, 
1653, 1684, 1713, 1741, 1767, 1792, 1815, 1836, 1856, 1875, 1892, 1907, 1921, 1933, 1944, 1953, 1961, 1967, 1972, 1976, 1979, 1980, 1981, 1981, 1980, 1979, 1978, 1974, 1966, 1956, 1943, 1929, 
1914, 1897, 1879, 1860, 1840, 1820, 1799, 1777, 1754, 1731, 1708, 1684, 1659, 1634, 1609, 1583, 1557, 1530, 1503, 1476, 1448, 1420, 1392, 1363, 1334, 1305, 1276, 1246, 1216, 1185, 1154, 1124, 
1092, 1061, 1029, 997, 964, 932, 899, 866, 832, 798, 764, 730, 695, 660, 624, 589, 552, 516, 479, 442, 405, 367, 328, 289, 250, 210, 170, 129, 88, 46, 3, -40, 
-83, -125, -166, -206, -246, -285, -323, -360, -397, -434, -470, -505, -540, -575, -609, -642, -676, -709, -741, -773, -805, -836, -867, -898, -929, -959, -989, -1018, -1047, -1076, -1105, -1133, 
-1162, -1189, -1217, -1244, -1271, -1298, -1324, -1350, -1376, -1402, -1427, -1452, -1477, -1501, -1525, -1549, -1572, -1595, -1618, -1640, -1662, -1684, -1705, -1726, -1747, -1766, -1786, -1805, -1823, -1841, -1858, -1874, 
-1890, -1905, -1918, -1931, -1942, -1951, -1958, -1963, -1965, -1965, -1964, -1964, -1964, -1963, -1961, -1959, -1955, -1950, -1943, -1935, -1926, -1914, -1901, -1886, -1869, -1851, -1831, -1808, -1785, -1759, -1731, -1702, 
-1671, -1638, -1604, -1568, -1531, -1491, -1451, -1409, -1365, -1320, -1273, -1225, -1176, -1125, -1074, -1020, -966, -910, -854, -796, -737, -677, -615, -553, -490, -426, -360, -294, -227, -159, -90, -20, 
};
static short curve_7_fm_choir_1_cycle_export_1b_0_[256] = {
2048, 2048, 2049, 2050, 2050, 2051, 2052, 2052, 2053, 2054, 2054, 2055, 2056, 2056, 2057, 2058, 2058, 2059, 2060, 2060, 2061, 2062, 2062, 2063, 2064, 2064, 2065, 2066, 2066, 2067, 2068, 2068, 
2069, 2070, 2070, 2071, 2072, 2072, 2073, 2074, 2074, 2075, 2076, 2076, 2077, 2078, 2078, 2079, 2080, 2080, 2081, 2082, 2082, 2083, 2084, 2084, 2085, 2086, 2086, 2087, 2088, 2088, 2089, 2090, 
2090, 2091, 2092, 2092, 2093, 2094, 2095, 2095, 2096, 2097, 2097, 2098, 2099, 2099, 2100, 2101, 2101, 2102, 2103, 2103, 2104, 2105, 2105, 2106, 2107, 2107, 2107, 2107, 2106, 2106, 2105, 2105, 
2105, 2104, 2104, 2104, 2103, 2103, 2103, 2102, 2102, 2102, 2101, 2101, 2100, 2100, 2100, 2099, 2099, 2099, 2098, 2098, 2098, 2097, 2097, 2097, 2096, 2096, 2095, 2095, 2095, 2094, 2094, 2094, 
2093, 2093, 2093, 2092, 2092, 2092, 2091, 2091, 2090, 2090, 2090, 2089, 2089, 2089, 2088, 2088, 2088, 2087, 2087, 2087, 2086, 2086, 2085, 2085, 2085, 2084, 2084, 2084, 2083, 2083, 2083, 2082, 
2082, 2082, 2081, 2081, 2080, 2080, 2080, 2079, 2079, 2079, 2078, 2078, 2078, 2077, 2077, 2077, 2076, 2076, 2075, 2075, 2075, 2074, 2074, 2074, 2073, 2073, 2073, 2072, 2072, 2072, 2071, 2071, 
2070, 2070, 2070, 2069, 2069, 2069, 2068, 2068, 2068, 2067, 2067, 2067, 2066, 2066, 2065, 2065, 2065, 2064, 2064, 2064, 2063, 2063, 2063, 2062, 2062, 2062, 2061, 2061, 2060, 2060, 2060, 2059, 
2059, 2059, 2058, 2058, 2058, 2057, 2057, 2057, 2056, 2056, 2055, 2055, 2055, 2054, 2054, 2054, 2053, 2053, 2053, 2052, 2052, 2052, 2051, 2051, 2050, 2050, 2050, 2049, 2049, 2049, 2048, 2048, 
};
static short curve_8_fm_choir_1_cycle_export_1b_0_[256] = {
0, 30, 60, 91, 121, 152, 182, 213, 243, 274, 304, 334, 365, 395, 426, 456, 487, 517, 548, 578, 608, 639, 669, 700, 730, 761, 791, 822, 852, 882, 913, 943, 
974, 1004, 1035, 1065, 1096, 1126, 1156, 1187, 1217, 1248, 1278, 1309, 1339, 1370, 1400, 1431, 1461, 1491, 1522, 1552, 1583, 1613, 1644, 1674, 1705, 1735, 1765, 1796, 1826, 1857, 1887, 1918, 
1948, 1979, 2009, 2039, 2046, 2043, 2039, 2036, 2033, 2029, 2026, 2023, 2019, 2016, 2013, 2009, 2006, 2003, 1999, 1996, 1993, 1989, 1986, 1983, 1979, 1976, 1973, 1969, 1966, 1963, 1958, 1953, 
1948, 1943, 1939, 1934, 1930, 1925, 1921, 1916, 1912, 1908, 1904, 1900, 1896, 1893, 1889, 1886, 1882, 1879, 1876, 1873, 1870, 1867, 1864, 1861, 1859, 1856, 1853, 1851, 1848, 1846, 1843, 1841, 
1838, 1835, 1833, 1830, 1827, 1825, 1822, 1819, 1816, 1813, 1810, 1807, 1804, 1801, 1797, 1794, 1791, 1788, 1784, 1781, 1778, 1774, 1771, 1767, 1764, 1760, 1757, 1753, 1750, 1746, 1742, 1739, 
1735, 1732, 1728, 1724, 1721, 1717, 1714, 1711, 1707, 1704, 1701, 1698, 1694, 1691, 1688, 1685, 1682, 1679, 1676, 1673, 1670, 1667, 1663, 1660, 1657, 1653, 1650, 1647, 1643, 1640, 1636, 1632, 
1629, 1625, 1622, 1618, 1614, 1611, 1607, 1603, 1599, 1596, 1592, 1588, 1585, 1581, 1578, 1575, 1571, 1568, 1564, 1561, 1558, 1554, 1551, 1548, 1544, 1541, 1538, 1535, 1531, 1528, 1525, 1521, 
1518, 1515, 1511, 1508, 1505, 1501, 1498, 1495, 1491, 1488, 1485, 1481, 1478, 1475, 1471, 1468, 1464, 1461, 1457, 1454, 1450, 1447, 1443, 1440, 1437, 1433, 1430, 1427, 1424, 1421, 1418, 1415, 
};

float tmp2_rmp_step__fm_choir_1_cycle_export_1b_0_;

static void calc_fm_choir_1_cycle_export_1b_0_(float *_d) {
   float tmp1_rmp_cur = 0.0f;
   float tmp3_pha_phase = 0.0f;
   short tmp8;
   short tmp9;
   float tmp13_pha_phase = 0.0f;
   float tmp14_pha_phase = 0.0f;
   float tmp15_hbx_last = 0.0f;
   // start=0 end=0.996094 #frames=24000:
   float tmp4;
   
   tmp2_rmp_step__fm_choir_1_cycle_export_1b_0_ = 4.15039e-05f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(24000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=rmp
      // ========
      
      // -- mod="rmp" dstVar=out
      out = tmp1_rmp_cur;
      tmp1_rmp_cur = mathMinf(tmp1_rmp_cur+tmp2_rmp_step__fm_choir_1_cycle_export_1b_0_, 0.996094);
      
      // -- mod="sto v_env_rmp" dstVar=out
      var_v_env_rmp_fm_choir_1_cycle_export_1b_0_ = out;
      
      // -- mod="pha" dstVar=out
      float tmp5;
      
      // ---- mod="pha" input "freq" seq 1/1
      
      // -- mod="$P_O3_RATIO" dstVar=tmp5/*pha_freq*/
      tmp5/*pha_freq*/ = 1.005f;
      
      // -- mod="$v_env_rmp" dstVar=tmp5/*pha_freq*/
      float tmp6;
      tmp6/*seq*/ = tmp5/*pha_freq*/;
      tmp6/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp6/*seq*/
      float tmp7;
      float tmp10;
      float tmp11;
      float tmp12;
      tmp7/*lut_f*/ = (tmp6/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp7/*lut_f*/;
      tmp10/*lut_frac*/ = tmp7/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp11/*lut_a*/ = curve_1_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp12/*lut_b*/ = curve_1_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp6/*seq*/ = tmp11/*lut_a*/ + (tmp12/*lut_b*/ - tmp11/*lut_a*/) * tmp10/*lut_frac*/;
      tmp5/*pha_freq*/ *= tmp6/*seq*/;
      tmp4/*pha_speed*/ = 0.00545062f * tmp5/*pha_freq*/;
      tmp5/*pha_tmp*/ = (tmp3_pha_phase);
      
      // ---- mod="pha" input "phasemod" seq 1/1
      
      // -- mod="0" dstVar=tmp6/*phasemod*/
      tmp6/*phasemod*/ = 0.0f;
      tmp5/*pha_tmp*/ += tmp6/*phasemod*/;
      out = ffrac_s(tmp5/*pha_tmp*/);
      tmp3_pha_phase = ffrac_s(tmp3_pha_phase + tmp4/*pha_speed*/);
      
      // -- mod="lut" dstVar=out
      tmp4/*lut_f*/ = (out * 256);
      tmp8/*lut_idx_a*/ = (int)tmp4/*lut_f*/;
      tmp5/*lut_frac*/ = tmp4/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp6/*lut_a*/ = curve_0_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp7/*lut_b*/ = curve_0_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      out = tmp6/*lut_a*/ + (tmp7/*lut_b*/ - tmp6/*lut_a*/) * tmp5/*lut_frac*/;
      
      // -- mod="$v_env_rmp" dstVar=out
      tmp4/*seq*/ = out;
      tmp4/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp4/*seq*/
      tmp5/*lut_f*/ = (tmp4/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp5/*lut_f*/;
      tmp6/*lut_frac*/ = tmp5/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp7/*lut_a*/ = curve_2_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp10/*lut_b*/ = curve_2_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp4/*seq*/ = tmp7/*lut_a*/ + (tmp10/*lut_b*/ - tmp7/*lut_a*/) * tmp6/*lut_frac*/;
      out *= tmp4/*seq*/;
      
      // -- mod="$P_O3_LEVEL" dstVar=out
      out *= 1.0f;
      
      // -- mod="sto v_o3" dstVar=out
      var_v_o3_fm_choir_1_cycle_export_1b_0_ = out;
      
      // -- mod="pha" dstVar=out
      
      // ---- mod="pha" input "freq" seq 1/1
      
      // -- mod="$P_O2_RATIO" dstVar=tmp5/*pha_freq*/
      tmp5/*pha_freq*/ = 1.5f;
      
      // -- mod="$v_env_rmp" dstVar=tmp5/*pha_freq*/
      tmp6/*seq*/ = tmp5/*pha_freq*/;
      tmp6/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp6/*seq*/
      tmp7/*lut_f*/ = (tmp6/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp7/*lut_f*/;
      tmp10/*lut_frac*/ = tmp7/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp11/*lut_a*/ = curve_4_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp12/*lut_b*/ = curve_4_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp6/*seq*/ = tmp11/*lut_a*/ + (tmp12/*lut_b*/ - tmp11/*lut_a*/) * tmp10/*lut_frac*/;
      tmp5/*pha_freq*/ *= tmp6/*seq*/;
      tmp4/*pha_speed*/ = 0.00545062f * tmp5/*pha_freq*/;
      tmp5/*pha_tmp*/ = (tmp13_pha_phase);
      
      // ---- mod="pha" input "phasemod" seq 1/1
      
      // -- mod="0" dstVar=tmp6/*phasemod*/
      tmp6/*phasemod*/ = 0.0f;
      
      // -- mod="$v_o3" dstVar=tmp6/*phasemod*/
      tmp7/*seq*/ = tmp6/*phasemod*/;
      tmp7/*seq*/ = var_v_o3_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O3_TO_O2" dstVar=tmp7/*seq*/
      tmp7/*seq*/ *= 0.02f;
      tmp6/*phasemod*/ += tmp7/*seq*/;
      tmp5/*pha_tmp*/ += tmp6/*phasemod*/;
      out = ffrac_s(tmp5/*pha_tmp*/);
      tmp13_pha_phase = ffrac_s(tmp13_pha_phase + tmp4/*pha_speed*/);
      
      // -- mod="lut" dstVar=out
      tmp4/*lut_f*/ = (out * 256);
      tmp8/*lut_idx_a*/ = (int)tmp4/*lut_f*/;
      tmp5/*lut_frac*/ = tmp4/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp6/*lut_a*/ = curve_3_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp7/*lut_b*/ = curve_3_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      out = tmp6/*lut_a*/ + (tmp7/*lut_b*/ - tmp6/*lut_a*/) * tmp5/*lut_frac*/;
      
      // -- mod="$v_env_rmp" dstVar=out
      tmp4/*seq*/ = out;
      tmp4/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp4/*seq*/
      tmp5/*lut_f*/ = (tmp4/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp5/*lut_f*/;
      tmp6/*lut_frac*/ = tmp5/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp7/*lut_a*/ = curve_5_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp10/*lut_b*/ = curve_5_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp4/*seq*/ = tmp7/*lut_a*/ + (tmp10/*lut_b*/ - tmp7/*lut_a*/) * tmp6/*lut_frac*/;
      out *= tmp4/*seq*/;
      
      // -- mod="$P_O2_LEVEL" dstVar=out
      out *= 1.0f;
      
      // -- mod="sto v_o2" dstVar=out
      var_v_o2_fm_choir_1_cycle_export_1b_0_ = out;
      
      // -- mod="pha" dstVar=out
      
      // ---- mod="pha" input "freq" seq 1/1
      
      // -- mod="$P_O1_RATIO" dstVar=tmp5/*pha_freq*/
      tmp5/*pha_freq*/ = 1.0f;
      
      // -- mod="$v_env_rmp" dstVar=tmp5/*pha_freq*/
      tmp6/*seq*/ = tmp5/*pha_freq*/;
      tmp6/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp6/*seq*/
      tmp7/*lut_f*/ = (tmp6/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp7/*lut_f*/;
      tmp10/*lut_frac*/ = tmp7/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp11/*lut_a*/ = curve_7_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp12/*lut_b*/ = curve_7_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp6/*seq*/ = tmp11/*lut_a*/ + (tmp12/*lut_b*/ - tmp11/*lut_a*/) * tmp10/*lut_frac*/;
      tmp5/*pha_freq*/ *= tmp6/*seq*/;
      tmp4/*pha_speed*/ = 0.00545062f * tmp5/*pha_freq*/;
      tmp5/*pha_tmp*/ = (tmp14_pha_phase);
      
      // ---- mod="pha" input "phasemod" seq 1/1
      
      // -- mod="0" dstVar=tmp6/*phasemod*/
      tmp6/*phasemod*/ = 0.0f;
      
      // -- mod="$v_o2" dstVar=tmp6/*phasemod*/
      tmp7/*seq*/ = tmp6/*phasemod*/;
      tmp7/*seq*/ = var_v_o2_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O2_TO_O1" dstVar=tmp7/*seq*/
      tmp7/*seq*/ *= 0.01f;
      tmp6/*phasemod*/ += tmp7/*seq*/;
      
      // -- mod="$v_o3" dstVar=tmp6/*phasemod*/
      tmp7/*seq*/ = tmp6/*phasemod*/;
      tmp7/*seq*/ = var_v_o3_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O3_TO_O1" dstVar=tmp7/*seq*/
      tmp7/*seq*/ *= 0.01f;
      tmp6/*phasemod*/ += tmp7/*seq*/;
      tmp5/*pha_tmp*/ += tmp6/*phasemod*/;
      out = ffrac_s(tmp5/*pha_tmp*/);
      tmp14_pha_phase = ffrac_s(tmp14_pha_phase + tmp4/*pha_speed*/);
      
      // -- mod="lut" dstVar=out
      tmp4/*lut_f*/ = (out * 256);
      tmp8/*lut_idx_a*/ = (int)tmp4/*lut_f*/;
      tmp5/*lut_frac*/ = tmp4/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp6/*lut_a*/ = curve_6_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp7/*lut_b*/ = curve_6_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      out = tmp6/*lut_a*/ + (tmp7/*lut_b*/ - tmp6/*lut_a*/) * tmp5/*lut_frac*/;
      
      // -- mod="$v_env_rmp" dstVar=out
      tmp4/*seq*/ = out;
      tmp4/*seq*/ = var_v_env_rmp_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="lut" dstVar=tmp4/*seq*/
      tmp5/*lut_f*/ = (tmp4/*seq*/ * 256);
      tmp8/*lut_idx_a*/ = (int)tmp5/*lut_f*/;
      tmp6/*lut_frac*/ = tmp5/*lut_f*/ - (float)tmp8/*lut_idx_a*/;
      tmp7/*lut_a*/ = curve_8_fm_choir_1_cycle_export_1b_0_[( (unsigned int)tmp8/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp10/*lut_b*/ = curve_8_fm_choir_1_cycle_export_1b_0_[(((unsigned int)tmp8/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      tmp4/*seq*/ = tmp7/*lut_a*/ + (tmp10/*lut_b*/ - tmp7/*lut_a*/) * tmp6/*lut_frac*/;
      out *= tmp4/*seq*/;
      
      // -- mod="$P_O1_LEVEL" dstVar=out
      out *= 1.0f;
      
      // -- mod="sto v_o1" dstVar=out
      var_v_o1_fm_choir_1_cycle_export_1b_0_ = out;
      
      // -- mod="$v_o1" dstVar=out
      out = var_v_o1_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O1_TO_OUT" dstVar=out
      out *= 0.7f;
      
      // -- mod="$v_o2" dstVar=out
      tmp4/*seq*/ = out;
      tmp4/*seq*/ = var_v_o2_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O2_TO_OUT" dstVar=tmp4/*seq*/
      tmp4/*seq*/ *= 0.11f;
      out += tmp4/*seq*/;
      
      // -- mod="$v_o3" dstVar=out
      tmp4/*seq*/ = out;
      tmp4/*seq*/ = var_v_o3_fm_choir_1_cycle_export_1b_0_;
      
      // -- mod="$P_O3_TO_OUT" dstVar=tmp4/*seq*/
      tmp4/*seq*/ *= 0.32f;
      out += tmp4/*seq*/;
      
      // -- mod="hbx" dstVar=out
      tmp15_hbx_last = mathLerpf(tmp15_hbx_last, out, 0.0625f);
      out = out - tmp15_hbx_last;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */


extern void sr_handle_cycle_sample_calc_finished (unsigned int _sampleIdx);
#define HSCF(a)  sr_handle_cycle_sample_calc_finished(a)

void cycle_calc_waveform_simple(float *_wfAddr) {
   // smp: "tuned_fb test" sz=3548 (3.46k) bProc=0
   // smp: "cycle_bd sat b" sz=4000 (3.9k) bProc=1
   calc_cycle_bd_sat_b_0_bd                     (_wfAddr +   3548);  // total=  4000 (15.62k)
   HSCF(1);
   // smp: "fm choir 1 cycle export 1b" sz=24000 (23.43k) bProc=1
   calc_fm_choir_1_cycle_export_1b_0_           (_wfAddr +   7548);  // total= 24000 (93.75k)
   HSCF(2);
   // procedural sample size: 112000 (109.37k)
   // static     sample size: 14192 (13.85k)
   // total      sample size: 126192 (123.23k)
}
